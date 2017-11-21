#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/menubar.hpp>

#include <gui/action_handler.hpp>
#include <gui/widget.hpp>

namespace gui {
	class menu_bar : public gui::widget {
	public:
		menu_bar(nana::form& window_form, action_handler& handler);
		virtual nana::widget& get_widget();

	private:

		void create_top_menu();
		nana::menubar bar;
		action_handler& handler;
	};
}