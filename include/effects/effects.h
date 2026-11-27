#ifndef EFFECTS_EFFECTS_H
#define EFFECTS_EFFECTS_H

#include "effects/event_status_types.h"
#include "math/real_math_types.h"

// Functions defined in tu/blamlibxbox/effects.cpp.

class c_light_volume_system;
struct effect_datum;
struct event_datum;
struct real_matrix4x3;
union real_point3d;
union real_vector3d;
struct s_cluster_reference;
struct s_effect_creation_data;
struct s_effect_message;
struct s_effect_vector;

const event_datum *effect_get_event(const effect_datum *effect, long event_count);
void __tls_set_g_effect_lightprobe_slot_data_allocator(void *object);
void build_gravity_effect_vectors(const real_point3d *point, s_effect_vector vectors[2]);
void __tls_set_g_effect_data_allocator(void *object);
void effect_stop(long effect_index, bool detach);
void effect_delete_internal(long effect_index, bool detach);
float effect_real_random_range(const char *name, bool deterministic, const effect_datum *effect, float minimum, float maximum);
bool effect_ping(long effect_index, long definition_index, float scale_a, float scale_b, bool force_restart);
void effect_creation_data_initialize(s_effect_creation_data *creation_data);
void effect_random_translational_velocity(const char *name, bool deterministic, const effect_datum *effect, const real_vector3d *base_direction,
    real_vector3d *direction, real_vector3d *velocity, float minimum, float maximum, float angle, unsigned long mask_a, unsigned long mask_b);
void effect_get_node_matrix(const effect_datum *effect, short node_index, real_matrix4x3 *matrix);
bool effect_update_attached_sound(long effect_index, real_point3d *point, real_vector3d *vector, float *scale);
void effect_handle_deleted_object(long effect_index);
void effect_destroy(long effect_index, bool delete_messages);
void effects_reset();
void effect_generate_lens_flares(long effect_index, bool local_space);
void effect_update_light_volume_system(c_light_volume_system *system, float delta_time);
bool dangerous_effects_near_player();
void effects_object_model_state_changed(long object_index);
void effects_notify_first_person(long user_index, long object_index, bool first_person);
void build_common_effect_vectors(const real_point3d *point, const real_vector3d *forward, const real_vector3d *up, s_effect_vector *effect_vectors);
void effects_frame_advance(float p0);
void effects_update();
long effect_new_from_creation_data(const s_effect_creation_data *creation_data);
void effect_refresh_location(long effect_index, const s_cluster_reference *cluster_reference);
void effect_handle_deleted_atmosphere(long effect_index);
void handle_effect_render_message(s_effect_message *message, long parameter);

struct effects_node_attachment;
struct real_plane3d;
struct s_effect_camera_source;
struct s_effect_contrail_profile;
class c_contrail_system;
typedef c_contrail_system s_effect_update_location;
struct s_location;

extern "C" void effect_send_render_message(const s_effect_message *message);
void effect_update_first_person_user_flags(long object_index, long user_index, bool first_person);
long effect_location_find_next_of_type(const effect_datum *effect, long location_index, short type);
void effect_set_location_vector(long effect_index, const real_vector3d *vector);
extern "C" void effect_set_camera_info(const s_effect_camera_source *source);
extern "C" int effect_spawn_impact_effects(const long *effect_indices, const real_point3d *position, const real_vector3d *normal,
    const real_vector3d *direction, const s_location *location, const real_plane3d *plane);
bool effect_attachment_update_position(effects_node_attachment *attachment);
extern "C" void effect_update_contrail_system(s_effect_update_location *record, float value);
extern "C" void effect_update_contrail_profile(s_effect_contrail_profile *profile, float delta_time);
extern "C" void effects_render_pass(long user_index, long pass);

struct c_render_batch;
struct effect_part_definition;
struct s_effect_definition;

void __tls_set_g_event_data_allocator(void *object);
void __tls_set_g_effect_message_queue_allocator(void *address);
extern "C" long event_allocate(const s_effect_definition *definition);
void effect_start_event(long effect_index, long event_index);
void effect_delete_locations(long effect_index);
bool effect_update_object(long effect_index);
extern "C" void effect_apply_accelerations(effect_datum *effect, long event_index, float elapsed_time);
void effect_start(long effect_index);
void adopt_old_effect_render_message_queue(void *allocation);
extern "C" void effect_refresh_particles(long effect_index);
extern "C" void event_refresh_particles(long effect_index, long event_index);
void effect_send_sample_lightmap_message(long effect_index);
extern "C" void effect_update_location_from_object(effect_datum *effect);
struct s_game_non_bsp_zone_set;
class c_scenario_resource_registry;
extern "C" void effects_delete_unused_for_new_map(const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set,
    c_scenario_resource_registry *pending_zone_registry);
extern "C" void effect_create_lens_flares(long effect_index, long user_index);
void event_create_particle_systems(long effect_index, long event_index);
extern "C" void effect_damage_batch_flush(c_render_batch *batch);
extern "C" void effect_update_contrail_locations(const effect_datum *effect, s_effect_update_location *record, float value);
extern "C" void effects_flush_pending_effects();
void event_generate_parts(long effect_index, long event_index, bool create);
void event_generate_part(long effect_index, long event_index, unsigned char *part_counter, const effect_part_definition *part,
    long location_index, bool unattached, const real_matrix4x3 *matrix, float scale);

union real_point2d;

bool effect_ping_from_impact(long effect_index, float scale_a, float scale_b, const real_matrix4x3 *matrix,
    const real_vector3d *velocity, const real_point2d *coordinates, const real_plane3d *plane);
extern "C" void effect_update_from_attached_particle(effect_datum *effect);
const real_argb_color effect_get_lightmap_tint(long lightprobe_index);
e_event_status effect_get_event_status(long effect_index, long event_block_index, unsigned short event_counter, float *fraction);

class c_tag_index;
struct s_damage_owner;
union real_rgb_color;
struct effect_vector_field;

long effect_new_attached_from_marker_name(c_tag_index definition_index, const s_damage_owner *damage_owner, long damage_reporting_type,
    long object_index, short node_index, long marker_name, float scale_a, float scale_b, const real_rgb_color *change_color,
    const effect_vector_field *vector_field, long region_permutation_index, long deterministic);

void effects_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);

class c_tag_index;
long effect_new_weather(c_tag_index definition_index);

extern "C" void effects_initialize(void);
void effects_dispose(void);
void effects_initialize_for_new_map(void);
void effects_dispose_from_old_map(void);
void effects_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
extern "C" void effects_process_pending_effects(unsigned long structure_bsp_mask);
#endif // EFFECTS_EFFECTS_H
