#include <gui/status_bar.hpp>

#include <nana/gui/filebox.hpp>

namespace gui {
	status_bar::status_bar(nana::form& window_form) : bar_panel(window_form) {
		bar_panel.bgcolor(nana::colors::red);
	}

	nana::widget& status_bar::get_widget()
	{
		return bar_panel;
	}
}
