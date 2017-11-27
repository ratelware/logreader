#include <gui/main_window.hpp>

#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/panel.hpp>

#include <datasource/data_source.hpp>

namespace {
	const std::string APP_NAME = "Logreader";
	const std::string APP_VERSION = "0.2";
	const std::string ICON_PATH = "I:/Ratelware/logreader/assets/icon.ico";
	
	void setup_visual_identification(nana::form& window) {
		window.caption(APP_NAME + " " + APP_VERSION);
		nana::API::window_icon(window.handle(), nana::paint::image(ICON_PATH));
	}
}

namespace gui {
	main_window::main_window(): place(window_form), menu(window_form, *this), main(std::make_shared<gui::main_app_panel>(window_form)), status(window_form) {
		place.div("vert<menubar weight=28><mainapp><statusbar weight=28>");

		setup_visual_identification(window_form);
		place.field("menubar") << menu.get_widget();
		place.field("mainapp") << main->get_widget();
		place.field("statusbar") << status.get_widget();
		place.collocate();
	}

	void main_window::analyze_file(const std::string& filename) {
		auto content = datasource::data_source().readfile(boost::filesystem::path(filename));
		content->get_sink()->add_child(main);
	}

	void main_window::show() {
		window_form.show();
		nana::exec();
	}
}