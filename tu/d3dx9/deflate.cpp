// d3dx9/deflate.obj: from the records 0x5d99c0 0x5d9da0
// @compiler xdk6130
// @flags /TP /O1
/* Derived from zlib 1.1.4 (deflate.c, deflate.h, zlib.h, zutil.h).
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

// zlib 1.1.4 deflate.c deflate (in namespace D3DX, as the library's copy is named)
#include <string.h>

#include "d3dx9/deflate.h"
namespace D3DX {

/* Values for max_lazy_match, good_match and max_chain_length, depending on
 * the desired pack level (0..9). The values given below have been tuned to
 * exclude worst case performance for pathological files. Better values may be
 * found for specific files.
 */
// @address 0x10680 ?configuration_table@D3DX@@3QBUconfig_s@1@B
// @rdata 0x82010680..0x820106f8
const config configuration_table[10] = {
/*      good lazy nice chain */
/* 0 */ {0,    0,  0,    0, deflate_stored},  /* store only */
/* 1 */ {4,    4,  8,    4, deflate_fast}, /* maximum speed, no lazy matches */
/* 2 */ {4,    5, 16,    8, deflate_fast},
/* 3 */ {4,    6, 32,   32, deflate_fast},

/* 4 */ {4,    4, 16,   16, deflate_slow},  /* lazy matches */
/* 5 */ {8,   16, 32,   32, deflate_slow},
/* 6 */ {8,   16, 128, 128, deflate_slow},
/* 7 */ {8,   32, 128, 256, deflate_slow},
/* 8 */ {32, 128, 258, 1024, deflate_slow},
/* 9 */ {32, 258, 258, 4096, deflate_slow}}; /* maximum compression */
// @rdata-end

// @address 0x11894 ?z_errmsg@D3DX@@3QBQBDB

