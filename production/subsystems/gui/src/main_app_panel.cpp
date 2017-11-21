#include <gui/main_app_panel.hpp>

#include <nana/gui/filebox.hpp>

namespace gui {
	main_app_panel::main_app_panel(nana::form& window_form) : main_panel(window_form) {

	}

	nana::widget& main_app_panel::get_widget()
	{
		return main_panel;
	}

	datasource::data_sink::entries_range main_app_panel::do_consume(const datasource::data_sink::entries_range& entries_to_analyze, const datasource::content::entry_container& all_entries) {

		std::size_t total_size = 0;
		for (auto i = entries_to_analyze.first; i < entries_to_analyze.second; ++i) {
			total_size += all_entries[*i].second;
		}

		std::string result;
		result.reserve(total_size);
		for (auto i = entries_to_analyze.first; i < entries_to_analyze.second; ++i) {
			result.append(all_entries[*i].first, all_entries[*i].second);
		}

		main_panel.append(result, false);
		return entries_to_analyze;
	}

}
