#ifndef SOUND_SOUND_EVENT_CALLBACKS_H
#define SOUND_SOUND_EVENT_CALLBACKS_H
#include <stddef.h>
// Order and names from the March 2007 debug table k_stopped_sound_reasons (sound_manager_events.obj).
// The sound manager also stops sounds with reasons 19 and 20, which that table does not name.
enum e_sound_event_stop_reason
{
    _sound_event_stop_reason_unknown = 0,
    _sound_event_stop_reason_scripting,
    _sound_event_stop_reason_ended,
    _sound_event_stop_reason_refresh_stopped,
    _sound_event_stop_reason_fade_out,
    _sound_event_stop_reason_too_many_same_attached_to_object,
    _sound_event_stop_reason_too_many_same_playing,
    _sound_event_stop_reason_no_free_channel,
    _sound_event_stop_reason_killed_by_loop,
    _sound_event_stop_reason_preempted_less_important,
    _sound_event_stop_reason_all,
    _sound_event_stop_reason_dialogue_game_over,
    _sound_event_stop_reason_dialogue_game_over_to_player,
    _sound_event_stop_reason_preempted_speech,
    _sound_event_stop_reason_not_cached,
    _sound_event_stop_reason_promotion,
    _sound_event_stop_reason_not_valid_for_platform,
    _sound_event_stop_reason_wrong_format_for_loop,
    _sound_event_stop_reason_not_playable_anymore,
    _sound_event_stop_reason_io_failure,
    _sound_event_stop_reason_cinematic_skipped,
};
struct s_cache_file_sound_permutation;
struct s_sound_output_parameters;
struct s_sound_source;

// Eight callback slots; the signatures are the March 2007 sound_effect_manager.obj callbacks that fill them.
struct s_sound_event_callbacks
{
    bool (*tracking_proc)(long object_index, long definition_index, const void *parameters, s_sound_source *source);
    void (*start_proc)(long object_index, long definition_index, long sound_index, void *parameters);
    void (*update_proc)(long object_index, const void *event_parameters, long definition_index, long pitch_range_index,
        const s_cache_file_sound_permutation *permutation, float channel_position);
    bool (*spatialization_proc)(long object_index, long definition_index, const void *parameters, s_sound_output_parameters *output);
    void (*stop_proc)(long object_index, long sound_index, e_sound_event_stop_reason stop_reason);
    void (*impulse_stop_proc)(long object_index, long sound_index);
    // The looping-sound detail table fills this slot with are_detail_sounds_equivalent (two parameter blocks -> bool).
    bool (*reserved_proc)(const void *parameters, const void *other_parameters);
    bool (*compare_proc)(long object_index, const void *parameters, long other_object_index, const void *other_parameters);
};
typedef char sound_event_callbacks_size[(sizeof(void *) != 4 || sizeof(s_sound_event_callbacks) == 0x20) ? 1 : -1];
typedef char sound_event_update_offset[(sizeof(void *) != 4 || offsetof(s_sound_event_callbacks, update_proc) == 8) ? 1 : -1];
typedef char sound_event_spatialization_offset[(sizeof(void *) != 4 || offsetof(s_sound_event_callbacks, spatialization_proc) == 12) ? 1 : -1];
#endif
