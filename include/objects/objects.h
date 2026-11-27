#ifndef OBJECTS_OBJECTS_H
#define OBJECTS_OBJECTS_H

// Functions defined in tu/objectslibxbox/objects.cpp.

class c_global_material_type;
c_global_material_type object_get_material(long object_index, long material_index);

struct object_marker;
struct object_placement_data;
struct object_region_information;
struct real_matrix4x3;
struct real_orientation;
union real_point3d;
union real_rgb_color;
union real_vector3d;
struct s_animation_event_data;
struct s_cluster_reference;
struct s_damage_owner;
extern "C" { extern const s_damage_owner k_damage_owner_unknown; }
struct s_location;
struct s_object_cluster_payload;
struct s_object_render_thread_message;

struct s_game_cluster_bit_vectors;

void objects_initialize(void);
void objects_initialize_for_new_map();
void objects_dispose_from_old_map();
void objects_initialize_for_new_structure_bsp(unsigned long mask);
void objects_activation(const s_game_cluster_bit_vectors *active_clusters, const s_game_cluster_bit_vectors *current_clusters);

extern "C" void objects_handle_deleted_player(long player_index);

long object_get_ultimate_parent(long object_index);
long object_accepted_local_physics_object_get(long object_index);
real_point3d *object_get_origin(long object_index, real_point3d *origin);
void object_get_velocities(long object_index, real_vector3d *linear_velocity, real_vector3d *angular_velocity);
void object_get_region_information(long handle, long *size, char **out_a, char **out_b, object_region_information **out_base);
real_point3d *object_get_center_of_mass(long object_index, real_point3d *center);
void object_get_orientation(long object_index, real_vector3d *forward, real_vector3d *up);
s_location *object_get_location(long object_index, s_location *location);
void *object_try_and_get_and_verify_type(long object_index, unsigned long type_mask);
long object_get_root_object(long object_index);
bool object_is_hidden(long object_index);
real_matrix4x3 *object_get_world_matrix(long object_index, real_matrix4x3 *matrix);
float object_get_function_value_simple(long object_index, long function_name, long function_owner_definition_index);
bool object_get_function_value(long object_index, long function_name, long function_owner_definition_index, float *out_function_magnitude, bool *deterministic);
bool object_is_or_contains_player(long object_index);
bool object_is_connected_to_map(long object_index);
void object_set_invalid_for_recycling(long object_index, bool enable);
short internal_object_get_markers_by_string_id(long object_index, long marker_name, object_marker *markers, short maximum_marker_count, bool use_object_index);
bool object_owns_object(long ancestor_index, long object_index);
bool object_force_inside_bsp(long object_index, const real_point3d *point, long ignore_object_index);
long object_get_attachment_marker_name(long object_index, short attachment_index);
void object_detach(long object_index);
long objects_get_contiguous_free_memory_size(void);
bool object_start_interpolation(long object_index, float interpolation_time);
bool object_get_localized_velocities(long object_index, real_vector3d *linear_velocity, real_vector3d *angular_velocity,
	real_vector3d *early_mover_linear_velocity, real_vector3d *early_mover_angular_velocity);
void object_reset(long object_index);
void __tls_set_g_object_globals_allocator(void *value);
void reset_object_cached_render_state(long object_index);
void object_delete(long object_index);
void object_set_scale(long object_index, float scale, float interpolation_time, bool compute_node_matrices);
void object_set_velocities_internal(long object_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity, bool suppress_notify);
void object_disconnect_from_physics(long object_index);
void object_reconnect_to_physics(long object_index);
void object_compute_instance_bitfield_visible(long object_index);
bool object_set_base_change_color_by_index(long object_index, long change_color_index, const real_rgb_color *color);
void *object_try_and_get_unsafe_and_verify_type(long object_index, unsigned long type_mask);
void object_attach_gamestate_entity(long object_index, long gamestate_index);
void object_detach_gamestate_entity(long object_index, long gamestate_index);
void resursive_add_child_objects_to_list(long object_index, long definition_index, long object_list_index);
void object_freeze_node_orientations(long object_index, bool freeze);
void object_reverse_compute_node_orientations_from_matrices(long object_index, unsigned long *node_mask);
long objects_in_clusters_by_indices(unsigned long class_flags, unsigned long object_flags, unsigned long type_flags, long cluster_count,
	const s_cluster_reference *clusters, long maximum_count, long *object_indices);
short objects_in_sphere(unsigned long class_flags, unsigned long type_flags, const s_location *location, const real_point3d *center, float radius,
	long *object_indices, short maximum_count);
void __tls_set_g_object_header_data_allocator(void *object);
bool object_visible_to_any_player(long object_index);
void object_set_velocities(long object_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity);
void object_wake(long object_index);
void object_set_at_rest(long object_index, bool at_rest);
// match: 0xe3730: __restrict payload lets callers avoid alias reloads
void cluster_build_object_payload(long object_index, s_object_cluster_payload *__restrict payload);
void object_compute_node_matrices(long object_index);
void object_update_collision_culling(long object_index);
void object_update_visibility_culling(long object_index);
void object_set_requires_motion(long object_index);
bool object_header_block_allocate(long object_index, short block_reference_offset, short size, short alignment_bits);
void object_adjust_node_orientations_for_facing_change(long object_index, const real_vector3d *old_facing_vector, const real_vector3d *new_facing_vector);
bool unit_compute_function_value(long unit_index, long function_name, long argument, float *value, bool *active, bool *deterministic);
void object_set_garbage(long object_index, bool garbage, long garbage_time);
void object_set_hidden(long object_index, bool hidden);
void handle_object_render_message(s_object_render_thread_message *object_render_thread_message, long parameter);
void objects_purge_deleted_objects(void);
void object_deactivate(long object_index, bool deactivate_children);
void object_set_always_active(long object_index, bool always_active);
void object_activate(long object_index);
void object_set_in_limbo(long object_index, bool in_limbo);
bool object_set_position_internal(long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
	const s_location *location, bool compute_node_matrices, bool set_havok_position, bool force, bool keep_connected);
