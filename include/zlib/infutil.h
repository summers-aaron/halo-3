#ifndef ZLIB_INFUTIL_H
#define ZLIB_INFUTIL_H

// zlib 1.1.4 infutil.h: the inflate blocks state, shared by tu/zlibxbox/infblock.cpp and tu/zlibxbox/infutil.cpp.

#include "zlib/zlib.h"

struct inflate_huft_s;
typedef struct inflate_huft_s inflate_huft;
struct inflate_codes_state;
typedef struct inflate_codes_state inflate_codes_statef;

typedef uLong (*check_func)(uLong check, const Bytef *buf, uInt len);

typedef struct inflate_blocks_state {
    int mode;                   // inflate_block_mode
    union {
        uInt left;              // STORED: bytes left to copy
        struct {
            uInt table;         // table lengths (14 bits)
            uInt index;         // index into blens (or border)
            uIntf *blens;       // bit lengths of codes
            uInt bb;            // bit length tree depth
            inflate_huft *tb;   // bit length decoding tree
        } trees;                // DTREE: decoding info for trees
        struct {
            inflate_codes_statef *codes;
        } decode;               // CODES: current state
    } sub;
    uInt last;                  // true if this block is the last block
    uInt bitk;                  // bits in bit buffer
    uLong bitb;                 // bit buffer
    inflate_huft *hufts;        // single malloc for tree space
    Bytef *window;              // sliding window
    Bytef *end;                 // one byte after sliding window
    Bytef *read;                // window read pointer
    Bytef *write;               // window write pointer
    check_func checkfn;         // check function
    uLong check;                // check on output
} inflate_blocks_state;
typedef char inflate_blocks_state_size_check[(sizeof(void *) != 4 || ((sizeof(inflate_blocks_state) == 0x40) )) ? 1 : -1];

#endif // ZLIB_INFUTIL_H
