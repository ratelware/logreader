#pragma once

#include <memory>
#include <limits>
#include <cassert>

#include <lz4.h>

#include <compressor/compressor.hpp>
#include <compressor/lz4_compressor.hpp>

namespace compressor {
	namespace lz4 {
		const std::size_t MAX_CHARS_PER_STREAM_CALL = std::numeric_limits<uint16_t>::max();
		const int COMPRESSION_LEVEL = 1;
		const auto SIZE_OF_char_COUNT_IN_BLOCK = sizeof(uint16_t);
		const int LZ4_DICTIONARY_SIZE = 64 * 1024;


		void encode_stream_deallocator(LZ4_stream_t* ptr) {
			LZ4_freeStream(ptr);
		}
		void decode_stream_deallocator(LZ4_streamDecode_t* ptr) {
			LZ4_freeStreamDecode(ptr);
		}

		lz4_compressor::lz4_compressor() :
			encode_stream(LZ4_createStream(), encode_stream_deallocator), 
			decode_stream(LZ4_createStreamDecode(), decode_stream_deallocator), 
			dictionary(std::make_unique<char[]>(LZ4_DICTIONARY_SIZE)) {
		}

		std::size_t lz4_compressor::stream_compress(const char* source, char* destination, std::size_t uncompressed_size, std::size_t compressed_capacity) {

			std::size_t written_compressed_chars = 0;
			auto begin = source;
			uint16_t current_stream_call_size = 0;
			for (std::size_t chars_to_compress = uncompressed_size; chars_to_compress > 0; chars_to_compress -= current_stream_call_size) {
				current_stream_call_size = (uncompressed_size > MAX_CHARS_PER_STREAM_CALL) ? MAX_CHARS_PER_STREAM_CALL : uncompressed_size;
				auto written = LZ4_compress_fast_continue(encode_stream.get(), begin, reinterpret_cast<char*>(destination + written_compressed_chars), current_stream_call_size, compressed_capacity, 1);
				written_compressed_chars += written;
				begin += current_stream_call_size;
			}

			return written_compressed_chars;

		}

		std::size_t lz4_compressor::decompress_chunk(chunk * c, char* destination) {
			reset_stream();
			char* data = c->compressed_data.data();
			std::size_t written_uncompressed_chars = 0;
			std::size_t read_compressed_chars = 0;

			uint16_t chars_to_read;
			uint16_t chars_in_block;
			std::size_t block_number = 0;
			while (written_uncompressed_chars < c->uncompressed_chars_available) {
				chars_to_read = c->compressed_block_sizes[block_number];
				chars_in_block = c->uncompressed_block_sizes[block_number];

				auto read_chars = LZ4_decompress_safe_continue(
					decode_stream.get(),
					data + read_compressed_chars,
					destination + written_uncompressed_chars,
					chars_to_read,
					c->uncompressed_chars_available - written_uncompressed_chars
				);

				assert(read_chars == chars_in_block);
				read_compressed_chars += chars_to_read;
				written_uncompressed_chars += chars_in_block;

				++block_number;
			}

			assert(written_uncompressed_chars == c->uncompressed_chars_available);
			assert(read_compressed_chars == c->compressed_chars_available);

			return read_compressed_chars;
		}

		std::size_t lz4_compressor::get_max_compressed_size(std::size_t char_count) {
			return LZ4_compressBound(char_count);
		}

		void lz4_compressor::reset_stream() {
			LZ4_resetStream(encode_stream.get());
			LZ4_setStreamDecode(decode_stream.get(), nullptr, 0);
		}

		void lz4_compressor::preserve_dictionaries() {
			LZ4_saveDict(encode_stream.get(), dictionary.get(), LZ4_DICTIONARY_SIZE);
		}

	}
}
