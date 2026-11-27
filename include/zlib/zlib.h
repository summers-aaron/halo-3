#ifndef ZLIB_ZLIB_H
#define ZLIB_ZLIB_H

// zlib 1.1.4 (zconf.h and zlib.h), as the zlibxbox units are built: the basic types, the stream structure and the
// constants of the interface.

#ifdef __cplusplus
extern "C" {
#endif

#define ZLIB_VERSION "1.1.4"
#define OF(args) args
#define ZEXPORT
#define FAR

#include "zlib/zlib_types.h"

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0
#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2
#define Z_DEFLATED   8
#define Z_NULL  0
#define MAX_WBITS 15
#define MAX_MEM_LEVEL 9

// tu/zlibxbox/adler32.cpp (zlib's adler32, Z_PREFIX spelling)
uLong z_adler32(uLong adler, const Bytef *buf, uInt len);

// deflate.c and inflate.c (Z_PREFIX spelling); the tree does not compile these objects yet.
int z_deflateInit2_(z_streamp strm, int level, int method, int windowBits, int memLevel, int strategy,
                    const char *version, int stream_size);
int z_deflate(z_streamp strm, int flush);
int z_deflateEnd(z_streamp strm);
int z_inflateInit2_(z_streamp strm, int windowBits, const char *version, int stream_size);
int z_inflate(z_streamp strm, int flush);
int z_inflateEnd(z_streamp strm);

// tu/zlibxbox/compress.cpp: the game's variant of compress2. Its caller sets up the stream and its buffers; the
// stream allocates through the game's functions with the caller's opaque.
int compress3(uLongf *destLen, int level, voidpf opaque, z_streamp stream);

// tu/zlibxbox/uncompr.cpp: the game's variant of uncompress. The stream allocates through the game's functions
// with the caller's opaque; the callee ignores the two allocation-function slots and uses blam_zlib_malloc/free.
int uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, alloc_func zalloc, free_func zfree,
                voidpf opaque);

#ifdef __cplusplus
}
#endif

#endif // ZLIB_ZLIB_H
