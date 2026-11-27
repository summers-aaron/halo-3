#ifndef SOUND_SOUND_PERMUTATION_CHUNK_H
#define SOUND_SOUND_PERMUTATION_CHUNK_H

// Cached permutation chunk: resource and cache handles, first/last sample
// bounds, and flags packed with the sample size.
struct s_sound_permutation_chunk
{
    long resource_index;
    long flags_and_sample_size;
    long cache_index;
    unsigned long first_sample;
    unsigned long last_sample;

    __forceinline long get_sample_size() const { return flags_and_sample_size & 0x03ffffff; }
    __forceinline bool test_flag(long bit) const { return (flags_and_sample_size & (1UL << (bit + 26))) != 0; }
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char sound_permutation_chunk_size[(sizeof(s_sound_permutation_chunk) == 20) ? 1 : -1];
#endif

#endif // SOUND_SOUND_PERMUTATION_CHUNK_H
