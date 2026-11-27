// zlibxbox/uncompr.obj: from the records 0x397660
// @flags /TP /O2
// @address 0x397660 uncompress2
#include <string.h>
#include "zlib/zlib.h"
#include "zlib/blam_zlib_memory.h"

#define DEF_WBITS MAX_WBITS

extern "C" int uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, alloc_func, free_func,
               voidpf opaque)
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;

    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = blam_zlib_malloc;
    stream.zfree = blam_zlib_free;
    stream.opaque = opaque;

    err = z_inflateInit2_(&stream, DEF_WBITS, ZLIB_VERSION, sizeof(z_stream));
    if (err != Z_OK) return err;

    err = z_inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        z_inflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = z_inflateEnd(&stream);
    return err;
}
