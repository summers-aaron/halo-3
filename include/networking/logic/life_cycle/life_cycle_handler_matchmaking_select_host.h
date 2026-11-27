#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_SELECT_HOST_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_SELECT_HOST_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"

class c_life_cycle_state_handler_matchmaking_select_host : public c_life_cycle_state_handler
{
public:
    c_life_cycle_state_handler_matchmaking_select_host() : flags(0) {}
    virtual const char *get_state_string();
    __forceinline void get_progress(s_life_cycle_matchmaking_progress *progress) { get_matchmaking_progress_for_setting_up_match(progress); }
    unsigned char flags;
    unsigned long start_time;
    bool attempted_delegation;
    bool require_random_delegation;
    virtual void update();
    virtual void enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data);
    void update_progress();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void exit(c_life_cycle_state_handler *next);
};

typedef char select_host_start_time_offset[
    (sizeof(void *) != 4 || ((offsetof(c_life_cycle_state_handler_matchmaking_select_host, start_time) == 0x2c))) ? 1 : -1];
typedef char select_host_handler_size[(sizeof(void *) != 4 || ((sizeof(c_life_cycle_state_handler_matchmaking_select_host) == 0x34))) ? 1 : -1];

#endif
