#ifndef MEMORY_DATA_INLINE_H
#define MEMORY_DATA_INLINE_H

// match: 0x85528: the shifted Boolean helper preserves the srwi/clrlwi pair; TEST_BIT resized datum_delete by -4 bytes.
static __forceinline bool flag_test(unsigned short flags, int bit)
{
    return (flags >> bit) & 1;
}

// Shifted access for full-width engine flag words.
static __forceinline bool flag_test(unsigned long flags, long bit)
{
    return (flags >> bit) & 1;
}

// match: 0x1ec3f8: March data_connect calls byte_swap_memory(&next_identifier, 1, -2); the case bodies must fold to the shift/or pair at 1-trip call sites.
static __forceinline void byte_swap_memory(void *memory, long count, long code)
{
    switch (code) {
    case -2: {
        short *p = (short *)memory;
        while (count-- > 0) {
            *p = (short)((*p << 8) | ((unsigned short)*p >> 8));
            p++;
        }
        break;
    }
    case -4: {
        unsigned long *p = (unsigned long *)memory;
        while (count-- > 0) {
            *p = (*p >> 24) | ((*p >> 8) & 0xff00) | ((*p << 8) & 0xff0000) | (*p << 24);
            p++;
        }
        break;
    }
    case -8: {
        unsigned __int64 *p = (unsigned __int64 *)memory;
        while (count-- > 0) {
            unsigned __int64 x = *p;
            *p = (x >> 56) | ((x >> 40) & 0xff00) | ((x >> 24) & 0xff0000) | ((x >> 8) & 0xff000000) | ((x << 8) & 0xff00000000ui64) |
                ((x << 24) & 0xff0000000000ui64) | ((x << 40) & 0xff000000000000ui64) | (x << 56);
            p++;
        }
        break;
    }
    }
}

#endif // MEMORY_DATA_INLINE_H
