#ifndef AI_AI_DIALOGUE_DEFINITIONS_H
#define AI_AI_DIALOGUE_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// AI dialogue (adlg) tag layouts.
struct s_involuntary_vocalization { short vocalization_index; short pad; };
struct vocalization_pattern {
    short pad0;
    short vocalization_index;
    union {
        struct { short next_index; short pad6; };
        long reply_data;
    };
    short speaker_type;
    short listener_type;
    short relation;
    short flags;
    unsigned short target_team;
    short target_object_type;
    long target_ai_type_name;
    short speaker_object_type;
    short padding1a;
    short executing_state;
    short danger_level;
    short attitude;
    unsigned char event_unit_relation;
    unsigned char event_target_relation;
    long condition_flags;
    unsigned short spatial_relation;
    short vocalization_filter;
    unsigned short speaker_team;
    short unit_object_type;
    long unit_ai_type_name;
};
typedef char vocalization_pattern_size[(sizeof(vocalization_pattern) == 0x34) ? 1 : -1];
struct vocalization_pattern_range { short start, count; };
struct s_dialogue_reply_option {
    long data;
    short flags;
    unsigned short vocalization_index;
    short speaker_type;
    unsigned short reply_vocalization;
};
struct s_dialogue_vocalization {
    long vocalization_name;
    short parent_index;
    short priority;
    unsigned long flags;
    short glance_behavior;
    short glance_recipient_behavior;
    short sound_source_type;
    short maximum_combat_status;
    short action;
    short response_type;
    float sound_repetition_delay;
    float allowable_queue_delay;
    float pre_vocalization_delay;
    float notification_delay;
    float post_vocalization_delay;
    float cooldown;
    float weight;
    float self_interrupt_seconds;
    float target_interrupt_seconds;
    short emotion;
    short listener_emotion;
    float player_skip_fraction;
    float ally_skip_fraction;
    float skip_fraction;
    long sample_line;
    long reply_count;
    TAG_POINTER(s_dialogue_reply_option) replies;
    long reply_definition;
};
typedef char dialogue_vocalization_size[(sizeof(s_dialogue_vocalization) == 0x5c) ? 1 : -1];
typedef char dialogue_vocalization_cooldown_offset[(offsetof(s_dialogue_vocalization, cooldown) == 0x2c) ? 1 : -1];
typedef char dialogue_vocalization_replies_offset[(offsetof(s_dialogue_vocalization, reply_count) == 0x50) ? 1 : -1];
struct ai_dialogue_definitions {
    float strike_delay_minimum;
    float strike_delay_maximum;
    float remind_delay;
    float cover_curse_chance;
    c_typed_tag_block<s_dialogue_vocalization> vocalizations;
    c_typed_tag_block<vocalization_pattern> patterns;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    c_typed_tag_block<vocalization_pattern_range> ranges;
    c_typed_tag_block<s_involuntary_vocalization> involuntary_vocalizations;
};
typedef char ai_dialogue_definitions_size[(sizeof(ai_dialogue_definitions) == 0x4c) ? 1 : -1];
typedef char ai_dialogue_definitions_involuntary_offset[(offsetof(ai_dialogue_definitions, involuntary_vocalizations) == 0x40) ? 1 : -1];

typedef char dialogue_pattern_vocalization_offset[(offsetof(vocalization_pattern, vocalization_index) == 0x2) ? 1 : -1];
typedef char dialogue_pattern_condition_offset[(offsetof(vocalization_pattern, condition_flags) == 0x24) ? 1 : -1];
typedef char dialogue_definitions_vocalizations_offset[(offsetof(ai_dialogue_definitions, vocalizations) == 0x10) ? 1 : -1];
typedef char dialogue_definitions_patterns_offset[(offsetof(ai_dialogue_definitions, patterns) == 0x1c) ? 1 : -1];
typedef char dialogue_definitions_ranges_offset[(offsetof(ai_dialogue_definitions, ranges) == 0x34) ? 1 : -1];

#endif // AI_AI_DIALOGUE_DEFINITIONS_H
