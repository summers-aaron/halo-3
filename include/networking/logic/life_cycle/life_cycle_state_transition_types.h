#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_STATE_TRANSITION_TYPES_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_STATE_TRANSITION_TYPES_H

enum e_life_cycle_state_transition_type
{
    _life_cycle_state_transition_none,
    _life_cycle_state_transition_continue,
    _life_cycle_state_transition_change
};

extern "C" { extern const char k_matchmaking_start_entry_data[12]; }

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_STATE_TRANSITION_TYPES_H
