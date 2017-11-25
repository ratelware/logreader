#pragma once

#include <memory>

#include <compressor/chunk.hpp>

namespace compressor {
	class compressor {
	public:
		class impl;
		compressor();

		std::size_t stream_compress(const char* source, char* destination, std::size_t uncompressed_size, std::size_t compressed_capacity);
		std::size_t decompress_chunk(chunk*, char* destination);

		std::size_t get_max_compressed_size(std::size_t byte_count);
		chunk get_chunk(std::size_t starting_at, std::size_t byte_count);

		~compressor();
	private:
		std::unique_ptr<impl> pimpl;
	};
}