#ifndef UNITS_UNIT_DIALOGUE_INTERNALS_H
#define UNITS_UNIT_DIALOGUE_INTERNALS_H

#include <stddef.h>
#include <string.h>
#include "cseries/cseries.h"
#include "tag_files/tag_types.h"
#include "ai/team_dialogue_state.h"
#include "sound/game_sound_deterministic_permutation.h"

extern "C" void ai_dialogue_notify_vocalization(
    long unit_index, short vocalization_index, long record_index, char *information);

struct s_vocalization_entry
{
    char header_storage[0x14];
    long sound_index;
};

struct s_unit_dialogue_definition
{
    char header_storage[0x14];
    s_tag_block vocalizations;
};

struct unit_dialogue_information_packet
{
    short type;
    short flags;
    long subject;
    long cause;
    long target;
    long extra0;
    long extra1;
    long extra2;
    long extra3;
};

struct unit_dialogue_speech_item
{
    short vocalization;
    c_enum_no_init<long, signed char, 0, 16> type;
    bool deterministic;
    unsigned short permutation;
    short permutation_flags;
    long dialogue_index;
    long sound_index;
    short notify_delay;
    short delay;
    unit_dialogue_information_packet information;

    __forceinline unit_dialogue_speech_item() { memset(this, 0, sizeof(*this)); }
    __forceinline unit_dialogue_speech_item(const unit_dialogue_speech_item &other)
    {
        dialogue_index = other.dialogue_index;
        vocalization = other.vocalization;
        type = other.type.get();
        deterministic = other.deterministic;
        permutation = other.permutation;
        permutation_flags = other.permutation_flags;
        sound_index = other.sound_index;
        notify_delay = other.notify_delay;
        delay = other.delay;
        information.type = other.information.type;
        information.flags = other.information.flags;
        information.subject = other.information.subject;
        information.cause = other.information.cause;
        information.target = other.information.target;
        information.extra0 = other.information.extra0;
        information.extra1 = other.information.extra1;
        information.extra2 = other.information.extra2;
        information.extra3 = other.information.extra3;
    }
};

struct unit_dialogue_vocalization_record
{
    short identifier;
    short vocalization;
    long unit_index;
    long sound_index;
    short team;
    signed char response;
    long queued_time;
    long start_time;
    short minimum_delay;
    short speech_delay;
    short duration;
    short window;
    s_game_sound_deterministic_permutation permutation;
    short speech_type;
    long clump_index;
    unit_dialogue_information_packet information;
    signed char keep_without_secondary_object;
    bool interrupted;
    long response_index;
    bool marked_for_deletion;
    long dialogue_variant;
    long next;
};
typedef char dialogue_vocalization_record_size[(sizeof(unit_dialogue_vocalization_record) == 0x5c) ? 1 : -1];
typedef char dialogue_vocalization_record_deletion_offset[(offsetof(unit_dialogue_vocalization_record, marked_for_deletion) == 0x50) ? 1 : -1];

struct unit_dialogue_update_state
{
    long dialogue_index;
    long dialogue_state;
    long playback_label;
    unit_dialogue_speech_item current;
    short pain_time;
    short pain_count;
    short minor_time;
    short major_time;
    long timer_state;
    bool started;
    bool notified;
    bool finished;
    char status_alignment;
    short duration;
    short notify_delay;
    short finish_delay;
    short delay_alignment;
    long sound_handle;
};

typedef char dialogue_item_size[(sizeof(unit_dialogue_speech_item) == 0x34) ? 1 : -1];
typedef char dialogue_state_size[(sizeof(unit_dialogue_update_state) == 0x5c) ? 1 : -1];
typedef char dialogue_current_offset[(offsetof(unit_dialogue_update_state, current) == 0xc) ? 1 : -1];
typedef char dialogue_started_offset[(offsetof(unit_dialogue_update_state, started) == 0x4c) ? 1 : -1];
typedef char dialogue_duration_offset[(offsetof(unit_dialogue_update_state, duration) == 0x50) ? 1 : -1];

#endif // UNITS_UNIT_DIALOGUE_INTERNALS_H
