#ifndef GAME_MULTIPLAYER_EVENT_RESPONSE_DEFINITION_H
#define GAME_MULTIPLAYER_EVENT_RESPONSE_DEFINITION_H

#include "tag_files/tag_types.h"
#include "cseries/language_types.h"

struct s_game_engine_simulation_sound_permutation {
    unsigned short flags;
    s_tag_reference sounds[k_language_count];
    float weight;
};
struct s_multiplayer_event_response_definition {
    unsigned short flags;
    short pad2;
    short event;
    short audience;
    short pad8;
    short pad_a;
    short message_context;
    short pad_e;
    long message_string_id;
    long medal_value;
    short required_field;
    short excluded_audience;
    short type;
    short pad1e;
    long alternate_string_id;
    long delay_seconds;
    unsigned char spare_after_delay_seconds[4];
    float sound_delay;
    unsigned short sound_flags;
    s_tag_reference sounds[k_language_count];
    unsigned char spare_after_sounds[4];
    long sound_permutation_count;
    TAG_POINTER(s_game_engine_simulation_sound_permutation) sound_permutations;
    TAG_POINTER(void) sound_permutation_definition;
};

#endif
