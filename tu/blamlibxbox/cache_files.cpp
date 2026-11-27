// blamlibxbox/cache_files.obj: from the records 0x82308 0x87af0 0xf17b8 0x1a0e60 0x1a1060 0x1a10b0 0x1a11d0 0x1a17e8 0x1a18d0 0x1a1948 0x1a19d0 0x1a1a30 0x1a1b68 0x1a1c60 0x1a1cf0 0x1f04d8 0x1f5de0 0x1f5e98 0x219bc0 0x2af458
// @flags /TP /O2 /Oc
#include "game/game_engine_waypoint_constants.h"
// @address 0x81e90 cache_files_windows_get_map_status
// @address 0x82308 ?print@?$c_static_string@$0BAA@@@QAAPBDPBDZZ
#include "cache/cached_map_file_read.h"
#include "tag_files/tag_groups.h"
#include "game/game_globals.h"
#include "main/thread_globals_lock.h"
#include "multithreading/thread_tls.h"
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include "cache/cache_file_globals.h"
#include "cache/cache_file_memory_region.h"
#include "cache/cache_file_progress_types.h"
#include "cache/cache_files.h"
#include "cache/cache_files_windows.h"
#include "cache/security_functions.h"

#include "main/main.h"
#include "main/loading.h"
#include "cseries/async.h"
#include "networking/session/network_session_parameters.h"
#include "cache/cache_file_allocation_range_types.h"
#include "cache/cache_file_ancestor_list_types.h"
#include "multithreading/threads.h"
#include "cache/cache_file_runtime_globals.h"
#include "cache/cache_file_byte_swap.h"
#include "cseries/async_block.h"
#include "cseries/xdk_memory.h"
#include "cseries/pointer_utilities.h"
#include "rasterizer/rasterizer_device_globals.h"
#include "memory/physical_memory_map.h"
#include "scenario/scenario_tag.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "main/game_preferences_types.h"
#include "main/thread_globals_lock_inlines.h"
#include "main/game_preferences_inlines.h"
#include "text/text_group.h"

#include "cache/security_signature.h"
#include "shell/shell_build.h"
// @address 0x8a1ead g_utility_drive_mounted
#include "shell/utility_drive.h"
#include "tag_files/tag_resource_internals_types.h"
#include "networking/online/voice_observer_constants.h"


#include "saved_games/game_state_xenon_types.h"
#include "cache/cache_files_windows_paths.h"
#include "multithreading/synchronized_value.h"
#include "xbox/system_time.h"
#include "main/loading_progress.h"
#include "scenario/scenario.h"

// @address 0x119a3e0 g_maps_directory
const char *g_maps_directory;

// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
const cache_file_tag_instance *global_tag_instances;
// @address 0x945f50 ?g_cache_file_globals@@3Us_cache_file_globals@@A
s_cache_file_globals g_cache_file_globals;

struct s_memory_region_information {
    const void *base_address;
    long size;
};
// 0xf17b8

#include "cache/tag_resource_page_table.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cache/tag_resource_cache_new.h"
#include "cache/geometry_globals.h"




struct s_exposed_variable {
    unsigned long identifier;
    const void *data;

    __forceinline s_exposed_variable() : identifier(0), data(0) {}
    static __forceinline s_exposed_variable *get_global_array() { return x_global_array; }
private:
    static s_exposed_variable x_global_array[256];
};

struct s_cache_file_security_globals {
    s_cache_file_header header;
    bool valid;
    long section_sizes[3];
    void *section_bases[3];
    s_network_http_request_hash section_hashes[3];
    unsigned char hash_state[0x400];
    s_network_http_request_hash hash;
    unsigned char signature[256];
};
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char cache_files_size_security_globals[(sizeof(void *) != 4 || (sizeof(s_cache_file_security_globals) == 0x3570)) ? 1 : -1];
typedef char cache_files_offset_section_hashes[(sizeof(void *) != 4 || (offsetof(s_cache_file_security_globals, section_hashes) == 0x301c)) ? 1 : -1];
#endif

