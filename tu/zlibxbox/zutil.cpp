// zlibxbox/zutil.obj: from the records 0x39b4b0 0x39b4c0
// @flags /TP /O2
// @address 0x39b4b0 zcalloc
// @address 0x39b4c0 zcfree
#include <string.h>
#include <stdlib.h>

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
extern "C" {
voidpf zcalloc(voidpf opaque, unsigned items, unsigned size)
{
    if (opaque) items += size - size; /* make compiler happy */
    return (voidpf)calloc(items, size);
}

void zcfree(voidpf opaque, voidpf ptr)
{
    free(ptr);
    if (opaque) return; /* make compiler happy */
}
}
