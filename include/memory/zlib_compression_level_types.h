#ifndef MEMORY_ZLIB_COMPRESSION_LEVEL_TYPES_H
#define MEMORY_ZLIB_COMPRESSION_LEVEL_TYPES_H

// The level handed straight to deflateInit as zlib's own compression level.
enum e_zlib_compression_level
{
    _zlib_compression_level_default = -1, // Z_DEFAULT_COMPRESSION
    _zlib_compression_level_none = 0,     // Z_NO_COMPRESSION
    _zlib_compression_level_fastest = 1,  // Z_BEST_SPEED
    _zlib_compression_level_fast = 3,     // the highest level zlib still runs with its fast (deflate_fast) matcher
    _zlib_compression_level_best = 9,     // Z_BEST_COMPRESSION
};

#endif // MEMORY_ZLIB_COMPRESSION_LEVEL_TYPES_H
