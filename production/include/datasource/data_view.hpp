#pragma once

#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <map>

#include <compressor/chunk.hpp>
#include <compressor/compressor.hpp>

#include <datasource/cache.hpp>

namespace datasource {
	const std::size_t MAX_CACHE_SIZE = 10;

	class data_view {
	public:
		typedef std::vector<std::shared_ptr<compressor::uncompressed_chunk>> chunks;

		virtual ~data_view();

		virtual chunks get_bytes(std::size_t byte_range_start, std::size_t byte_range_end) = 0;
	};

	class compressed_view : public data_view {
	public:
		compressed_view(std::deque<compressor::chunk>&);
		virtual chunks get_bytes(std::size_t start, std::size_t end);

	private: 
		std::deque<compressor::chunk>& chunks;
		cache<std::size_t, std::shared_ptr<compressor::uncompressed_chunk> > local_memory;
		compressor::compressor compressor;
	};

}