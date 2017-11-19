#pragma once

#include <memory>
#include <deque>
#include <boost/filesystem.hpp>

namespace datasource {
	typedef std::string datatype;

	class content {
	public:
		std::deque<std::pair<datatype::const_iterator, datatype::const_iterator> > content_rows;

	private:
		std::deque<datatype> source_content;
	};

	class data_sink {
		void consume() {}
	};

	class data_source {
		virtual data_sink readfile(boost::filesystem::path p);
	};

}
