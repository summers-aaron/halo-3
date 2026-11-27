// networkinglibxbox/online_statistics_xenon.obj: from the records 0x407fd8 0x407fe8 0x408010 0x408020 0x408090 0x408128 0x408238 0x408300 0x4084b8 0x408530 0x408758 0x408850
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include <stddef.h>
#include "cseries/cseries.h"
// @address 0x408020 ?start@c_statistics_write_overlapped_task@@UAAKPAX@Z
// @address 0x408530 ?online_stats_query_get_results@@YA_NJPAUs_online_stat_query_result@@@Z
// @address 0x8c4630 ?g_online_statistics_globals@@3Us_online_statistics_globals@@A
// @address 0x20e770 ?overlapped_task_block_until_finished@@YAXPAVc_overlapped_task@@@Z
// @address 0x20e4e8 ?reset@c_overlapped_task@@UAAXXZ
// @address 0x20e550 ?is_result_successful@c_overlapped_task@@UAA_NKKK@Z
// @address 0x5bec98 ?complete@c_overlapped_task@@UAAXXZ
// @address 0x5bec98 ?failure@c_overlapped_task@@UAAXKKK@Z
// @address 0x5bec98 ?success@c_overlapped_task@@UAAXK@Z
// @address 0x5bec98 ?update@c_overlapped_task@@UAAXK@Z
#include <xtl.h>
#include <string.h>
#include "networking/online/online_statistics_xenon_types.h"
#include "networking/online/online_statistics_xenon_constants.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "networking/online/online_statistics_xenon.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_managed_session.h"
#include "networking/online/overlapped_allocation_marker_types.h"
#include "networking/online/online_stats_query_status_types.h"

struct s_internal_leaderboard_write;
struct s_online_stat_query;
struct s_online_stat_write;
#include "cseries/async_xoverlapped_allocation.h"




#include "cseries/async_xoverlapped_types.h"


s_online_statistics_globals g_online_statistics_globals;

