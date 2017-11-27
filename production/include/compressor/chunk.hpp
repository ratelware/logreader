#pragma once

#include <vector>

namespace compressor {
	struct chunk {
		std::size_t first_char_number;
		std::size_t written_uncompressed_chars;
		std::size_t written_compressed_chars;
		std::vector<char> compressed_data;
		std::vector<uint16_t> compressed_block_sizes;
		std::vector<uint16_t> uncompressed_block_sizes;
	};

	std::size_t finalize_chunk(chunk&);
}