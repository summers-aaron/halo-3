#ifndef NETWORKING_ONLINE_ONLINE_STATISTICS_XENON_H
#define NETWORKING_ONLINE_ONLINE_STATISTICS_XENON_H

#include "networking/online/online_stats_query_status_types.h"

// Functions defined in tu/networkinglibxbox/online_statistics_xenon.cpp that other units call.

struct s_online_stat_query;
struct s_online_stat_query_result;
struct s_online_stat_write;
struct s_online_statistics_globals;

extern s_online_statistics_globals g_online_statistics_globals;

void online_statistics_initialize();
bool online_stats_query_initiate(long user_index, long user_count, unsigned __int64 * user_xuids, long query_count, struct s_online_stat_query const * queries);
bool online_stats_query_get_results(long user_index, s_online_stat_query_result *result_out);
bool online_global_stats_write_initiate(void * context, long write_count, struct s_online_stat_write const * _sym_p2);
bool online_stats_write_initiate(long player_index, void * handle, unsigned __int64 xuid, long write_count, struct s_online_stat_write const * _sym_p4);
extern "C" void online_statistics_dispose();
e_online_stats_query_status online_stats_query_get_status(long user_index);

#endif // NETWORKING_ONLINE_ONLINE_STATISTICS_XENON_H
