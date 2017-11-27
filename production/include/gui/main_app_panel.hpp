#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include <gui/widget.hpp>
#include <datasource/data_sink.hpp>

namespace gui {
	class main_app_panel : public gui::widget, public datasource::data_sink {
	public:
		main_app_panel(nana::form& main_app_panel);
		virtual nana::widget& get_widget();

	protected:
		virtual bool main_app_panel::should_stay(const char* start, std::size_t size);

	private:
		nana::textbox main_panel;
	};
}