#pragma once

#include <memory>
#include <limits>
#include <cassert>

#include <lizard_common.h>
#include <lizard_compress.h>
#include <lizard_decompress.h>

#include <compressor/compressor.hpp>
#include <compressor/lizard_compressor.hpp>

namespace compressor {
	namespace lizard {
		const std::size_t MAX_BYTES_PER_STREAM_CALL = std::numeric_limits<uint16_t>::max();
		const int COMPRESSION_LEVEL = 1;
		const auto SIZE_OF_BYTE_COUNT_IN_BLOCK = sizeof(uint16_t);

		void encode_stream_deallocator::operator()(Lizard_stream_t* ptr) {
			Lizard_freeStream(ptr);
		}
		void decode_stream_deallocator::operator()(Lizard_streamDecode_t* ptr) {
			Lizard_freeStreamDecode(ptr);
		}

		lizard_compressor::lizard_compressor() : encode_stream(Lizard_createStream(COMPRESSION_LEVEL)), decode_stream(Lizard_createStreamDecode()) {}

		std::size_t lizard_compressor::stream_compress(const char* source, char* destination, std::size_t uncompressed_size, std::size_t compressed_capacity) {

			std::size_t written_compressed_bytes = 0;
			auto begin = source;
			uint16_t current_stream_call_size = 0;
			for (std::size_t bytes_to_compress = uncompressed_size; bytes_to_compress > 0; bytes_to_compress -= current_stream_call_size) {
				current_stream_call_size = (uncompressed_size > MAX_BYTES_PER_STREAM_CALL) ? MAX_BYTES_PER_STREAM_CALL : uncompressed_size;
				std::memcpy(destination, &current_stream_call_size, SIZE_OF_BYTE_COUNT_IN_BLOCK);
				written_compressed_bytes += SIZE_OF_BYTE_COUNT_IN_BLOCK;
				auto size_location = destination + written_compressed_bytes;

				written_compressed_bytes += SIZE_OF_BYTE_COUNT_IN_BLOCK;
				auto written = Lizard_compress_continue(encode_stream.get(), begin, destination + written_compressed_bytes, current_stream_call_size, compressed_capacity - written_compressed_bytes);
				assert(written < MAX_BYTES_PER_STREAM_CALL);
				
				auto size_to_write = uint16_t(written);
				std::memcpy(size_location, &size_to_write, SIZE_OF_BYTE_COUNT_IN_BLOCK);

				written_compressed_bytes += written;
				begin += current_stream_call_size;
			}

			return written_compressed_bytes;
			
		}

		std::size_t lizard_compressor::decompress_chunk(chunk * c, char* destination) {
			char* data = c->compressed_data.data();
			std::size_t written_uncompressed_bytes = 0;
			std::size_t read_compressed_bytes = 0;
			
			uint16_t bytes_to_read;
			uint16_t bytes_in_block;
			while (written_uncompressed_bytes < c->uncompressed_bytes_available) {
				std::memcpy(&bytes_to_read, data + read_compressed_bytes, SIZE_OF_BYTE_COUNT_IN_BLOCK);
				read_compressed_bytes += SIZE_OF_BYTE_COUNT_IN_BLOCK;
				
				std::memcpy(&bytes_in_block, data + read_compressed_bytes, SIZE_OF_BYTE_COUNT_IN_BLOCK);
				read_compressed_bytes += SIZE_OF_BYTE_COUNT_IN_BLOCK;


				auto read_bytes = Lizard_decompress_safe_continue(
					decode_stream.get(),
					data + read_compressed_bytes, 
					destination + written_uncompressed_bytes, 
					bytes_in_block,
					c->compressed_bytes_available - read_compressed_bytes
				);

				assert(read_bytes == bytes_to_read);
				read_compressed_bytes += bytes_in_block;
				written_uncompressed_bytes += bytes_to_read;
			}

			assert(written_uncompressed_bytes == c->uncompressed_bytes_available);
			assert(read_compressed_bytes == c->compressed_bytes_available);

			return read_compressed_bytes;
		}
	
		std::size_t lizard_compressor::get_max_compressed_size(std::size_t byte_count) {
			return Lizard_compressBound(byte_count) + 2 * sizeof(uint16_t) * (byte_count / MAX_BYTES_PER_STREAM_CALL + 1);
		}
	}
}
