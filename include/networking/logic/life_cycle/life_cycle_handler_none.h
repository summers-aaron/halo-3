#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_NONE_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_NONE_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"

class c_life_cycle_state_handler_none : public c_life_cycle_state_handler
{
public:
    virtual void update();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
    virtual void exit(c_life_cycle_state_handler *to_handler);
    virtual const char *get_state_string();
};

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_NONE_H
