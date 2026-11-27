#ifndef EFFECTS_LIGHT_VOLUME_LIMITS_TYPES_H
#define EFFECTS_LIGHT_VOLUME_LIMITS_TYPES_H
#include <stddef.h>
#include "tag_files/tag_types.h"

// EffectGlobals holdback and priority records with 20- and 16-byte strides.
struct s_light_volume_limit
{
    long priority;
    unsigned long absolute_count;
    unsigned long relative_percentage;
    unsigned long maximum_count;
};
struct s_light_volume_priority_block
{
    long count;
    TAG_POINTER(s_light_volume_limit) address;
    TAG_POINTER(void) definition;
};
struct s_light_volume_holdback
{
    long type;
    unsigned long overall_budget;
    s_light_volume_priority_block priorities;
};
struct s_light_volume_limit_table
{
    s_light_volume_holdback effects;
    s_light_volume_holdback events;
    s_light_volume_holdback effect_locations;
    s_light_volume_holdback lightprobes;
    s_light_volume_holdback messages;
    s_light_volume_holdback systems;
    s_light_volume_holdback locations;
    s_light_volume_holdback volumes;
};
struct s_light_volume_limit_owner
{
    long count;
    TAG_POINTER(s_light_volume_limit_table) table;
    TAG_POINTER(void) definition;
};
typedef char light_volume_priority_size[(sizeof(s_light_volume_limit) == 16) ? 1 : -1];
typedef char light_volume_holdback_size[(sizeof(s_light_volume_holdback) == 20) ? 1 : -1];
typedef char light_volume_system_limits_offset[(offsetof(s_light_volume_limit_table, systems)
    + offsetof(s_light_volume_holdback, priorities) + offsetof(s_light_volume_priority_block, address) == 0x70) ? 1 : -1];
#endif
