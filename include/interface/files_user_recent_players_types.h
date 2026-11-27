#ifndef INTERFACE_FILES_USER_RECENT_PLAYERS_TYPES_H
#define INTERFACE_FILES_USER_RECENT_PLAYERS_TYPES_H

#include <stddef.h>

struct s_network_banhammer_repeated_play_entry
{
    union { unsigned short value; unsigned short hopper_identifier; };
    union { unsigned short history[4]; unsigned short player_identifier[4]; };
};
typedef char banhammer_repeated_play_entry_size_check[(sizeof(s_network_banhammer_repeated_play_entry) == 10) ? 1 : -1];
typedef char banhammer_repeated_play_identifier_offset_check[(offsetof(s_network_banhammer_repeated_play_entry, player_identifier) == 2) ? 1 : -1];

// 'furp' chunk payload (major version 2). The banhammer reader bounds
// entry_count by 384 and copies 10-byte entries from offset 4.
struct s_files_user_recent_players
{
    long entry_count;
    s_network_banhammer_repeated_play_entry entries[384];
};
typedef char files_user_recent_players_size_check[(sizeof(s_files_user_recent_players) == 3844) ? 1 : -1];

#endif // INTERFACE_FILES_USER_RECENT_PLAYERS_TYPES_H
