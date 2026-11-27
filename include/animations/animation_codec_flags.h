#ifndef ANIMATIONS_ANIMATION_CODEC_FLAGS_H
#define ANIMATIONS_ANIMATION_CODEC_FLAGS_H

#include "memory/static_arrays_flags.h"

struct flags256 : c_static_flags_no_init<256>
{
    void and_with(const flags256 *other);
    void or_with(const flags256 *other);
};

typedef char animation_codec_flags_size[(sizeof(flags256) == 8 * sizeof(unsigned long)) ? 1 : -1];

struct s_node_flag_sets
{
    flags256 rotation;
    flags256 translation;
    flags256 scale;
};

#if defined(_XBOX) && !defined(__clang__)
inline bool animation_node_flags_are_empty(const flags256 *flags);
inline void animation_mask_node_flags(s_node_flag_sets *flags, const flags256 *mask);
#else
bool animation_node_flags_are_empty(const flags256 *flags);
void animation_mask_node_flags(s_node_flag_sets *flags, const flags256 *mask);
#endif

#endif
