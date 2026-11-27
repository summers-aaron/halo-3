#ifndef MATH_INTEGER_MATH_COUNT_BITS_H
#define MATH_INTEGER_MATH_COUNT_BITS_H

// The population count of a word (integer_math.cpp instantiates count_bits<unsigned long> out of line, 0x1b6fa8).
template<typename T> inline unsigned long count_bits(T value) {
    unsigned long bits = value;
    bits = ((bits >> 1) & 0x55555555) + (bits & 0x55555555);
    bits = ((bits >> 2) & 0x33333333) + (bits & 0x33333333);
    bits = ((bits >> 4) & 0x0f0f0f0f) + (bits & 0x0f0f0f0f);
    bits = ((bits >> 8) & 0x00ff00ff) + (bits & 0x00ff00ff);
    bits = ((bits >> 16) & 0xffff) + (bits & 0xffff);
    return bits;
}

#endif
