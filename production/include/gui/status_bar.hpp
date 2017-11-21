#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>

#include <gui/widget.hpp>

namespace gui {
	class status_bar : public gui::widget {
	public:
		status_bar(nana::form& window_form);
		virtual nana::widget& get_widget();

	private:
		nana::panel<true> bar_panel;
	};
}