#ifndef EFFECTS_EFFECT_BUDGET_LIMITS_H
#define EFFECTS_EFFECT_BUDGET_LIMITS_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "effects/particle_system_limits_owner_types.h"

// Same 16-byte records as s_light_volume_limit: one table behind g_effect_budget_limits.
struct s_effect_limit
{
    long priority;
    unsigned long absolute_count;
    unsigned long relative_percentage;
    unsigned long maximum_count;
};

// effects.cpp's s_effect_limit_block is 20 bytes, with the limits pointer at +12.
// Contrail allocation readers select the same blocks at +c0, +d4, +e8, +fc and +214.
struct s_effect_limit_block
{
    long type;
    unsigned long overall_budget;
    union
    {
        struct { long count; TAG_POINTER(s_effect_limit) limits; TAG_POINTER(void) definition; };
        s_tag_block tag_block;
    };
};

enum e_contrail_budget_block
{
    _contrail_system_budget = 9,
    _contrail_location_budget = 10,
    _contrail_budget = 11,
    _contrail_gpu_row_budget = 12,
    _contrail_gpu_budget = 20,
    _particle_gpu_row_budget = 25,
    _contrail_gpu_profile_budget = 26,
    _particle_queued_gpu_budget = 27
};

struct s_particle_system_limits
{
    s_effect_limit_block blocks[28];
};

extern "C" s_particle_system_limits_owner *g_effect_budget_limits;

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char effect_budget_block_size[(sizeof(s_effect_limit_block) == 0x14) ? 1 : -1];
typedef char effect_budget_limits_offset[(offsetof(s_effect_limit_block, limits) == 12) ? 1 : -1];
typedef char effect_budget_blocks_offset[(offsetof(s_particle_system_limits, blocks) == 0) ? 1 : -1];
#endif
#endif
