#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_FIND_AND_ASSEMBLE_MATCH_INTERNALS_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_FIND_AND_ASSEMBLE_MATCH_INTERNALS_H

#include "cseries/cseries_wrapped.h"
#include "networking/life_cycle_session_address_state_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_entry_types.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/suitable_matchmaking_session_types.h"

struct s_matchmaking_seeker_search_start_parameters;
struct s_matchmaking_search_party_properties;
struct s_matchmaking_gather_party_properties;
class c_matchmaking_quality;

typedef c_matchmaking_seeker s_find_assemble_matchmaking_seeker;

class c_life_cycle_state_handler_matchmaking_find_and_assemble_match : public c_life_cycle_state_handler
{
public:
    __forceinline void notify_qos_reply_block_set() { flags.set(0, true); }
    c_life_cycle_state_handler_matchmaking_find_and_assemble_match() { flags.clear(); }
    virtual void update();
    virtual const char *get_state_string();
    __forceinline void get_progress(s_life_cycle_matchmaking_progress *progress) { get_matchmaking_progress_for_squad_search(progress); }
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    bool get_search_party_properties(s_matchmaking_search_party_properties *properties);
    bool get_gather_party_properties(s_matchmaking_gather_party_properties *properties);
    void exit(c_life_cycle_state_handler *next_handler);
    virtual void enter(c_life_cycle_state_handler *previous_handler, long entry_data_size, void *entry_data);

private:
    void update_gathering();
    void update_joining();
    void update_leaving();
    c_flags_no_init<long, unsigned char, 7> flags;
    char alignment_storage[0x30 - 0x29];
    s_find_assemble_matchmaking_seeker seeker;
    char seeker_trailing_storage[0x3488 - sizeof(c_matchmaking_seeker)];
    s_suitable_matchmaking_session m_session;
    unsigned long m_time;
    unsigned long leaving_start_time;
    char leaving_timer_storage[0x4904 - 0x4900];
    s_matchmaking_session_properties m_advertised_session_properties;
    s_find_and_assemble_data find_and_assemble_data;
    union
    {
        long session_address_words[9];
        s_life_cycle_session_address_state session_address_state;
    };
};

struct s_find_assemble_quality
{
    char header_storage[8];
    unsigned char aborted;
    char metrics_storage[0x88 - 0x09];
    long search_time;
};

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_FIND_AND_ASSEMBLE_MATCH_INTERNALS_H
