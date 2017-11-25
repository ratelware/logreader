#pragma once

#include <memory>

#include <lizard_common.h>
#include <lizard_compress.h>
#include <lizard_decompress.h>

#include <compressor/compressor_impl.hpp>

namespace compressor {
	namespace lizard {
		class encode_stream_deallocator {
		public:
			void operator()(Lizard_stream_t* ptr);
		};

		class decode_stream_deallocator {
		public:
			void operator()(Lizard_streamDecode_t* ptr);
		};

		class lizard_compressor : public ::compressor::compressor::impl {
		public:
			lizard_compressor();
			virtual std::size_t stream_compress(const char* source, char* destination, std::size_t uncompressed_size, std::size_t compressed_capacity);
			virtual std::size_t decompress_chunk(chunk* c, char* destination);
			virtual std::size_t get_max_compressed_size(std::size_t byte_count);

		private:
			std::unique_ptr<Lizard_stream_t, lizard::encode_stream_deallocator> encode_stream;
			std::unique_ptr<Lizard_streamDecode_t, lizard::decode_stream_deallocator> decode_stream;
		};
	}
}