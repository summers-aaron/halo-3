#ifndef D3DX9_DEFLATE_H
#define D3DX9_DEFLATE_H

#include "d3dx9/zlib.h"
#include "zlib/deflate.h"

namespace D3DX {
enum {
    INIT_STATE = 42,
    BUSY_STATE = 113,
    FINISH_STATE = 666,
    PRESET_DICT = 0x20
};

typedef unsigned char uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long ulg;

typedef ush Pos;
typedef Pos Posf;
typedef unsigned IPos;

typedef struct internal_state {
    z_streamp strm;
    int status;
    Bytef *pending_buf;
    ulg pending_buf_size;
    Bytef *pending_out;
    int pending;
    int noheader;
    Byte data_type;
    Byte method;
    int last_flush;
    uInt w_size;
    uInt w_bits;
    uInt w_mask;
    Bytef *window;
    ulg window_size;
    Posf *prev;
    Posf *head;
    uInt ins_h;
    uInt hash_size;
    uInt hash_bits;
    uInt hash_mask;
    uInt hash_shift;
    long block_start;
    uInt match_length;
    IPos prev_match;
    int match_available;
    uInt strstart;
    uInt match_start;
    uInt lookahead;
    uInt prev_length;
    uInt max_chain_length;
    uInt max_lazy_match;
#   define max_insert_length  max_lazy_match
    int level;
    int strategy;
    uInt good_match;
    int nice_match;
    ct_data dyn_ltree[573];
    ct_data dyn_dtree[61];
    ct_data bl_tree[39];
    tree_desc l_desc;
    tree_desc d_desc;
    tree_desc bl_desc;
    ush bl_count[16];
    int heap[573];
    int heap_len;
    int heap_max;
    uch depth[573];
    uchf *l_buf;
    uInt lit_bufsize;
    uInt last_lit;
    ushf *d_buf;
    ulg opt_len;
    ulg static_len;
    uInt matches;
    int last_eob_len;
    ush bi_buf;
    int bi_valid;
} deflate_state;
typedef enum {
    need_more,
    block_done,
    finish_started,
    finish_done
} block_state;
typedef block_state (*compress_func) (deflate_state *s, int flush);
block_state deflate_stored(deflate_state *s, int flush);
block_state deflate_fast(deflate_state *s, int flush);
block_state deflate_slow(deflate_state *s, int flush);
typedef struct config_s {
   ush good_length;
   ush max_lazy;
   ush nice_length;
   ush max_chain;
   compress_func func;
} config;

// The vendor implementation shares its helper bodies with the trees object.
void bi_flush(internal_state *s);
void bi_windup(internal_state *s);
void _tr_align(deflate_state *s);
void _tr_flush_block(deflate_state *s, charf *buf, ulg stored_len, int eof);
void _tr_stored_block(deflate_state *s, charf *buf, ulg stored_len, int eof);
uLong adler32(uLong adler, const Bytef *buf, uInt len);
void error(const char *msg);
void flush_pending(z_streamp strm);
int deflate(z_streamp strm, int flush);
int deflateEnd(z_streamp strm);
int deflateReset(z_streamp strm);
int deflateInit2_(z_streamp strm, int level, int method, int window_bits, int memory_level, int strategy, const char *version, int stream_size);

extern const config configuration_table[10];
extern const char * const z_errmsg[10]; /* owned by zutil.obj */

}

namespace D3DX { typedef char deflate_state_size_check[(sizeof(void *) != 4 || ((sizeof(deflate_state) == 0x16b8) )) ? 1 : -1]; }
#endif // D3DX9_DEFLATE_H
