#ifndef SOUND_SOUND_MANAGER_H
#define SOUND_SOUND_MANAGER_H

// Functions defined in tu/blamlibxbox/sound_manager.cpp.

union real_point3d;
struct s_cache_file_sound_permutation;
struct s_sound_channel_properties;
struct s_sound_location;
struct s_sound_output_parameters;
struct s_sound_source;
struct sound_channel_summary;
struct sound_datum;
struct s_sound_manager_globals;

extern s_sound_manager_globals *g_sound_manager_globals;

bool sound_definition_is_valid(long tag_index);
float source_distance_squared(long listener_index, const s_sound_source *source);
void channel_signal_complete(short channel_index);
void sound_dispose();
short sound_find_like_channel(long sound_index, short *channel_indices, short channel_count);
void sound_channel_summary_build(sound_channel_summary *out_summary, long sound_index);
long source_audible_internal(const s_sound_source *source, float minimum_distance, float maximum_distance, float *out_distance);
bool sound_make_permutation_ready(sound_datum *datum, bool *out_ready);
void channel_initialize(short channel_index, const s_sound_channel_properties *properties);
void scripted_sound_predict(long definition_index);
void sound_stop_impulse(long sound_index);
void channel_stop(short channel_index);
void channel_properties_initialize_from_sound(short channel_index, const sound_datum *sound,
    const s_sound_output_parameters *output, s_sound_channel_properties *properties);
bool sound_definition_hardware_formats_match(long definition_index_a, long definition_index_b);
float calculate_doppler_shift_cents(long listener_index, const s_sound_location *sound_location);
void channel_queue_sound(short channel_index, struct s_cache_file_sound_permutation * _sym_p1, short chunk_index);
void sound_calculate_listener_relative_position_and_attenuation(long listener_index, const s_sound_source *source,
    long sound_index, float *out_attenuation, float *out_cone_attenuation, real_point3d *out_position);
void sound_idle(void);
void sound_render(void);
void refresh_listener(bool update_platform);
void sound_initialize();
extern "C" void sound_reset_promotions(void);
extern "C" void sound_set_permutation(long sound_index, long definition_index, short pitch_range_index, short permutation_index, short chunk_index);
extern "C" void sound_datum_unlock(void *sound);

struct real_decibel;
struct s_sound_environment_parameters;
struct s_sound_impulse_definition;

void sound_datum_lock(sound_datum *datum);
void sound_initialize_for_new_map();
void sound_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
void reset_platform_sound_mix();
extern "C" unsigned char sound_update_tracking(long sound_index);
void refresh_channels(void);
void channel_refresh_state(short channel_index);
void channel_restore_from_pause(short channel_index);
long sound_impulse_new_internal(long definition_index, long listener_index, const s_sound_impulse_definition *impulse, long promotion_index);
void channel_refresh_location(short channel_index, const s_sound_environment_parameters *environment);
void sound_stop_all(long flags);
void update_channels(void);
void channel_update_spatialization(short channel_index, real_decibel *gain, s_sound_output_parameters *output);
extern "C" void sound_update_time(void);
float normalize_and_clamp_linear(float lower, float upper, float value, bool reverse);

// Data defined in tu/blamlibxbox/sound_manager.cpp that other units use.

struct sound_channel_datum;

extern sound_channel_datum *g_sound_channels;

enum sound_event_fade_reason;
enum e_sound_event_stop_reason;
enum e_sound_impulse_new_failure_reason;
real_decibel sound_calculate_fade_db(long sound_index);
void sound_start_fade(short, sound_event_fade_reason, float, long, long);
void sound_stop(long sound_index, e_sound_event_stop_reason stop_reason);
long sound_impulse_new(long definition_index, const s_sound_impulse_definition *impulse, e_sound_impulse_new_failure_reason *out_failure_reason);

void sound_dispose_from_old_map(void);
void sound_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);

#endif // SOUND_SOUND_MANAGER_H
