#ifndef AI_PROP_DATUM_H
#define AI_PROP_DATUM_H

#include <stddef.h>

#include "ai/prop_state.h"
#include "ai/prop_view.h"

// Remembered search areas: behavior pursuit_discard_area/pursuit_area_discarded
// read five packed zone/area pairs at +0x3a and the current area at +0x50.
#pragma pack(push, 2)
union s_prop_search_area
{
    struct { short zone_index; short area_index; };
    long value;
};
#pragma pack(pop)

// A single AI prop: salience, ownership, dialogue, and search state.
struct prop_datum {
    short salt;
    short type;
    short status;
    short pad06;
    long object_index;
    float importance;
    float maximum_salience;
    short count;
    short pad16;
    long expiration_time;
    long next_prop;
    union {
        long first_prop_ref_index;
        long first;
    };
    union {
        long actor_index;
        long owner_index;
    };
    short team;                                   // (clump_new_prop_init, actor_select_prop)
    bool owned_by_actor;
    union {
        bool valid;
        bool active;
        bool dead;
    };
    bool allegiance_changed;
    union {
        unsigned char flag;
        bool hostile;
        bool enemy;
        bool player;
    };
    union {
        bool ignore;
        bool dead_2e;
    };
    char pad2f;
    short flags;
    unsigned char dialogue_flags;
    unsigned char previous_dialogue_flags;
    short ineffability_ticks;                     // (prop_status_get_ineffability)
    short comment_ticks;
    short lost_ticks;
    s_prop_search_area discarded_areas[5];
    signed char next_discarded_area;
    signed char discarded_area_count;
    s_prop_search_area search_area;
    prop_state state;
};
typedef prop_datum s_prop_datum;

typedef char prop_datum_size[(sizeof(prop_datum) == 0xc4) ? 1 : -1];
typedef char prop_datum_off_type[(offsetof(prop_datum, type) == 2) ? 1 : -1];
typedef char prop_datum_off_team[(offsetof(prop_datum, team) == 0x28) ? 1 : -1];
typedef char prop_datum_off_ineffability_ticks[(offsetof(prop_datum, ineffability_ticks) == 0x34) ? 1 : -1];

typedef char prop_datum_discarded_areas_offset[(offsetof(prop_datum, discarded_areas) == 0x3a) ? 1 : -1];
typedef char prop_datum_search_area_offset[(offsetof(prop_datum, search_area) == 0x50) ? 1 : -1];

// March: c_game_state_consumer_datum_array<tracking_datum> over props.obj's tracking_data (TLS 0x831df768, 0x100-byte
// datums). prop_state_get returns &state when the prop ref has a view; prop_view_get returns the view bytes after it
// (the byte-array alias is retained for existing readers).
struct tracking_datum {
    short identifier;                             // datum salt
    short pad2;
    prop_state state;
    union
    {
        char view[0x100 - 0x74];
        prop_view tracking_view;
    };
};

typedef char tracking_datum_size[(sizeof(tracking_datum) == 0x100) ? 1 : -1];
typedef char tracking_datum_off_state[(offsetof(tracking_datum, state) == 4) ? 1 : -1];

typedef char tracking_datum_off_view[(offsetof(tracking_datum, tracking_view) == 0x74) ? 1 : -1];

#endif // AI_PROP_DATUM_H
