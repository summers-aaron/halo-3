#ifndef GAME_CAMPAIGN_METAGAME_LEVEL_TYPES_H
#define GAME_CAMPAIGN_METAGAME_LEVEL_TYPES_H

// The one-byte result of the level search: 0..3, 4 when none.
struct c_level
{
    c_level() {}
    c_level(signed char v) : value(v) {}
    signed char value;
};

typedef char campaign_metagame_level_size[(sizeof(c_level) == 1) ? 1 : -1];

#endif // GAME_CAMPAIGN_METAGAME_LEVEL_TYPES_H