// @rdata 0x82025b60..0x8202647c
static const s_online_statistics_xenon_column_id k_online_statistics_global_columns[] =
{
    { 0x0004, 0, 4 },
    { 0x0005, 0, 5 },
    { 0x0007, 0, 6 },
    { 0x0009, 0, 7 },
    { 0x000a, 0, 8 },
    { 0x000b, 0, 9 },
    { 0x000c, 0, 10 },
    { 0x000d, 0, 11 },
    { 0x000e, 0, 12 },
    { 0x0006, 0, 13 },
    { 0x0008, 0, 14 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_custom_games_columns[] =
{
    { 0x0001, 0, 2 },
    { 0x0003, 0, 3 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_skill_columns[] =
{
    { 0x003f, 0, 0 },
    { 0x0040, 0, 1 },
};

// Hopper suffixes are XLAST indices.
static const s_online_statistics_xenon_column_id k_online_statistics_hopper_0_columns[] =
{
    { 0x0004, 0, 15 },
    { 0x0001, 0, 16 },
    { 0x0002, 0, 17 },
    { 0x0003, 0, 18 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_1_columns[] =
{
    { 0x0004, 0, 19 },
    { 0x0001, 0, 20 },
    { 0x0002, 0, 21 },
    { 0x0003, 0, 22 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_2_columns[] =
{
    { 0x0004, 0, 23 },
    { 0x0001, 0, 24 },
    { 0x0002, 0, 25 },
    { 0x0003, 0, 26 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_3_columns[] =
{
    { 0x0004, 0, 27 },
    { 0x0001, 0, 28 },
    { 0x0002, 0, 29 },
    { 0x0003, 0, 30 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_4_columns[] =
{
    { 0x0004, 0, 31 },
    { 0x0001, 0, 32 },
    { 0x0002, 0, 33 },
    { 0x0003, 0, 34 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_5_columns[] =
{
    { 0x0004, 0, 35 },
    { 0x0001, 0, 36 },
    { 0x0002, 0, 37 },
    { 0x0003, 0, 38 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_6_columns[] =
{
    { 0x0004, 0, 39 },
    { 0x0001, 0, 40 },
    { 0x0002, 0, 41 },
    { 0x0003, 0, 42 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_7_columns[] =
{
    { 0x0004, 0, 43 },
    { 0x0001, 0, 44 },
    { 0x0002, 0, 45 },
    { 0x0003, 0, 46 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_8_columns[] =
{
    { 0x0004, 0, 47 },
    { 0x0001, 0, 48 },
    { 0x0002, 0, 49 },
    { 0x0003, 0, 50 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_9_columns[] =
{
    { 0x0004, 0, 51 },
    { 0x0001, 0, 52 },
    { 0x0002, 0, 53 },
    { 0x0003, 0, 54 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_10_columns[] =
{
    { 0x0004, 0, 55 },
    { 0x0001, 0, 56 },
    { 0x0002, 0, 57 },
    { 0x0003, 0, 58 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_11_columns[] =
{
    { 0x0004, 0, 59 },
    { 0x0001, 0, 60 },
    { 0x0002, 0, 61 },
    { 0x0003, 0, 62 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_12_columns[] =
{
    { 0x0004, 0, 63 },
    { 0x0001, 0, 64 },
    { 0x0002, 0, 65 },
    { 0x0003, 0, 66 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_13_columns[] =
{
    { 0x0004, 0, 67 },
    { 0x0001, 0, 68 },
    { 0x0002, 0, 69 },
    { 0x0003, 0, 70 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_14_columns[] =
{
    { 0x0004, 0, 71 },
    { 0x0001, 0, 72 },
    { 0x0002, 0, 73 },
    { 0x0003, 0, 74 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_15_columns[] =
{
    { 0x0004, 0, 75 },
    { 0x0001, 0, 76 },
    { 0x0002, 0, 77 },
    { 0x0003, 0, 78 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_16_columns[] =
{
    { 0x0004, 0, 79 },
    { 0x0001, 0, 80 },
    { 0x0002, 0, 81 },
    { 0x0003, 0, 82 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_17_columns[] =
{
    { 0x0004, 0, 83 },
    { 0x0001, 0, 84 },
    { 0x0002, 0, 85 },
    { 0x0003, 0, 86 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_18_columns[] =
{
    { 0x0004, 0, 87 },
    { 0x0001, 0, 88 },
    { 0x0002, 0, 89 },
    { 0x0003, 0, 90 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_19_columns[] =
{
    { 0x0004, 0, 91 },
    { 0x0001, 0, 92 },
    { 0x0002, 0, 93 },
    { 0x0003, 0, 94 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_20_columns[] =
{
    { 0x0004, 0, 95 },
    { 0x0001, 0, 96 },
    { 0x0002, 0, 97 },
    { 0x0003, 0, 98 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_21_columns[] =
{
    { 0x0004, 0, 99 },
    { 0x0001, 0, 100 },
    { 0x0002, 0, 101 },
    { 0x0003, 0, 102 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_22_columns[] =
{
    { 0x0004, 0, 103 },
    { 0x0001, 0, 104 },
    { 0x0002, 0, 105 },
    { 0x0003, 0, 106 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_23_columns[] =
{
    { 0x0004, 0, 107 },
    { 0x0001, 0, 108 },
    { 0x0002, 0, 109 },
    { 0x0003, 0, 110 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_24_columns[] =
{
    { 0x0004, 0, 111 },
    { 0x0001, 0, 112 },
    { 0x0002, 0, 113 },
    { 0x0003, 0, 114 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_25_columns[] =
{
    { 0x0004, 0, 115 },
    { 0x0001, 0, 116 },
    { 0x0002, 0, 117 },
    { 0x0003, 0, 118 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_26_columns[] =
{
    { 0x0004, 0, 119 },
    { 0x0001, 0, 120 },
    { 0x0002, 0, 121 },
    { 0x0003, 0, 122 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_27_columns[] =
{
    { 0x0004, 0, 123 },
    { 0x0001, 0, 124 },
    { 0x0002, 0, 125 },
    { 0x0003, 0, 126 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_28_columns[] =
{
    { 0x0004, 0, 127 },
    { 0x0001, 0, 128 },
    { 0x0002, 0, 129 },
    { 0x0003, 0, 130 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_29_columns[] =
{
    { 0x0004, 0, 131 },
    { 0x0001, 0, 132 },
    { 0x0002, 0, 133 },
    { 0x0003, 0, 134 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_30_columns[] =
{
    { 0x0004, 0, 135 },
    { 0x0001, 0, 136 },
    { 0x0002, 0, 137 },
    { 0x0003, 0, 138 },
};

static const s_online_statistics_xenon_column_id k_online_statistics_hopper_31_columns[] =
{
    { 0x0004, 0, 139 },
    { 0x0001, 0, 140 },
    { 0x0002, 0, 141 },
    { 0x0003, 0, 142 },
};

static const s_online_statistics_xenon_column_view k_online_statistics_column_views[] =
{
    { 0xffff0000, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000001, k_online_statistics_global_columns, 0x0000000b },
    { 0x00000003, k_online_statistics_custom_games_columns, 0x00000002 },
    { 0x00000002, k_online_statistics_hopper_0_columns, 0x00000004 },
    { 0xffff0001, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0001, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000004, k_online_statistics_hopper_1_columns, 0x00000004 },
    { 0xffff0002, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0002, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000005, k_online_statistics_hopper_2_columns, 0x00000004 },
    { 0xffff0003, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0003, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000006, k_online_statistics_hopper_3_columns, 0x00000004 },
    { 0xffff0004, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0004, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000007, k_online_statistics_hopper_4_columns, 0x00000004 },
    { 0xffff0005, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0005, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000008, k_online_statistics_hopper_5_columns, 0x00000004 },
    { 0xffff0006, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0006, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000009, k_online_statistics_hopper_6_columns, 0x00000004 },
    { 0xffff0007, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0007, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000000a, k_online_statistics_hopper_7_columns, 0x00000004 },
    { 0xffff0008, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0008, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000000b, k_online_statistics_hopper_8_columns, 0x00000004 },
    { 0xffff0009, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0009, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000000c, k_online_statistics_hopper_9_columns, 0x00000004 },
    { 0xffff000a, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe000a, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000000d, k_online_statistics_hopper_10_columns, 0x00000004 },
    { 0xffff000b, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe000b, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000000e, k_online_statistics_hopper_11_columns, 0x00000004 },
    { 0xffff000c, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe000c, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000000f, k_online_statistics_hopper_12_columns, 0x00000004 },
    { 0xffff000d, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe000d, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000010, k_online_statistics_hopper_13_columns, 0x00000004 },
    { 0xffff000e, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe000e, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000011, k_online_statistics_hopper_14_columns, 0x00000004 },
    { 0xffff000f, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe000f, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000012, k_online_statistics_hopper_15_columns, 0x00000004 },
    { 0xffff0010, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0010, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000013, k_online_statistics_hopper_16_columns, 0x00000004 },
    { 0xffff0011, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0011, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000014, k_online_statistics_hopper_17_columns, 0x00000004 },
    { 0xffff0012, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0012, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000015, k_online_statistics_hopper_18_columns, 0x00000004 },
    { 0xffff0013, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0013, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000016, k_online_statistics_hopper_19_columns, 0x00000004 },
    { 0xffff0014, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0014, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000021, k_online_statistics_hopper_20_columns, 0x00000004 },
    { 0xffff0015, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0015, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000018, k_online_statistics_hopper_21_columns, 0x00000004 },
    { 0xffff0016, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0016, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000019, k_online_statistics_hopper_22_columns, 0x00000004 },
    { 0xffff0017, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0017, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000001a, k_online_statistics_hopper_23_columns, 0x00000004 },
    { 0xffff0018, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0018, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000001b, k_online_statistics_hopper_24_columns, 0x00000004 },
    { 0xffff0019, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0019, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000001c, k_online_statistics_hopper_25_columns, 0x00000004 },
    { 0xffff001a, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe001a, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000001d, k_online_statistics_hopper_26_columns, 0x00000004 },
    { 0xffff001b, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe001b, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000001e, k_online_statistics_hopper_27_columns, 0x00000004 },
    { 0xffff001c, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe001c, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x0000001f, k_online_statistics_hopper_28_columns, 0x00000004 },
    { 0xffff001d, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe001d, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000020, k_online_statistics_hopper_29_columns, 0x00000004 },
    { 0xffff001e, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe001e, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000017, k_online_statistics_hopper_30_columns, 0x00000004 },
    { 0xffff001f, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe001f, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0x00000022, k_online_statistics_hopper_31_columns, 0x00000004 },
    { 0xffff0020, k_online_statistics_hopper_skill_columns, 0x00000002 },
    { 0xfffe0020, k_online_statistics_hopper_skill_columns, 0x00000002 },
};
// @rdata-end

// @address 0x73a308 k_online_leaderboard_id_map
unsigned long k_online_leaderboard_id_map[] =
{
    0xffff0000,
    0x00000001,
    0x00000003,
    0x00000002,
    0xffff0001,
    0xfffe0001,
    0x00000004,
    0xffff0002,
    0xfffe0002,
    0x00000005,
    0xffff0003,
    0xfffe0003,
    0x00000006,
    0xffff0004,
    0xfffe0004,
    0x00000007,
    0xffff0005,
    0xfffe0005,
    0x00000008,
    0xffff0006,
    0xfffe0006,
    0x00000009,
    0xffff0007,
    0xfffe0007,
    0x0000000a,
    0xffff0008,
    0xfffe0008,
    0x0000000b,
    0xffff0009,
    0xfffe0009,
    0x0000000c,
    0xffff000a,
    0xfffe000a,
    0x0000000d,
    0xffff000b,
    0xfffe000b,
    0x0000000e,
    0xffff000c,
    0xfffe000c,
    0x0000000f,
    0xffff000d,
    0xfffe000d,
    0x00000010,
    0xffff000e,
    0xfffe000e,
    0x00000011,
    0xffff000f,
    0xfffe000f,
    0x00000012,
    0xffff0010,
    0xfffe0010,
    0x00000013,
    0xffff0011,
    0xfffe0011,
    0x00000014,
    0xffff0012,
    0xfffe0012,
    0x00000015,
    0xffff0013,
    0xfffe0013,
    0x00000016,
    0xffff0014,
    0xfffe0014,
    0x00000021,
    0xffff0015,
    0xfffe0015,
    0x00000018,
    0xffff0016,
    0xfffe0016,
    0x00000019,
    0xffff0017,
    0xfffe0017,
    0x0000001a,
    0xffff0018,
    0xfffe0018,
    0x0000001b,
    0xffff0019,
    0xfffe0019,
    0x0000001c,
    0xffff001a,
    0xfffe001a,
    0x0000001d,
    0xffff001b,
    0xfffe001b,
    0x0000001e,
    0xffff001c,
    0xfffe001c,
    0x0000001f,
    0xffff001d,
    0xfffe001d,
    0x00000020,
    0xffff001e,
    0xfffe001e,
    0x00000017,
    0xffff001f,
    0xfffe001f,
    0x00000022,
    0xffff0020,
    0xfffe0020,
};

class c_statistics_query_overlapped_task : public c_overlapped_task
{
public:
    __forceinline c_statistics_query_overlapped_task(const char *file, long line) : c_overlapped_task(file, line) {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    __forceinline XUSER_STATS_READ_RESULTS *get_results() { return (XUSER_STATS_READ_RESULTS *)m_result_buffer; }
    __forceinline void initialize(long user_count, const unsigned __int64 *user_xuids, long query_count, const XUSER_STATS_SPEC *queries)
    {
        memset(m_user_xuids, 0, sizeof(m_user_xuids));
        memset(m_queries, 0, sizeof(m_queries));
        memset(m_result_buffer, 0, sizeof(m_result_buffer));
        m_user_count = user_count;
        memcpy(m_user_xuids, user_xuids, user_count * sizeof(unsigned __int64));
        m_query_count = query_count;
        memcpy(m_queries, queries, query_count * sizeof(XUSER_STATS_SPEC));
        m_result_size = sizeof(m_result_buffer);
    }
private:
    char m_result_buffer[1024];
    unsigned long m_result_size;
    long m_user_count;
    unsigned __int64 m_user_xuids[4];
    long m_query_count;
    XUSER_STATS_SPEC m_queries[4];
};

#include "networking/online/online_statistics_tasks.h"





const char *c_statistics_query_overlapped_task::get_context_string() const
{
    return "stats-query";
}

unsigned long c_statistics_query_overlapped_task::start(void *overlapped)
{
    unsigned long result = XUserReadStats(0, m_user_count, m_user_xuids, m_query_count, m_queries,
        &m_result_size, (XUSER_STATS_READ_RESULTS *)m_result_buffer, (XOVERLAPPED *)overlapped);
    return result;
}

const char *c_statistics_write_overlapped_task::get_context_string() const
{
    return "stats-write";
}

unsigned long c_statistics_write_overlapped_task::start(void *overlapped)
{
    unsigned long result = E_FAIL;
    if (((unsigned char)::online_local_xuid_is_online_enabled()))
    {
        if (managed_session_is_valid_handle(m_session))
            result = XSessionWriteStats(m_session, m_xuid, m_write_count, m_views, (XOVERLAPPED *)overlapped);
    }
    return result;
}

void c_statistics_write_overlapped_task::initialize(void *session, unsigned __int64 xuid, long write_count,
    const s_internal_leaderboard_write *leaderboard_writes)
{
    m_session = session;
    m_xuid = xuid;
    m_write_count = write_count;
    memcpy(m_leaderboard_writes, leaderboard_writes, write_count * sizeof(s_internal_leaderboard_write));
    for (long write_index = 0; write_index < m_write_count; write_index++)
    {
        XSESSION_VIEW_PROPERTIES *view = &m_views[write_index];
        s_internal_leaderboard_write *write = &m_leaderboard_writes[write_index];
        view->dwViewId = write->leaderboard_id;
        view->dwNumProperties = write->property_count;
        view->pProperties = write->properties;
    }
}

void online_statistics_initialize()
{
    memset(&g_online_statistics_globals, 0, sizeof(g_online_statistics_globals));
    g_online_statistics_globals.global_write_task = new (0, 0, _overlapped_allocation_marker) c_statistics_write_overlapped_task(0, 0);
    for (long query_index = 0; query_index < 4; query_index++)
        g_online_statistics_globals.query_tasks[query_index] = new (0, 0, _overlapped_allocation_marker) c_statistics_query_overlapped_task(0, 0);
    for (long write_index = 0; write_index < 16; write_index++)
        g_online_statistics_globals.write_tasks[write_index] = new (0, 0, _overlapped_allocation_marker) c_statistics_write_overlapped_task(0, 0);
    g_online_statistics_globals.initialized = true;
}

// @address 0x408238 online_statistics_dispose
extern "C" void online_statistics_dispose()
{
    c_overlapped_task *global_write_task = g_online_statistics_globals.global_write_task;
    global_write_task->~c_overlapped_task();
    overlapped_free(global_write_task);
    for (long query_index = 0; query_index < 4; query_index++)
    {
        c_overlapped_task *query_task = g_online_statistics_globals.query_tasks[query_index];
        query_task->~c_overlapped_task();
        overlapped_free(query_task);
    }
    for (long write_index = 0; write_index < 16; write_index++)
    {
        c_overlapped_task *write_task = g_online_statistics_globals.write_tasks[write_index];
        write_task->~c_overlapped_task();
        overlapped_free(write_task);
    }
    memset(&g_online_statistics_globals, 0, sizeof(g_online_statistics_globals));
    g_online_statistics_globals.initialized = false;
}

static __forceinline unsigned short online_statistics_xenon_column_id_by_view_get(long column, long view)
{
    unsigned long result = (unsigned long)-1;
    const s_online_statistics_xenon_column_view *current_view = &k_online_statistics_column_views[view];
    for (long index = 0; index < current_view->count; index++)
    {
        if (current_view->columns[index].column == column)
        {
            result = current_view->columns[index].id;
            break;
        }
    }
    return (unsigned short)result;
}

// @address 0x408300 ?online_stats_query_initiate@@YA_NJJPA_KJPBUs_online_stat_query@@@Z
bool online_stats_query_initiate(long user_index, long user_count, unsigned __int64 * user_xuids, long query_count, struct s_online_stat_query const * queries)
{
    // match: 0x408300: value range assumption guides branch lowering
    __assume(user_count == 1);
    XUSER_STATS_SPEC stat_queries[4];
    unsigned __int64 xuids[16];
    memset(stat_queries, 0, sizeof(stat_queries));
    for (long user = 0; user < user_count; user++)
        xuids[user] = user_xuids[user];
    for (long query_index = 0; query_index < query_count; query_index++)
    {
        const s_online_stat_query *current_query = &queries[query_index];
        XUSER_STATS_SPEC *current_stat_spec = &stat_queries[query_index];
        current_stat_spec->dwViewId = k_online_leaderboard_id_map[current_query->leaderboard_id];
        current_stat_spec->dwNumColumnIds = current_query->column_count;
        for (long column_index = 0; column_index < current_query->column_count; column_index++)
            current_stat_spec->rgwColumnIds[column_index] = online_statistics_xenon_column_id_by_view_get(
                current_query->columns[column_index], current_query->leaderboard_id);
    }
    g_online_statistics_globals.query_tasks[user_index]->initialize(user_count, xuids, query_count, stat_queries);
    return ::overlapped_task_start_internal(g_online_statistics_globals.query_tasks[user_index],
        (char const *)("..\\networking\\online\\online_statistics_xenon.cpp"), 314);
}

e_online_stats_query_status online_stats_query_get_status(long user_index)
{
    short task_state = g_online_statistics_globals.query_tasks[user_index]->get_task_state();
    e_online_stats_query_status result = _online_stats_query_status_none;
    switch (task_state)
    {
    case 1:
    case 2:
    case 3:
        result = _online_stats_query_status_pending;
        break;
    case 4:
        result = _online_stats_query_status_success;
        break;
    case 5:
        result = _online_stats_query_status_failure;
        break;
    case 0:
        result = _online_stats_query_status_none;
        break;
    default:
        result = _online_stats_query_status_none;
        break;
    }
    return result;
}


static __forceinline long online_statistics_xenon_leaderboard_by_id(unsigned long id)
{
    long result = -1;
    for (unsigned long index = 0; index < 99; index++)
    {
        if (k_online_leaderboard_id_map[index] == id)
        {
            result = index;
            break;
        }
    }
    return result;
}

static __forceinline long online_statistics_xenon_column_by_id_and_view(unsigned short id, unsigned long view)
{
    long result = -1;
    for (unsigned long view_index = 0; view_index < 99; view_index++)
    {
        const s_online_statistics_xenon_column_view *current_view = &k_online_statistics_column_views[view_index];
        if ((unsigned long)current_view->view == view)
        {
            long column_index = 0;
            if (current_view->count > 0)
            {
                for (; column_index < current_view->count; column_index++)
                {
                    if (current_view->columns[column_index].id == id)
                    {
                        result = current_view->columns[column_index].column;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

bool online_stats_query_get_results(long user_index, s_online_stat_query_result *result_out)
{
    bool result = false;
    memset(result_out, 0, sizeof(*result_out));
    XUSER_STATS_READ_RESULTS *results = g_online_statistics_globals.query_tasks[user_index]->get_results();
    if (results->dwNumViews > 4)
        goto invalid_results;
    result_out->leaderboard_count = results->dwNumViews;
    for (long leaderboard_index = 0; leaderboard_index < result_out->leaderboard_count; leaderboard_index++)
    {
        s_online_stat_query_leaderboard *leaderboard_result_out = &result_out->leaderboard_results[leaderboard_index];
        XUSER_STATS_VIEW *view = &results->pViews[leaderboard_index];
        if (view->dwNumRows > 16)
            goto invalid_results;
        leaderboard_result_out->leaderboard_id = online_statistics_xenon_leaderboard_by_id(view->dwViewId);
        leaderboard_result_out->player_count = view->dwNumRows;
        for (long player_index = 0; player_index < leaderboard_result_out->player_count; player_index++)
        {
            XUSER_STATS_ROW *row = &view->pRows[player_index];
            s_online_stat_query_player *player_stat_collection = &leaderboard_result_out->player_stats[player_index];
            if (row->dwNumColumns > 32)
                goto invalid_results;
            player_stat_collection->xuid = row->xuid;
            csstrnzcpy(player_stat_collection->gamertag, row->szGamertag, 16);
            player_stat_collection->stat_count = row->dwNumColumns;
            for (long stat_index = 0; stat_index < player_stat_collection->stat_count; stat_index++)
            {
                XUSER_STATS_COLUMN *column = &row->pColumns[stat_index];
                s_online_stat_query_stat *stat = &player_stat_collection->stats[stat_index];
                stat->column_id = online_statistics_xenon_column_by_id_and_view(column->wColumnId, view->dwViewId);
                stat->data = column->Value;
            }
        }
    }
    result = true;
invalid_results:
    return result;
}

static __forceinline unsigned long online_statistics_xenon_leaderboard_id_get(long leaderboard_id) { return k_online_leaderboard_id_map[leaderboard_id]; }
static __forceinline unsigned long online_statistics_xenon_property_id_get(long property_id) { return k_online_property_id_map[property_id]; }

// @address 0x408758 ?online_global_stats_write_initiate@@YA_NPAXJPBUs_online_stat_write@@@Z
bool online_global_stats_write_initiate(void * context, long write_count, struct s_online_stat_write const * writes)
{
    // match: 0x408758: value range assumption guides branch lowering
    __assume(write_count == 1);
    bool success = false;
    s_internal_leaderboard_write leaderboard_writes[4];
    memset(leaderboard_writes, 0, sizeof(leaderboard_writes));
    for (long write_index = 0; write_index < write_count; write_index++)
    {
        s_internal_leaderboard_write *current_write = &leaderboard_writes[write_index];
        const s_online_stat_write *write = &writes[write_index];
        current_write->leaderboard_id = online_statistics_xenon_leaderboard_id_get(write->leaderboard_id);
        current_write->property_count = write->property_count;
        for (long property_index = 0; property_index < write->property_count; property_index++)
        {
            XUSER_PROPERTY *current_property = &current_write->properties[property_index];
            const XUSER_PROPERTY *property = &write->properties[property_index];
            current_property->dwPropertyId = online_statistics_xenon_property_id_get(property->dwPropertyId);
            current_property->value = property->value;
        }
    }
    (g_online_statistics_globals.global_write_task)
        ->initialize((void *)(context), 0, write_count, leaderboard_writes);
    success = ::overlapped_task_start_internal(g_online_statistics_globals.global_write_task,
        (char const *)("..\\networking\\online\\online_statistics_xenon.cpp"), 492);
    return success;
}

// @address 0x408850 ?online_stats_write_initiate@@YA_NJPAX_KJPBUs_online_stat_write@@@Z
bool online_stats_write_initiate(long player_index, void * handle, unsigned __int64 xuid, long write_count, struct s_online_stat_write const * writes)
{
    s_internal_leaderboard_write leaderboard_writes[4];
    memset(leaderboard_writes, 0, sizeof(leaderboard_writes));
    for (long write_index = 0; write_index < write_count; ++write_index)
    {
        s_internal_leaderboard_write *current_write = &leaderboard_writes[write_index];
        const s_online_stat_write *source_write = &writes[write_index];
        current_write->leaderboard_id = online_statistics_xenon_leaderboard_id_get(source_write->leaderboard_id);
        current_write->property_count = source_write->property_count;
        for (long property_index = 0; property_index < source_write->property_count; ++property_index)
        {
            XUSER_PROPERTY *current_property = &current_write->properties[property_index];
            const XUSER_PROPERTY *source_property = &source_write->properties[property_index];
            current_property->dwPropertyId = online_statistics_xenon_property_id_get(source_property->dwPropertyId);
            current_property->value = source_property->value;
        }
    }
    (g_online_statistics_globals.write_tasks[player_index])
        ->initialize((void *)(handle), xuid, write_count, leaderboard_writes);
    return ::overlapped_task_start_internal(g_online_statistics_globals.write_tasks[player_index],
        (char const *)("..\\networking\\online\\online_statistics_xenon.cpp"), 580);
}
