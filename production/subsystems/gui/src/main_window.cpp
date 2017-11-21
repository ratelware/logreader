#include <gui/main_window.hpp>

#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/panel.hpp>

namespace {
	const std::string APP_NAME = "Logreader";
	const std::string APP_VERSION = "0.2";
	const std::string ICON_PATH = "I:/Ratelware/logreader/assets/icon.ico";

	namespace menu {
		const std::string FILE_ENTRY_NAME = "File";
		const std::string FILE_ENTRY_SHORTCUT = "&F";
		const std::string FILE_ENTRY = FILE_ENTRY_NAME + "(" + FILE_ENTRY_SHORTCUT + ")";

		namespace file {
			const std::string OPEN_ENTRY = "Open";
			const std::string EXIT_ENTRY = "Exit";
		}
	}

	void create_top_menu(nana::menubar& bar) {
		auto& file_menu = bar.push_back(menu::FILE_ENTRY);
		file_menu.append(menu::file::OPEN_ENTRY, [&bar](nana::drawerbase::menu::menu_item_type::item_proxy& p) {
			nana::filebox fbox(bar.handle(), true);
			fbox.show();
		});
		file_menu.append_splitter();
		file_menu.append(menu::file::EXIT_ENTRY);

		bar.show();
	}

	void setup_visual_identification(nana::form& window) {
		window.caption(APP_NAME + " " + APP_VERSION);
		nana::API::window_icon(window.handle(), nana::paint::image(ICON_PATH));
	}
}

namespace gui {
	void main_window::show() {
		nana::form window;
		nana::place place;
		nana::menubar bar(window.handle());
		place.div("vert<menubar weight=30><mainapp><statusbar weight=30>");
		setup_visual_identification(window);
		create_top_menu(bar);
		place.bind(window);
		place.field("menubar") << bar;
		
		nana::panel<true> main_panel(window);

		main_panel.bgcolor(nana::colors::yellow);
		place.field("mainapp") << main_panel;

		nana::panel<true> status_bar(window);
		status_bar.bgcolor(nana::colors::red);
		place.field("statusbar") << status_bar;
		window.show();
		place.collocate();
		/*
		nana::label label(window, nana::rectangle(10, 10, 100, 100));
		label.caption("Welcome NANA");
		*/
		nana::exec();
	}
}