static void deflate_put_short_msb(deflate_state *s, uInt b)
{
    s->pending_buf[s->pending++] = (Byte)(b >> 8);
    s->pending_buf[s->pending++] = (Byte)(b & 0xff);
}
int deflate(z_streamp strm, int flush)
{
    int old_flush; /* value of flush param for previous deflate call */
    deflate_state *s;

    if (strm == Z_NULL || strm->state == Z_NULL ||
    flush > Z_FINISH || flush < 0) {
        return Z_STREAM_ERROR;
    }
    s = strm->state;

    if (strm->next_out == Z_NULL ||
        (strm->next_in == Z_NULL && strm->avail_in != 0) ||
    (s->status == FINISH_STATE && flush != Z_FINISH)) {
        return (strm->msg = (char*)z_errmsg[Z_NEED_DICT-(Z_STREAM_ERROR)], (Z_STREAM_ERROR));
    }
    if (strm->avail_out == 0) return (strm->msg = (char*)z_errmsg[Z_NEED_DICT-(Z_BUF_ERROR)], (Z_BUF_ERROR));

    s->strm = strm; /* just in case */
    old_flush = s->last_flush;
    s->last_flush = flush;

    /* Write the zlib header */
    if (s->status == INIT_STATE) {

        uInt header = (Z_DEFLATED + ((s->w_bits-8)<<4)) << 8;
        uInt level_flags = (s->level-1) >> 1;

        if (level_flags > 3) level_flags = 3;
        header |= (level_flags << 6);
    if (s->strstart != 0) header |= PRESET_DICT;
        header += 31 - (header % 31);

        s->status = BUSY_STATE;
        deflate_put_short_msb(s, header);

    /* Save the adler32 of the preset dictionary: */
    if (s->strstart != 0) {
        deflate_put_short_msb(s, (uInt)(strm->adler >> 16));
        deflate_put_short_msb(s, (uInt)(strm->adler & 0xffff));
    }
    strm->adler = 1L;
    }

    /* Flush as much pending output as possible */
    if (s->pending != 0) {
        flush_pending(strm);
        if (strm->avail_out == 0) {
        /* Since avail_out is 0, deflate will be called again with
         * more output space, but possibly with both pending and
         * avail_in equal to zero. There won't be anything to do,
         * but this is not an error situation so make sure we
         * return OK instead of BUF_ERROR at next call of deflate:
             */
        s->last_flush = -1;
        return Z_OK;
    }

    /* Make sure there is something to do and avoid duplicate consecutive
     * flushes. For repeated and useless calls with Z_FINISH, we keep
     * returning Z_STREAM_END instead of Z_BUFF_ERROR.
     */
    } else if (strm->avail_in == 0 && flush <= old_flush &&
           flush != Z_FINISH) {
        return (strm->msg = (char*)z_errmsg[Z_NEED_DICT-(Z_BUF_ERROR)], (Z_BUF_ERROR));
    }

    /* User must not provide more input after the first FINISH: */
    if (s->status == FINISH_STATE && strm->avail_in != 0) {
        return (strm->msg = (char*)z_errmsg[Z_NEED_DICT-(Z_BUF_ERROR)], (Z_BUF_ERROR));
    }

    /* Start a new block or continue the current one.
     */
    if (strm->avail_in != 0 || s->lookahead != 0 ||
        (flush != Z_NO_FLUSH && s->status != FINISH_STATE)) {
        block_state bstate;

    bstate = (*(configuration_table[s->level].func))(s, flush);

        if (bstate == finish_started || bstate == finish_done) {
            s->status = FINISH_STATE;
        }
        if (bstate == need_more || bstate == finish_started) {
        if (strm->avail_out == 0) {
            s->last_flush = -1; /* avoid BUF_ERROR next call, see above */
        }
        return Z_OK;
        /* If flush != Z_NO_FLUSH && avail_out == 0, the next call
         * of deflate should use the same flush parameter to make sure
         * that the flush is complete. So we don't have to output an
         * empty block here, this will be done at next call. This also
         * ensures that for a very small output buffer, we emit at most
         * one empty block.
         */
    }
        if (bstate == block_done) {
            if (flush == Z_PARTIAL_FLUSH) {
                _tr_align(s);
            } else { /* FULL_FLUSH or SYNC_FLUSH */
                _tr_stored_block(s, (char*)0, 0L, 0);
                /* For a full flush, this empty block will be recognized
                 * as a special marker by inflate_sync().
                 */
                if (flush == Z_FULL_FLUSH) {
                    s->head[s->hash_size-1] = 0;
                    memset((Bytef *)s->head, 0, (unsigned)(s->hash_size-1)*sizeof(*s->head));
                }
            }
            flush_pending(strm);
        if (strm->avail_out == 0) {
          s->last_flush = -1; /* avoid BUF_ERROR at next call, see above */
          return Z_OK;
        }
        }
    }

    if (flush != Z_FINISH) return Z_OK;
    if (s->noheader) return Z_STREAM_END;

    /* Write the zlib trailer (adler32) */
    deflate_put_short_msb(s, (uInt)(strm->adler >> 16));
    deflate_put_short_msb(s, (uInt)(strm->adler & 0xffff));
    flush_pending(strm);
    /* If avail_out is zero, the application will call deflate again
     * to flush the rest.
     */
    s->noheader = -1; /* write the trailer only once! */
    return s->pending != 0 ? Z_OK : Z_STREAM_END;
}

int deflateEnd(z_streamp strm)
{
    int status;

    if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;

    status = strm->state->status;
    if (status != INIT_STATE && status != BUSY_STATE &&
    status != FINISH_STATE) {
      return Z_STREAM_ERROR;
    }

    /* Deallocate in reverse order of allocations: */
    if (strm->state->pending_buf) {
        (*strm->zfree)(strm->opaque, (voidpf)strm->state->pending_buf);
    }
    if (strm->state->head) {
        (*strm->zfree)(strm->opaque, (voidpf)strm->state->head);
    }
    if (strm->state->prev) {
        (*strm->zfree)(strm->opaque, (voidpf)strm->state->prev);
    }
    if (strm->state->window) {
        (*strm->zfree)(strm->opaque, (voidpf)strm->state->window);
    }

    (*strm->zfree)(strm->opaque, (voidpf)strm->state);
    strm->state = Z_NULL;

    return status == BUSY_STATE ? Z_DATA_ERROR : Z_OK;
}

}
