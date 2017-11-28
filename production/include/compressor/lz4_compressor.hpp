#pragma once

#include <memory>

#include <lz4.h>

#include <compressor/compressor_impl.hpp>

namespace compressor {
	namespace lz4 {
		class lz4_compressor : public ::compressor::compressor::impl {
		public:
			lz4_compressor();
			virtual std::size_t compress_into_chunk(uncompressed_block* block, chunk* destination);
			virtual std::size_t decompress_chunk(chunk* c, uncompressed_chunk* destination);
			virtual std::size_t get_max_compressed_size(std::size_t char_count);
			virtual void reset_stream();
			virtual void preserve_dictionaries();
		private:
			std::unique_ptr<LZ4_stream_t, void(*)(LZ4_stream_t*)> encode_stream;
			std::unique_ptr<LZ4_streamDecode_t, void(*)(LZ4_streamDecode_t*)> decode_stream;
			std::unique_ptr<char[]> dictionary;
		};
	}
}