static __forceinline const cache_file_tag_group *tag_instance_get_group(const cache_file_tag_instance *instance)
{
    const cache_file_tag_group *group = &g_cache_file_globals.tags_header->groups[instance->group_index];
    return group;
}
static __forceinline const cache_file_tag_instance *tag_instance_try_and_get(long tag_index)
{
    short salt = (short)(tag_index >> 16);
    short index = (short)tag_index;
    const cache_file_tag_instance *instance = 0;
    if ((unsigned long)index < (unsigned long)g_cache_file_globals.tags_header->instance_count &&
        salt == (short)global_tag_instances[index].salt)
        instance = &global_tag_instances[index];
    return instance;
}
template<long k_maximum_count>
const char *c_static_string<k_maximum_count>::print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnprintf(m_string, k_maximum_count - 1, format, args);
    m_string[k_maximum_count - 1] = 0;
    return m_string;
}
template const char *c_static_string<256>::print(const char *format, ...);
void *tag_try_and_get_unsafe(unsigned long group_tag, long tag_index)
{
    const cache_file_tag_instance *instance = tag_instance_try_and_get(tag_index);
    void *result = 0;
    if (instance)
    {
        const cache_file_tag_group *group = tag_instance_get_group(instance);
        if (group->group_tags[0] == group_tag || group->group_tags[1] == group_tag || group->group_tags[2] == group_tag)
            result = instance->definition;
    }
    return result;
}

// @address 0xf17b8 tag_group_get_min_ancestor_depth
extern "C" int tag_group_get_min_ancestor_depth(int initial_group_tag, s_cache_file_ancestor_list ancestors)
{
    int best = 0x7fffffff;
    for (long i = 0; i < ancestors.count; ++i)
    {
        int depth = tag_group_get_ancestor_depth(initial_group_tag, ancestors.tags[i]);
        if (depth <= best)
            best = depth;
    }
    return best;
}

// @address 0x1a1948 cache_file_compare_digests
extern "C" int cache_file_compare_digests(const unsigned char *a, unsigned int size, const unsigned char *b, unsigned char full)
{
    int result = 0;
    if (size == 60) {
        int difference;
        if (full)
            difference = memcmp(a, b, size);
        else
            difference = memcmp(a + size - sizeof(s_network_http_request_hash),
                b + size - sizeof(s_network_http_request_hash), sizeof(s_network_http_request_hash));
        if (difference == 0)
            result = 1;
    }
    return result;
}
static __forceinline bool cache_files_range_contains(const cache_file_allocation_range *range, unsigned int address)
{
    return address >= (unsigned int)range->start && address <= (unsigned int)(range->start + range->size);
}
// @address 0x1a1a30 cache_file_is_tag_address
extern "C" unsigned char cache_file_is_tag_address(unsigned int address)
{
    int result;
    if (cache_files_range_contains(&g_cache_file_globals.header.ranges[0], address) ||
        cache_files_range_contains(&g_cache_file_globals.header.ranges[1], address) ||
        cache_files_range_contains(&g_cache_file_globals.header.ranges[2], address) ||
        cache_files_range_contains(&g_cache_file_globals.header.ranges[3], address) ||
        cache_files_range_contains(&g_cache_file_globals.header.ranges[4], address) ||
        cache_files_range_contains(&g_cache_file_globals.header.ranges[5], address))
        result = 1;
    else
        result = 0;
    return (unsigned char)result;
}

static __forceinline long tag_instance_get_tag_index(const cache_file_tag_instance *instance, long index)
{
    return index | ((long)instance->salt << 16);
}
// match: 0x1a1b68: iterator cannot alias the tag globals, so instance_count stays hoisted in r31 across the next_index++ store
long tag_iterator_next(tag_iterator *__restrict iterator)
{
    long result = -1;
    while (iterator->next_index < g_cache_file_globals.tags_header->instance_count) {
        long index = iterator->next_index;
        const cache_file_tag_instance *instance = &global_tag_instances[index];
        iterator->next_index++;
        if (instance && instance->salt != 0xffff && instance->group_index != -1) {
            bool group_matches;
            if (iterator->group_tag == 0xffffffff)
                group_matches = true;
            else {
                const cache_file_tag_group *group = tag_instance_get_group(instance);
                group_matches = iterator->group_tag == group->group_tags[0] ||
                    iterator->group_tag == group->group_tags[1] ||
                    iterator->group_tag == group->group_tags[2];
            }
            if (group_matches) {
                long tag_index = tag_instance_get_tag_index(instance, index);
                iterator->index = tag_index;
                result = tag_index;
                break;
            }
        }
    }
    return result;
}

