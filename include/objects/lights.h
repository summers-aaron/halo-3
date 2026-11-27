#ifndef OBJECTS_LIGHTS_H
#define OBJECTS_LIGHTS_H

// Functions defined in tu/objectslibxbox/lights.cpp.

struct light_datum;
struct light_geometry;
union real_point3d;
union real_vector3d;
struct render_camera;
struct s_scenario_light;
struct visibility_projection;

void light_disconnect_from_map(long light_index);
void light_reconnect_to_map(long light_index);
bool light_adjust_position_from_attachment_last_minute(light_datum *light, const render_camera *camera);
void lights_update(void);
void light_build_intermediate_geometry(const real_point3d *position, const real_point3d *target_position,
    const real_vector3d *forward, const real_vector3d *up, float scale, long gel_index, light_geometry *geometry);
long light_new_unattached(long definition_index, long object_index, short node_index,
    const real_point3d *position, const real_vector3d *forward, const real_vector3d *up, float scale);
long light_new_from_scenario(long definition_index, long scenario_light_index, bool lightmap_mode, const s_scenario_light *scenario_light);
void light_update_state(long light_index);
bool light_build_render_geometry(long light_index, light_geometry *geometry);
void light_delete_internal(long light_index, bool update_object);
void __tls_set_g_light_render_data_allocator(void *allocation);
void light_build_visibility_projections(const real_vector3d *forward, const real_vector3d *left,
    const light_geometry *geometry, visibility_projection *projections, short *projection_count);
bool light_theoretically_produces_light(long light_index, long player_index, long);

struct real_matrix4x3;
struct s_cinematic_light_globals;
struct s_light_sh_data;

extern "C" long lights_find_cinematic_object_entry(s_cinematic_light_globals *cinematic, long object_index, bool add);
extern "C" void lights_reset_cinematic_lighting(void);
extern "C" void lights_delete_cinematic_object_lighting(long object_index);
extern "C" void lights_update_cinematic_sh_lighting(s_light_sh_data *light, long definition_index, const real_matrix4x3 *matrix);
void lights_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
void lights_dispose_from_old_structure_bsp(unsigned long old_structure_bsp_mask);
extern "C" void lights_set_cinematic_object_lighting(long object_index, long marker_name, long lighting_index, real_matrix4x3 *matrix);
extern "C" void light_render_lens_flare(long light_index, long player_index);
void force_orthogonal(real_vector3d *forward, real_vector3d *up);
void light_get_geometry(long light_index, light_geometry *geometry);
void __tls_set_g_lights_game_globals_allocator(void *allocation);
void __tls_set_g_light_first_data_allocator(void *allocation);
void __tls_set_g_light_data_reference_allocator(void *allocation);
void __tls_set_g_light_cluster_reference_allocator(void *allocation);
void __tls_set_g_light_data_allocator(void *allocation);
extern "C" void lights_set_render_data_address(void *address);
extern "C" void lights_set_game_globals_address(void *address);
bool light_find_valid_point(const real_point3d *position, const real_point3d *endpoint, real_point3d *valid_point, float *radius);
long lights_add_structure_bsp_marker_lights(long structure_bsp_index, long marker_index);
void light_set_active_bit(long index, bool enable);
bool light_get_frustum_points(long light_index, real_point3d *out_points, long max_points);
void light_update_bounding_sphere(long light_index);
void light_calculate_lod_bounding_sphere(long light_index);
extern "C" bool light_update_position_from_attachment(light_datum *light);
void lights_initialize(void);

class c_player_view;
extern "C" void light_update_fade_intensity(long light_index, long player_index, const real_matrix4x3 *camera, float scale);
struct s_lights_attachment_definition;
union real_euler_angles2d;
struct s_lights_reference_table;
extern "C" void lights_delete_cinematic_light_entry(s_lights_reference_table *table, long index);
extern "C" void lights_compute_direction_from_angles(const real_euler_angles2d *angles, const real_matrix4x3 *matrix, real_vector3d *result, real_vector3d *up);
extern "C" long light_new_from_attachment(const s_lights_attachment_definition *definition, const real_point3d *origin,
    const real_matrix4x3 *matrix, long object_index, long marker_name);
#endif // OBJECTS_LIGHTS_H
