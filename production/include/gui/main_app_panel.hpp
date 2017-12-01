#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include <gui/widget.hpp>
#include <datasource/data_sink.hpp>

#include <datasource/data_view.hpp>

namespace gui {
	const std::size_t BYTES_PER_VIEW = 512 * 1024;

	class main_app_panel : public gui::widget, public datasource::data_sink {
	public:
		main_app_panel(nana::form& main_app_panel);
		virtual nana::widget& get_widget();

		void set_view(std::unique_ptr<datasource::data_view>&&);

	protected:
		virtual bool main_app_panel::should_stay(const char* start, std::size_t size);

	private:
		nana::textbox main_panel;

		std::unique_ptr<datasource::data_view> view;
		std::size_t current_location;
	};
}