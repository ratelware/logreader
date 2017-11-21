#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>

#include <gui/action_handler.hpp>
#include <gui/menu_bar.hpp>
#include <gui/main_app_panel.hpp>
#include <gui/status_bar.hpp>

namespace gui {
	class main_window : public gui::action_handler {
	public:
		main_window();
		void show();

		virtual void analyze_file(const std::string& filename);
	private:
		nana::form window_form;
		nana::place place;
		gui::menu_bar menu;
		std::shared_ptr<gui::main_app_panel> main;
		gui::status_bar status;
	};
}