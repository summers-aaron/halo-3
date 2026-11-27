// zlibxbox/infblock.obj: from the records 0x39b650
// @flags /TP /O2
// @address 0x39b650 inflate_blocks_reset

#include "zlib/infutil.h"
enum { MODE_TYPE = 0, MODE_BTREE = 4, MODE_DTREE = 5, MODE_CODES = 6 };
#define FREE_BLOCK(stream, address) (*((stream)->zfree))((stream)->opaque, (void *)(address))

extern "C" {
void inflate_blocks_reset(inflate_blocks_state *blocks, z_stream *stream, unsigned long *saved_check)
{
  if (saved_check != 0)
    *saved_check = blocks->check;
  if (blocks->mode == MODE_BTREE || blocks->mode == MODE_DTREE)
    FREE_BLOCK(stream, blocks->sub.trees.blens);
  if (blocks->mode == MODE_CODES)
    FREE_BLOCK(stream, blocks->sub.decode.codes);
  blocks->mode = MODE_TYPE;
  blocks->bitk = 0;
  blocks->bitb = 0;
  blocks->read = blocks->write = blocks->window;
  if (blocks->checkfn != 0)
    stream->adler = blocks->check = (*blocks->checkfn)(0, 0, 0);
}
}
