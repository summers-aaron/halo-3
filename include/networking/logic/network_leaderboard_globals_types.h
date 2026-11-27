#ifndef NETWORKING_LOGIC_NETWORK_LEADERBOARD_GLOBALS_TYPES_H
#define NETWORKING_LOGIC_NETWORK_LEADERBOARD_GLOBALS_TYPES_H
#include "networking/logic/logic_managed_user_types.h"
#include "interface/online_leaderboard_id_types.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
struct s_network_leaderboard_write_slot { long state; long value; };
struct s_leaderboard_user {
  bool valid;
  long controller_index;
  unsigned __int64 xuid;
  wchar_t gamertag[16];
  long state;
  unsigned char initiate_flag;
  unsigned long query_time;
  long failure_count;
  s_managed_user_property_data statistics[20];
  // Tail of the query-time c_http_stored_buffer_downloader<128>, constructed at
  // +0x208 during query init (its head overlaps statistics[]; see
  // s_network_leaderboard_query_user_statistics below); untouched by
  // steady-state leaderboard code.
  char query_downloader_storage[0x918 - 0x220];
  long highest_skill_valid;
  long highest_skill;
};
struct s_network_leaderboard_globals {
  bool initialized;
  e_online_leaderboard_id global_leaderboard, custom_leaderboard;
  bool refresh_pending;
  unsigned long refresh_time;
  unsigned short playlist;
  e_online_leaderboard_id playlist_leaderboard, playlist_skill_leaderboard;
  s_leaderboard_user users[4];
  s_network_leaderboard_write_slot slots[17];
};

// Query initialization uses this alternate view of each 0x920-byte user record.
struct s_network_leaderboard_query_user_statistics
{
    union { bool active; bool valid; };
    long controller_index;
    qword xuid;
    wchar_t gamertag[16];
    union {
        char statistics[472];
        struct { long connection_state; char statistics_data[468]; };
    };
    c_http_stored_buffer_downloader<128> downloader;
    union { long retry_count; long highest_skill_state; };
    union { long state; long highest_skill; };
};

typedef char leaderboard_query_user_size_check[
    (sizeof(void *) != 4 || ((sizeof(s_network_leaderboard_query_user_statistics) == sizeof(s_leaderboard_user)))) ? 1 : -1];
typedef char leaderboard_query_retry_offset_check[
    (sizeof(void *) != 4 || ((offsetof(s_network_leaderboard_query_user_statistics, retry_count)
    == offsetof(s_leaderboard_user, highest_skill_valid)))) ? 1 : -1];

typedef char leaderboard_highest_skill_state_offset[
    (sizeof(void *) != 4 || ((offsetof(s_network_leaderboard_query_user_statistics, highest_skill_state) == 0x918))) ? 1 : -1];
typedef char leaderboard_connection_state_offset[(offsetof(s_network_leaderboard_query_user_statistics, connection_state) == 0x30) ? 1 : -1];

typedef char leaderboard_user_gamertag_offset[(offsetof(s_leaderboard_user, gamertag) == 0x10) ? 1 : -1];

#endif
