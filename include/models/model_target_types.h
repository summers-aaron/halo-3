#ifndef MODELS_MODEL_TARGET_TYPES_H
#define MODELS_MODEL_TARGET_TYPES_H
#include <stddef.h>

struct s_model_target
{
    long marker_name;
    float size;
    float cone_angle;
    short damage_section_index; // actor_combat tests the object's damage-section state at this index
    // Two bytes of implicit alignment before targeting_relevance.
    float targeting_relevance;
    float maximum_distance;
    long flags;
    float damage_modifier;
};

typedef char model_target_size[(sizeof(s_model_target) == 0x20) ? 1 : -1];
typedef char model_target_relevance_offset[(offsetof(s_model_target, targeting_relevance) == 0x10) ? 1 : -1];
typedef char model_target_maximum_distance_offset[(offsetof(s_model_target, maximum_distance) == 0x14) ? 1 : -1];
typedef char model_target_flags_offset[(offsetof(s_model_target, flags) == 0x18) ? 1 : -1];
#endif