// @address 0x72f77f ?cache_files_enable_memory_protection@@3_NA
bool cache_files_enable_memory_protection = true;

// @address 0x219bc0 cache_file_try_and_get_memory_region
extern "C" bool cache_file_try_and_get_memory_region(s_memory_region_information *information)
{
    bool result = false;
    if (cache_files_enable_memory_protection)
    {
        information->base_address = cache_file_globals_get_tag_cache_base_address();
        information->size = cache_files_get_total_tags_size();
        result = true;
    }
    return result;
}

static __forceinline long cache_files_find_group(const s_cache_file_tags_header *header, unsigned long group_tag)
{
    const cache_file_tag_group *base = header->groups;
    long count = header->group_count;
    while (count)
    {
        const cache_file_tag_group *group = base + (count >> 1);
        long difference = group_tag - group->group_tags[0];
        if (!difference)
            return group - header->groups;
        if (difference > 0)
        {
            base = group + 1;
            --count;
        }
        count >>= 1;
    }
    return -1;
}

static __forceinline unsigned long cache_files_parent_group(unsigned long group_tag)
{
    unsigned long parent_group = 0xffffffff;
    const s_cache_file_tags_header *header = g_cache_file_globals.tags_header;
    if (header)
    {
        long index = cache_files_find_group(header, group_tag);
        if (index != -1)
        {
            const cache_file_tag_group *group = &header->groups[index];
            if (group)
                parent_group = group->group_tags[1];
        }
    }
    return parent_group;
}

// match: 0x1a18d0 0x1a19d0: `inline` lets link-time code generation expand this lookup into those two small
// callers; scenario_tags_load calls the out-of-line copy
// @address 0x1a1060 cache_file_get_security_globals
extern "C" inline s_cache_file_security_globals *cache_file_get_security_globals(void)
{
    const void *section_address = 0;
    unsigned long section_size = 0;
    security_get_section_data(0, &section_address, &section_size);
    if (section_size >= sizeof(s_cache_file_security_globals))
        return (s_cache_file_security_globals *)section_address;
    return 0;
}
// @address 0x1a18d0 cache_file_try_and_get_map_digest
extern "C" unsigned char cache_file_try_and_get_map_digest(unsigned long *digest_size, const void **digest_data)
{
    s_cache_file_security_globals *security_globals = cache_file_get_security_globals();
    bool has_map_digest = false;
    if (security_globals && security_globals->valid)
    {
        has_map_digest = true;
        *digest_size = sizeof(security_globals->section_hashes);
        *digest_data = security_globals->section_hashes;
    }
    return has_map_digest;
}
// @address 0x1a19d0 cache_file_reset_security_globals
extern "C" void cache_file_reset_security_globals(void)
{
    s_cache_file_security_globals *security_globals = cache_file_get_security_globals();
    if (security_globals)
        memset(security_globals, 0, sizeof(*security_globals));
}
static __forceinline void cache_files_dispose_all_tags(s_cache_file_tags_header *header)
{
    long tag_index = 0;
    global_tag_instances = 0;
    if (header->interop_fixup_count > 0)
    {
        long entry_offset = 0;
        do
        {
            s_cache_file_tag_interop_type_fixup *entry = (s_cache_file_tag_interop_type_fixup *)((char *)header->interop_fixups + entry_offset);
            const s_tag_resource_interop_type *type = k_tag_resource_interop_types[entry->type_index];
            if (type->teardown != 0)
            {
                type->teardown((s_tag_interop *)entry->interop);
                header = g_cache_file_globals.tags_header;
            }
            tag_index++;
            entry_offset += 8;
        } while (tag_index < header->interop_fixup_count);
    }
}
// @address 0x1a17e8 scenario_tags_unload
extern "C" void scenario_tags_unload(void)
{
    if (xenon_game_state_globals.active_file_index != -1)
    {
        cache_file_block_until_not_busy();
        xenon_game_state_globals.active_file_index = -1;
    }
    cache_file_reset_security_globals();
    g_cache_file_globals.tags_loaded = false;
    cache_files_dispose_all_tags(g_cache_file_globals.tags_header);
    g_cache_file_globals.tag_cache_base_address = 0;
    memset(&g_cache_file_globals.header, 0, sizeof(g_cache_file_globals.header));
    g_cache_file_globals.tags_header = 0;
}
// @address 0x2af458 tag_group_get_ancestor_depth
extern "C" int tag_group_get_ancestor_depth(int initial_group_tag, unsigned int ancestor_group_tag)
{
    long result = 0x7fffffff;
    unsigned long group_tag = initial_group_tag;
    long depth = 0;
    while (group_tag != -1)
    {
        if ((unsigned long)group_tag == ancestor_group_tag)
        {
            result = depth;
            break;
        }
        ++depth;
        group_tag = cache_files_parent_group(group_tag);
    }
    return result;
}

