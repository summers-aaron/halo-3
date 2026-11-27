#ifndef ANIMATIONS_ANIMATION_INHERITENCE_H
#define ANIMATIONS_ANIMATION_INHERITENCE_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "animations/animation_codec_flags.h"
#include "math/real_math_types.h"

// Inheritance entry: a graph reference followed by node-remapping and flag blocks.
struct s_animation_inheritence
{
    s_tag_reference reference;
    s_tag_block node_remap;
    s_tag_block node_flags;
    real root_translation_scale;
    unsigned long flags;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char animation_inheritence_size[(sizeof(s_animation_inheritence) == 0x30) ? 1 : -1];
typedef char animation_inheritence_remap_offset[(offsetof(s_animation_inheritence, node_remap) == 0x10) ? 1 : -1];
typedef char animation_inheritence_flags_offset[(offsetof(s_animation_inheritence, node_flags) == 0x1c) ? 1 : -1];
typedef char animation_inheritence_scale_offset[(offsetof(s_animation_inheritence, root_translation_scale) == 0x28) ? 1 : -1];
#endif

#endif
