#include <numeric>

#include <datasource/data_view.hpp>

namespace datasource {
	data_view::~data_view() {}

	compressed_view::compressed_view(std::deque<compressor::chunk>& c): chunks(c) {}

	data_view::chunks compressed_view::get_bytes(std::size_t start, std::size_t end) {
		auto first = chunks.begin();
		while (first->first_char_number < start) ++first;

		if (first == chunks.end()) {
			return data_view::chunks();
		}

		auto last = first;
		while (last != chunks.end() && last->first_char_number + last->written_uncompressed_chars < end) ++last;
		
		std::size_t total_size = std::accumulate(first, last, 0, [](std::size_t acc, compressor::chunk& val) { return acc + val.written_uncompressed_chars; });

		data_view::chunks items;
		/*for (auto i = first; i != last; ++i) {
			if()
		}*/

		auto ptr = std::make_shared<compressor::uncompressed_chunk>(
			compressor::uncompressed_chunk{ first->first_char_number, std::vector<char>(chunks[0].written_uncompressed_chars), chunks[0].block_numbers }
		);
		compressor.decompress_chunk(&chunks[0], ptr.get());
		items.push_back(ptr);
		return items;
	}
}