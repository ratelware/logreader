#include <memory>
#include <executor/configurator.hpp>
#include <executor/application.hpp>

#include <boost/program_options/variables_map.hpp>

namespace executor
{
	configurator::configurator(const boost::program_options::variables_map&)
	{
		app = std::make_unique<executor::whole_application>();
	}

	configurator::~configurator()
	{}

	executor::application& configurator::get_application() const
	{
		return *app;
	}
}
