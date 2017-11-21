#pragma once

#include <memory>
#include <deque>
#include <boost/filesystem.hpp>
#include <re2/re2.h>


namespace datasource {
	class content {
	public:
		typedef std::pair<const char*, std::size_t> entry;
		typedef std::deque<entry> entry_container;
		typedef std::vector<std::size_t> entries_journal;

		entries_journal add_data(const char* buffer, std::size_t byte_count);
		entry_container& get_rows();

	private:
		entry_container content_rows;
		std::deque<std::string> source_content;
		std::string carry;
	};

	class data_sink {
	public:
		typedef std::pair<content::entries_journal::const_iterator, content::entries_journal::const_iterator> entries_range;

		void consume(const entries_range&, const content::entry_container&);

		void add_child(const std::shared_ptr<data_sink>& child);

	protected:
		virtual entries_range do_consume(const entries_range& entries_to_analyze, const content::entry_container&) = 0;
		content::entries_journal entries;

	private:
		std::vector<std::weak_ptr<data_sink> > children;
	};

	class grepping_data_sink : public data_sink {
	public:
		grepping_data_sink(std::unique_ptr<re2::RE2>&& regex);

		virtual entries_range do_consume(const entries_range& entries_to_analyze, const content::entry_container&);

	private:
		std::unique_ptr<re2::RE2> regex;
	};

	class data_source {
	public:
		virtual void readfile(const boost::filesystem::path& p, const std::shared_ptr<data_sink>& sink);
	};

}
