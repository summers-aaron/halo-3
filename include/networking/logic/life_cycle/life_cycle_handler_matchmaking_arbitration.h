#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_MATCHMAKING_ARBITRATION_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_MATCHMAKING_ARBITRATION_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/online/online_arbitration_types.h"

class c_life_cycle_state_handler_matchmaking_arbitration : public c_life_cycle_state_handler {
public:
    c_life_cycle_state_handler_matchmaking_arbitration() : arbitration_flags(0) {}
    virtual void update();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
    virtual void exit(c_life_cycle_state_handler *next_handler);
    __forceinline void get_progress(s_life_cycle_matchmaking_progress *progress) { get_matchmaking_progress_for_setting_up_match(progress); }
    virtual const char *get_state_string();
private:
    void mark_arbitration_complete();
    void update_start_match();
    void update_arbitration();
    void update_initial_stats_write();
    bool ready_to_start();
    bool session_membership_matches_arbitration_registration_results();
    void update_repeated_play();
    unsigned int arbitration_flags;
    unsigned long arbitration_start_time;
    unsigned long completion_start_time;
    s_online_arbitration_result arbitration_result;
};

typedef char matchmaking_arbitration_handler_size[(sizeof(void *) != 4 || ((sizeof(c_life_cycle_state_handler_matchmaking_arbitration) == 0x1c0))) ? 1 : -1];

#endif
