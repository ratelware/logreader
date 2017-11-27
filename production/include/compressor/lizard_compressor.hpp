/*#pragma

#include <memory>

#include <lizard_common.h>
#include <lizard_compress.h>
#include <lizard_decompress.h>

#include <compressor/compressor_impl.hpp>

namespace compressor {
	namespace lizard {
		class lizard_compressor : public ::compressor::compressor::impl {
		public:
			lizard_compressor();
			virtual std::size_t compress_into_chunk(const char* source, char* destination, std::size_t uncompressed_size, std::size_t compressed_capacity);
			virtual std::size_t decompress_chunk(chunk* c, char* destination);
			virtual std::size_t get_max_compressed_size(std::size_t char_count);

		private:
			std::unique_ptr<Lizard_stream_t, void(*)(Lizard_stream_t*)> encode_stream;
			std::unique_ptr<Lizard_streamDecode_t, void(*)(Lizard_streamDecode_t*)> decode_stream;
		};
	}
}*/