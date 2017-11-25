#pragma once

#include <vector>

namespace compressor {
	struct chunk {
		std::size_t first_byte_number;
		std::size_t uncompressed_bytes_available;
		std::size_t compressed_bytes_available;
		std::vector<char> compressed_data;
	};
}