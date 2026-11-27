#ifndef GAME_CAMPAIGN_METAGAME_PLAYER_TYPES_H
#define GAME_CAMPAIGN_METAGAME_PLAYER_TYPES_H
#include <stddef.h>
// match: 0x2195d8: byte alignment preserves the identifier-table memcpy codegen.
#pragma pack(push, 1)
struct s_campaign_metagame_player_slot
{
    unsigned char key[8];
    union {
        struct { long deaths; long betrayals; };
        struct { long score_without_bonus; long bonus; long : 32; };
    };
    long score;
};
#pragma pack(pop)
typedef char campaign_metagame_player_slot_deaths_offset[(offsetof(s_campaign_metagame_player_slot, deaths) == 8) ? 1 : -1];
typedef char campaign_metagame_player_slot_betrayals_offset[(offsetof(s_campaign_metagame_player_slot, betrayals) == 0xc) ? 1 : -1];
typedef char campaign_metagame_player_slot_size[(sizeof(s_campaign_metagame_player_slot) == 0x18) ? 1 : -1];

struct s_campaign_metagame_player_globals
{
    long ticks;
    unsigned char active;
    char : 8;
    char : 8;
    char : 8;
    s_campaign_metagame_player_slot slots[4];
};
typedef char campaign_metagame_player_globals_size[(sizeof(s_campaign_metagame_player_globals) == 0x68) ? 1 : -1];
extern "C" s_campaign_metagame_player_globals g_campaign_metagame_player_globals;

#endif
