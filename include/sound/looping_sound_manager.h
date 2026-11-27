#ifndef SOUND_LOOPING_SOUND_MANAGER_H
#define SOUND_LOOPING_SOUND_MANAGER_H

// Functions defined in tu/blamlibxbox/looping_sound_manager.cpp.

#include "sound/sound_event_callbacks.h"

struct s_sound_source;

void looping_sound_delete_all_of_sound_class(long sound_class);
void looping_sound_manager_reset();
float looping_sound_try_to_get_mouth_apeture(long tag_index, long identifier, long unused, const s_sound_source *source);
void process_looping_sounds(bool p0);

struct loop_impulse_sound_tracking_data;
struct looping_sound_datum;
struct looping_sound_definition;
struct looping_sound_detail;
struct looping_sound_track_datum;
struct s_cache_file_sound_definition;
struct s_sound_impulse_definition;

long looping_sound_find_sound(long owner_index, long key, long type, short slot);
bool track_loop_detail_impulse_sound(long loop_index, long sound_index, const loop_impulse_sound_tracking_data *tracking, s_sound_source *source);
bool are_detail_sounds_equivalent(const loop_impulse_sound_tracking_data *a, const loop_impulse_sound_tracking_data *b);
bool track_loop_track_sound(long looping_sound_index, long definition_index, const void *unknown, s_sound_source *out_source);
bool looping_sound_update_output_properties(long loop_index, long definition_index, const void *source, s_sound_output_parameters *parameters);
void looping_sound_stop(long loop_index, long sound_index, e_sound_event_stop_reason stop_reason);
unsigned long looping_sound_track_get_range_permutation_flags(s_cache_file_sound_definition *sound_definition, looping_sound_definition *definition,
    looping_sound_datum *loop, long track_index, short pitch_range_index, bool reset);
void looping_sound_track_set_range_permutation_flags(s_cache_file_sound_definition *sound_definition, looping_sound_definition *definition,
    looping_sound_datum *loop, long track_index, short pitch_range_index, short last_permutation_index, unsigned long flags);
long looping_sound_track_get_last_playing_permutation_index(s_cache_file_sound_definition *sound_definition, looping_sound_definition *definition,
    long pitch_range_index);
long looping_sound_find(long definition_index, long sound_identifier);
bool sound_commit_looping_definition(struct looping_sound_datum * loop, struct looping_sound_track_datum * track, long index, unsigned long * random);
bool detail_sound_build_impulse_definition(looping_sound_detail *detail, long loop_index, s_sound_impulse_definition *impulse);

enum e_looping_sound_refresh_state
{
    _looping_sound_refresh_start = 0,
    _looping_sound_refresh_update = 1,
    _looping_sound_refresh_stop = 2,
    _looping_sound_refresh_inaudible = _looping_sound_refresh_start,
    _looping_sound_refresh_audible = _looping_sound_refresh_update,
    _looping_sound_refresh_stopped = _looping_sound_refresh_stop,
    _looping_sound_refresh_3 = 3
};

bool sound_refresh_looping(long definition_index, unsigned long flags, long identifier, long source_index, long playback_index,
    const s_sound_source *source, e_looping_sound_refresh_state state, unsigned long extra_flags, float fade);

#endif // SOUND_LOOPING_SOUND_MANAGER_H
