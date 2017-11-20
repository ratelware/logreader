#include <datasource/data_source.hpp>

#include <fstream>
#include <algorithm>
#include <set>

namespace datasource
{
	void data_source::readfile(const boost::filesystem::path& p, const std::shared_ptr<data_sink>& sink) {
		const int BLOCK_SIZE = 1024 * 256;
		
		auto ptr = std::make_shared<content>();
		auto path = p.string();
		std::ifstream s(path);
		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(BLOCK_SIZE);
		while (!s.eof() && s.good()) {
			s.read(buffer.get(), BLOCK_SIZE);
			auto bytes_read = s.gcount();	
			if (bytes_read > 0) {
				auto added_entries = ptr->add_data(buffer.get(), bytes_read);

				if (added_entries.is_initialized()) {
					sink->consume(added_entries.value());
				}
			}
		}
	}
	
	// consider multi-byte tokens!
	boost::optional<data_sink::entry_range> content::add_data(const char* buffer, std::size_t byte_count) {
		const char LINE_TERMINATOR = '\n';

		const char* after_last_token = buffer + byte_count;
		const char* last_start_of_row = after_last_token;
		while (last_start_of_row > buffer && *--last_start_of_row != LINE_TERMINATOR);
				
		if (last_start_of_row == buffer) {
			carry.append(buffer, byte_count);
			return boost::optional<std::pair<content::entry_iterator, content::entry_iterator>>();
		}
		else {

			std::size_t bytes_to_carry = after_last_token - last_start_of_row;
			auto analyzed = datatype(carry).append(buffer, byte_count - bytes_to_carry);
			auto beginning = analyzed.cbegin();
			auto size_before = content_rows.size();
			auto start = analyzed.cbegin();
			// if this becomes too slow, use std::memchr
			auto end = std::find(start, analyzed.cend(), LINE_TERMINATOR);
			while (end != analyzed.cend()) {
				content_rows.push_back(std::make_pair(start, end));
				start = end + 1;
				end = std::find(start, analyzed.cend(), LINE_TERMINATOR);
			}

			carry.assign(last_start_of_row, bytes_to_carry);

			source_content.emplace_back(std::move(analyzed));
			return boost::make_optional(std::make_pair(content_rows.cbegin() + size_before, content_rows.cend()));
		}
	}

	void data_sink::add_child(const std::shared_ptr<data_sink>& child) {
		children.push_back(std::weak_ptr<data_sink>(child));
	}
	
	void data_sink::consume(const entry_range& r) {
		auto remaining_entries = do_consume(r);
		if (!remaining_entries.is_initialized())
			return;

		for (auto i = children.begin(); i < children.end(); ++i) {
			auto child = (*i).lock();
			if (child.get() != nullptr) {
				child->consume(remaining_entries.value());
			}
		}

		children.erase(std::remove_if(children.begin(), children.end(), [](std::weak_ptr<data_sink> it) { return it.lock().get() == nullptr; }), children.end());
	}

	grepping_data_sink::grepping_data_sink(const boost::regex& regex): regex(regex) {}

	boost::optional<data_sink::entry_range> grepping_data_sink::do_consume(const data_sink::entry_range& items) {
		auto item_count_before = entries.size();
		for (auto i = items.first; i != items.second; ++i) {
			if (boost::regex_search(i->first, i->second, regex)) {
				entries.push_back(*i);
			};
		}

		auto item_count_after = entries.size();
		if (item_count_before == item_count_after) {
			return boost::optional<data_sink::entry_range>();
		}
		else {
			return boost::optional<data_sink::entry_range>(std::make_pair(entries.begin() + item_count_before, entries.end()));
		}
	}
}