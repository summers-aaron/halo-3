#ifndef SOUND_SOUND_EFFECT_MANAGER_H
#define SOUND_SOUND_EFFECT_MANAGER_H

template<class t_datum> class c_smart_data_array;

struct s_sound_effect_datum;
extern c_smart_data_array<s_sound_effect_datum> g_sound_effect_data;

// Functions defined in tu/blamlibxbox/sound_effect_manager.cpp.

struct s_sound_impulse_definition;

long sound_effect_manager_create_impulse(long sound_index, s_sound_impulse_definition *impulse);
void sound_effect_stop_internal(long effect_index);
long sound_effect_manager_create_manual(long playback_index, float scale);
void sound_effects_update();
extern "C" void sound_effect_manager_stop_all(void);

struct s_sound_event_callbacks;
struct s_sound_source;
struct sound_effect_manager_definition;

extern "C" long sound_effect_new(const sound_effect_manager_definition *definition);

extern "C" bool sound_playback_has_sound_effect_components(long sound_index, long playback_index, const sound_effect_manager_definition **out_definition);
void sound_effect_get_source(long sound_effect_index, s_sound_source *sound_source);
void sound_effect_set_track_data(long sound_effect_index, s_sound_impulse_definition *impulse_definition);

enum e_sound_effect_refresh_state
{
    _sound_effect_refresh_start = 0,
    _sound_effect_refresh_update = 1,
    _sound_effect_refresh_stop = 2
};
void sound_effect_update_components_internal(long effect_index, e_sound_effect_refresh_state refresh_state);
extern "C" const s_sound_event_callbacks k_sound_effect_impulse_callbacks;

#endif // SOUND_SOUND_EFFECT_MANAGER_H
