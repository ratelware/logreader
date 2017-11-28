#pragma once

#include <cstddef>

namespace datasource {
	const char LINE_TERMINATOR = '\n';
	const std::size_t READ_BLOCK_SIZE = 1024 * 1024 * 32;
	const std::size_t DEFAULT_UNCOMPRESSED_CHARS_PER_CHUNK = 512 * 1024;
}