void object_detach_immediate(long object_index);
void object_set_region_permutation_direct(long object_index, long region_name, long permutation_name);
void object_set_model_state(long object_index, long region_name, long state, bool force);
void object_set_model_state_property_per_region(long object_index, long region_name, long property, short value, bool force);
void object_reinitialize_from_placement(long object_index, object_placement_data *placement, bool force);
const s_damage_owner *damage_owner_unknown();
void object_placement_data_new(object_placement_data *data, long definition_index, long owner_object_index, const s_damage_owner *damage_owner);
void objects_garbage_collection(void);
void object_apply_acceleration(long object_index, long unknown, const real_point3d *position, const real_vector3d *linear_acceleration,
	const real_vector3d *angular_acceleration);
void objects_move();
void objects_update();
void objects_post_update();
void object_set_initial_change_colors(long object_index, unsigned long flags, const real_rgb_color *colors);
void object_animation_callback(const s_animation_event_data *event_data, long user_param);
long object_new_by_name(short name_index, bool clone, bool cinematic);
long object_new(object_placement_data *data);

struct effects_node_attachment;
struct s_melee_clamp_data;
struct s_object_acceleration_info;

extern "C" void object_rebuild_havok(long object_index);
long effects_node_attachment_try_and_get_object(effects_node_attachment *attachment);
void effects_node_attachment_new(long owner, long object_index, short node_index, const real_point3d *point);
extern "C" void object_attach_to_marker_with_type(long parent_object_index, long parent_marker_name, long child_object_index, long child_marker_name);
extern "C" void object_reset_after_teleport(long object_index);
extern "C" void object_clamp_melee_velocity(const s_melee_clamp_data *clamp_data, long object_index);
extern "C" void object_apply_acceleration_info(const s_object_acceleration_info *info);
extern "C" void objects_flush_tracked_accelerations(void);
extern "C" void object_attach_to_node_with_type(long parent_index, long child_index, short node_index);
extern "C" bool object_compute_animation_blend(long object_index, float inner_radius, float outer_radius, float *fade);

struct s_object_function_definition;
struct s_objects_element_array;

extern "C" long object_get_player_index(long object_index);
extern "C" void object_set_damage_owner(long object_index, const s_damage_owner *damage_owner, bool skip_validation);
extern "C" unsigned char object_get_damage_reporting_type(long object_index);
extern "C" void object_initialize_node_orientations(s_objects_element_array *array, long count, char (*destination)[32]);
extern "C" real_orientation *object_get_node_orientations(s_objects_element_array *array, unsigned int count);
void objects_handle_deleted_object(long object_index);
extern "C" long cluster_partition_get_next_datum_and_payload(long *iterator, const void **payload);
extern "C" void object_clear_excess_flag_bits(unsigned long *data, long bit_count);
extern "C" void __tls_set_g_object_data_allocator(void *object);
void __tls_set_g_object_name_list_allocator(void *allocator);
void __tls_set_g_object_render_data_allocator(void *allocator);
extern "C" long object_find_predicted_object(long object_index);
long object_get_marker_object(long object_index);
void object_connect_lights_recursive(long object_index, bool disconnect, bool reconnect, bool recursive, bool parent_visible);
extern "C" void object_update_ai_flags(long object_index);
extern "C" bool object_try_and_get_location(long object_index, s_location *location);
extern "C" void objects_unregister_scenario_object(long object_index);
extern "C" bool objects_is_main_thread_with_lock(void);
extern "C" void object_placement_reset_scale_for_physics(object_placement_data *data);
extern "C" bool object_try_and_get_marker_point(long object_index, long marker_name, long marker_index, real_point3d *point);
extern "C" long objects_compact_memory_pool(void);
void objects_update_header_callback(long object_index, long handle);
void objects_dispose();
void object_disconnect_from_map(long object_index, bool recursive);
void object_detach_from_node(long object_index, const real_matrix4x3 *node_matrix);
bool object_function_get_function_value(long object_index, s_object_function_definition *function, long function_owner_definition_index,
	float *out_function_magnitude, bool *deterministic);
void object_pre_delete_recursive(long object_index, bool recursive);
void object_delete_recursive(long object_index, bool recursive);
void attachments_delete(long object_index);
void object_reconnect_to_map(long object_index, bool broadphase, const s_location *location);
extern "C" void objects_initialize_structure_widgets(unsigned long mask);
bool object_compute_bounding_sphere(long object_index);
void object_compute_node_matrices_non_recursive(long object_index);
void object_attach_to_marker(long parent_index, long parent_marker_name, long child_index, long child_marker_name);
bool object_update(long object_index);
void attachments_update(long object_index);
bool object_get_change_color(long object_index, long change_color_index, real_rgb_color *color);
void object_set_position_direct(long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
	const s_location *location, bool unknown);

struct s_objects_hierarchy_list;

extern "C" void objects_hierarchy_list_add_cluster(s_objects_hierarchy_list *list, const s_cluster_reference *cluster);
extern "C" void objects_hierarchy_list_add_object(s_objects_hierarchy_list *list, long object_index);

// Data defined in tu/objectslibxbox/objects.cpp that other units use.
extern bool object_memory_non_gamestate;

#endif // OBJECTS_OBJECTS_H
