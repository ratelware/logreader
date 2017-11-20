#include <gui/main_window.hpp>

namespace gui {
	void main_window::show() {
		nana::form window;
		window.caption("Logreader 1.0");
		nana::paint::image icon = nana::paint::image("I:/Ratelware/logreader/assets/icon.ico");
		nana::API::window_icon(window.handle(), icon);
		nana::label label(window, nana::rectangle(10, 10, 100, 100));
		label.caption("Welcome NANA");

		window.show();
		nana::exec();
	}
}