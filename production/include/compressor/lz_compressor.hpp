#pragma once

#include <compressor/compressor_impl.hpp>

namespace compressor {
	template <typename lz_algorithm>
	class lz_compressor : public compressor::impl {
	public:


	private:
		typedef std::unique_ptr_ptr<lz_algorithm::compression_stream> compression_stream_ptr;
		typedef std::unique_ptr<lz_algorithm::decompression_stream> decompression_stream_ptr;

		decompression_stream_ptr compression;
		decompression_stream_ptr decompression;
	};

}