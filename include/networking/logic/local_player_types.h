#ifndef NETWORKING_LOGIC_LOCAL_PLAYER_TYPES_H
#define NETWORKING_LOGIC_LOCAL_PLAYER_TYPES_H

struct s_network_web_event_local_player
{
    long controller_index;
    unsigned __int64 xuid;
    wchar_t name[16];
};
typedef char network_web_event_local_player_size[(sizeof(s_network_web_event_local_player) == 0x30) ? 1 : -1];

#endif // NETWORKING_LOGIC_LOCAL_PLAYER_TYPES_H