static __forceinline bool cache_files_paths_equal(const char *a, const char *b)
{
    c_static_string<256> first;
    cache_files_windows_get_canonical_map_path(a, &first);
    c_static_string<256> second;
    cache_files_windows_get_canonical_map_path(b, &second);
    return strncmp(first.m_string, second.m_string, 256) == 0;
}
// @address 0x1a1cf0 ?cache_file_map_progress_helper@@YAMW4e_scenario_type@@PBDW4e_cache_file_progress_type@@@Z
float cache_file_map_progress_helper(e_scenario_type type, const char *path, e_cache_file_progress_type progress_type)
{
    float map_weight = 100.0f;
    float completed = 0.0f;
    float total = 0.0f;
    long status = cache_files_windows_get_map_status(path);
    float map_progress = cache_files_windows_get_map_copy_progress(path);
    float menu_progress = cache_files_windows_get_map_copy_progress("levels\\ui\\mainmenu\\mainmenu");
    float multiplayer_progress = cache_files_windows_get_map_copy_progress("levels\\shared\\shared\\shared");
    float campaign_progress = cache_files_windows_get_map_copy_progress("levels\\shared\\shared\\campaign");
    bool introduction = cache_files_paths_equal(path, "levels\\solo\\005_intro\\005_intro");
    if (introduction)
    {
        type = _scenario_type_multiplayer;
        map_weight = 100.0f;
    }
    else if (type == _scenario_type_campaign)
        map_weight = 280.0f;
    else if (type == _scenario_type_multiplayer)
        map_weight = 100.0f;
    long menu_status = cache_files_windows_get_map_status("levels\\ui\\mainmenu\\mainmenu");
    long multiplayer_status = cache_files_windows_get_map_status("levels\\shared\\shared\\shared");
    long campaign_status = cache_files_windows_get_map_status("levels\\shared\\shared\\campaign");
    if (menu_status == 4) menu_progress = 1.0f;
    if (multiplayer_status == 4) multiplayer_progress = 1.0f;
    if (campaign_status == 4) campaign_progress = 1.0f;
    if (introduction) campaign_progress = 1.0f;
    total = 0.0f;
    switch (type)
    {
    case _scenario_type_campaign:
        total = k_game_engine_invalid_dead_point.z;
        completed = campaign_progress * total;
    case _scenario_type_multiplayer:
    case _scenario_type_single_player_shared:
        total += 200.0f;
        completed = multiplayer_progress * 200.0f + completed;
    case _scenario_type_shared:
        total += 80.0f;
        completed = menu_progress * 80.0f + completed;
    case _scenario_type_mainmenu:
        total += map_weight;
        completed = map_progress * map_weight + completed;
    }
    if (total >= 0.0f)
    {
        switch (progress_type)
        {
        case _cache_file_progress_fraction:
            completed = completed / total;
            break;
        case _cache_file_progress_remaining:
            completed = total - completed;
            break;
        default:
            // match: 0x1a1cf0: unreachable hint omits dead branch check
            __assume(0);
        }
    }
    else
    {
        completed = 1.0f;
    }
    if (progress_type == _cache_file_progress_fraction)
        completed = ((completed > 0.0f ? completed : 0.0f) < 1.0f) ? (completed > 0.0f ? completed : 0.0f) : 1.0f;
    if (status == 4)
        completed = 0.0f;
    return completed;
}

