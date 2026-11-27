#ifndef EFFECTS_PARTICLE_LOCATION_RUNTIME_TYPES_H
#define EFFECTS_PARTICLE_LOCATION_RUNTIME_TYPES_H
#include <stddef.h>
#include <string.h>
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
class c_particle_system_definition;
// Mirrors effect_event_definition with a typed particle block (same shape as s_particle_movement_effect_event).
struct s_particle_location_effect_event
{
    long name;
    unsigned long flags;
    unsigned char limit_type;
    float skip_fraction;
    real_bounds delay_bounds;
    real_bounds duration_bounds;
    s_tag_block parts;
    s_tag_block accelerations;
    c_typed_tag_block<c_particle_system_definition> particle_systems;
};
typedef char particle_location_event_size[(sizeof(s_particle_location_effect_event) == 0x44) ? 1 : -1];
// A particle location record carrying its random seed, system index and level of detail.
class c_particle_location
{
public:
    union { unsigned short salt; short identifier; };
    union { unsigned char flags; unsigned char m_random_seed; };
    char reserved_identifier;
    unsigned long state;
    long first_emitter_index;
    long last_emitter_index;
    long next_location_index;
    long system_index;
    real_matrix4x3 matrix;
    float scale;
    union { real_vector3d velocity; real_point3d bounding_sphere_center; };
    union { float age; float bounding_sphere_radius; };
    static __forceinline void initialize_for_map();
    static __forceinline void dispose_from_map();
    static __forceinline void dispose_from_game();
    __forceinline const real_point3d *get_position() const { return &matrix.position; }
    __forceinline const real_matrix4x3 *get_matrix() const { return &matrix; }
    __forceinline void set_matrix(const real_matrix4x3 *value) { memcpy(&matrix, value, sizeof(matrix)); }
    __forceinline void set_first_person(bool first_person)
    {
        if (first_person)
            state |= 1;
        else
            state &= ~1;
    }
    __forceinline long get_next_location() const { return next_location_index; }
    __forceinline long get_system_index() const { return system_index; }
    __forceinline float get_lod_amount() const { return scale; }
    __forceinline float get_bounding_sphere_radius() const { return bounding_sphere_radius; }
    __forceinline float get_random_seed() const { return m_random_seed * (1.0f / 255.0f); }
};
typedef char particle_location_record_size[(sizeof(c_particle_location) == 0x60) ? 1 : -1];
typedef char particle_location_record_system_offset[(offsetof(c_particle_location, system_index) == 0x14) ? 1 : -1];
typedef char particle_location_record_lod_offset[(offsetof(c_particle_location, scale) == 0x4c) ? 1 : -1];
// The simulation update and allocation paths share the complete location record.
struct s_particle_location_datum : c_particle_location
{
    __forceinline long get_index() const;
    __forceinline void add_to_list(long *first, long *last);
};
#endif
