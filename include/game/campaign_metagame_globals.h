#ifndef GAME_CAMPAIGN_METAGAME_GLOBALS_H
#define GAME_CAMPAIGN_METAGAME_GLOBALS_H

#include <stddef.h>
#include "memory/static_arrays.h"

// match: 0x219420: byte alignment keeps the identifier copy as a byte loop instead of two word copies.
#pragma pack(push, 1)
struct s_campaing_metagame_player
{
  unsigned char identifier[8];
  long pending;
  union { long total; long awarded_bonus; };
  union { long bonus; long bonus_difference; };
  long score;
  union {
    long values[28][8];
    c_static_array<c_static_array<long, 8>, 28> bucket_kills;
  };
  union { long extra[8]; c_static_array<long, 8> style_kills; };
  long ai_kills;
  long timer0;
  long points;
  long timer1;
  long last_delta;
  long timer2;
  unsigned short sums[8];
  long total_kills;
  long extra_total;
  unsigned char valid;
  unsigned char : 8;
  unsigned char : 8;
  unsigned char : 8;
  long scaled;
};

#pragma pack(pop)

// The first eight bytes are copied from the object's metagame tag definition.
struct s_campaign_metagame_damage_definition
{
  unsigned char flags;
  signed char category;
  signed char kind;
  char reserved3;
  short score;
  short reserved6;
};


struct s_campaing_metagame_damage_event {
  union {
    s_campaign_metagame_damage_definition definition;
    struct {
      unsigned char flags; signed char category; signed char kind; char reserved3;
      short score; short reserved6;
    };
  };
  long time;
  long player_index;
  signed char style;
  char reserved11;
  bool multikill;
};

struct s_campaing_metagame_globals
{
    char pad0[0x68];
    s_campaing_metagame_player players[4];
    c_static_array<s_campaing_metagame_damage_event, 256> events;
    long event_count;
    long flags;
    float time_multiplier;
    float final_score;
};

typedef char campaign_metagame_damage_definition_size[(sizeof(s_campaign_metagame_damage_definition) == 8) ? 1 : -1];
typedef char campaign_metagame_damage_event_size[(sizeof(s_campaing_metagame_damage_event) == 0x14) ? 1 : -1];
typedef char campaign_metagame_event_multikill_offset[(offsetof(s_campaing_metagame_damage_event, multikill) == 0x12) ? 1 : -1];
typedef char campaign_metagame_player_score_offset[(offsetof(s_campaing_metagame_player, score) == 0x14) ? 1 : -1];
typedef char campaign_metagame_player_buckets_offset[(offsetof(s_campaing_metagame_player, bucket_kills) == 0x18) ? 1 : -1];
typedef char campaign_metagame_player_timer_offset[(offsetof(s_campaing_metagame_player, timer2) == 0x3cc) ? 1 : -1];
typedef char campaign_metagame_player_scaled_offset[(offsetof(s_campaing_metagame_player, scaled) == 0x3ec) ? 1 : -1];
typedef char campaign_metagame_player_size[(sizeof(s_campaing_metagame_player) == 0x3f0) ? 1 : -1];
typedef char campaign_metagame_events_offset[(offsetof(s_campaing_metagame_globals, events) == 0x1028) ? 1 : -1];
typedef char campaign_metagame_flags_offset[(offsetof(s_campaing_metagame_globals, flags) == 0x242c) ? 1 : -1];
typedef char campaign_metagame_globals_size[(sizeof(s_campaing_metagame_globals) == 0x2438) ? 1 : -1];

extern "C" __declspec(thread) s_campaing_metagame_globals *g_campaing_metagame_globals;

#endif
