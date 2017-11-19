#include <executor/application.hpp>

#include <datasource/data_source.hpp>

namespace executor
{
	class data_counter : public datasource::data_sink {
		virtual void consume(std::pair<datasource::content::entry_iterator, datasource::content::entry_iterator>) {
			
		}
	};


	int whole_application::run()
	{
		return 0;
	}
}