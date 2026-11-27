#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_JOINING_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_JOINING_H
#include "networking/logic/life_cycle/life_cycle_handler_types.h"

class c_life_cycle_state_handler_joining : public c_life_cycle_state_handler
{
public:
    c_life_cycle_state_handler_joining() : m_flags(0) {}
    // Entering this state records the game mode and the previous state.
    unsigned char m_flags;
    long m_game_mode;
    e_life_cycle_state m_previous_state;
    virtual void update();
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual const char *get_state_string();
};
#endif
