#include <chrono>

#include <executor/application.hpp>

#include <datasource/data_source.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace executor
{
	class data_counter : public datasource::data_sink {
	public:		
		data_counter(std::string num): num(num),console(spdlog::stdout_color_mt(num.c_str())), count(0), started_at(std::chrono::steady_clock::now()) {}

		virtual boost::optional<datasource::data_sink::entry_range> do_consume(const datasource::data_sink::entry_range& r) {
			console->info("{3:s}: Got {0:d} log rows to analyze in batch {1:d}. Time since startup: {2:d} us", r.second - r.first, count, (-started_at.time_since_epoch() + std::chrono::steady_clock::now().time_since_epoch()).count() / 1000, num);
			count++;
			return boost::optional<datasource::data_sink::entry_range>(r);
		}

	private:
		std::string num;
		std::shared_ptr<spdlog::logger> console;
		int count;
		std::chrono::steady_clock::time_point started_at;
	};


	int whole_application::run()
	{
		auto path = boost::filesystem::path("I:/Freelance/Octant/agv-supervisor/target/logs/Full started up system when/initialized properly should/Full started up system when initialized properly should send vehicle to handle task.log");
		auto sink = std::make_shared<data_counter>("1");
		auto grepper = std::make_shared<datasource::grepping_data_sink>(std::make_unique<re2::RE2>("INFO"));
		auto sink2 = std::make_shared<data_counter>("2");
		sink->add_child(grepper);
		grepper->add_child(sink2);

		datasource::data_source().readfile(path, sink);
		return 0;
	}
}