class c_tag_resource_page_table;
// @address 0x8a1eaf g_cache_files_globals
// @address 0x1f04d8 cache_file_get_load_progress
extern "C" float cache_file_get_load_progress(int scenario_type, short map_index, const char *path)
{
    float map_progress = cache_file_map_progress_helper((e_scenario_type)scenario_type, path, _cache_file_progress_fraction);
    float result = 0.0f;
    long status = loading_get_map_load_state(-1, path);
    if ((unsigned long)status >= 2)
    {
        if ((unsigned long)status >= 4)
        {
            result = 1.0f;
        }
        else
        {
            c_cache_file_tag_resource_runtime_manager *manager = g_geometry_globals.manager;
            if (manager != 0 && manager->m_cache_file_resource_gestalt_index != -1)
            {
                c_static_string<256> name;
                cache_files_windows_get_canonical_map_path(path, &name);
                if (!manager->m_active_zone_changed && strncmp(manager->m_zone_names.m_first.m_name.m_string, name.m_string, 256) == 0)
                {
                    if (map_index == -1 || manager->m_zone_names.m_first.m_value == map_index)
                        result = tag_resource_page_table_get_loading_progress(&manager->m_resource_cache.m_cache.m_page_table);
                }
            }
        }
    }
    bool unlimited = CACHE_FILES_OVERRIDE || (CACHE_FILES_ENABLED && !g_utility_drive_mounted);
    if (unlimited)
        return result;
    return (result + map_progress) * 0.5f;
}

// @address 0x1f5e98 cache_file_header_reset_for_hash
extern "C" void cache_file_header_reset_for_hash(s_cache_file_header *header)
{
    header->checksum = 0;
    header->timestamp328 = 0;
    memset(&header->digest, 0, sizeof(header->digest));
    memset(header->signature, 0, sizeof(header->signature));
    memset(header->source_path, 0, sizeof(header->source_path));
    memset(&header->cached_map_date, 0, sizeof(header->cached_map_date));
    memset(header->timestamp, 0, sizeof(header->timestamp));
    memset(header->level_timestamps, 0, sizeof(header->level_timestamps));
    header->trailing_fields[0] = 0;
    header->trailing_fields[1] = 0;
    header->trailing_fields[2] = 0;
    header->trailing_fields[3] = 0;
}
static __forceinline void cache_files_header_construct(s_cache_file_header *header)
{
    header->source_path[0] = 0;
    header->build[0] = 0;
    header->name[0] = 0;
    header->scenario_path[0] = 0;
    header->copyright[0] = 0;
    header->ranges[0].start = 0;
    header->ranges[0].size = 0;
    header->ranges[1].start = 0;
    header->ranges[1].size = 0;
    header->ranges[2].start = 0;
    header->ranges[2].size = 0;
    header->ranges[3].start = 0;
    header->ranges[3].size = 0;
    header->ranges[4].start = 0;
    header->ranges[4].size = 0;
    header->ranges[5].start = 0;
    header->ranges[5].size = 0;
}
// @address 0x1a10b0 cache_file_header_verify_signature
extern "C" unsigned char cache_file_header_verify_signature(const s_cache_file_header *header)
{
    bool result = false;
    if (header->header_signature == k_cache_file_header_signature && header->footer_signature == k_cache_file_footer_signature)
    {
        s_cache_file_header copy;
        cache_files_header_construct(&copy);
        memcpy(&copy, header, sizeof(copy));
        cache_file_header_reset_for_hash(&copy);
        cache_file_header_to_disk(&copy);
        const s_cache_file_digest *expected = &header->digest;
        if (text_group_verify_digest(&copy, sizeof(copy), true, &expected->hashes[0], 0))
        {
            s_network_http_request_hash hash;
            security_calculate_hash((const char *)expected, 60, true, &hash);
            unsigned char signature[256];
            memcpy(signature, header->signature, sizeof(signature));
            if (security_verify_signature(&hash, signature) != 0)
                result = true;
        }
    }
    return result;
}

static __forceinline void cache_files_async_yield_until_done(c_synchronized_long *done, bool with_spinner, bool with_idle, int file, long line)
{
    async_block_until_done(done, simple_yield_function, with_spinner, with_idle, file, line);
}

// @address 0x1a1c60 ?cache_file_blocking_read@@YA_NJJJPAX@Z
bool cache_file_blocking_read(long file, long offset, long size, void *destination)
{
    c_synchronized_long done(0);
    c_synchronized_long size_read(0);
    bool success = false;
    cached_map_file_read_header(file, offset, size, destination, &size_read, &done, 2, 9);
    cache_files_async_yield_until_done(&done, false, true, 1, 0);
    if (size_read.peek() == size)
        success = true;
    return success;
}

