#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_PREPARE_MAP_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_PREPARE_MAP_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"

enum e_matchmaking_prepare_map_flags
{
    _matchmaking_prepare_map_request_next_game_and_map_bit = 0,
    _matchmaking_prepare_map_downloading_bit = 1,
    _matchmaking_prepare_map_started_bit = 2,
    _matchmaking_prepare_map_game_variant_downloaded_bit = 3,
    _matchmaking_prepare_map_map_variant_downloaded_bit = 4
};

#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"

class c_life_cycle_state_handler_matchmaking_prepare_map : public c_life_cycle_state_handler
{
public:
    c_life_cycle_state_handler_matchmaking_prepare_map() { m_flags.clear(); }
    virtual const char *get_state_string();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    void get_progress(s_life_cycle_matchmaking_progress *progress);
    void update_countdown(s_replicated_life_cycle_matchmaking_progress_configuring_match *progress);
private:
    void request_next_game_and_map(s_replicated_life_cycle_matchmaking_progress_configuring_match *progress);
    void update_loading(s_replicated_life_cycle_matchmaking_progress_configuring_match *progress);
    void update_game_and_map_download(s_replicated_life_cycle_matchmaking_progress_configuring_match *progress);
public:

    virtual void update();
    virtual void exit(c_life_cycle_state_handler *next_handler);
    virtual void enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data);

    // Shift-and-extract flag test; flag_mask uses the c_flags mask test instead.
    __forceinline bool flag_test(e_matchmaking_prepare_map_flags bit) const { return (unsigned char)((m_flags.m_storage >> bit) & 1); }
    __forceinline bool flag_mask(e_matchmaking_prepare_map_flags bit) const { return m_flags.test(bit); }
    __forceinline void flag_set(e_matchmaking_prepare_map_flags bit, bool value) { m_flags.set(bit, value); }

    typedef c_flags_no_init<e_matchmaking_prepare_map_flags, unsigned char, 5> t_flags;
    t_flags m_flags;
    unsigned long m_start_time;
    unsigned long m_time2;
    unsigned long m_time3;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char prepare_map_handler_size[(sizeof(void *) != 4 || ((sizeof(c_life_cycle_state_handler_matchmaking_prepare_map) == 0x38))) ? 1 : -1];
typedef char prepare_map_handler_start_time_offset[
    (sizeof(void *) != 4 || ((offsetof(c_life_cycle_state_handler_matchmaking_prepare_map, m_start_time) == 0x2c))) ? 1 : -1];
typedef char prepare_map_handler_time3_offset[
    (sizeof(void *) != 4 || ((offsetof(c_life_cycle_state_handler_matchmaking_prepare_map, m_time3) == 0x34))) ? 1 : -1];
#endif

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_PREPARE_MAP_H
