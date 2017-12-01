#include <numeric>

#include <gui/main_app_panel.hpp>

#include <nana/gui/filebox.hpp>

namespace gui {

	main_app_panel::main_app_panel(nana::form& window_form) : main_panel(window_form) {
		main_panel.editable(false);
		main_panel.enable_caret();
	}

	nana::widget& main_app_panel::get_widget()
	{
		return main_panel;
	}

	void main_app_panel::set_view(std::unique_ptr<datasource::data_view>&& v)
	{
		view.swap(v);
		current_location = 0;
		auto start = current_location <= BYTES_PER_VIEW / 2 ? 0 : current_location - BYTES_PER_VIEW / 2;
		auto chunks = view->get_bytes(start, current_location + BYTES_PER_VIEW / 2);
		main_panel.reset();
		auto total_size = std::accumulate(chunks.begin(), chunks.end(), 0, [](std::size_t acc, std::shared_ptr<compressor::uncompressed_chunk>& c) { return acc + c->data.size(); });

		std::string content;
		content.resize(total_size);
		std::size_t written = 0;
		for (auto& c : chunks) {
			std::memcpy(&content[0], c->data.data() + written, c->data.size());
			written += c->data.size();
		}

		auto visible_start = chunks.front()->first_char_number;
		auto visible_end = chunks.back()->first_char_number;
		main_panel.append(std::move(content), false);
		main_panel.events().text_exposed([this, &visible_start, &visible_end] (const nana::arg_textbox& box) {
			auto start = box.text_position[0].y;


			return box.stop_propagation();
		});
	}

	bool main_app_panel::should_stay(const char* start, std::size_t size) {
		return false;
	}

}
