#ifndef PHYSICS_HAVOK_H
#define PHYSICS_HAVOK_H

// Functions defined in tu/blamlibxbox/havok.cpp.

class phys_shape;
class phys_entity;
union real_point3d;
union real_vector3d;
struct real_matrix4x3;
enum e_havok_group;

void havok_world_remove_entity(phys_entity *entity);
void havok_component_aquire(long object_index, bool recursive, bool force);
bool havok_can_allocate_space_for_instance_of_object_definition(long object_definition_index);
void havok_notify_thread_frame_started(long thread_index);
void *havok_thread_user_data_get(long thread_index);
void havok_notify_thread_frame_finished(long thread_index);
void havok_notify_static_geometry_modfied(long object_index, long region_index, long old_permutation, long new_permutation, long flags);
void havok_initialize_all_allocated_state(bool enable);
void havok_dispose_from_old_structure_bsp(unsigned long structure_bsp_index);
void havok_object_set_velocities(long object_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity, bool unknown);
void havok_display_stats(bool display_on_screen);
void havok_object_rebuild(long object_index, long unknown);
void havok_world_add_entity(phys_entity *entity, bool active);
void havok_notify_object_changed_region_permutation(long object_index, long unused, const char *old_permutations);
bool havok_object_apply_acceleration(long object_index, long node_index, const real_point3d *point,
	const real_vector3d *translational_acceleration, const real_vector3d *angular_acceleration,
	long *out_accelerated_object_index, real_vector3d *out_translational_velocity, real_vector3d *out_angular_velocity);
void havok_update(void);
void havok_entity_post_update(void);
class c_havok_component;
extern "C" unsigned char havok_component_has_active_entity(c_havok_component *component);
extern "C" bool havok_object_needs_physics(long object_index, long definition_index);
extern "C" void havok_component_release(long object_index, bool recursive);
extern "C" void havok_memory_garbage_collect_locked(void);
extern "C" bool havok_crate_needs_physics_update(long index);
extern "C" void havok_object_set_position(long object_index, bool ignore_infinite_inertia, bool force, bool keep_rotation);
bool havok_world_shape_collides(const phys_shape *shape, long component_index, const real_matrix4x3 *matrix, e_havok_group group);

class phys_island;

void havok_object_new_recursive(long root_object_index, long object_index);
void havok_build_environment_surface_mopp(long structure_bsp_index);
bool havok_all_entities_in_island_contain_players(class phys_island const *island);
void havok_object_new(long object_index);
extern "C" void havok_object_update_component_state(long object_index, bool was_set);
void havok_make_space_for_connected_rigid_bodies(long object_index);
void havok_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
void havok_initialize_havok_world();
void havok_save_to_game_state(long flags);
void havok_load_from_game_state(long flags);

// Data defined in tu/blamlibxbox/havok.cpp that other units use.

struct s_havok_constants;

extern s_havok_constants g_havok_constants;

class phys_world;

extern phys_world *g_havok_world;

void havok_initialize();
void havok_dispose();
void havok_initialize_for_new_map();
void havok_dispose_from_old_map();

extern "C" void havok_handle_new_structure_bsp(unsigned long structure_bsp_mask);
extern "C" void havok_handle_old_structure_bsp(unsigned long structure_bsp_mask);
#endif // PHYSICS_HAVOK_H
