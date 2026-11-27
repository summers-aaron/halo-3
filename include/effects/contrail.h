#ifndef EFFECTS_CONTRAIL_H
#define EFFECTS_CONTRAIL_H

#include <stddef.h>

#include "memory/data.h"
#include "memory/data_typed_access.h"
#include "math/matrix_math_types.h"
#include "effects/effect_array_globals.h"
#include "gpu_particle/gpu_particle_globals.h"

class c_contrail;
class c_contrail_profile;
class c_contrail_states;
extern "C" __declspec(thread) s_data_array *g_contrail_data_array;
extern "C" void contrail_create_profiles(c_contrail *, unsigned long, c_contrail_states *, bool, const real_point3d *, float);

class c_contrail {
public:
    static void initialize_for_map();
    static void dispose_from_game();
    long identifier;
    long sibling;
    long gpu_index;
    long first_profile;
    long definition_index;
    long location_index;
    unsigned long flags;
    mutable unsigned long random_seed;
    float random[2];
    float age;
    real_point3d position;
    float distance;
    union { unsigned long property0; float emission_rate; };
    union { unsigned long property1; float profile_lifespan; };
    union { real_vector2d initial; struct { float initial0, initial1; }; };
    static long create(unsigned char type, long parent, long definition_index, unsigned long seed);
    void pulse(float delta_time, bool force, c_contrail_states *states);
private:
    void initialize_profile(c_contrail_profile *profile, c_contrail_states *states, const real_point3d *direction, float distance);
    void create_profile(unsigned long queue_context, c_contrail_states *states, bool temporary, const real_point3d *direction, float distance);
    friend void contrail_create_profiles(c_contrail *contrail, unsigned long queue_context, c_contrail_states *states, bool temporary,
        const real_point3d *direction, float delta_distance);
public:
    __forceinline float real_internal_random_range(float minimum, float maximum) const;
    static __forceinline c_contrail *get(long index)
    {
        return datum_get_absolute<c_contrail>(g_contrail_data_array, static_cast<unsigned short>(index));
    }
    __forceinline bool get_should_frame_advance() const { return (flags & 1) != 0; }
    __forceinline long get_next_sibling_index() const { return sibling; }
    __forceinline void set_should_frame_advance(bool value) { if (value) flags |= 1; else flags &= ~1u; }
    __forceinline void frame_advance(float delta_time);
    static void destroy(long index);
    static void dispose_from_map();
    void render() const;
    __forceinline void submit() const;
private:
    static void render_callback(const void *data, long user_data);
  friend class c_contrail_location;
};


typedef char contrail_size[(sizeof(c_contrail) == 0x4c) ? 1 : -1];
typedef char contrail_initial0_offset[(offsetof(c_contrail, initial0) == 0x44) ? 1 : -1];

#endif
