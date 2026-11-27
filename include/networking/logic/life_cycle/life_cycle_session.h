#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_SESSION_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_SESSION_H

class c_network_session;

extern "C" void network_squad_session_set_privacy_mode(c_network_session *session, long privacy_mode, long closed_mode,
	long is_closed_by_user, long maximum_player_count);
extern "C" long network_life_cycle_session_get_map_load_status(long map_id, long *out_progress, unsigned long *out_player_mask, long unused);

extern "C" bool life_cycle_handle_simulation_abort(c_network_session *session, bool *error_posted, unsigned long *start_time);

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_SESSION_H
