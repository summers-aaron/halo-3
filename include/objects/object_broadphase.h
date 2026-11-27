#ifndef OBJECTS_OBJECT_BROADPHASE_H
#define OBJECTS_OBJECT_BROADPHASE_H

// Functions defined in tu/objectslibxbox/object_broadphase.cpp.

union real_point3d;
union real_rectangle3d;
union real_vector3d;
struct s_object_broadphase_handle_data;

bool clip_vector_to_rectangle(const real_rectangle3d *rectangle, const real_point3d *point,
    const real_vector3d *vector, float distance, float *minimum, float *maximum);
void object_broadphase_update_object(long object_index);
void object_broadphase_set_handle_data(long object_index, const s_object_broadphase_handle_data *data);
void __tls_set_g_object_broadphase_allocator(void *allocator);
void object_broadphase_add_object(long object_index, const s_object_broadphase_handle_data *data);
void object_broadphase_remove_object(long object_index);

void object_broadphase_initialize();
void object_broadphase_dispose();
void object_broadphase_initialize_for_new_map();
void object_broadphase_dispose_from_old_map();
void object_broadphase_initialize_for_new_structure_bsp(unsigned long activating_structure_bsp_mask);
void object_broadphase_dispose_from_old_structure_bsp(unsigned long deactivating_structure_bsp_mask);
void object_broaphase_save_to_game_state(long game_state);
void object_broaphase_load_from_game_state(long game_state);

class phys_aabb;

void object_broadphase_post_copy_fixup(void *allocation);
void object_calculate_broadphase_aabb(long object_index, phys_aabb *aabb);

struct collision_result;

bool object_broadphase_ray_cast(unsigned __int64 filter, unsigned long collision_model_flags, const real_point3d *point,
    const real_vector3d *vector, long first_ignore_object_index, long second_ignore_object_index, long ignore_index,
    collision_result *result);

// Data defined in tu/objectslibxbox/object_broadphase.cpp that other units use.

struct s_object_broadphase_vtable_globals;

extern s_object_broadphase_vtable_globals g_object_broadphase_vtable_globals;

#endif // OBJECTS_OBJECT_BROADPHASE_H
