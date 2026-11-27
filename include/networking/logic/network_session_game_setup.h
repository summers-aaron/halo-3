#ifndef NETWORKING_LOGIC_NETWORK_SESSION_GAME_SETUP_H
#define NETWORKING_LOGIC_NETWORK_SESSION_GAME_SETUP_H

enum e_gui_game_mode;

// Game setup parameters owned by network_session_interface.cpp.
extern "C" bool network_squad_session_set_ui_game_mode(e_gui_game_mode ui_game_mode);
extern "C" bool network_squad_session_set_primary_skulls(long value);
extern "C" bool network_squad_session_set_secondary_skulls(long value);
extern "C" bool network_squad_session_set_campaign_metagame_scoring(short value);
extern "C" bool network_squad_session_set_campaign_insertion_point(short value);

class c_game_variant;
enum e_network_game_countdown_delayed_reason;

extern "C" bool network_squad_session_set_game_variant(const c_game_variant *variant);
bool network_squad_session_set_countdown_timer(bool delayed, long countdown_timer, e_network_game_countdown_delayed_reason delayed_reason, long user_index);

#endif // NETWORKING_LOGIC_NETWORK_SESSION_GAME_SETUP_H
