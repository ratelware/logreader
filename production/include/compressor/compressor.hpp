#pragma once

#include <memory>

#include <compressor/chunk.hpp>

namespace compressor {
	class compressor {
	public:
		class impl;
		compressor();

		std::size_t compress_into_chunk(uncompressed_block* block, chunk* destination);
		std::size_t decompress_chunk(chunk*, uncompressed_chunk*);

		std::size_t get_max_compressed_size(std::size_t char_count);
		chunk get_chunk(std::size_t starting_at, std::size_t char_count);

		void reset_stream();
		void preserve_dictionaries();  

		~compressor();
	private:
		std::unique_ptr<impl> pimpl;
	};
}