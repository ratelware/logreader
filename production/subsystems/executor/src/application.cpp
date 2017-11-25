#include <chrono>

#include <executor/application.hpp>

#include <datasource/data_source.hpp>

#include <gui/main_window.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace executor
{
	int whole_application::run()
	{
/*		auto path = boost::filesystem::path("I:/Freelance/Octant/agv-supervisor/target/logs/Full started up system when/initialized properly should/Full started up system when initialized properly should send vehicle to handle task.log");
		auto sink = std::make_shared<data_counter>("1");
		auto grepper = std::make_shared<datasource::grepping_data_sink>(std::make_unique<re2::RE2>("INFO"));
		auto sink2 = std::make_shared<data_counter>("2");
		sink->add_child(grepper);
		grepper->add_child(sink2);

		datasource::data_source().readfile(path, sink);*/

		gui::main_window().show();
		return 0;
	}
}