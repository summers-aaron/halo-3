// binkxenon/binkxenon.obj: from the records 0x71d0a0
// @flags /TP /O2 /Oc

// @address 0x71d0a0 radmemset16
extern "C" void radmemset16(void *destination, unsigned short value, unsigned long count)
{
    unsigned long pattern = value;
    unsigned long word_count = count >> 1;
    unsigned short *tail = (unsigned short *)destination;
    pattern = (pattern << 16) | pattern;
    if (word_count)
    {
        unsigned long *current = (unsigned long *)destination;
        for (unsigned long remaining = word_count; remaining > 0; --remaining)
            *current++ = pattern;
        tail = (unsigned short *)((unsigned long *)destination + word_count);
    }
    if (count & 1)
        *tail = (unsigned short)pattern;
}
