#ifndef OBJECTS_WIDGETS_ANTENNA_H
#define OBJECTS_WIDGETS_ANTENNA_H

#include "math/real_math_types.h"

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *),
    void (*k_set_allocator_function)(void *)>
class t_restricted_allocation_manager;

// The antenna datum array, in the owning unit's thread-local slot.
struct s_data_array;
extern "C" { extern __declspec(thread) s_data_array *g_antenna_data; }

// Functions defined in tu/objectslibxbox/antenna.cpp.

struct real_matrix4x3;
struct s_location;

void antennas_initialize(void);
void antennas_initialize_for_new_map(void);
void antennas_dispose_from_old_map(void);
void antennas_dispose(void);
void antenna_delete(long antenna_index);
void antenna_get_sorting_position(long antenna_index, real_point3d *position, s_location *location, long *object_index);
long antenna_new(long tag_index, long object_index, real_matrix4x3 *matrix);
extern "C" void antennas_update(void);
extern "C" void antenna_render_transparent(void *user_data, unsigned short antenna_index);

struct antenna_vertex {
    real_point3d position;
    real_vector3d velocity;
    short flags;
};

struct antenna_datum {
    long header;
    bool active;
    bool initialized;
    long tag_index;
    long object_index;
    real_point3d position;
    antenna_vertex vertices[2];
    long last_update_time;
    real_vector3d acceleration;
};

typedef char antenna_vertex_size[(sizeof(antenna_vertex) == 0x1c) ? 1 : -1];
typedef char antenna_datum_size[(sizeof(antenna_datum) == 0x64) ? 1 : -1];

struct antenna_definition;

void __tls_set_g_antenna_data_allocator(void *address);
void antenna_render_proper(antenna_datum *antenna, antenna_definition *definition);
void antenna_update(antenna_datum *antenna, antenna_definition *definition, float delta_time);

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_antenna_data_allocator> antenna_data_allocation_manager;
extern antenna_data_allocation_manager g_antenna_data_allocator;

extern "C" float antenna_update_time_step;

#endif // OBJECTS_WIDGETS_ANTENNA_H
