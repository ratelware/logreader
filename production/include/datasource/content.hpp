#pragma once

#include <memory>
#include <vector>

namespace datasource {
	class data_sink;

	class content {
	public:
		content();

		void add_data(const char* buffer, std::size_t char_count);
		data_sink* get_sink();
		void end_stream();
	private:
		std::unique_ptr<data_sink> sink;
		std::vector<char> carry;
	};
}