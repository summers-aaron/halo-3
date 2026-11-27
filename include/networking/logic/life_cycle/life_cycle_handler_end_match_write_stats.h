#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_END_MATCH_WRITE_STATS_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_END_MATCH_WRITE_STATS_H
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/write_stats_flags.h"
class c_life_cycle_state_handler_end_match_write_stats : public c_life_cycle_state_handler {
public:
    virtual const char *get_state_string();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void exit(c_life_cycle_state_handler *handler);
    virtual void enter(c_life_cycle_state_handler *previous_handler, long entry_data_size, void *entry_data);
    virtual void update();
private:
    c_write_stats_flags m_flags;
    unsigned long m_start_time;
    void update_write_stats();
};
#endif
