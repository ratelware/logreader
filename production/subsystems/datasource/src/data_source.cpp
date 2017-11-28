#include <fstream>
#include <algorithm>
#include <set>
#include <numeric>

#include<boost/filesystem.hpp>

#include <re2/stringpiece.h>
#include <spdlog/spdlog.h>

#include <datasource/constants.hpp>
#include <datasource/data_source.hpp>
#include <datasource/data_sink.hpp>
#include <datasource/content.hpp>

#include <compressor/compressor.hpp>


namespace datasource
{
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
}