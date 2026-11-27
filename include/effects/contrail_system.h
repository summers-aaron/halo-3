#ifndef EFFECTS_CONTRAIL_SYSTEM_H
#define EFFECTS_CONTRAIL_SYSTEM_H
#include <stddef.h>
#include "memory/data_types.h"
#include "effects/effect_array_globals.h"
#include "math/matrix_math_types.h"

class c_contrail_system {
public:
    unsigned short identifier;
    union { long owner; long effect_index; };
    union { unsigned char priority; unsigned char event_block_index; };
    unsigned char type;
    short location;
    long part_block_index;
    long definition_index;
    long lightprobe_slot_index;
    long first_location;
    unsigned short flags;
    unsigned char extra;
    union { signed char bsp_index; unsigned char first_person_flags; };
    float age;
    union { float pulse_time; float effect_scale_a; };
    union { float distance; float effect_scale_b; };
    long lod;
    static long create(unsigned char type, long definition_index, long owner, unsigned char priority, long location, long unused, unsigned char extra);
    static __forceinline c_contrail_system *get(long index)
    {
        return &((c_contrail_system *)g_contrail_system_data->data)[(unsigned short)index];
    }
    static void initialize_for_map();
    static void dispose_from_map();
    static void dispose_from_game();
    void frame_advance(float delta_time);
    void query_effect(float delta_time);
    void pulse(float delta_time, bool force);
    static void frame_advance_all(float delta_time);
    long add_contrail_location(const real_matrix4x3 *matrix, unsigned long seed);
    __forceinline bool get_should_pulse() const { return (bool)((flags >> 1) & 1); }
    __forceinline bool get_should_frame_advance() const { return (bool)((flags >> 2) & 1); }
    __forceinline bool get_should_live() const { return (flags & 1) != 0; }
    __forceinline void set_flag(unsigned short mask, bool value) { if (value) flags |= mask; else flags &= ~mask; }
};
typedef char contrail_system_size[(sizeof(c_contrail_system) == 0x30) ? 1 : -1];
typedef char contrail_system_effect_offset[(offsetof(c_contrail_system, owner) == 4) ? 1 : -1];
typedef char contrail_system_scale_offset[(offsetof(c_contrail_system, pulse_time) == 0x24) ? 1 : -1];
typedef char contrail_system_lod_offset[(offsetof(c_contrail_system, lod) == 0x2c) ? 1 : -1];
#endif
