#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_MANAGER_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_MANAGER_H

// The header March 2007's asserts name for c_life_cycle_state_manager (networking\logic\life_cycle\
// life_cycle_manager.h:191..225: get_current_state() asserts "m_current_state>=0&&m_current_state<
// k_life_cycle_state_count"). Holds only the state enum so far; the manager class is still private to its units.

// One state per handler. Names: the handler classes March registers in network_life_cycle_initialize (each handler's
// initialize passes its state). Values follow handler order, with joining
// at 6 (c_life_cycle_state_handler_joining, "joining"), so from 6 on March's numbers are one lower.
enum e_life_cycle_state
{
    _life_cycle_state_none = 0,
    _life_cycle_state_pre_game,
    _life_cycle_state_start_game,
    _life_cycle_state_in_game,
    _life_cycle_state_end_game_write_stats,
    _life_cycle_state_leaving,
    _life_cycle_state_joining,
    _life_cycle_state_matchmaking_start,
    _life_cycle_state_matchmaking_find_match_client,
    _life_cycle_state_matchmaking_find_match,
    _life_cycle_state_matchmaking_find_and_assemble_match,  // 10
    _life_cycle_state_matchmaking_assemble_match,
    _life_cycle_state_matchmaking_arbitration,
    _life_cycle_state_matchmaking_select_host,
    _life_cycle_state_matchmaking_prepare_map,
    _life_cycle_state_in_match,
    _life_cycle_state_end_match_write_stats,
    _life_cycle_state_post_match,

    k_life_cycle_state_count                                // 18
};

// Functions defined in tu/networkinglibxbox/life_cycle_manager.cpp.

class c_life_cycle_state_handler;
class c_life_cycle_state_manager;
struct s_life_cycle_session_address_state;

extern "C" void life_cycle_update_session_qos_address(c_life_cycle_state_handler *handler, s_life_cycle_session_address_state *state);
extern "C" void life_cycle_manager_handle_game_end_errors(c_life_cycle_state_manager *manager);

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_MANAGER_H
