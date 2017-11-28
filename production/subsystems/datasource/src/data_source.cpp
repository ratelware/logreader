#include <fstream>
#include <algorithm>
#include <set>
#include <numeric>

#include<boost/filesystem.hpp>

#include <re2/stringpiece.h>
#include <spdlog/spdlog.h>


#include <datasource/data_source.hpp>
#include <datasource/data_sink.hpp>
#include <datasource/content.hpp>

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
		std::size_t read_size = (file_size > READ_BLOCK_SIZE) ? READ_BLOCK_SIZE : file_size;

		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(read_size);
		while (!s.eof() && s.good()) {
			s.read(buffer.get(), read_size);
			auto chars_read = s.gcount();	
			if (chars_read > 0) {
				ptr->add_data(buffer.get(), chars_read);
			}
		}

		ptr->end_stream();
		return ptr;
	}
	
	data_source::~data_source(){}

	content::content() : sink(std::make_unique<promiscous_sink>()) {}

	void content::add_data(const char* buffer, std::size_t char_count) {
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
			carry.insert(carry.end(), buffer, buffer + char_count);
			return;
		}

		std::size_t chars_to_carry = after_last_token - last_start_of_row;
		carry.insert(carry.end(), buffer, buffer + char_count - chars_to_carry);
		
		compressor::uncompressed_chunk chunk{ -1, std::move(carry), nullptr };
		sink->consume_raw(&chunk);
		carry.assign(last_start_of_row, last_start_of_row + chars_to_carry);
	}

	void content::end_stream() {
		if (!carry.empty()) {
			carry.push_back(LINE_TERMINATOR);
			compressor::uncompressed_chunk chunk{ -1, std::move(carry), nullptr };
			sink->consume_raw(&chunk);
			carry.clear();
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
		compressor::uncompressed_chunk target{ c->first_char_number, std::vector<char>(c->written_uncompressed_chars), c->block_numbers };
		auto compressed_size = compressor.decompress_chunk(c, &target);
		assert(compressed_size == c->written_compressed_chars);
		consume_raw(&target);
	}

	std::size_t get_block_number(compressor::uncompressed_chunk* chunk, std::size_t analyzed_lines, std::size_t& default_line_number) {
		if (chunk->block_numbers)
			return (*chunk->block_numbers)[analyzed_lines];
		else
			return ++default_line_number;
	}

	void data_sink::consume_raw(compressor::uncompressed_chunk* data) {
		char* begin = data->data.data();
		char* end = reinterpret_cast<char*>(std::memchr(begin, LINE_TERMINATOR, data->data.size()));
		std::size_t analyzed_chars = 0;
		std::size_t analyzed_lines = 0;
		std::size_t default_line_number = get_previous_line_number();
		while (end != nullptr) {
			auto line_length = std::distance(begin, end) + 1;
			if (should_stay(begin, line_length)) {
				compressor::uncompressed_block b{ line_length, begin, get_block_number(data, analyzed_lines, default_line_number) };
				auto current_chunk = get_active_chunk(line_length);
				auto compressed_chars = compressor.compress_into_chunk(&b, current_chunk);
				assert(compressed_chars > 0);
			}

			analyzed_chars += line_length;
			begin = end + 1;
			++analyzed_lines;
			end = reinterpret_cast<char*>(std::memchr(begin, LINE_TERMINATOR, data->data.size() - analyzed_chars));
		}

		assert(analyzed_chars == data->data.size());
		compressor.preserve_dictionaries();
	}
	
	std::size_t data_sink::get_previous_line_number() {
		for (auto i = chunks.rbegin(); i != chunks.rend(); ++i) {
			if (!(i->block_numbers->empty())) {
				return i->block_numbers->back();
			}
		}

		return 0;
	}

	compressor::chunk* data_sink::get_active_chunk(std::size_t bytes_to_compress) {
		auto compressed_max_size = compressor.get_max_compressed_size(bytes_to_compress);
		assert(compressed_max_size < DEFAULT_UNCOMPRESSED_CHARS_PER_CHUNK);
		if (chunks.empty()) {
			chunks.push_back(compressor.get_chunk(0, DEFAULT_UNCOMPRESSED_CHARS_PER_CHUNK));
		}
		else {
			auto& previously_active_chunk = chunks.back();
			if (previously_active_chunk.compressed_data.size() - previously_active_chunk.written_compressed_chars < compressed_max_size) {
				auto next_char_number = compressor::finalize_chunk(previously_active_chunk);
				chunks.push_back(compressor.get_chunk(next_char_number, DEFAULT_UNCOMPRESSED_CHARS_PER_CHUNK));
				compressor.reset_stream();
			}
		}

		return &chunks.back();
	}

	grepping_data_sink::grepping_data_sink(std::unique_ptr<re2::RE2>&& regex): regex(std::move(regex)) {}

	bool grepping_data_sink::should_stay(const char* start, std::size_t size) {
		auto bufferPiece = re2::StringPiece(start, size);
		return regex->Match(bufferPiece, 0, size, re2::RE2::UNANCHORED, &bufferPiece, 1);
	}
}