// @address 0x1f5de0 cache_file_get_max_file_size
extern "C" unsigned long cache_file_get_max_file_size(unsigned long mode)
{
    unsigned long size = 0;
    switch (mode)
    {
    case 1:
        size = 0x9600000;
        break;
    case 0:
        size = 0x1f400000;
        break;
    case 2:
        size = 0x4100000;
        break;
    case 3:
        size = 0xbe00000;
        break;
    case 4:
        size = 0x22600000;
        break;
    }
    unsigned char unlimited;
    if (CACHE_FILES_OVERRIDE == 0 && (CACHE_FILES_ENABLED == 0 || g_utility_drive_mounted != 0))
        unlimited = 0;
    else
        unlimited = 1;
    if (unlimited == 0)
        return size;
    return 0x7fffffff;
}

// @address 0x74b868 g_cache_tag_memory_base
// @address 0x74b86c g_cache_tag_memory_end
extern "C" {
long g_cache_tag_memory_base = -0x60000000;
long g_cache_tag_memory_end = -0x40400000;
}
// @address 0x1a0e60 ?cache_file_header_verify@@YA_NPBUs_cache_file_header@@PBD_N@Z
bool cache_file_header_verify(const s_cache_file_header *game_state_header, const char *file_name, bool p2)
{
    c_static_string<256> message;
    bool result = false;
    unsigned __int64 file_time[2];
    long maximum_size;
    unsigned long tag_memory_size;
    unsigned long tag_memory_base;
    unsigned long tag_memory_end;

    GetSystemTimeAsFileTime(file_time);
    if (game_state_header->version != k_cache_file_version)
        message.print("is an old version (%d, should be %d)", game_state_header->version, k_cache_file_version);
    else if (game_state_header->header_signature != k_cache_file_header_signature)
        message.print("does not have a valid header signature (is %08x, should be %08x)", game_state_header->header_signature, k_cache_file_header_signature);
    else if (game_state_header->footer_signature != k_cache_file_footer_signature)
        message.print("does not have a valid footer signature (is %08x, should be %08x)", game_state_header->footer_signature, k_cache_file_footer_signature);
    else if (game_state_header->xdk_version != 0x17f4)
        message.print("mismatch in xdk versions (cache file is %d, code is %d)", game_state_header->xdk_version, 0x17f4);
    else if ((tag_memory_size = game_state_header->total_tags_size) <= (unsigned long)(g_cache_tag_memory_end - g_cache_tag_memory_base))
    {
        do
        {
            if (tag_memory_size != 0)
            {
                tag_memory_base = game_state_header->tag_memory_base;
                tag_memory_end = tag_memory_size + tag_memory_base;
                if (tag_memory_end != (unsigned long)g_cache_tag_memory_end)
                {
                    message.print("tag memory address invalid (cache file is %p-%p, code is %p-%p)",
                        tag_memory_base, tag_memory_end, g_cache_tag_memory_base, g_cache_tag_memory_end);
                    break;
                }
            }
            if (game_state_header->file_size >= 0 && game_state_header->file_size <= 0x7fffffff)
            {
                if (game_state_header->scenario_type >= 0 && game_state_header->scenario_type < 5)
                {
                    maximum_size = (long)cache_file_get_max_file_size(game_state_header->scenario_type);
                    if (game_state_header->file_size > maximum_size)
                        message.print("was too large for its scenario type (%d, should be within [0, %d])", game_state_header->file_size, maximum_size);
                    else if (strlen(game_state_header->name) >= 32)
                        message.print("had a corrupt name");
                    else if ((unsigned char)cache_file_header_verify_signature(game_state_header) == 0)
                        message.print("failed actual validation");
                    else
                        result = true;
                }
                else
                    message.print("had an invalid scenario type (%d, should be within [0, %d))", game_state_header->scenario_type, 5);
            }
            else
                message.print("had a size out of range for any cache file (%d, should be within [0, %d])", game_state_header->file_size, 0x7fffffff);
        } while (0);
    }
    else
        message.print("tag memory size invalid (%d, should be within [0, %d]", tag_memory_size, g_cache_tag_memory_end - g_cache_tag_memory_base);
    return result;
}

// ---- 0x1a11d0 scenario_tags_load ----
enum memory_stage;

