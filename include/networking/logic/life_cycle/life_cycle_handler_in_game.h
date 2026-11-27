#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_IN_GAME_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_IN_GAME_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"

// Functions defined in tu/networkinglibxbox/life_cycle_handler_in_game.cpp that other units call.

class c_network_session;
struct s_game_matchmaking_options;

extern "C" bool life_cycle_in_game_initialize_options_from_session(c_network_session *session, s_game_matchmaking_options *matchmaking_options);

struct life_cycle_in_game_handler_flags {
    life_cycle_in_game_handler_flags() : bits(0) {}
    unsigned char bits;
    __forceinline void clear() { bits = 0; }
    __forceinline void set(long bit, bool value) {
        if (value) bits |= (unsigned char)(1U << bit);
        else bits &= (unsigned char)~(1U << bit);
    }
    __forceinline operator unsigned char() const { return bits; }
};
class c_life_cycle_state_handler_in_game : public c_life_cycle_state_handler {
public:
    virtual const char *get_state_string();
    virtual void update();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
    virtual void exit(c_life_cycle_state_handler *to_handler);
    static unsigned __int64 end_load_map(c_network_session *session);
    __forceinline life_cycle_in_game_handler_flags &get_flags() { return m_flags; }
    __forceinline bool is_map_load_pending() const { return (m_flags & 1) != 0; }
private:
    life_cycle_in_game_handler_flags m_flags;
    bool m_x41;
    unsigned long m_time;
    unsigned long m_index;
public:
    __int64 m_match_start_time;
};

typedef char life_cycle_in_game_handler_size[(sizeof(void *) != 4 || ((sizeof(c_life_cycle_state_handler_in_game) == 0x40))) ? 1 : -1];

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_IN_GAME_H
