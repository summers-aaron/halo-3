#ifndef AI_PROP_REF_DATUM_H
#define AI_PROP_REF_DATUM_H

#include <stddef.h>

#include "cseries/cseries_wrapped.h"

enum e_prop_ref_flags;

struct prop_ref_datum {
    short identifier;                             // (datum salt)
    signed char type;
    signed char dialogue_event;
    unsigned char dialogue_index;
    unsigned char dialogue_ticks;
    union {
        unsigned char stimulus;                   // prop_get_visibility compares with 255
        signed char stimulus_type;                // actor_perception stores the signed _prop_stimulus_none (-1)
    };
    unsigned char stimulus_decay_ticks;
    long actor_index;
    long prop_index;
    long object_index;
    union {
        long view_index;
        long owner_index;
    };
    float accumulated_awareness;                 // (actor_props_update integrates get_awareness_delta)
    union {
        signed char knowledge_state;
        signed char sighting_state;
        signed char awareness_class;
        signed char status;
    };
    signed char maximum_awareness;                // (prop_perceive, actor_prop_refresh_status)
    union {
        signed char flags_1e;
        // awareness is the high nibble (actor_type_scarab.cpp, actor_type_flood.cpp: flags_1e << 24 & 0xf0000000),
        // visibility_type the low one (ai_player.cpp: flags_1e << 28); MSB-first on the console, LSB-first on x86.
        struct {
#if defined(_XBOX) && !defined(__clang__)
            signed char awareness : 4;
            signed char visibility_type : 4;
#else
            signed char visibility_type : 4;
            signed char awareness : 4;
#endif
        };
    };
    union {
        unsigned char flags;
        // March clump_dialogue_update 82f97588 calls this specialization's set/test.
        c_flags_no_init<e_prop_ref_flags, unsigned char, 3> m_flags;
    };
    short status_refresh_ticks;
    short state_refresh_ticks;
    long next_prop_ref_index;
    long previous_prop_ref_index;                 // (actor's doubly linked list)
    long prop_next_ref_index;
    long prop_previous_ref_index;                 // (prop's doubly linked list)
    float distance;
    union {
        float track_weight;
        float salience;
    };
};

typedef char prop_ref_datum_size[(sizeof(prop_ref_datum) == 0x3c) ? 1 : -1];
typedef char prop_ref_datum_off_dialogue_event[(offsetof(prop_ref_datum, dialogue_event) == 0x03) ? 1 : -1];
typedef char prop_ref_datum_off_dialogue_index[(offsetof(prop_ref_datum, dialogue_index) == 0x04) ? 1 : -1];
typedef char prop_ref_datum_off_dialogue_ticks[(offsetof(prop_ref_datum, dialogue_ticks) == 0x05) ? 1 : -1];
typedef char prop_ref_datum_off_previous_ref[(offsetof(prop_ref_datum, previous_prop_ref_index) == 0x28) ? 1 : -1];
typedef char prop_ref_datum_off_prop_previous_ref[(offsetof(prop_ref_datum, prop_previous_ref_index) == 0x30) ? 1 : -1];
typedef char prop_ref_datum_off_flags[(offsetof(prop_ref_datum, m_flags) == 0x1f) ? 1 : -1];
typedef char prop_ref_datum_off_stimulus_ticks[(offsetof(prop_ref_datum, stimulus_decay_ticks) == 0x07) ? 1 : -1];
typedef char prop_ref_datum_off_awareness[(offsetof(prop_ref_datum, accumulated_awareness) == 0x18) ? 1 : -1];
typedef char prop_ref_datum_off_maximum_awareness[(offsetof(prop_ref_datum, maximum_awareness) == 0x1d) ? 1 : -1];
typedef char prop_ref_datum_off_status_refresh[(offsetof(prop_ref_datum, status_refresh_ticks) == 0x20) ? 1 : -1];
typedef char prop_ref_datum_off_state_refresh[(offsetof(prop_ref_datum, state_refresh_ticks) == 0x22) ? 1 : -1];

#endif // AI_PROP_REF_DATUM_H
