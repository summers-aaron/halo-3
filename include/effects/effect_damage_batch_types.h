#ifndef EFFECTS_EFFECT_DAMAGE_BATCH_TYPES_H
#define EFFECTS_EFFECT_DAMAGE_BATCH_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"

struct s_effect_damage_batch_entry {
    long definition_index;
    s_damage_owner owner;
    unsigned char type;
    char pad11;
    short pad12;
    long reference_count;
    real_point3d position;
    union { long reserved24; s_location location; };
    short node_indices[4];
    long node_count;
};
struct s_effect_damage_batch_record { real_point3d position; long entry_index; float radius; };
struct c_render_batch {
    s_effect_damage_batch_entry entries[8];
    long count;
    s_effect_damage_batch_record records[128];
    union { long count2; long record_count; };
    long selected_index;
    float search_radius;
    float minimum_distance;
    float maximum_distance;
    long padbb8;
    long padbbc;
    __forceinline c_render_batch() { count2 = 0; count = 0; }
};
typedef char effect_damage_batch_entry_size[(sizeof(s_effect_damage_batch_entry) == 0x34) ? 1 : -1];
typedef char effect_damage_batch_size[(sizeof(c_render_batch) == 0xbc0) ? 1 : -1];
typedef char effect_damage_batch_count_offset[(offsetof(c_render_batch, count) == 0x1a0) ? 1 : -1];
typedef char effect_damage_batch_records_offset[(offsetof(c_render_batch, records) == 0x1a4) ? 1 : -1];
typedef char effect_damage_batch_record_count_offset[(offsetof(c_render_batch, record_count) == 0xba4) ? 1 : -1];

#endif
