#ifndef UNITS_UNIT_DIALOGUE_H
#define UNITS_UNIT_DIALOGUE_H

// Functions defined in tu/objectslibxbox/unit_dialogue.cpp that other units call.

struct ai_information_packet;
struct s_game_sound_deterministic_permutation;
struct unit_dialogue_information_packet;
struct unit_dialogue_sound_permutation;
struct unit_speech_item;

// Speech priority of a unit's current and queued dialogue (March asserts name _dialogue_priority_none).
enum e_dialogue_priority {
    _dialogue_priority_none = 0,
    _dialogue_priority_recall,
    _dialogue_priority_idle,
    _dialogue_priority_comment,
    _dialogue_priority_idle_response,
    _dialogue_priority_postcombat,
    _dialogue_priority_combat,
    _dialogue_priority_status,
    _dialogue_priority_respond,
    _dialogue_priority_warn,
    _dialogue_priority_act,
    _dialogue_priority_react,
    _dialogue_priority_involuntary,
    _dialogue_priority_scream,
    _dialogue_priority_scripted,
    _dialogue_priority_death,
    k_dialogue_priority_count
};

bool unit_can_vocalize(long unit_index, short vocalization_index, long *out_sound_index);
bool unit_test_speech(long unit_index, short speech_type, short marker_index, long *unused, long *out_sound_index);
void unit_dialogue_interrupt(long unit_index);
short unit_dialogue_get_speech_duration(short speech_type, long sound_index, const s_game_sound_deterministic_permutation *permutation);
void unit_speak(long unit_index, struct unit_speech_item * _sym_p1);
bool unit_scream(long unit_index, short scream_type, short vocalization, short speech_type, struct unit_dialogue_information_packet * information,
    struct ai_information_packet * _sym_p5);
void unit_dialogue_update(long unit_index);
bool unit_make_damage_sound(long unit_index, long damage_index, bool killed, bool violent, float body_damage, float shield_damage);
extern "C" bool unit_dialogue_test_scream(long unit_index, short scream_type, short vocalization, short speech_type,
    const unit_dialogue_sound_permutation *permutation);

#endif // UNITS_UNIT_DIALOGUE_H
