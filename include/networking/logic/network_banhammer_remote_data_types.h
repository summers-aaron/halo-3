#ifndef NETWORKING_LOGIC_NETWORK_BANHAMMER_REMOTE_DATA_TYPES_H
#define NETWORKING_LOGIC_NETWORK_BANHAMMER_REMOTE_DATA_TYPES_H

#include "memory/data_types.h"
#include <stddef.h>

struct s_network_banhammer_remote_data {
    bool valid;
    unsigned __int64 xuid, identifier;
    wchar_t name[16];
    unsigned __int64 machine;
    unsigned long cheats;
    unsigned char reported;
};

typedef char banhammer_remote_data_size[(sizeof(s_network_banhammer_remote_data) == 0x48) ? 1 : -1];
typedef char banhammer_remote_data_xuid_offset[(offsetof(s_network_banhammer_remote_data, xuid) == 8) ? 1 : -1];
typedef char banhammer_remote_data_reported_offset[(offsetof(s_network_banhammer_remote_data, reported) == 0x44) ? 1 : -1];

#endif // NETWORKING_LOGIC_NETWORK_BANHAMMER_REMOTE_DATA_TYPES_H
