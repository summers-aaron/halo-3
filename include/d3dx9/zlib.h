#ifndef D3DX9_ZLIB_H
#define D3DX9_ZLIB_H

#define Z_NO_FLUSH 0
#define Z_PARTIAL_FLUSH 1
#define Z_FULL_FLUSH 3
#define Z_FINISH 4
#define Z_OK 0
#define Z_STREAM_END 1
#define Z_NEED_DICT 2
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR (-3)
#define Z_BUF_ERROR (-5)
#define Z_DEFLATED 8
#define Z_NULL 0
#define Z_FILTERED 1
#define Z_DEFAULT_STRATEGY 0
#define Z_DEFAULT_COMPRESSION (-1)

// D3DX embeds a separate zlib 1.1.4 instance in its own namespace.
namespace D3DX {
#include "zlib/zlib_types.h"
}

#endif // D3DX9_ZLIB_H
