#pragma once

#include <string>

namespace gui {
	class action_handler {
	public:
		virtual void analyze_file(const std::string& path) = 0;
	};
}