#ifndef EFFECTS_EFFECT_CONTRAIL_PROFILE_TYPES_H
#define EFFECTS_EFFECT_CONTRAIL_PROFILE_TYPES_H

#include <stddef.h>
#include "memory/data_types.h"

struct s_effect_contrail_profile {
    short identifier;
    long definition_index;
    unsigned char event_block_index;
    unsigned char limit_type;
    unsigned short event_counter;
    long next_index;
    long effect_index;
    long location_index;
    union {
        struct { unsigned short flags; unsigned char render_type; signed char active_index; };
        struct { unsigned long flags : 16; unsigned long render_type : 8; unsigned long first_person_flags : 8; } packed;
    };
    float age;
    float lifespan;
    float interpolation;
    long references;
};

typedef char effect_contrail_profile_size[(sizeof(void *) != 4 || sizeof(s_effect_contrail_profile) == 44) ? 1 : -1];
typedef char effect_contrail_profile_definition_offset[(sizeof(void *) != 4 || offsetof(s_effect_contrail_profile, definition_index) == 4) ? 1 : -1];
typedef char effect_contrail_profile_next_offset[(sizeof(void *) != 4 || offsetof(s_effect_contrail_profile, next_index) == 12) ? 1 : -1];
typedef char effect_contrail_profile_references_offset[(sizeof(void *) != 4 || offsetof(s_effect_contrail_profile, references) == 40) ? 1 : -1];

#endif // EFFECTS_EFFECT_CONTRAIL_PROFILE_TYPES_H
