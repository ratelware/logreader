#pragma once

#include <memory>

#include <boost/filesystem/path.hpp>

#include <datasource/content.hpp>

namespace datasource {
	class data_source {
	public:
		virtual std::shared_ptr<content> readfile(const boost::filesystem::path& p);
	};

}
