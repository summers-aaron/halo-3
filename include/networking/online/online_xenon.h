#ifndef NETWORKING_ONLINE_ONLINE_XENON_H
#define NETWORKING_ONLINE_ONLINE_XENON_H

// Functions defined in tu/networkinglibxbox/online_xenon.cpp that other units call.

enum e_online_nat_type;
e_online_nat_type online_get_nat_type(void);

bool online_has_signed_in_user(long controller_index);
unsigned __int64 online_user_get_xuid(long controller_index);
const wchar_t *online_user_get_name(long controller_index);
bool online_local_xuid_is_silver_or_gold_live(long controller_index);
bool online_xenon_user_is_online_enabled(long controller_index);
bool online_local_xuid_is_online_restricted(long controller_index);
unsigned __int64 online_user_get_player_identifier(long controller_index);
bool online_xenon_user_can_use_file_share(long controller_index);
bool online_xenon_user_get_unknown5(long controller_index);
bool online_local_xuid_is_online_enabled(void);

extern "C" bool g_network_online_connected;

#endif // NETWORKING_ONLINE_ONLINE_XENON_H
