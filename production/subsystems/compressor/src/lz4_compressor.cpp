#pragma once

#include <memory>
#include <limits>
#include <cassert>

#include <lz4.h>

#include <compressor/compressor.hpp>
#include <compressor/lz4_compressor.hpp>

namespace {
	const std::size_t MAX_CHARS_PER_STREAM_CALL = std::numeric_limits<uint16_t>::max();

	std::size_t get_uncompressed_block_size(std::size_t chars_available) {
		return (chars_available > MAX_CHARS_PER_STREAM_CALL) ? MAX_CHARS_PER_STREAM_CALL : chars_available;
	}
}

namespace compressor {
	namespace lz4 {
		const int COMPRESSION_LEVEL = 1;
		const auto SIZE_OF_CHAR_COUNT_IN_BLOCK = sizeof(uint16_t);
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

		std::size_t lz4_compressor::compress_into_chunk(uncompressed_block* block, chunk* destination) {
			std::size_t written_compressed_chars = 0;
			std::size_t written_uncompressed_chars = 0;
			char* target = destination->compressed_data.data() + destination->written_compressed_chars;
			while (written_uncompressed_chars < block->size) {
				std::size_t current_block_size = get_uncompressed_block_size(block->size - written_uncompressed_chars);
				written_compressed_chars += LZ4_compress_fast_continue(
					encode_stream.get(), 
					block->start + written_uncompressed_chars, 
					target + written_compressed_chars, 
					current_block_size,
					destination->compressed_data.size() - destination->written_compressed_chars - written_compressed_chars, 
					COMPRESSION_LEVEL
				);
				written_uncompressed_chars += current_block_size;
			}

			destination->written_compressed_chars += written_compressed_chars;
			destination->written_uncompressed_chars += written_uncompressed_chars;

			destination->compressed_block_sizes.push_back(written_compressed_chars);
			destination->uncompressed_block_sizes.push_back(written_uncompressed_chars);
			destination->block_numbers->push_back(block->number);

			return written_compressed_chars;
		}

		std::size_t lz4_compressor::decompress_chunk(chunk* c, uncompressed_chunk* destination) {
			assert(c->compressed_block_sizes.size() == c->uncompressed_block_sizes.size());
			
			LZ4_setStreamDecode(decode_stream.get(), nullptr, 0);
			std::size_t written_uncompressed_chars = 0;
			std::size_t read_compressed_chars = 0;

			for (std::size_t block_number = 0; block_number < c->compressed_block_sizes.size(); ++block_number) {
				uint16_t chars_to_read = c->compressed_block_sizes[block_number];
				auto read_chars = LZ4_decompress_safe_continue(
					decode_stream.get(),
					c->compressed_data.data() + read_compressed_chars,
					destination->data.data() + written_uncompressed_chars,
					chars_to_read,
					c->written_uncompressed_chars - written_uncompressed_chars
				);

				assert(read_chars == c->uncompressed_block_sizes[block_number]);
				read_compressed_chars += chars_to_read;
				written_uncompressed_chars += read_chars;

			}

			assert(written_uncompressed_chars == c->written_uncompressed_chars);
			assert(read_compressed_chars == c->written_compressed_chars);

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
