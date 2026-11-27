#ifndef NETWORKING_ONLINE_ONLINE_STATISTICS_XENON_CONSTANTS_H
#define NETWORKING_ONLINE_ONLINE_STATISTICS_XENON_CONSTANTS_H
#include <xtl.h>

extern unsigned long k_online_leaderboard_id_map[];

// One stats write: 392 bytes of XUSER_PROPERTY records (the mangled parameter
// type names s_online_stat_write), built by the leaderboard staging code.
struct s_online_stat_write { long leaderboard_id; long property_count; XUSER_PROPERTY properties[16]; };
typedef char online_stat_write_size[(sizeof(void *) != 4 || sizeof(s_online_stat_write) == 392) ? 1 : -1];

struct s_online_statistics_xenon_column_id { unsigned short id; unsigned short pad; long column; };
struct s_online_statistics_xenon_column_view { long view; const s_online_statistics_xenon_column_id *columns; long count; };

extern unsigned long k_online_property_id_map[];

#endif
