#pragma once

#include <compressor/compressor.hpp>

namespace compressor {
	class compressor::impl {
	public:
		virtual std::size_t stream_compress(const char* source, char* destination, std::size_t uncompressed_size, std::size_t compressed_capacity) = 0;
		virtual std::size_t decompress_chunk(chunk* c, char* destination) = 0;
		virtual std::size_t get_max_compressed_size(std::size_t char_count) = 0;		

		virtual void preserve_dictionaries() = 0;
		virtual void reset_stream() = 0;

		virtual ~impl(){}
	};
}
