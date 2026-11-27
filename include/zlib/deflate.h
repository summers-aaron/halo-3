#ifndef ZLIB_DEFLATE_H
#define ZLIB_DEFLATE_H

#include "zlib/zlib.h"

// Deflate's literal/length and distance trees and compression state.
typedef unsigned char uch;
typedef unsigned short ush;
typedef unsigned long ulg;

typedef struct ct_data_s {
    union {
        ush freq;
        ush code;
    } fc;
    union {
        ush dad;
        ush len;
    } dl;
} ct_data;

typedef struct static_tree_desc_s {
    const ct_data *static_tree;
    const int *extra_bits;
    int extra_base;
    int elems;
    int max_length;
} static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;
    int max_code;
    static_tree_desc *stat_desc;
} tree_desc;

typedef struct deflate_state_s {
    z_streamp strm;
    int status;
    uch *pending_buf;
    ulg pending_buf_size;
    uch *pending_out;
    int pending;
    int noheader;
    uch data_type;
    uch method;
    int last_flush;
    unsigned int w_size;
    unsigned int w_bits;
    unsigned int w_mask;
    uch *window;
    ulg window_size;
    ush *prev;
    ush *head;
    unsigned int ins_h;
    unsigned int hash_size;
    unsigned int hash_bits;
    unsigned int hash_mask;
    unsigned int hash_shift;
    long block_start;
    unsigned int match_length;
    unsigned int prev_match;
    int match_available;
    unsigned int strstart;
    unsigned int match_start;
    unsigned int lookahead;
    unsigned int prev_length;
    unsigned int max_chain_length;
    unsigned int max_lazy_match;
    int level;
    int strategy;
    unsigned int good_match;
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
    uch *l_buf;
    unsigned int lit_bufsize;
    unsigned int last_lit;
    ush *d_buf;
    ulg opt_len;
    ulg static_len;
    unsigned int matches;
    int last_eob_len;
    ush bi_buf;
    int bi_valid;
} deflate_state;


typedef char deflate_state_size_check[(sizeof(void *) != 4 || ((sizeof(deflate_state) == 0x16b8) )) ? 1 : -1];
#endif // ZLIB_DEFLATE_H
