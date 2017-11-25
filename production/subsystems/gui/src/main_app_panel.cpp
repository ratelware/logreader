#include <gui/main_app_panel.hpp>

#include <nana/gui/filebox.hpp>

namespace gui {
	main_app_panel::main_app_panel(nana::form& window_form) : main_panel(window_form) {

	}

	nana::widget& main_app_panel::get_widget()
	{
		return main_panel;
	}

	bool main_app_panel::should_stay(const char* start, std::size_t size) {
		return true;
	}

}