extern "C" {
struct _XHASH_STATE;
typedef _XHASH_STATE XHASH_STATE;
}
class c_interlocked_long;
// @address 0x8c29b4 ?g_exposed_variable_list@@3PAUs_exposed_variable@@A
s_exposed_variable *g_exposed_variable_list = 0;
// @address 0x8c46c8 g_rasterizer_max_vs_gprs
// @address 0x8c46cc g_rasterizer_max_ps_gprs
static __forceinline s_cache_file_header *cache_file_globals_get_header()
{
    return &g_cache_file_globals.header;
}
static __forceinline const cache_file_tag_instance *tag_header_get_instances(const s_cache_file_tags_header *header)
{
    return header->instances;
}
static __forceinline long cache_file_round_up_read_size(long size)
{
    long rounded_size = size;
    if ((size & 0xfff) != 0)
        rounded_size = (size | 0xfff) + 1;
    return rounded_size;
}
static __forceinline bool loading_basic_progress_enabled()
{
    return g_loading_progress.visible;
}
static __forceinline void loading_basic_progress_phase_begin(long stage, unsigned int units)
{
    if (loading_basic_progress_enabled()) {
        g_loading_progress.stage = stage;
        loading_update_progress(units);
    }
}
static __forceinline void loading_basic_progress_phase_end()
{
    if (loading_basic_progress_enabled()) {
        loading_update_progress(0);
        g_loading_progress.stage = -1;
    }
}
static __forceinline bool cache_file_header_verify_and_version(const s_cache_file_header *header, const char *path, bool fail_fatally)
{
    bool valid = false;
    if (cache_file_header_verify(header, path, fail_fatally)
        && header->tracked_build
        && shell_build_string_is_compatible(header->build) != 0)
        valid = true;
    return valid;
}
static __forceinline void global_preferences_invalidate_maps()
{
    bool skip = g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 3;
    if (!skip) {
        c_thread_globals_lock lock;
        s_global_preferences *preferences = global_preferences_thread_storage();
        preferences->primary_file.data.map_count = 0;
        preferences->dirty = true;
    }
}
static __forceinline void cache_file_close()
{
    if (xenon_game_state_globals.active_file_index != -1) {
        cache_file_block_until_not_busy();
        xenon_game_state_globals.active_file_index = -1;
    }
}
// @address 0x1a11d0 ?scenario_tags_load@@YA_NPBD@Z
bool scenario_tags_load(const char *scenario_path)
{
    long scenario_index = -1;
    bool success = false;
    bool file_opened = false;
    cache_file_reset_security_globals();
    if (cache_files_windows_set_active_map(scenario_path, cache_file_globals_get_header()))
    {
        file_opened = true;
        if (cache_file_header_verify_and_version(cache_file_globals_get_header(), scenario_path, false))
        {
            g_cache_file_globals.tag_cache_base_address = _physical_memory_malloc((memory_stage)5, (const char *)0,
                cache_file_globals_get_header()->total_tags_size, 4, (const char *)0, -1);
            success = g_cache_file_globals.tag_cache_base_address != 0;
            XPhysicalProtect(g_cache_file_globals.tag_cache_base_address, cache_file_globals_get_header()->total_tags_size, 4);
            if (success)
            {
                loading_basic_progress_phase_begin(1, (unsigned int)cache_file_round_up_read_size(cache_file_globals_get_header()->total_tags_size));
                success = cache_file_blocking_read(2, cache_file_globals_get_header()->tags_offset,
                    cache_file_round_up_read_size(cache_file_globals_get_header()->total_tags_size), g_cache_file_globals.tag_cache_base_address);
                loading_basic_progress_phase_end();
            }
            if (success)
            {
                main_update_idle_tasks();
                s_cache_file_security_globals *security = cache_file_get_security_globals();
                if (security)
                {
                    security->valid = false;
                    memset(security->section_hashes, 187, sizeof(security->section_hashes));
                    memset(&security->hash, 204, sizeof(security->hash));
                    memset(security->signature, 221, sizeof(security->signature));
                    memcpy(&security->header, cache_file_globals_get_header(), sizeof(s_cache_file_header));
                    cache_file_header_reset_for_hash(&security->header);
                    cache_file_header_to_disk(&security->header);
                    security->section_sizes[0] = 0x3000;
                    security->section_sizes[1] = g_cache_file_globals.header.ranges[0].size +
                        g_cache_file_globals.header.ranges[1].size + g_cache_file_globals.header.ranges[2].size;
                    security->section_sizes[2] = g_cache_file_globals.header.ranges[3].size +
                        g_cache_file_globals.header.ranges[4].size + g_cache_file_globals.header.ranges[5].size;
                    security->section_bases[0] = security;
                    security->section_bases[1] = g_cache_file_globals.tag_cache_base_address;
                    security->section_bases[2] = (char *)g_cache_file_globals.tag_cache_base_address + security->section_sizes[1];
                    XHASH_STATE *hash_state = (XHASH_STATE *)security->hash_state;
                    for (long section_index = 0; section_index < 3; section_index++)
                    {
                        const char *data = (const char *)security->section_bases[section_index];
                        long remaining = security->section_sizes[section_index];
                        if (security_initialize_hash(hash_state, 1024, true))
                        {
                            while (remaining > 0)
                            {
                                long chunk_size = remaining > 0x100000 ? 0x100000 : remaining;
                                XHashMemoryUpdate(hash_state, data, chunk_size);
                                main_update_idle_tasks();
                                remaining -= chunk_size;
                                data += chunk_size;
                            }
                            XHashMemoryEnd(hash_state, security->section_hashes[section_index].hash);
                        }
                    }
                    security_calculate_hash((const char *)security->section_hashes, sizeof(security->section_hashes), true, &security->hash);
                    memcpy(security->signature, cache_file_globals_get_header()->signature, sizeof(security->signature));
                    main_update_idle_tasks();
                    s_cache_file_security_globals *verified = cache_file_get_security_globals();
                    success = security_verify_signature(&verified->hash, verified->signature);
                    if (success)
                        verified->valid = true;
                }
                else
                {
                    success = false;
                }
            }
            if (success && !cache_file_tags_to_host(cache_file_globals_get_header(), g_cache_file_globals.tag_cache_base_address))
                success = false;
            if (success)
            {
                s_cache_file_tags_header *tags_header = cache_file_globals_get_header()->tags_header;
                success = success && cache_file_is_tag_address(address_from_pointer(tags_header)) != 0;
                success = success && tag_header_get_instances(tags_header) != 0;
                success = success && tags_header->instance_count > 0;
                success = success && tags_header->signature == 'tags';
                if (success)
                    g_cache_file_globals.tags_header = tags_header;
            }
            if (success)
            {
                void *protect_address = pointer_from_address(g_cache_file_globals.header.ranges[4].start);
                XPhysicalProtect(protect_address, g_cache_file_globals.header.ranges[4].size, 0x404);
                XQueryMemoryProtect(protect_address);
            }
            if (success)
            {
                global_tag_instances = g_cache_file_globals.tags_header->instances;
                s_exposed_variable::get_global_array()[1].identifier = 0x475449;
                s_exposed_variable::get_global_array()[1].data = global_tag_instances;
                g_exposed_variable_list = s_exposed_variable::get_global_array();
                g_cache_file_globals.tags_loaded = true;
                scenario_index = cache_file_get_global_tag_index('scnr');
            }
        }
    }
    if (!success)
    {
        if (file_opened)
            global_preferences_invalidate_maps();
        if (g_cache_file_globals.tag_cache_base_address != 0)
            g_cache_file_globals.tag_cache_base_address = 0;
        cache_file_close();
    }
    global_scenario_index = scenario_index;
    if (scenario_index != -1)
    {
        global_scenario_game_globals_index = cache_file_get_global_tag_index('matg');
        global_scenario = TAG_GET('scnr', scenario, global_scenario_index);
        global_game_globals = TAG_GET('matg', s_game_globals, global_scenario_game_globals_index);
        long rasterizer_globals_index = global_game_globals->rasterizer_globals_ref.index;
        c_rasterizer_globals *rasterizer_globals = rasterizer_globals_index != -1
            ? TAG_GET('rasg', c_rasterizer_globals, rasterizer_globals_index) : 0;
        g_rasterizer_max_vs_gprs = rasterizer_globals->max_vs_gprs;
        g_rasterizer_max_ps_gprs = rasterizer_globals->max_ps_gprs;
        success = true;
    }
    return success;
}

s_exposed_variable s_exposed_variable::x_global_array[256];

