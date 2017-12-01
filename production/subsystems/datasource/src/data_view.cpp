#include <numeric>
#include <cassert>

#include <datasource/data_view.hpp>

namespace datasource {
	data_view::~data_view() {}

	compressed_view::compressed_view(std::deque<compressor::chunk>& c): chunks(c), local_memory(MAX_CACHE_SIZE) {}

	data_view::chunks compressed_view::get_bytes(std::size_t start, std::size_t end) {
		assert(start < end);
		auto first = chunks.begin();
		while (first->first_char_number < start) ++first;

		if (first == chunks.end()) {
			return data_view::chunks();
		}

		auto last = first;
		while (last != chunks.end()) {
			auto current = last++;
			if (current->first_char_number + current->written_uncompressed_chars >= end) {
				break;
			}
		}
		

		std::size_t total_size = std::accumulate(first, last, 0, [](std::size_t acc, compressor::chunk& val) { return acc + val.written_uncompressed_chars; });

		auto count = std::distance(first, last);
		data_view::chunks items(count);

		// optimization - first go through cached items, then fetch the rest
		for (std::size_t i = 0; i < count; ++i) {
			auto cached = local_memory.get((first + i)->first_char_number);
			items[i] = (cached != nullptr) ? *cached : nullptr;
		}

		for (std::size_t i = 0; i < count; ++i) {
			if (items[i] != nullptr) continue;

			auto ptr = std::make_shared<compressor::uncompressed_chunk>(
				compressor::uncompressed_chunk{ (first + i)->first_char_number, std::vector<char>((first + i)->written_uncompressed_chars), (first + i) -> block_numbers }
			);
			compressor.decompress_chunk(&chunks[i], ptr.get());
			items[i] = ptr;
			local_memory.put((first + i)->first_char_number, ptr);
		}

		return items;
	}
}