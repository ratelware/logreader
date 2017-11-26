#include <datasource/data_source.hpp>

#include<boost/filesystem.hpp>

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
	const auto DEFAULT_UNCOMPRESSED_CHARS_PER_CHUNK = 512 * 1024;

	std::shared_ptr<content> data_source::readfile(const boost::filesystem::path& p) {
		auto ptr = std::make_shared<content>();
		auto path = p.string();
		auto file_size = boost::filesystem::file_size(p);
		std::ifstream s(path);
		auto read_size = (file_size > READ_BLOCK_SIZE) ? READ_BLOCK_SIZE : file_size;

		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(read_size);
		while (!s.eof() && s.good()) {
			s.read(buffer.get(), read_size);
			auto chars_read = s.gcount();	
			if (chars_read > 0) {
				ptr->add_data(reinterpret_cast<const char*>(buffer.get()), chars_read);
			}
		}

		ptr->end_stream();
		return ptr;
	}
	
	content::content() : sink(std::make_unique<promiscous_sink>()) {}

	// consider multi-char tokens!
	void content::add_data(const char* buffer, std::size_t char_count) {
		/*auto logger = spdlog::stdout_color_mt("content");*/

		const char* after_last_token = buffer + char_count;
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
			carry.append(buffer, char_count);
			return;
		}

		std::size_t chars_to_carry = after_last_token - last_start_of_row;
		carry.append(buffer, char_count - chars_to_carry);
		sink->consume_raw(carry.data(), carry.size());
		carry.assign(last_start_of_row, chars_to_carry);
		/*
		std::size_t first_char(0);
		if (!chunks.empty()) {
			first_char = chunks.back().first_char_number + chunks.back().compressed_data.size() + 1;
		}
		
		std::vector<char> chunk_content(MAX_CHUNK_SIZE);
		auto chars_written = LZ4_compress_default(analyzed.data(), chunk_content.data(), analyzed.size(), MAX_CHUNK_SIZE);
		assert(chars_written > 0);
		chunk_content.resize(chars_written);
		chunk_content.shrink_to_fit();
		chunks.push_back(chunk{ first_char, analyzed.size(), static_cast<std::size_t>(chars_written), std::move(chunk_content) });
		*/
		/*logger->debug("Compressed {0:d} chars into {1:d} chars in chunk {2:d}. chars carried to next: {3:d}", analyzed.size(), chars_written, chunks.size(), carry.size());*/
	}

	void content::end_stream() {
		if (!carry.empty()) {
			carry.push_back(LINE_TERMINATOR);
			sink->consume_raw(carry.data(), carry.size());
		}
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
	}

	void data_sink::consume(compressor::chunk* c) {
		std::vector<char> data(c->uncompressed_chars_available);
		auto compressed_size = compressor.decompress_chunk(c, data.data());
		assert(compressed_size == c->compressed_chars_available);
		consume_raw(data.data(), data.size());
	}

	void data_sink::consume_raw(const char* data, std::size_t chars_of_data) {
		if (chunks.empty()) {
			chunks.push_back(compressor.get_chunk(0, DEFAULT_UNCOMPRESSED_CHARS_PER_CHUNK));
		}
		auto current_chunk = &chunks.back();
		const char* begin = data;
		const char* end = reinterpret_cast<const char*>(std::memchr(begin, LINE_TERMINATOR, chars_of_data));
		std::size_t analyzed_chars = 0;
		while (end != nullptr) {
			auto line_length = std::distance(begin, end) + 1;
			if (should_stay(begin, line_length)) {
				auto compressed_max_line_length = compressor.get_max_compressed_size(line_length);
				if (current_chunk->compressed_data.size() - current_chunk->compressed_chars_available < compressed_max_line_length) {
					chunks.push_back(compressor.get_chunk(current_chunk->first_char_number + current_chunk->uncompressed_chars_available, DEFAULT_UNCOMPRESSED_CHARS_PER_CHUNK));
					current_chunk->compressed_data.resize(current_chunk->compressed_chars_available);
					current_chunk->compressed_data.shrink_to_fit();
					current_chunk = &chunks.back();
					compressor.reset_stream();
				}

				auto destination = current_chunk->compressed_data.data() + current_chunk->compressed_chars_available;
				auto capacity = current_chunk->compressed_data.size() - current_chunk->compressed_chars_available;

				auto compressed_chars = compressor.stream_compress(begin, destination, line_length, capacity);
				assert(compressed_chars > 0);

				current_chunk->compressed_chars_available += compressed_chars;
				current_chunk->uncompressed_chars_available += line_length;				

				current_chunk->compressed_block_sizes.push_back(compressed_chars);
				current_chunk->uncompressed_block_sizes.push_back(line_length);
			}

			analyzed_chars += line_length;
			begin = end + 1;
			end = reinterpret_cast<const char*>(std::memchr(begin, LINE_TERMINATOR, chars_of_data - analyzed_chars));
		}

		assert(analyzed_chars == chars_of_data);
		compressor.preserve_dictionaries();
	}

	grepping_data_sink::grepping_data_sink(std::unique_ptr<re2::RE2>&& regex): regex(std::move(regex)) {}

	bool grepping_data_sink::should_stay(const char* start, std::size_t size) {
		auto bufferPiece = re2::StringPiece(reinterpret_cast<const char*>(start), size);
		return regex->Match(bufferPiece, 0, size, re2::RE2::UNANCHORED, &bufferPiece, 1);
	}
}