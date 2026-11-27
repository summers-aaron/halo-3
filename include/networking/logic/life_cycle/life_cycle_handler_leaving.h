#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_LEAVING_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_LEAVING_H
#include "networking/logic/life_cycle/life_cycle_handler_types.h"

class c_life_cycle_state_handler_leaving : public c_life_cycle_state_handler
{
public:
    c_life_cycle_state_handler_leaving() : m_flags(0) {}
    // The joining-only previous state is not part of this object.
    unsigned char m_flags;
    char m_leaving_41;
    long m_game_mode;
public:
    virtual const char *get_state_string();
    virtual void update();
    virtual void exit(c_life_cycle_state_handler *to_handler);
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
    virtual e_life_cycle_state_transition_type update_for_state_transition();
};
#endif
