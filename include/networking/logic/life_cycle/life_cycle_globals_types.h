#ifndef NETWORKING_LOGIC_LIFE_CYCLE_GLOBALS_TYPES_H
#define NETWORKING_LOGIC_LIFE_CYCLE_GLOBALS_TYPES_H
#include <stddef.h>
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_tracker_view.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_assemble_match.h"
#include "networking/logic/life_cycle/life_cycle_handler_none.h"
#include "networking/logic/life_cycle/life_cycle_handler_pre_game.h"
#include "networking/logic/life_cycle/life_cycle_handler_start_game.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_game.h"
#include "networking/logic/life_cycle/life_cycle_handler_end_game_write_stats.h"
#include "networking/logic/life_cycle/life_cycle_handler_leaving.h"
#include "networking/logic/life_cycle/life_cycle_handler_joining.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_start.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_find_match_client.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_select_host.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_arbitration.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_prepare_map.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_match_internals.h"
#include "networking/logic/life_cycle/life_cycle_handler_end_match_write_stats.h"
#include "networking/logic/life_cycle/life_cycle_handler_post_match.h"
class c_network_session;
class c_network_observer;
struct s_life_cycle_globals
{
    bool initialized;
    __declspec(align(8)) c_life_cycle_state_manager manager;
    c_life_cycle_state_handler_none none;
    c_life_cycle_state_handler_pre_game pre_game;
    __declspec(align(8)) c_life_cycle_state_handler_start_game start_game;
    __declspec(align(8)) c_life_cycle_state_handler_in_game in_game;
    c_life_cycle_state_handler_end_game_write_stats end_game_write_stats;
    c_life_cycle_state_handler_leaving leaving;
    c_life_cycle_state_handler_joining joining;
    __declspec(align(8)) c_life_cycle_state_handler_matchmaking_start matchmaking_start;
    __declspec(align(8)) c_life_cycle_state_handler_matchmaking_find_match_client matchmaking_find_match_client;
    __declspec(align(8)) c_life_cycle_state_handler_matchmaking_find_match matchmaking_find_match;
    __declspec(align(8)) c_life_cycle_state_handler_matchmaking_find_and_assemble_match find_and_gather;
    c_life_cycle_state_handler_matchmaking_assemble_match assemble;
    __declspec(align(8)) c_life_cycle_state_handler_matchmaking_select_host matchmaking_select_host;
    __declspec(align(8)) c_life_cycle_state_handler_matchmaking_arbitration matchmaking_arbitration;
    c_life_cycle_state_handler_matchmaking_prepare_map matchmaking_prepare_map;
    c_life_cycle_state_handler_in_match in_match;
    c_life_cycle_state_handler_end_match_write_stats end_match_write_stats;
    c_life_cycle_state_handler_post_match post_match;

    static __forceinline void initialize_handler(s_life_cycle_globals *globals, c_life_cycle_state_handler *handler, long state,
        long flags, unsigned __int64 state1, unsigned __int64 state2)
    {
        handler->m_manager=&globals->manager; handler->m_state=(e_life_cycle_state)state; handler->m_handler_flags=flags;
        handler->m_required_squad_session_parameter_mask=state1; handler->m_required_group_session_parameter_mask=state2;
        globals->manager.m_handlers[state]=handler;
    }
};
typedef char life_cycle_manager_offset[(offsetof(s_life_cycle_globals, manager) == 8) ? 1 : -1];
typedef char life_cycle_squad_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, manager) + offsetof(c_life_cycle_state_manager, m_active_squad_session) == 0x58))) ? 1 : -1];
typedef char life_cycle_group_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, manager) + offsetof(c_life_cycle_state_manager, m_group_session) == 0x60))) ? 1 : -1];
typedef char life_cycle_observer_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, manager) + offsetof(c_life_cycle_state_manager, m_observer) == 0x64))) ? 1 : -1];
typedef char life_cycle_in_game_offset[(sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, in_game) == 0x99a0))) ? 1 : -1];
typedef char life_cycle_find_and_gather_offset[(sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, find_and_gather) == 0xe3f0))) ? 1 : -1];
typedef char life_cycle_assemble_offset[(sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, assemble) == 0x12de8))) ? 1 : -1];
typedef char life_cycle_joining_destination_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, joining) +
    offsetof(c_life_cycle_state_handler_joining, m_previous_state) == 0x9a70))) ? 1 : -1];
typedef char life_cycle_post_match_offset[(sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, post_match) == 0x131b0))) ? 1 : -1];
extern "C" s_life_cycle_globals g_network_life_cycle_globals;
static __forceinline s_life_cycle_globals *network_life_cycle_globals_get()
{
    return &g_network_life_cycle_globals;
}

#endif
