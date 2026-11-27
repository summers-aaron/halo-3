#ifndef EFFECTS_CONTRAIL_LOCATION_H
#define EFFECTS_CONTRAIL_LOCATION_H

#include <stddef.h>

#include "memory/data.h"
#include "math/matrix_math_types.h"
#include "effects/effect_array_globals.h"
#include "gpu_particle/gpu_particle_globals.h"

class c_contrail;
class c_contrail_profile;
class c_contrail_states;

class c_contrail_location {
public:
    static void initialize_for_map();
    static void dispose_from_game();
    long identifier;
    long sibling;
    long system_index;
    long contrail_index;
    unsigned long flags;
    real_matrix4x3 matrix;
    real_vector3d velocity;
    real_point3d last_position;
    void frame_advance(float delta_time);
    __forceinline void set_should_frame_advance(bool value) { if (value) flags |= 1; else flags &= ~1u; }
    static __forceinline c_contrail_location *get(long index)
    {
        // match: 0x2b5d08: unsigned short truncation keeps the second location_index load as lwz + clrlwi; the bare & 0xffff index narrows it to lhz +22
        return datum_get<c_contrail_location>(g_contrail_location_data, (unsigned short)index);
    }
    static void dispose_from_map();
    static void destroy(long index);
    void add_sibling(long index);
    static long create(unsigned char type, long parent, const real_matrix4x3 *matrix, unsigned long seed);
public:
  void submit();
};


typedef char contrail_location_size[(sizeof(c_contrail_location) == 0x60) ? 1 : -1];
typedef char contrail_location_last_position_offset[(offsetof(c_contrail_location, last_position) == 0x54) ? 1 : -1];

#endif
