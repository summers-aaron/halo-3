#ifndef OBJECTS_WIDGETS_LEAF_SYSTEM_H
#define OBJECTS_WIDGETS_LEAF_SYSTEM_H

// Functions defined in tu/objectslibxbox/leaf_system.cpp.

struct real_matrix4x3;
union real_point3d;
struct s_location;

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *),
    void (*k_set_allocator_function)(void *)>
class t_restricted_allocation_manager;

void leaf_system_unattached_visible_render(long view);
void leaf_system_initialize(void);
void leaf_system_initialize_for_new_map(void);
void leaf_system_dispose_from_old_map(void);
void leaf_system_dispose(void);
void leaf_system_get_location(long leaf_system_index, real_point3d *position, s_location *location, long *object_index);
void __tls_set_g_leaf_system_data_allocator(void *address);
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_leaf_system_data_allocator> g_leaf_system_data_allocator;
extern "C" float g_leaf_system_time;
void leaf_system_update(float delta);
long leaf_system_new(long definition_index, long object_index, real_matrix4x3 *matrix);
void leaf_system_render_opaque(long leaf_system_index, long entry_point);
bool leaf_system_visible(long index);
bool leaf_system_setup_shader(long index);
void leaf_system_emit(long leaf_system_index, float delta);
void leaf_system_update_physics(long leaf_system_index, float delta);

#endif // OBJECTS_WIDGETS_LEAF_SYSTEM_H
