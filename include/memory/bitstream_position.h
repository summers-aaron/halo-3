#ifndef MEMORY_BITSTREAM_POSITION_H
#define MEMORY_BITSTREAM_POSITION_H

class c_bitstream;

// Saves the current bitstream position so it can be restored later.
// Copies m_current onto the position stack and increments the saved-position depth.
// @address 0x3ee730 bitstream_push_position
extern "C" void bitstream_push_position(c_bitstream *bitstream);

#endif // MEMORY_BITSTREAM_POSITION_H
