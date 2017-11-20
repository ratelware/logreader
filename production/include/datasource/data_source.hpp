#pragma once

#include <memory>
#include <deque>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <re2/re2.h>


namespace datasource {
	typedef std::string datatype;

	class content {
	public:
		typedef std::pair<datatype::const_iterator, datatype::const_iterator> entry;
		typedef std::deque<entry> entry_container;
		typedef entry_container::const_iterator entry_iterator;

		boost::optional<std::pair<content::entry_iterator, content::entry_iterator>> add_data(const char* buffer, std::size_t byte_count);

	private:
		entry_container content_rows;
		std::deque<datatype> source_content;
		datatype carry;
	};

	class data_sink {
	public:
		typedef std::pair<content::entry_iterator, content::entry_iterator> entry_range;

		void consume(const entry_range&);

		void add_child(const std::shared_ptr<data_sink>& child);

	protected:
		virtual boost::optional<entry_range> do_consume(const entry_range&) = 0;
		content::entry_container entries;

	private:
		std::vector<std::weak_ptr<data_sink> > children;
	};

	class grepping_data_sink : public data_sink {
	public:
		grepping_data_sink(std::unique_ptr<re2::RE2>&& regex);

		virtual boost::optional<entry_range> do_consume(const entry_range&);

	private:
		std::unique_ptr<re2::RE2> regex;
	};

	class data_source {
	public:
		virtual void readfile(const boost::filesystem::path& p, const std::shared_ptr<data_sink>& sink);
	};

}
