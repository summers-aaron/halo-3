#ifndef AI_STYLE_VECTOR_H
#define AI_STYLE_VECTOR_H

// Global bit vector with one bit per AI style.
extern "C" unsigned long global_style_vector[7];

static __forceinline bool style_vector_test(long style)
{
    return (bool)((global_style_vector[style >> 5] >> (style & 31)) & 1);
}

#endif
