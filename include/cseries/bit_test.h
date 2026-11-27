#ifndef CSERIES_BIT_TEST_H
#define CSERIES_BIT_TEST_H

static __forceinline bool test_bit(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}

#endif // CSERIES_BIT_TEST_H
