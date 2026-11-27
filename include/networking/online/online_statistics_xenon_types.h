#ifndef NETWORKING_ONLINE_ONLINE_STATISTICS_XENON_TYPES_H
#define NETWORKING_ONLINE_ONLINE_STATISTICS_XENON_TYPES_H
#include <xtl.h>

struct s_online_stat_query { long leaderboard_id; long column_count; long columns[32]; };

struct s_online_stat_query_stat
{
    long column_id;
    XUSER_DATA data;
};
struct s_online_stat_query_player
{
    unsigned __int64 xuid;
    char gamertag[16];
    long stat_count;
    s_online_stat_query_stat stats[32];
};
struct s_online_stat_query_leaderboard
{
    long leaderboard_id;
    long player_count;
    s_online_stat_query_player player_stats[16];
};
struct s_online_stat_query_result
{
    long leaderboard_count;
    s_online_stat_query_leaderboard leaderboard_results[4];
};

class c_statistics_query_overlapped_task;
class c_statistics_write_overlapped_task;

struct s_online_statistics_globals
{
    bool initialized;
    c_statistics_query_overlapped_task *query_tasks[4];
    c_statistics_write_overlapped_task *global_write_task;
    c_statistics_write_overlapped_task *write_tasks[16];
};

#endif
