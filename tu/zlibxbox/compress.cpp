// zlibxbox/compress.obj: from the records 0x3975b8
// @flags /TP /O2
#include "zlib/zlib.h"
/* Derived from zlib 1.1.4 (compress.c, zlib.h, zutil.h).
   Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.

   Jean-loup Gailly        Mark Adler
   jloup@gzip.org          madler@alumni.caltech.edu

   Altered: one function of the library per file for byte-matching the Xbox 360
   build (headers reduced to
   what the function uses); compress2 as the game changed it: its caller supplies the stream, whose
   allocators are the game's. */

#include <string.h>
#include "zlib/blam_zlib_memory.h"

#define DEF_MEM_LEVEL 8

// @address 0x397b98 z_deflate

// @address 0x3975b8 compress3
extern "C" int compress3(uLongf *destLen, int level, voidpf opaque, z_streamp stream)
{
    int err;

    stream->zalloc = blam_zlib_malloc;
    stream->zfree = blam_zlib_free;
    stream->opaque = opaque;

    err = z_deflateInit2_(stream, level, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, ZLIB_VERSION,
                          sizeof(z_stream));
    if (err != Z_OK) return err;

    err = z_deflate(stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        z_deflateEnd(stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream->total_out;

    err = z_deflateEnd(stream);
    return err;
}
