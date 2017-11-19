#include <datasource/data_source.hpp>
#include <fstream>

namespace datasource
{
	void data_source::readfile(boost::filesystem::path p, std::shared_ptr<data_sink> sink) {
		const int BLOCK_SIZE = 1024 * 64;
		
		auto ptr = std::make_shared<content>();

		std::ifstream s(p.generic_string());
		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(BLOCK_SIZE);
		while (!s.eof()) {
			s.read(buffer.get(), BLOCK_SIZE);
			auto bytes_read = s.gcount();	
			auto added_entries = ptr->add_data(buffer.get(), bytes_read);
			
			if (added_entries.is_initialized()) {
				sink->consume(added_entries.value());
			}
		}
	}
	
	// consider multi-byte tokens!
	boost::optional<std::pair<content::entry_iterator, content::entry_iterator>> content::add_data(const char* buffer, std::size_t byte_count) {
		const char LINE_TERMINATOR = '\n';

		const char* after_last_token = buffer + byte_count;
		const char* last_start_of_row = after_last_token;
		while (--last_start_of_row >= buffer && *last_start_of_row != LINE_TERMINATOR);
				
		if (last_start_of_row == buffer) {
			carry.append(buffer, byte_count);
			return boost::optional<std::pair<content::entry_iterator, content::entry_iterator>>();
		}
		else {
			std::size_t bytes_to_carry = after_last_token - last_start_of_row;
			carry.assign(last_start_of_row, bytes_to_carry);

			auto analyzed = datatype(carry).append(buffer, byte_count - bytes_to_carry);
			auto beginning = analyzed.cbegin();
			auto size_before = content_rows.size();
			for (auto i = analyzed.cbegin(); i < analyzed.cend(); ++i) {
				if (*i == LINE_TERMINATOR) {
					content_rows.push_back(std::make_pair(beginning, i));
					beginning = i + 1;
				}
			}

			source_content.emplace_back(std::move(analyzed));
			return boost::make_optional(std::make_pair(content_rows.cbegin() + size_before, content_rows.cend()));
		}
	}
}