#include <gui/menu_bar.hpp>

#include <nana/gui/filebox.hpp>


namespace {
	namespace menu {
		const std::string FILE_ENTRY_NAME = "File";
		const std::string FILE_ENTRY_SHORTCUT = "&F";
		const std::string FILE_ENTRY = FILE_ENTRY_NAME + "(" + FILE_ENTRY_SHORTCUT + ")";

		namespace file {
			const std::string OPEN_ENTRY = "Open";
			const std::string EXIT_ENTRY = "Exit";
		}
	}
}

namespace gui {
	menu_bar::menu_bar(nana::form& window_form, action_handler& h): bar(window_form), handler(h) {
		create_top_menu();
	}

	void menu_bar::create_top_menu() {
		auto& file_menu = bar.push_back(menu::FILE_ENTRY);
		file_menu.append(menu::file::OPEN_ENTRY, [this](nana::drawerbase::menu::menu_item_type::item_proxy& p) {
			nana::filebox fbox(bar.handle(), true);
			if (fbox.show()) {
				handler.analyze_file(fbox.file());
			};
		});
		file_menu.append_splitter();
		file_menu.append(menu::file::EXIT_ENTRY, [](nana::drawerbase::menu::menu_item_type::item_proxy& p) { nana::API::exit(); });

		bar.show();
	}

	nana::widget& menu_bar::get_widget()
	{
		return bar;
	}
}
