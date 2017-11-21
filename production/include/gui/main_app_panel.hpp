#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include <gui/widget.hpp>
#include <datasource/data_source.hpp>

namespace gui {
	class main_app_panel : public gui::widget, public datasource::data_sink {
	public:
		main_app_panel(nana::form& main_app_panel);
		virtual nana::widget& get_widget();

	protected:
		virtual datasource::data_sink::entries_range main_app_panel::do_consume(const datasource::data_sink::entries_range& entries_to_analyze, const datasource::content::entry_container& all_entries);

	private:
		nana::textbox main_panel;
	};
}