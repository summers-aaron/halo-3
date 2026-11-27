#ifndef EFFECTS_DECAL_DEFINITION_TYPES_H
#define EFFECTS_DECAL_DEFINITION_TYPES_H

#include "render_methods/render_method.h"
#include "cache/cache_files.h"
#include "tag_files/tag_types.h"

struct c_decal_definition
{
    long name;
    unsigned long flags;
    c_render_method shader;
    long pad44;
    float radius_min;
    float radius_max;
    float decay_min;
    float decay_max;
    float lifespan_min;
    float lifespan_max;
    float clamp_angle;
    float cull_angle;
    union { long visual_index; long pass; };
    union { long reserved6c; float multiplier; };
    float bitmap_aspect;
    enum e_pass { _pass_default = 0 };
    __forceinline bool get_has_sprite() const { return ((flags >> 2) & 1) != 0; }
};

struct s_decals_system_definition {
    unsigned long flags; unsigned long overlap_max_count; float overlap_amount;
    float near_distance; float far_distance;
    long decal_count; TAG_POINTER(c_decal_definition) decals; long decal_block_definition; float radius_max;
    __forceinline bool get_no_secondary_collisions() const { return ((flags >> 3) & 1) != 0; }
    __forceinline bool get_no_instance_secondary_collisions() const { return ((flags >> 4) & 1) != 0; }
    __forceinline bool get_same_material_only() const { return ((flags >> 5) & 1) != 0; }
    __forceinline bool get_first_collision_only() const { return ((flags >> 6) & 1) != 0; }
    __forceinline bool get_no_structure_collision() const { return ((flags >> 7) & 1) != 0; }
    __forceinline bool get_no_instance_collision() const { return ((flags >> 8) & 1) != 0; }
    static __forceinline const s_decals_system_definition *get(long definition_index)
    {
        return (const s_decals_system_definition *)tag_get('decs', definition_index);
    }
};

#endif
