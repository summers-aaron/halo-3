#ifndef CSERIES_BYTE_SWAPPING_H
#define CSERIES_BYTE_SWAPPING_H

static __forceinline unsigned long byte_swap_long(unsigned long value)
{
    return (value >> 24) | ((value >> 8) & 0xff00) | ((value << 8) & 0xff0000) | (value << 24);
}

#endif
