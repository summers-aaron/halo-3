// zlibxbox/adler32.obj: from the records 0x39b4c8
// @flags /TC /O2
/* Derived from zlib 1.1.4 (adler32.c, zlib.h, zutil.h).
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
   build (functions and tables named by their addresses, headers reduced to
   what the function uses). */

// zlib 1.1.4 adler32.c adler32
#include <string.h>

#include "zlib/zlib.h"
typedef unsigned char uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long ulg;

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]
#define ERR_RETURN(strm,err) return (strm->msg = (char*)ERR_MSG(err), (err))
#define DEF_MEM_LEVEL 8
#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
#define MIN_MATCH  3
#define MAX_MATCH  258
#define PRESET_DICT 0x20
#define zmemcpy memcpy
#define zmemzero(dest, len) memset(dest, 0, len)
#define ZALLOC(strm, items, size) (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}
#define Assert(cond,msg)
#define Trace(x)
#define Tracev(x)
#define Tracevv(x)
#define Tracec(c,x)
#define Tracecv(c,x)

#define BASE 65521L
#define NMAX 5552
#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

// @address 0x39b4c8 z_adler32
uLong z_adler32(uLong adler, const Bytef *buf, uInt len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(buf);
        buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
        s2 += s1;
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}
