#ifndef OBJECTS_WIDGETS_CLOTH_H
#define OBJECTS_WIDGETS_CLOTH_H

// Functions defined in tu/objectslibxbox/cloth.cpp.

struct real_matrix4x3;
union real_point3d;
union real_vector3d;
struct s_location;
struct s_data_array;
extern "C" __declspec(thread) s_data_array *g_cloth_data;

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *),
    void (*k_set_allocator_function)(void *)>
class t_restricted_allocation_manager;

void __tls_set_g_cloth_data_allocator(void *address);
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cloth_data_allocator> t_cloth_data_allocator;
extern t_cloth_data_allocator g_cloth_data_allocator;

extern "C" {
extern unsigned long g_cloth_wind_direction_guard;
extern real_vector3d g_cloth_wind_direction;
extern float g_cloth_wind_time;
extern bool g_cloth_wind_direction_valid;
extern long g_cloth_wind_last_gust_frame;
extern float g_cloth_wind_oscillation;
}

void cloth_initialize(void);
void cloth_initialize_for_new_map();
void cloth_invalidate_all();
void cloth_dispose();
void cloth_render_opaque(long cloth_index, long p1);
bool cloth_get_is_transparent(long cloth_index);
void cloth_render_transparent(const void *user_data, long cloth_index);
void cloth_get_sorting_position(long cloth_index, real_point3d *position, s_location *location, long *object_index);
void cloth_delete(long cloth_index);
long __cdecl cloth_new(long tag_index, long object_index, real_matrix4x3 *);
bool real_vector3d_exclude_from_pole_by_angle(const real_vector3d *pole, float angle, real_vector3d *vector);
void cloth_update(float dt);
void cloth_update_wind(real_point3d *point, real_vector3d *vector, bool update);
void cloth_constrain_positions_relaxation(long cloth_index);
bool cloth_setup_attachments(long cloth_index);
void __cdecl cloth_enforce_attachments(long cloth_index);
void __cdecl cloth_update_positions_verlet(long cloth_index);


#endif // OBJECTS_WIDGETS_CLOTH_H
