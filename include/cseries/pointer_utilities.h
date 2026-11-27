#ifndef CSERIES_POINTER_UTILITIES_H
#define CSERIES_POINTER_UTILITIES_H

#include <stdlib.h>

static __forceinline unsigned int address_from_pointer(const void *pointer)
{
    return static_cast<unsigned int>(reinterpret_cast<uintptr_t>(pointer));
}

static __forceinline void *pointer_from_address(unsigned int address)
{
    return reinterpret_cast<void *>(static_cast<uintptr_t>(address));
}

static __forceinline unsigned int align_address(unsigned int address, long alignment_bits)
{
    unsigned int mask = (unsigned int)((1ui64 << alignment_bits) - 1);
    return (address + mask) & ~mask;
}

static __forceinline void *align_pointer(void *pointer, long alignment_bits)
{
    uintptr_t mask = (uintptr_t)((1ui64 << alignment_bits) - 1);
    return reinterpret_cast<void *>((reinterpret_cast<uintptr_t>(pointer) + mask) & ~mask);
}

static __forceinline void *offset_pointer(void *pointer, long offset)
{
    return static_cast<unsigned char *>(pointer) + offset;
}

// Byte distance from base to pointer (March cseries_windows.cpp).
static __forceinline long pointer_difference(const void *base, const void *pointer)
{
    return (const char *)pointer - (const char *)base;
}

#endif // CSERIES_POINTER_UTILITIES_H
