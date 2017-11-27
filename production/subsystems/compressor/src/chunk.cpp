#include <compressor/chunk.hpp>

namespace compressor {
	std::size_t finalize_chunk(chunk& c) {
		c.compressed_data.resize(c.written_compressed_chars);
		c.compressed_data.shrink_to_fit();

		return c.first_char_number + c.written_uncompressed_chars;
	}
}