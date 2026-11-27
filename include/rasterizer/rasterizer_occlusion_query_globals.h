#ifndef RASTERIZER_RASTERIZER_OCCLUSION_QUERY_GLOBALS_H
#define RASTERIZER_RASTERIZER_OCCLUSION_QUERY_GLOBALS_H

#include <stddef.h>
#include "rasterizer/simple_hash_table.h"

struct D3DQuery;

struct s_occlusion_query
{
    bool pending;
    bool available;
    long result;
    long frame_index;
    long context;
    D3DQuery *device_query;
};

typedef c_simple_hash_table<s_occlusion_query, 256, 3307, 3, 337> c_occlusion_query_hash_table;
typedef c_occlusion_query_hash_table s_occlusion_pool;
typedef c_occlusion_query_hash_table::s_entry s_occlusion_query_hash_data;

template<> inline long c_occlusion_query_hash_table::find_entry(long key0, long key1, long key2);
template<> inline long c_occlusion_query_hash_table::add_entry(long key0, long key1, long key2);

struct s_occlusion_query_globals
{
    s_occlusion_pool pools[4];
    signed char active;
};

extern s_occlusion_query_globals g_occlusion_query_globals;

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char occlusion_query_size[(sizeof(void *) != 4 || sizeof(s_occlusion_query) == 20) ? 1 : -1];
typedef char occlusion_pool_size[(sizeof(void *) != 4 || sizeof(s_occlusion_pool) == 9220) ? 1 : -1];
typedef char occlusion_globals_active_offset[(sizeof(void *) != 4 || offsetof(s_occlusion_query_globals, active) == 36880) ? 1 : -1];
#endif

#endif // RASTERIZER_RASTERIZER_OCCLUSION_QUERY_GLOBALS_H
