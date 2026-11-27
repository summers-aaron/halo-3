#ifndef AI_DIALOGUE_EVENT_TYPES_H
#define AI_DIALOGUE_EVENT_TYPES_H

// March g_dialogue_names (0x83d6f220), indexed by dialogue event type.
enum e_dialogue_event_type
{
    _dialogue_event_evict_driver = 62,
    _dialogue_event_evict_gunner = 63,
    _dialogue_event_evict_passenger = 64,
    _dialogue_event_curse = 78,
    _dialogue_event_threaten = 79,
    _dialogue_event_strike = 81,
    _dialogue_event_gloat = 83,
    _dialogue_event_player_look = 85,
    _dialogue_event_player_look_longtime = 86,
    _dialogue_event_weapon_trade_better = 90,
    _dialogue_event_weapon_trade_worse = 91,
    _dialogue_event_weapon_trade_equal = 92,
    _dialogue_event_betray = 93,
    _dialogue_event_forgive = 94,
    _dialogue_event_reanimate = 100,
    _dialogue_event_thank = 105,
    _dialogue_event_ok = 106
};

#endif // AI_DIALOGUE_EVENT_TYPES_H
