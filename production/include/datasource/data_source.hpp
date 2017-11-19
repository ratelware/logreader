#pragma once

#include <memory>
#include <deque>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

namespace datasource {
	typedef std::string datatype;

	class content {
	public:
		typedef std::pair<datatype::const_iterator, datatype::const_iterator> entry;
		typedef std::deque<entry>::const_iterator entry_iterator;

		boost::optional<std::pair<entry_iterator, entry_iterator>> add_data(const char* buffer, std::size_t byte_count);

	private:
		std::deque<entry> content_rows;
		std::deque<datatype> source_content;
		datatype carry;
	};

	class data_sink {
	public:
		virtual void consume(std::pair<content::entry_iterator, content::entry_iterator>) = 0;
	};

	class data_source {
		virtual void readfile(boost::filesystem::path p, std::shared_ptr<data_sink> sink);
	};

}
