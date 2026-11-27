#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_ASSEMBLE_MATCH_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_ASSEMBLE_MATCH_H

// Functions defined in tu/networkinglibxbox/life_cycle_handler_matchmaking_assemble_match.cpp that other units call.

#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/logic_matchmaking_composition_types.h"
#include "networking/logic/logic_matchmaking_composition_data_types.h"
#include "networking/life_cycle_session_address_state_types.h"

struct s_life_cycle_qos_request;

class c_life_cycle_state_handler_matchmaking_assemble_match : public c_life_cycle_state_handler {
public:
    __forceinline void notify_qos_reply_block_set() { m_flag_bits.set(0, true); }
    c_life_cycle_state_handler_matchmaking_assemble_match() : m_flags(0) {}
    virtual const char *get_state_string();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void update();
    bool get_gather_party_properties(s_matchmaking_gather_party_properties *properties);
    void get_progress(struct s_life_cycle_matchmaking_progress *progress);
    virtual void exit(c_life_cycle_state_handler *life_cycle_state_handler);
    void handle_missing_required_session_parameter(e_network_session_type network_session_type);
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
private:
    void update_progress();
    bool ready_to_lockdown();
    bool ready_to_start();
    bool continue_assembling();
    bool pick_games();
    unsigned long get_peer_mask_to_evict_for_team_balance();
    union { unsigned char m_flags; c_flags_no_init<long, unsigned char, 7> m_flag_bits; };
    unsigned long m_start_time;
    unsigned long m_gather_start_time;
    s_matchmaking_session_properties m_advertised_properties;
    unsigned long m_progress_update_time;
    unsigned long m_qos_update_time;
    // match: 0x3e6b38: the address state is eight-byte aligned and cleared with doubleword stores.
    __declspec(align(8)) s_life_cycle_session_address_state m_address_state;
};

extern "C" bool life_cycle_initialize_qos_request(const void *description, s_life_cycle_qos_request *request);

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_MATCHMAKING_ASSEMBLE_MATCH_H
