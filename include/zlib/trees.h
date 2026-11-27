#ifndef ZLIB_TREES_H
#define ZLIB_TREES_H

#include "zlib/deflate.h"

// The bit-length alphabet's transmission order, defined by trees.obj.
extern "C" {
// @address 0x2741c k_bl_order
extern const uch k_bl_order[19];
}

#endif // ZLIB_TREES_H
