#ifndef NETWORKING_ONLINE_VOICE_XENON_H
#define NETWORKING_ONLINE_VOICE_XENON_H

#include "networking/online/voice_data_types.h"
#include "shell/shell_types.h"

// Functions defined in tu/networkinglibxbox/voice_xenon.cpp.

struct s_voice_speaker_volume;

bool voice_enforcing_team_restrictions(void);
bool voice_lifecycle_in_game(void);
void voice_initialize(void);
bool voice_player_is_talking(long player_index);
void voice_set_mute(long player_index, long muted_player_index, bool mute);
bool voice_get_mute(long source_player_index, long destination_player_index);
bool voice_any_local_player_muting(long player_index);
bool voice_get_local_mute_through_guide(long player_index, long muted_player_index);
bool voice_players_on_same_team(long first_player_index, long second_player_index);
bool voice_players_are_allies(long first_player_index, long second_player_index);
unsigned long voice_get_squad_session_peer_mask_by_group_session(void);
long voice_get_player_peer_mask(long player_index);
long voice_get_local_peer_index(void);
bool voice_lifecycle_in_matchmaking(void);
bool voice_lifecycle_out_of_game(void);
bool voice_set_speaker_volume(long player_index, const s_voice_speaker_volume *volume);
void voice_update(void);
bool voice_player_is_valid_in_game(long player_index);
extern "C" long voice_get_player_datum(long player_index);
extern "C" unsigned char voice_any_local_player_can_communicate(long player_index);

class c_network_session_membership;
class c_xenon_voice_engine;
struct s_voice_interface_globals;

void voice_update_poll_for_local_chat_data(void);
extern "C" c_network_session_membership *voice_get_session_membership(void);
void voice_interface_update_from_session_membership(void);
void voice_interface_update_player_voice_registration(void);
extern "C" bool voice_interface_register_local_player(unsigned int controller_to_register, unsigned char skip_talker_registration);
void voice_interface_update_remote_player_voice(long player_index);
void voice_interface_update_voice_lifecycle(void);
extern "C" void voice_interface_remove_players_with_membership(const c_network_session_membership *m);
void voice_interface_initialize_player_speaker_states(void);
extern "C" bool voice_players_can_communicate(long player_index, long other_player_index);
void voice_interface_update_local_player_voice(long player_index);

// Data defined in tu/networkinglibxbox/voice_xenon.cpp that other units use.
extern s_voice_interface_globals g_voice_interface_globals;
extern c_xenon_voice_engine g_voice_engine;
extern "C" void voice_get_player_voice_state(long player_index, long *state, long *talking);

class c_voice_availability;
class c_voice_observer;
class c_voice_queue;
class c_voice_routing;

void voice_queue_received_packet(long sender, const void *data, long size, e_voice_data_type type);
e_controller_index voice_get_player_controller_index(long player_index);
unsigned long voice_get_controller_settings(e_controller_index controller_index);
void voice_interface_update_controller_network_properties(e_controller_index controller_index);
void voice_interface_queue_voice_data_for_sending( e_controller_index controller_index, const unsigned char *data, long size,
    long packet_count, e_voice_data_type type);
extern "C" unsigned char voice_controller_can_communicate(e_controller_index controller_index, long player_index);

extern c_voice_observer g_voice_observer;
extern c_voice_queue g_voice_queue;
extern c_voice_availability g_voice_availability;
extern c_voice_routing g_voice_routing;

static __forceinline void voice_interface_reset_player(long player_index);
static __forceinline void voice_interface_reset_controller(long controller_index);

#endif // NETWORKING_ONLINE_VOICE_XENON_H
