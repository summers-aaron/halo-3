#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_IN_MATCH_INTERNALS_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_IN_MATCH_INTERNALS_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "simulation/simulation_abort.h"

class c_network_session;
struct utf8;
struct s_networking_join_data;

struct s_life_cycle_handler_in_match_flags
{
    s_life_cycle_handler_in_match_flags() : bits(0) {}
    unsigned char bits;
    __forceinline bool test(long bit) const { return (bits & (unsigned char)(1U << bit)) != 0; }
    __forceinline void set(long bit, bool value)
    {
        if (value) bits |= (unsigned char)(1U << bit);
        else bits &= (unsigned char)~(1U << bit);
    }
};

class c_life_cycle_state_handler_in_match : public c_life_cycle_state_handler
{
public:
    virtual void update();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data);
    virtual void exit(c_life_cycle_state_handler *next);
    virtual const char *get_state_string();
    __forceinline bool is_map_load_pending() const { return m_flags.test(0); }
private:
    __forceinline bool group_has_required_session_parameters() const;
    s_life_cycle_handler_in_match_flags m_flags;
    unsigned long m_time;
    __int64 m_match_start_time;
    bool m_simulation_aborted;
    unsigned long m_index;
};

struct s_life_cycle_handler_in_match_globals
{
    bool initialized;
    char manager_storage[0x60 - 1];
    c_network_session *group_session;
};

struct s_life_cycle_handler_in_match_session
{
    char session_prefix[0x679f8];
    long group_state;
    char migration_prefix[0x68301 - 0x679fc];
    bool speculative_host_migration_enabled;
};

struct s_life_cycle_handler_in_match_manager
{
    char state_manager_prefix[0x80];
    c_matchmaking_quality matchmaking_quality;
};

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_IN_MATCH_INTERNALS_H
