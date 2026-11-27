// blamlibxbox/crc.obj: from the records 0x183ef0 0x1c21d0
// @flags /TP /O2 /Oc
// @address 0x183ef0 ?crc_checksum_buffer@@YAKKPBXJ@Z

#define CRC32_POLYNOMIAL 0xEDB88320L
#include "memory/crc.h"
extern "C" {
// @address 0x119a3ed g_crc_table_initialized
bool g_crc_table_initialized;
// @address 0x119aa58 g_crc_table
unsigned long g_crc_table[256];
}
static void crc_build_table(unsigned long *table)
{
    for (short index = 0; index < 256; index++)
    {
        unsigned long crc = index;
        for (short bit = 0; bit < 8; bit++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
            else
                crc >>= 1;
        }
        table[index] = crc;
    }
}

unsigned long crc_checksum_buffer(unsigned long crc, const void *buffer, long buffer_size)
{
    const unsigned char *bytes = (const unsigned char *)buffer;

    if (!g_crc_table_initialized)
    {
        crc_build_table(g_crc_table);
        g_crc_table_initialized = true;
    }

    while (buffer_size > 0)
    {
        unsigned long shifted_crc = (crc >> 8) & 0x00FFFFFFL;
        unsigned long table_entry = g_crc_table[((long)crc ^ *bytes++) & 0xff];
        crc = shifted_crc ^ table_entry;
        buffer_size--;
    }

    return crc;
}

// @address 0x1c21d0 ?fast_checksum_buffer@@YAKKPBXJ@Z
unsigned long fast_checksum_buffer(unsigned long checksum, const void *buffer, long buffer_size)
{
    unsigned long a, b, c;
    unsigned long length = buffer_size;
    const unsigned long *words = (const unsigned long *)buffer;
    a = b = c = 0xdeadbeef + length + checksum;
    while (length > 12)
    {
        a += words[0];
        b += words[1];
        c += words[2];
        a -= c; a ^= (c >> 28) ^ (c << 4); c += b;
        b -= a; b ^= (a >> 26) ^ (a << 6); a += c;
        c -= b; c ^= (b >> 24) ^ (b << 8); b += a;
        a -= c; a ^= (c >> 16) ^ (c << 16); c += b;
        b -= a; b ^= (a << 19) ^ (a >> 13); a += c;
        c -= b; c ^= (b >> 28) ^ (b << 4); b += a;
        length -= 12;
        words += 3;
    }
    if (length == 0)
        return c;
    const unsigned short *halfwords = (const unsigned short *)words;
    const unsigned char *bytes = (const unsigned char *)words;
    switch (length)
    {
    case 3 * sizeof(*words): a += words[0]; b += words[1]; c += words[2]; break;
    case 3 * sizeof(*words) - 1: a += words[0]; b += words[1]; c += words[2] >> 8; break;
    case 2 * sizeof(*words) + sizeof(*halfwords): a += words[0]; b += words[1]; c += halfwords[4]; break;
    case 9: a += words[0]; b += words[1]; c += bytes[8]; break;
    case 8: a += words[0]; b += words[1]; break;
    case 7: a += words[0]; b += words[1] >> 8; break;
    case 6: a += words[0]; b += halfwords[2]; break;
    case 5: a += words[0]; b += bytes[4]; break;
    case 4: a += words[0]; break;
    case 3: a += words[0] >> 8; break;
    case 2: a += halfwords[0]; break;
    case 1: a += bytes[0]; break;
    }
    c ^= b; c -= (b >> 18) | (b << 14);
    a ^= c; a -= (c >> 21) | (c << 11);
    b ^= a; b -= (a << 25) | (a >> 7);
    c ^= b; c -= (b >> 16) | (b << 16);
    a ^= c; a -= (c >> 28) | (c << 4);
    b ^= a; b -= (a >> 18) | (a << 14);
    c ^= b; c -= (b << 24) | (b >> 8);
    return c;
}
