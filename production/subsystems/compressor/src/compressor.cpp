#include <compressor/compressor.hpp>
#include <compressor/compressor_impl.hpp>
#include <compressor/lizard_compressor.hpp>

namespace compressor {
	compressor::compressor() : pimpl(std::make_unique<::compressor::lizard::lizard_compressor>()) {}
	compressor::~compressor() {}

	std::size_t compressor::stream_compress(const char* source, char* destination, std::size_t uncompressed_size, std::size_t compressed_capacity) {
		return pimpl->stream_compress(source, destination, uncompressed_size, compressed_capacity);
	}

	std::size_t compressor::decompress_chunk(chunk* c, char* destination) {
		return pimpl->decompress_chunk(c, destination);
	}

	std::size_t compressor::get_max_compressed_size(std::size_t byte_count) {
		return pimpl->get_max_compressed_size(byte_count);
	}

	chunk compressor::get_chunk(std::size_t starting_at, std::size_t byte_count) {
		return chunk{ starting_at, 0, 0, std::vector<char>(get_max_compressed_size(byte_count)) };
	}
}