#include <datasource/data_source.hpp>

#include <fstream>
#include <algorithm>
#include <set>
#include <numeric>

#include <re2/stringpiece.h>

namespace datasource
{
	const char LINE_TERMINATOR = '\n';

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
				sink->consume(data_sink::entries_range(added_entries.begin(), added_entries.end()), ptr->get_rows());
			}
		}

		auto last_entry = ptr->add_data("\n", 1);
		sink->consume(data_sink::entries_range(last_entry.begin(), last_entry.end()), ptr->get_rows());
	}
	
	content::entry_container& content::get_rows() {
		return content_rows;
	}

	// consider multi-byte tokens!
	content::entries_journal content::add_data(const char* buffer, std::size_t byte_count) {

		const char* after_last_token = buffer + byte_count;
		const char* last_start_of_row = after_last_token;
		while (last_start_of_row > buffer && *--last_start_of_row != LINE_TERMINATOR);
				
		if (last_start_of_row == buffer) {
			carry.append(buffer, byte_count);
			return content::entries_journal();
		}
		else {

			std::size_t bytes_to_carry = after_last_token - last_start_of_row;
			auto analyzed = carry.append(buffer, byte_count - bytes_to_carry);
			auto beginning = analyzed.data();
			auto size_before = content_rows.size();
			auto start = analyzed.data();
			auto end = reinterpret_cast<const char*>(std::memchr(start, LINE_TERMINATOR, analyzed.size()));
			auto remaining_size = analyzed.size();
			while (end != nullptr) {
				auto processed_bytes = std::distance(start, end) + 1;
				content_rows.push_back(std::make_pair(start, processed_bytes));
				start = end + 1;
				remaining_size -= processed_bytes;
				end = reinterpret_cast<const char*>(std::memchr(start, LINE_TERMINATOR, remaining_size));
			}

			carry.assign(last_start_of_row, bytes_to_carry);

			source_content.emplace_back(std::move(analyzed));
			entries_journal journal(content_rows.size() - size_before);
			std::iota(journal.begin(), journal.end(), size_before);
			return journal;
		}
	}

	void data_sink::add_child(const std::shared_ptr<data_sink>& child) {
		children.push_back(std::weak_ptr<data_sink>(child));
	}
	
	void data_sink::consume(const entries_range& journal, const content::entry_container& all_entries) {
		auto remaining_entries = do_consume(journal, all_entries);

		for (auto i = children.begin(); i < children.end(); ++i) {
			auto child = (*i).lock();
			if (child.get() != nullptr) {
				child->consume(remaining_entries, all_entries);
			}
		}

		children.erase(std::remove_if(children.begin(), children.end(), [](std::weak_ptr<data_sink> it) { return it.lock().get() == nullptr; }), children.end());
	}

	grepping_data_sink::grepping_data_sink(std::unique_ptr<re2::RE2>&& regex): regex(std::move(regex)) {}

	data_sink::entries_range grepping_data_sink::do_consume(const entries_range& entries_to_analyze, const content::entry_container& all_entries) {
		auto item_count_before = entries.size();
		auto bufferPiece = re2::StringPiece("");
		for (auto i = entries.begin(); i != entries.end(); ++i) {
			auto entry = all_entries[*i];
			auto piece = re2::StringPiece(entry.first, entry.second);
			if (regex->Match(piece, 0, entry.second, re2::RE2::UNANCHORED, &bufferPiece, 1)) {
				entries.push_back(*i);
			};
		}

		return entries_range(entries.begin() + item_count_before, entries.end());
	}
}