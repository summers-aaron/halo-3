#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_END_GAME_WRITE_STATS_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_END_GAME_WRITE_STATS_H
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/write_stats_flags.h"
enum e_end_game_stats_write_desire {
    _stats_write_desire_none, _stats_write_desire_not_xbox_live, _stats_write_desire_game_not_active,
    _stats_write_desire_campaign, _stats_write_desire_cooperative, _stats_write_desire_not_finalized,
    _stats_write_desire_no_results, _stats_write_desire_results_invalid, _stats_write_desire_aborted,
    _stats_write_desire_corrupt, _stats_write_desire_too_few_players, _stats_write_desire_write
};
class c_life_cycle_state_handler_end_game_write_stats : public c_life_cycle_state_handler {
public:
    virtual const char *get_state_string();
    virtual void update();
    virtual void exit(c_life_cycle_state_handler *to_handler);
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
private:
    unsigned long m_time;
    c_write_stats_flags m_flags;
    e_end_game_stats_write_desire game_get_stats_write_desire();
    void update_write_stats();
};
#endif
