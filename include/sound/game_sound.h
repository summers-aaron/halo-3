#ifndef SOUND_GAME_SOUND_H
#define SOUND_GAME_SOUND_H

// Functions defined in tu/blamlibxbox/game_sound.cpp.

union real_point3d;
union real_vector3d;
struct s_game_sound_impulse_internal_parameters;
struct s_location;
struct s_sound_location;
struct s_sound_source;
struct real_decibel;

void game_sound_disable_at_game_finish();
void game_sound_restore(long flags);
long game_looping_sound_attachment_new(long object_index, long definition_index, long attachment_index, long identifier);
void __tls_set_g_game_sound_scripted_impulses_allocator(void *allocation);
long scripted_sound_time_legacy_do_not_use(long definition_index);
long scripted_sound_language_time_explicit_hs_ticks(long definition_index);
long scripted_sound_time_explicit_game_ticks(long definition_index, long hs_ticks);
void scripted_sound_stop(long definition_index);
long impact_looping_sound_start(long p0, long p1);
long scripted_looping_sound_get_sound_index(long owner);
void scripted_looping_sound_start_with_effect(long definition_index, long object_index, float scale, long effect_label);
float unattached_looping_sound_get_amplitude(long looping_sound_index);
long game_sound_impulse_new_internal(long sound_index, s_game_sound_impulse_internal_parameters *parameters);
void game_sound_reset_at_round_finish();
long unspatialized_impulse_sound_new(long sound_index, float scale);
long unattached_looping_sound_start(long definition_index, long object_index, float scale);
long unattached_impulse_sound_new(long handle, const s_sound_location *location, float volume);
void scripted_sound_new_3d(long sound_index, float angle, float volume);
bool projectile_flyby_sound_try_new(long user_index, long owner_player_index, long definition_index,
    const real_point3d *projectile_position, const real_vector3d *travel_distance, const s_location *projectile_location);
void scripted_sound_trigger(long definition_index, long object_index, float scale, long count);
void scripted_sound_new_internal(long definition_index, long object_index, float scale,
    real_decibel gain, real_decibel distance, long playback_label, float *playing_time);
long object_impulse_sound_new(long object_index, long definition_index, short node_index,
    const real_point3d *point, const real_vector3d *forward, float volume);
long unattached_looping_sound_start_with_effect(long definition_index, long object_index, float scale, long effect_label);
void game_looping_sound_get_source(long looping_sound_index, s_sound_source *sound_source_param);
void game_sound_update(float elapsed_time);
extern "C" void game_sound_location_set_rotation_from_direction(s_sound_location *sound_location, const real_vector3d *vector);
extern "C" bool game_sound_new_material_effect_sounds(long *definition_indices, const real_point3d *position, const real_vector3d *normal,
    const real_vector3d *direction, const long *location, long user_index, float volume);
extern "C" long game_sound_compute_group_item_offset(long count, long offset);
extern "C" void scripted_sound_set_outro_flag(long definition_index);
extern "C" void game_sound_update_timeline(void);
extern "C" long object_impulse_sound_new_at_location(long object_index, long sound_index, const s_sound_location *location, float volume);

struct s_game_sound_deterministic_permutation;
struct s_sound_attachment_data;

void game_sound_clear(long reason);
void __tls_set_g_game_looping_sound_data_allocator(void *allocation);
void __tls_set_g_game_sound_globals_allocator(void *allocation);
extern "C" void game_sound_set_scripted_impulses_address(void *address);
extern "C" bool object_unattached_impulse_sound_new_internal(long user_index, long definition_index, long *out_listener_index);
bool sound_attachment_is_sky(long object_index);
bool looping_sound_definition_is_music(long definition_index);
void update_spatialization_if_first_person(long object_index, char *spatialization, long *user_index);
void data_mine_insert_sp_audio_event(const char *event_name, long definition_index, long object_index, float scale);
extern "C" void game_sound_register_shared_empty_destructor(void);
long scripted_sound_create_internal(long sound_index, const s_game_sound_deterministic_permutation *permutation);
void game_sound_impulses_update(void);
long first_person_impulse_sound_new_internal(long listener_index, long object_index, long sound_index,
    bool force_center, const real_point3d *point, float volume);
void compute_combined_pas(void);
bool track_object_impulse_sound(long object_index, long sound_index, const s_sound_attachment_data *attachment, s_sound_source *source);
bool track_scripted_impulse_sound(long object_index, long definition_index, const s_sound_attachment_data *attachment, s_sound_source *source);
void game_sound_update_listeners(float delta);

struct s_portal_reference;
struct s_cluster_reference;
struct s_sound_tracker;
struct s_sound_attachment_data;
struct s_music_identifier_generator;
template<long k_maximum_count> class c_static_flags;
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;

s_portal_reference game_sound_find_closest_portal(const real_point3d *position, s_cluster_reference cluster, float *out_closest_distance);
void object_impulse_playing_in_channel(long, long, long, void *);
inline void scripted_looping_sound_reset_internal(long definition_index, long sound_index);
extern "C" bool are_sound_attachments_equivalent(const s_sound_attachment_data *a, const s_sound_attachment_data *b);

extern const s_sound_tracker k_object_impulse_sound_trackers[];
extern c_static_flags<256> g_combined_pas;
extern c_static_flags<32> g_valid_skies;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_looping_sound_data_allocator> g_game_looping_sound_data_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_globals_allocator> g_game_sound_globals_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_scripted_impulses_allocator> g_game_sound_scripted_impulses_allocator;
extern "C" {
extern s_music_identifier_generator g_game_sound_identifier_generator;
extern const s_sound_tracker k_scripted_impulse_sound_tracker;
}

void game_sound_initialize();
void game_sound_dispose();
void game_sound_initialize_for_new_map();
void game_sound_dispose_from_old_map();
void game_sound_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);

void game_sound_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
#endif // SOUND_GAME_SOUND_H
