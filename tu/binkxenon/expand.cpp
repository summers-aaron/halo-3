// binkxenon/expand.obj: from the records 0x713b10
// @flags /TP /O2 /Oc

// @address 0x713b10 simpmergesort
extern "C" unsigned int simpmergesort(unsigned int bits, unsigned char *destination,
                                      const unsigned char *left, const unsigned char *right,
                                      unsigned int count)
{
    unsigned int right_count = count;
    unsigned int consumed = 0;
    do {
        ++consumed;
        if (bits & 1) {
            *destination = *right++;
            --right_count;
        } else {
            *destination = *left++;
            --count;
        }
        bits >>= 1;
        ++destination;
    } while (count && right_count);
    while (count) {
        *destination++ = *left++;
        --count;
    }
    while (right_count) {
        *destination++ = *right++;
        --right_count;
    }
    return consumed;
}

