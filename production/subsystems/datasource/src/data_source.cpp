#include <datasource/data_source.hpp>

#include <fstream>
#include <algorithm>
#include <set>
#include <numeric>

#include <re2/stringpiece.h>
#include <spdlog/spdlog.h>
#include <compressor/compressor.hpp>


namespace datasource
{
	const char LINE_TERMINATOR = '\n';
	const std::size_t READ_BLOCK_SIZE = 1024 * 1024 * 32;
	const auto DEFAULT_UNCOMPRESSED_BYTES_PER_CHUNK = 512 * 1024;

	std::shared_ptr<content> data_source::readfile(const boost::filesystem::path& p) {
		auto ptr = std::make_shared<content>();
		auto path = p.string();
		std::ifstream s(path);
		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(READ_BLOCK_SIZE);
		while (!s.eof() && s.good()) {
			s.read(buffer.get(), READ_BLOCK_SIZE);
			auto bytes_read = s.gcount();	
			if (bytes_read > 0) {
				ptr->add_data(buffer.get(), bytes_read);
			}
		}

		ptr->end_stream();
		return ptr;
	}
	
	content::content() : sink(std::make_unique<promiscous_sink>()) {}

	// consider multi-byte tokens!
	void content::add_data(const char* buffer, std::size_t byte_count) {
		/*auto logger = spdlog::stdout_color_mt("content");*/

		const char* after_last_token = buffer + byte_count;
		const char* last_start_of_row = after_last_token;
		while (last_start_of_row > buffer) {
			if (*last_start_of_row == LINE_TERMINATOR) {
				++last_start_of_row;
				break;
			}
			else
				--last_start_of_row;
		}
				
		if (last_start_of_row == buffer) {
			carry.append(buffer, byte_count);
			return;
		}

		std::size_t bytes_to_carry = after_last_token - last_start_of_row;
		carry.append(buffer, byte_count - bytes_to_carry);
		sink->consume_raw(carry.data(), carry.size());
		carry.assign(last_start_of_row, bytes_to_carry);
		/*
		std::size_t first_byte(0);
		if (!chunks.empty()) {
			first_byte = chunks.back().first_byte_number + chunks.back().compressed_data.size() + 1;
		}
		
		std::vector<char> chunk_content(MAX_CHUNK_SIZE);
		auto bytes_written = LZ4_compress_default(analyzed.data(), chunk_content.data(), analyzed.size(), MAX_CHUNK_SIZE);
		assert(bytes_written > 0);
		chunk_content.resize(bytes_written);
		chunk_content.shrink_to_fit();
		chunks.push_back(chunk{ first_byte, analyzed.size(), static_cast<std::size_t>(bytes_written), std::move(chunk_content) });
		*/
		/*logger->debug("Compressed {0:d} bytes into {1:d} bytes in chunk {2:d}. Bytes carried to next: {3:d}", analyzed.size(), bytes_written, chunks.size(), carry.size());*/
	}

	void content::end_stream() {
		if (!carry.empty()) {
			carry.push_back(LINE_TERMINATOR);
			sink->consume_raw(carry.data(), carry.size());
		}
		sink->end_stream();
	}

	bool promiscous_sink::should_stay(const char*, std::size_t) {
		return true;
	}

	data_sink* content::get_sink() {
		return sink.get();
	}

	void data_sink::add_child(const std::shared_ptr<data_sink>& child) {
		children.push_back(std::weak_ptr<data_sink>(child));

		for (auto c : chunks)
			child->consume(&c);
	}

	void data_sink::consume(content& content) {
		auto cs = content.get_sink();
		for (auto& c : cs->chunks) {
			consume(&c);
		}
		end_stream();
	}

	void data_sink::consume(compressor::chunk* c) {
		std::vector<char> data(c->uncompressed_bytes_available);
		auto compressed_size = compressor.decompress_chunk(c, data.data());
		assert(compressed_size == c->compressed_bytes_available);
		consume_raw(data.data(), data.size());
	}

	void data_sink::consume_raw(const char* data, std::size_t bytes_of_data) {
		if (chunks.empty()) {
			chunks.push_back(compressor.get_chunk(0, bytes_of_data));
		}

		auto current_chunk = &chunks.back();
		const char* begin = data;
		const char* end = reinterpret_cast<const char*>(std::memchr(begin, LINE_TERMINATOR, bytes_of_data));
		std::size_t analyzed_bytes = 0;
		while (end != nullptr) {
			auto line_length = std::distance(begin, end) + 1;
			if (should_stay(begin, line_length)) {
				auto compressed_max_line_length = compressor.get_max_compressed_size(line_length);
				if (current_chunk->compressed_data.size() - current_chunk->compressed_bytes_available < compressed_max_line_length) {
					chunks.push_back(compressor.get_chunk(current_chunk->first_byte_number + current_chunk->uncompressed_bytes_available, DEFAULT_UNCOMPRESSED_BYTES_PER_CHUNK));
					current_chunk = &chunks.back();
				}

				auto destination = current_chunk->compressed_data.data() + current_chunk->compressed_bytes_available;
				auto capacity = current_chunk->compressed_data.size() - current_chunk->compressed_bytes_available;

				auto compressed_bytes = compressor.stream_compress(begin, destination, line_length, capacity);
				assert(compressed_bytes > 0);

				current_chunk->compressed_bytes_available += compressed_bytes;
				current_chunk->uncompressed_bytes_available += line_length;				
			}

			analyzed_bytes += line_length;
			begin = end + 1;
			end = reinterpret_cast<const char*>(std::memchr(begin, LINE_TERMINATOR, bytes_of_data - analyzed_bytes));
		}

		assert(analyzed_bytes == bytes_of_data);
	}

	void data_sink::end_stream() {
		for (auto& c : chunks) {
			c.compressed_data.resize(c.compressed_bytes_available);
			c.compressed_data.shrink_to_fit();
		}
	}

	grepping_data_sink::grepping_data_sink(std::unique_ptr<re2::RE2>&& regex): regex(std::move(regex)) {}

	bool grepping_data_sink::should_stay(const char* start, std::size_t size) {
		auto bufferPiece = re2::StringPiece(start, size);
		return regex->Match(bufferPiece, 0, size, re2::RE2::UNANCHORED, &bufferPiece, 1);
	}
}