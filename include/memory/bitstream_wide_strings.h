#ifndef MEMORY_BITSTREAM_WIDE_STRINGS_H
#define MEMORY_BITSTREAM_WIDE_STRINGS_H

#include "memory/bitstream.h"

// The 16-bit string scanner used by the bitstream writer.
static __forceinline unsigned long bitstream_wide_strings_ustrnlen(const unsigned short *string, unsigned long maximum)
{
    unsigned long length;
    for (length = 0; length < maximum; length++) {
        if (string[length] == 0)
            break;
    }
    return length;
}

// c_bitstream::write_string_wchar writes 16-bit characters; its length scanner lives beside it in this header.
__forceinline void c_bitstream::write_string_wchar(const char *name, const unsigned short *string, long maximum_length)
{
    unsigned long length = bitstream_wide_strings_ustrnlen(string, maximum_length);
    m_encoded_value_bits += ((length ? maximum_length - 1 : 0) + 1) * 16;
    for (long index = 0; index < maximum_length; index++)
    {
        unsigned short character = string[index];
        write_value_internal((unsigned long)character, 16);
        if (!character)
            break;
    }
}

#endif // MEMORY_BITSTREAM_WIDE_STRINGS_H
