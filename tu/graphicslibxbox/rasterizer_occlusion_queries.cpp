// graphicslibxbox/rasterizer_occlusion_queries.obj: from the records 0x183068 0x1837b0 0x183848 0x183938 0x468158 0x468200
// @flags /TP /O2 /Oc
#include <stdlib.h>
#include <xtl.h>
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_occlusion_queries.h"
#include "rasterizer/occlusion_query_types.h"
#include "d3d9/device.h"
#include "render/render.h"
#include "rasterizer/rasterizer_occlusion_query_globals.h"

// @address 0x88e370 g_occlusion_query_globals
s_occlusion_query_globals g_occlusion_query_globals;

// @address 0x183068 ?rasterizer_occlusions_retrieve@@YAXJ@Z
// @address 0x1837b0 ?rasterizer_occlusions_get_result@@YA_NW4e_occlusion_query_types@@JJPAJ1@Z
// @address 0x183848 ?find_entry@?$c_simple_hash_table@Us_occlusion_query@@$0BAA@$0MOL@$02$0BFB@@@QAAJJJJ@Z
// @address 0x183938 ?rasterizer_occlusion_submit@@YAJW4e_occlusion_query_types@@JJJP6AXJ@Z@Z
// @address 0x468200 ?add_entry@?$c_simple_hash_table@Us_occlusion_query@@$0BAA@$0MOL@$02$0BFB@@@QAAJJJJ@Z
// @address 0x18f208 D3DDevice_EndConditionalSurvey
// @address 0x73a188 ?m_frame_index@c_render_globals@@0KA

static __forceinline long calculate_occlusion_query_hash(long key0, long key1, long key2)
{
    return abs(key0 * 3307 + key2 * 337 + key1 * 3) % 256;
}

static __forceinline long find_new_occlusion_query_hash_entry(c_occlusion_query_hash_table *table, long original_hash)
{
    long remaining_entries = table->hash_entry_count[original_hash];
    long probe = original_hash;
    while (remaining_entries > 0)
    {
        if (table->original_hash_index[(probe++) % 256] == original_hash)
            remaining_entries--;
    }
    bool found = false;
    while (!found)
    {
        found = table->original_hash_index[probe % 256] == -1;
        if (!found)
            probe++;
    }
    return probe % 256;
}


void rasterizer_occlusions_retrieve(long pool_index)
{
    long frame_index = c_render_globals::get_frame_index();
    c_occlusion_query_hash_table *pool = &g_occlusion_query_globals.pools[pool_index];
    for (long index = pool->iterator_next(-1);
         index != -1;
         index = pool->iterator_next(index))
    {
        s_occlusion_query *query = pool->get_element(index);
        if (query->pending)
        {
            D3DOCCLUSIONSTATS data;
            HRESULT status = query->device_query->GetData(&data, sizeof(data), 0);
            switch (status)
            {
            case S_FALSE:
                query->pending = true;
                break;
            case S_OK:
                query->pending = false;
                query->result = data.ZPass;
                query->available = true;
                break;
            default:
                query->device_query->Release();
                query->device_query = 0;
                c_rasterizer::get_device()->CreateQueryTiled(D3DQUERYTYPE_OCCLUSION, 6, &query->device_query);
                break;
            }
        }
        if (frame_index - query->frame_index > 3)
            pool->remove_entry(index);
    }
}


bool rasterizer_occlusions_get_result(e_occlusion_query_types type, long key, long player_window_index, long *result, long *context)
{
    c_occlusion_query_hash_table *table = &g_occlusion_query_globals.pools[player_window_index];
    bool has_result = false;
    long index = table->find_entry(type, key, player_window_index);
    *result = 0;
    *context = -1;
    if (index != -1)
    {
        s_occlusion_query *query = table->get_element(index);
        if (query->available)
        {
            has_result = true;
            *result = query->result;
        }
        if (query->context != -1)
        {
            *context = query->context;
            has_result = true;
        }
    }
    return has_result;
}

template<>
inline long c_occlusion_query_hash_table::find_entry(long key0, long key1, long key2)
{
    long original_hash = calculate_occlusion_query_hash(key0, key1, key2);
    long probe = original_hash;
    long remaining_entries = hash_entry_count[original_hash];
    long result = -1;
    long iterations = 0;
    while (remaining_entries > 0 && result == -1)
    {
        if (original_hash_index[probe] == original_hash)
        {
            s_occlusion_query_hash_data *entry = &data[probe];
            remaining_entries--;
            if (entry->key0 == key0 && entry->key1 == key1 && entry->key2 == key2)
                result = probe;
        }
        probe = (probe + 1) % 256;
        if (iterations++ > 256)
            break;
    }
    return result;
}

long rasterizer_occlusion_submit(e_occlusion_query_types type, long flags, long key, long player_window_index, void (*callback)(long))
{
    c_occlusion_query_hash_table *table = &g_occlusion_query_globals.pools[player_window_index];
    long index = table->find_entry(type, key, player_window_index);
    if (index == -1)
    {
        index = table->add_entry(type, key, player_window_index);
        if (index != -1)
        {
            s_occlusion_query *query = table->get_element(index);
            query->frame_index = 0;
            query->pending = false;
            query->result = 0;
            query->available = false;
            query->context = -1;
        }
    }
    if (index != -1)
    {
        s_occlusion_query *query = table->get_element(index);
        if (!query->pending && (flags & 1))
        {
            query->device_query->Issue(2);
            callback(key);
            query->device_query->Issue(1);
            query->pending = true;
            query->frame_index = c_render_globals::get_frame_index();
        }
        if (flags & 2)
        {
            query->context = -1;
            if (g_occlusion_query_globals.active <= 63)
            {
                query->context = g_occlusion_query_globals.active;
                g_occlusion_query_globals.active++;
                d3d_device_issue_occlusion_query((D3D::CDevice *)c_rasterizer::get_device(), query->context, (~(unsigned long)flags >> 2) & 1);
            }
            callback(key);
            if (query->context >= 0 && query->context <= 63)
                D3DDevice_EndConditionalSurvey(c_rasterizer::get_device(), 0);
        }
    }
    return index;
}

// @address 0x468158 rasterizer_occlusion_initialize
void rasterizer_occlusion_initialize(void)
{
  for (long pool_index = 0; pool_index < 4; pool_index++) {
    s_occlusion_pool *pool = &g_occlusion_query_globals.pools[pool_index];
    pool->entry_count = 0;
    memset(pool->hash_entry_count, 0, sizeof(pool->hash_entry_count));
    memset(pool->original_hash_index, 0xffff, sizeof(pool->original_hash_index));
    for (long i = 0; i < 256; i++)
      c_rasterizer::get_device()->CreateQueryTiled(D3DQUERYTYPE_OCCLUSION, 6, &pool->data[i].value.device_query);
  }
  g_occlusion_query_globals.active = false;
}

template<>
inline long c_occlusion_query_hash_table::add_entry(long key0, long key1, long key2)
{
    long result = -1;
    if (entry_count < 256)
    {
        long original_hash = calculate_occlusion_query_hash(key0, key1, key2);
        result = find_new_occlusion_query_hash_entry(this, original_hash);
        if (result != -1)
        {
            s_occlusion_query_hash_data *entry = &data[result];
            entry->key0 = key0;
            entry->key1 = key1;
            entry->key2 = key2;
            original_hash_index[result] = (short)original_hash;
            hash_entry_count[original_hash]++;
        }
        entry_count++;
    }
    return result;
}

