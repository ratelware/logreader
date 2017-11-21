#pragma once

#include <nana/gui/widgets/widget.hpp>

namespace gui {
	class widget {
	public:
		virtual nana::widget& get_widget() = 0;
	};
}