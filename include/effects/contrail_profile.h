#ifndef EFFECTS_CONTRAIL_PROFILE_H
#define EFFECTS_CONTRAIL_PROFILE_H

#include <stddef.h>

#include "memory/data.h"
#include "math/matrix_math_types.h"
#include "effects/effect_array_globals.h"
#include "gpu_particle/gpu_particle_globals.h"

class c_contrail;
class c_contrail_profile;
class c_contrail_states;
#include "effects/contrail_profile_data.h"

class c_contrail_profile {
public:
    static void initialize_for_map();
    __forceinline float get_age() const { return age; }
    __forceinline float get_lifespan() const { return lifespan; }
    __forceinline const real_point3d *get_position() const { return &position; }
    __forceinline const real_vector3d *get_velocity() const { return &velocity; }
    __forceinline const unsigned short *get_random_seed_ref() const { return random; }
    __forceinline float get_cumulative_length() const { return cumulative_length; }
    __forceinline const real_argb_color *get_color() const { return &color; }
    __forceinline float get_scale() const { return scale; }
    long identifier;
    long sibling;
    unsigned long flags;
    float age;
    float lifespan;
    unsigned short random[4];
    real_point3d position;
    real_vector3d velocity;
    float cumulative_length;
    real_argb_color color;
    float scale;
    __forceinline bool get_temporary() const { return (flags & 1) != 0; }
    __forceinline void set_temporary(bool temporary) { if (temporary) flags |= 1; else flags &= ~1u; }
    static long create(const class c_contrail *contrail);
    static __forceinline c_contrail_profile *get(long index) { return datum_get<c_contrail_profile>(g_contrail_profile_data, index); }
    __forceinline long get_next_sibling_index() const { return sibling; }
    static __forceinline void destroy(long index) { ::datum_delete(g_contrail_profile_data, index); }
    static void dispose_from_map();
};


typedef char contrail_profile_size[(sizeof(c_contrail_profile) == 0x4c) ? 1 : -1];
typedef char contrail_profile_position_offset[(offsetof(c_contrail_profile, position) == 0x1c) ? 1 : -1];

#endif
