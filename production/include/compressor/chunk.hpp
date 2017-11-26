#pragma once

#include <vector>

namespace compressor {
	struct chunk {
		std::size_t first_char_number;
		std::size_t uncompressed_chars_available;
		std::size_t compressed_chars_available;
		std::vector<char> compressed_data;
		std::vector<uint16_t> compressed_block_sizes;
		std::vector<uint16_t> uncompressed_block_sizes;
	};
}