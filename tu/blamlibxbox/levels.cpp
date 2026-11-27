// blamlibxbox/levels.obj: from the records 0x845a8 0xa6cf8 0xa6e18 0x1e50f8 0x1e51e8 0x1e5208 0x1e5330 0x1e53f0 0x1e5500 0x1e5620 0x1e5768 0x1e5828 0x1e58e8 0x1e5a18 0x1e66a0 0x1e6b28 0x1e7238 0x21bed8 0x4d8ca8
#include "scenario/scenario.h"
#include "memory/runtime_state_allocation.h"
#include "cseries/async_helpers.h"
#include "cseries/byte_swapping.h"
#include "cseries/async_block.h"
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include "game/game_globals_storage.h"
#include "game/game_types.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_object_types.h"
#include "main/main_flag_state.h"
#include "bink/bink_playback_types.h"
#include "interface/user_interface.h"
#include "text/unicode.h"
#include "text/font_cache.h"
#include "main/levels.h"
#include "saved_games/content_catalogue.h"
#include "cache/cache_files.h"
#include "saved_games/autosave_queue.h"
#include "cseries/cseries.h"
#include "bink/bink_playback.h"
#include "cseries/async.h"
#include "main/levels_map_datum_types.h"
#include "saved_games/iterator_types.h"
#include "memory/wrapped_arrays.h"





#include "multithreading/synchronized_value.h"
#include "scenario/scenario_tag.h"
#include "memory/data.h"

#include "main/levels_globals.h"
#include "cseries/crt_xdk.h"
#include "main/main_runtime_globals.h"
#include "networking/tools/blf_header_types.h"
#include "xbox/language_bridge.h"
#include "cseries/language.h"
#include "cseries/bit_operations.h"
#include "cache/cache_file_resource_element_set.h"


struct levels_campaign_datum
{
    short identifier;
    unsigned short flags;
    long unique_id;
    wchar_t name[64];
    wchar_t description[128];
    long map_ids[64];
};




s_levels_globals g_level_globals;
struct _bink_globals;
struct s_synchronization_globals;
// @address 0x74b80c g_runtime_state_allocation
// @address 0x74b540 g_current_language




struct s_levels_campaign_source
{
    s_blf_header header;
    unsigned long campaign_id;
    unsigned long flags;
    wchar_t names[12][64];
    wchar_t descriptions[12][128];
    unsigned long map_ids[64];
};

struct s_levels_insertion_source
{
    bool visible;
    unsigned char flags;
    unsigned short zone_set_index;
    long pad4;
    wchar_t names[12][32];
    wchar_t descriptions[12][128];
};

struct s_levels_map_source
{
    s_blf_header header;
    unsigned long map_id;
    unsigned long flags;
    wchar_t names[12][32];
    wchar_t descriptions[12][128];
    char image_file_base[256];
    char scenario_file[256];
    unsigned long presence_context_id;
    unsigned long sort_order;
    unsigned char mp_minimum_desired_players;
    unsigned char mp_maximum_desired_players;
    signed char maximum_teams[11];
    char insertion_alignment[7]; // Align the insertion records to an eight-byte boundary.
    s_levels_insertion_source insertions[4];
};



static __forceinline void levels_critical_section_leave(long index)
{
    s_critical_section_table *globals = (s_critical_section_table *)(&g_synch_globals);
    _InterlockedDecrement(&globals->objects[index].lock_count);
    RtlLeaveCriticalSection((_RTL_CRITICAL_SECTION *)&globals->objects[index].critical_section);
}

// @address 0xa6e18 levels_initialize
extern "C" void levels_initialize()
{
    memset(&g_level_globals, 0, sizeof(g_level_globals));
    g_level_globals.enumeration_task_id = -1;
    g_level_globals.campaign_levels = data_new("campaigns", 4, 0x288, 0, (c_allocation_base *)&g_runtime_state_allocation);
    g_level_globals.map_levels = data_new("campaign levels", 64, 0x360, 0, (c_allocation_base *)&g_runtime_state_allocation);
    g_level_globals.campaign_insertions = data_new("campaign insertions", 32, 0x50c, 0, (c_allocation_base *)&g_runtime_state_allocation);
    g_level_globals.multiplayer_levels = data_new("multiplayer levels", 50, 0x360, 0, (c_allocation_base *)&g_runtime_state_allocation);
    data_make_valid(g_level_globals.campaign_levels);
    data_make_valid(g_level_globals.map_levels);
    data_make_valid(g_level_globals.campaign_insertions);
    data_make_valid(g_level_globals.multiplayer_levels);
    g_level_globals.default_map.flags = 16;
    g_level_globals.default_map.unique_id = 0x10231971;
    g_maps_directory = "d:\\maps\\";
    csnzprintf(g_level_globals.default_map.name, 256, "%s%s", "d:\\maps\\", "mainmenu");
    g_level_globals.enabled = 1;
    g_level_globals.enumerate_info_files = 1;
}



static __forceinline bool levels_enumeration_in_progress()
{
    return g_level_globals.enumeration_task_id != -1 && g_level_globals.enumeration_complete.peek() == 0;
}

// @address 0x1e50f8 levels_dispose
extern "C" void levels_dispose()
{
    if (levels_enumeration_in_progress())
        async_block_until_done(&g_level_globals.enumeration_complete, simple_yield_function, 0, 0, 0, 0);
    g_level_globals.enabled = 0;
    g_level_globals.campaign_levels->valid = 0;
    g_level_globals.map_levels->valid = 0;
    g_level_globals.campaign_insertions->valid = 0;
    g_level_globals.multiplayer_levels->valid = 0;
    if (g_level_globals.campaign_levels)
        data_dispose(g_level_globals.campaign_levels);
    if (g_level_globals.map_levels)
        data_dispose(g_level_globals.map_levels);
    if (g_level_globals.campaign_insertions)
        data_dispose(g_level_globals.campaign_insertions);
    if (g_level_globals.multiplayer_levels)
        data_dispose(g_level_globals.multiplayer_levels);
}

// @address 0x1e51e8 ?levels_dispose_from_old_map@@YAXXZ
void levels_dispose_from_old_map()
{
    content_catalogue_update_active_map_items(game_globals->options.game_mode == 3);
}

// @address 0x5bec98 ?levels_initialize_for_new_map@@YAXXZ
void levels_initialize_for_new_map(void)
{
}

// @address 0x1e5330 levels_try_and_get_insertion
extern "C" bool levels_try_and_get_insertion(long unique_id, levels_insertion_datum *result)
{
    bool found = false;
    if (g_level_globals.enabled && unique_id != -1) {
        internal_critical_section_enter(22);
        c_data_iterator<levels_insertion_datum> iterator;
        iterator.begin(g_level_globals.campaign_insertions);
        while (iterator.next()) {
            if (iterator.get_datum()->unique_id == unique_id) {
                memcpy(result, iterator.get_datum(), sizeof(levels_insertion_datum));
                found = true;
                break;
            }
        }
        internal_critical_section_leave(22);
    }
    return found;
}

// @address 0x1e53f0 levels_find_campaign_for_map_name
extern "C" long levels_find_campaign_for_map_name(const char *name)
{
    long campaign_id = -1;
    if (g_level_globals.enabled) {
        internal_critical_section_enter(22);
        c_data_iterator<levels_campaign_datum> iterator;
        iterator.begin(g_level_globals.campaign_levels);
        while (iterator.next() && campaign_id == -1) {
            levels_campaign_datum *campaign = iterator.get_datum();
            for (unsigned long index = 0; index < 64 && campaign->map_ids[index] != -1; index++) {
                levels_map_datum map_datum;
                if (levels_try_and_get_datum(g_level_globals.map_levels, campaign->map_ids[index], (s_datum *)&map_datum)
                    && strstr(static_cast<const char *>(map_datum.name), name) != 0) {
                    campaign_id = campaign->unique_id;
                    break;
                }
            }
        }
        levels_critical_section_leave(22);
    }
    return campaign_id;
}

// @address 0x1e5500 levels_get_campaign_map_index
extern "C" short levels_get_campaign_map_index(long campaign_id, long map_id)
{
    short result = -1;
    if (g_level_globals.enabled && map_id != -1) {
        internal_critical_section_enter(22);
        c_data_iterator<levels_campaign_datum> iterator;
        iterator.begin(g_level_globals.campaign_levels);
        while (iterator.next() && result == -1) {
            levels_campaign_datum *campaign = iterator.get_datum();
            if (campaign->unique_id == campaign_id) {
                for (long index = 0; (unsigned long)index < 64 && campaign->map_ids[index] != -1; index++) {
                    levels_map_datum map_datum;
                    if (levels_try_and_get_datum(g_level_globals.map_levels, campaign->map_ids[index], (s_datum *)&map_datum)
                        && map_datum.unique_id == map_id) {
                        result = (short)index;
                        break;
                    }
                }
            }
        }
        levels_critical_section_leave(22);
    }
    return result;
}

// @address 0x1e5620 levels_get_main_campaign_map_count
extern "C" long levels_get_main_campaign_map_count()
{
    long map_count = 0;
    if (g_level_globals.enabled) {
        internal_critical_section_enter(22);
        c_data_iterator<levels_campaign_datum> iterator;
        iterator.begin(g_level_globals.campaign_levels);
        while (iterator.next()) {
            if (iterator.get_datum()->unique_id == 1) {
                levels_campaign_datum *campaign = iterator.get_datum();
                for (long index = 0; index < 64; index++) {
                    if (campaign->map_ids[index] != 0 && campaign->map_ids[index] != -1) {
                        map_count++;
                    }
                }
                break;
            }
        }
        levels_critical_section_leave(22);
    }
    return map_count;
}


// @address 0x1e5768 levels_get_default_multiplayer_map_id
extern "C" long levels_get_default_multiplayer_map_id(void)
{
    long smallest_map_id = -1;
    if (g_level_globals.enabled)
    {
        internal_critical_section_enter(22);
        c_data_iterator<s_datum> level_iterator;
        level_iterator.begin(g_level_globals.multiplayer_levels);
        while (level_iterator.next())
        {
            long candidate_map_id = level_iterator.get_datum()->unique_id;
            if (levels_is_map_type_allowed(candidate_map_id) &&
                (smallest_map_id == -1 || smallest_map_id > candidate_map_id))
                smallest_map_id = candidate_map_id;
        }
        internal_critical_section_leave(22);
    }
    return smallest_map_id;
}


// @address 0x1e5828 levels_is_map_type_allowed
extern "C" bool levels_is_map_type_allowed(long type)
{
    bool result = true;
    if (get_alpha_is_external_beta())
        result = type == 310 || type == 340 || type == 360;
    else if (get_alpha_is_internal_beta())
        result = type == 310 || type == 340 || type == 360 || type == 350 || type == 30;
    return result;
}

// @address 0x1e58e8 levels_get_multiplayer_map_ids
extern "C" void levels_get_multiplayer_map_ids(long *unique_ids, long *count)
{
    long written = 0;
    if (g_level_globals.enabled) {
        internal_critical_section_enter(22);
        long capacity = *count;
        memset(unique_ids, -1, capacity * sizeof(long));
        c_data_iterator<s_datum> iterator;
        iterator.begin(g_level_globals.multiplayer_levels);
        while (iterator.next() && written < capacity) {
            long unique_id = iterator.get_datum()->unique_id;
            bool different = true;
            for (long index = 0; index < written && different; index++) {
                different = (unique_ids[index] - unique_id) != 0;
            }
            if (different) {
                unique_ids[written] = unique_id;
                written++;
            }
        }
        levels_critical_section_leave(22);
    }
    *count = written;
}

// @address 0x1e5a18 levels_get_map_scenario_path
extern "C" char *levels_get_map_scenario_path(long campaign_id, long map_id, char *name, long name_size)
{
    internal_critical_section_enter(22);
    name[0] = 0;
    if (map_id == 0x10231971) {
        strncpy(name, g_level_globals.default_map.name, name_size);
        name[name_size - 1] = 0;
    } else {
        c_data_iterator<s_datum> iterator;
        iterator.begin(campaign_id == -1 ? g_level_globals.multiplayer_levels : g_level_globals.map_levels);
        while (iterator.next()) {
            if (iterator.get_datum()->unique_id == map_id) {
                levels_map_datum *map = iterator.get_datum();
                strncpy(name, map->name, name_size);
                name[name_size - 1] = 0;
                break;
            }
        }
    }
    internal_critical_section_leave(22);
    return name;
}

// @address 0x1e7238 levels_try_and_get_datum
extern "C" bool levels_try_and_get_datum(s_data_array *data, int unique_id, s_datum *result)
{
    bool found = false;
    if (g_level_globals.enabled && unique_id != -1) {
        internal_critical_section_enter(22);
        c_data_iterator<s_datum> iterator;
        iterator.begin(data);
        while (iterator.next()) {
            if (iterator.get_datum()->unique_id == unique_id) {
                memcpy(result, iterator.get_datum(), sizeof(s_datum));
                found = true;
                break;
            }
        }
        internal_critical_section_leave(22);
    }
    return found;
}


e_map_id levels_get_campaign_next_map_id(e_campaign_id campaign_id, e_map_id map_id)
{
    long next_map_id = -1;
    if (g_level_globals.enabled && campaign_id != -1 && map_id != -1) {
        internal_critical_section_enter(22);
        c_data_iterator<levels_campaign_datum> iterator;
        iterator.begin(g_level_globals.campaign_levels);
        while (iterator.next()) {
            if (iterator.get_datum()->unique_id == campaign_id) {
                break;
            }
        }
        if (iterator.get_datum()) {
            levels_campaign_datum *campaign = iterator.get_datum();
            for (unsigned long index = 0; index < 64; index++) {
                if (campaign->map_ids[index] == map_id) {
                    next_map_id = index < 63 ? campaign->map_ids[index + 1] : -1;
                    break;
                }
            }
        }
        internal_critical_section_leave(22);
    }
    return (e_map_id)next_map_id;
}

static __forceinline bool levels_index_in_range(long index, long base, long count)
{
    return index - base >= 0 && index - base < count;
}

// @address 0x21bed8 levels_is_index_in_range
extern "C" unsigned char levels_is_index_in_range(const s_cache_file_resource_element_set *set, long index)
{
    bool result = false;
    if (levels_index_in_range(index, 0, set->first->count)) {
        result = true;
    } else {
        long base = set->first->count;
        if (levels_index_in_range(index, base, (*set->second)->actual_count)) {
            result = index >= base && index < (*set->second)->actual_count + base;
        } else {
            long size = (*set->second)->maximum_count;
            long start = size + base;
            if (levels_index_in_range(index, start, (*set->third)->actual_count)) {
                result = index >= start && index < (*set->third)->actual_count + size + base;
            }
        }
    }
    return result;
}




// @address 0x4d8ca8 levels_get_campaign_map_display_name
extern "C" bool levels_get_campaign_map_display_name(long unused, wchar_t *name, long name_size)
{
    bool result = false;
    game_globals_storage *globals = tls_game_globals();
    name[0] = 0;
    if (globals->options.game_mode == 1) {
        levels_map_datum datum;
        long map_id = globals->options.map_id;
        if (levels_try_and_get_datum(g_level_globals.map_levels, map_id, (s_datum *)&datum)) {
            ustrnzcpy(name, datum.display_name, name_size);
        }
        result = true;
    }
    return result;
}

static __forceinline unsigned char levels_campaign_map_list_busy()
{
    unsigned char busy = g_level_globals.enumeration_task_id != -1 && g_level_globals.enumeration_complete.peek() == 0;
    return busy;
}

// @address 0x1e5208 levels_get_campaign_map_ids
extern "C" void levels_get_campaign_map_ids(long campaign_id, long *map_ids, long *count)
{
    long map_count = 0;
    if (g_level_globals.enabled) {
        if (!levels_campaign_map_list_busy() && campaign_id != -1) {
            c_data_iterator<levels_campaign_datum> iterator;
            iterator.begin(g_level_globals.campaign_levels);
            levels_campaign_datum *campaign = NULL;
            while (iterator.next()) {
                if (iterator.get_datum()->unique_id == campaign_id) {
                    campaign = iterator.get_datum();
                    break;
                }
            }
            if (campaign) {
                long max_count = *count;
                if (max_count > 0) {
                    memset(map_ids, -1, max_count * sizeof(*map_ids));
                    while (map_count < max_count) {
                        long map_id = campaign->map_ids[map_count];
                        if (map_id == 0 || map_id == -1)
                            break;
                        map_ids[map_count] = map_id;
                        map_count++;
                    }
                }
            }
        }
    }
    *count = map_count;
}

// @address 0xa6cf8 levels_update
extern "C" void levels_update(void)
{
    long handle;
    s_async_task task;
    unsigned char busy;
    unsigned char blocked;
    if (g_level_globals.enumerate_info_files == 0 || main_globals.startup_masking_done == 0)
    {
        handle = g_level_globals.enumeration_task_id;
    }
    else
    {
        handle = g_level_globals.enumeration_task_id;
        busy = handle != -1 && g_level_globals.enumeration_complete.peek() == 0;
        if (!busy)
        {
            blocked = bink_globals.active != 0 && bink_globals.enabled != 0;
            if (!blocked)
            {
                task.enumerate_levels.state = 0;
                task.enumerate_levels.find_file_data = &g_level_globals.enumeration_file_data;
                handle = async_task_add_ex(_async_priority_blocking_generic, &task, _async_category_saved_games,
                    reinterpret_cast<e_async_completion (*)(s_async_task *)>(levels_enumerate_info_files),
                    &g_level_globals.enumeration_complete, true);
                g_level_globals.enumeration_task_id = handle;
                if (handle == -1)
                {
                    levels_update_enumeration();
                    return;
                }
                g_level_globals.enumerate_info_files = 0;
            }
        }
    }
    if (handle != -1 && g_level_globals.enumeration_complete.peek() == 1)
        g_level_globals.enumeration_task_id = -1;
    levels_update_enumeration();
}

static __forceinline void levels_byte_swap_wide_string(wchar_t *string, long count)
{
    for (long i = count - 1; i >= 0; i--)
    {
        wchar_t character = string[i];
        string[i] = (wchar_t)((character << 8) | (character >> 8));
    }
}

// @address 0x1e66a0 levels_add_campaign
extern "C" void levels_add_campaign(const s_levels_campaign_source *source, bool byte_swap, const char *map_directory, bool downloadable)
{
    internal_critical_section_enter(22);
    long campaign_index = datum_new(g_level_globals.campaign_levels);
    if (campaign_index != -1)
    {
        long language = get_current_language();
        levels_campaign_datum *campaign = datum_get<levels_campaign_datum>(g_level_globals.campaign_levels, campaign_index);
        unsigned long flags = source->flags;
        if (byte_swap)
            flags = byte_swap_long(flags);
        campaign->flags = 0;
        operation_flag_set(campaign->flags, 0, downloadable);
        operation_flag_set(campaign->flags, 1, ((flags >> 1) & 1) != 0);
        if (byte_swap)
        {
            campaign->unique_id = byte_swap_long(source->campaign_id);
            ustrnzcpy_inline(campaign->name, source->names[language], 64);
            levels_byte_swap_wide_string(campaign->name, 64);
            ustrnzcpy_inline(campaign->description, source->descriptions[language], 128);
            levels_byte_swap_wide_string(campaign->description, 128);
            for (long i = 0; i < 64; i++)
                campaign->map_ids[i] = (source->map_ids[i] >> 24) | ((source->map_ids[i] >> 8) & 0xff00) |
                    ((source->map_ids[i] << 8) & 0xff0000) | (source->map_ids[i] << 24);
        }
        else
        {
            campaign->unique_id = source->campaign_id;
            ustrnzcpy_inline(campaign->name, source->names[language], 64);
            ustrnzcpy_inline(campaign->description, source->descriptions[language], 128);
            memset(campaign->map_ids, -1, sizeof(campaign->map_ids));
            for (long i = 0; i < 64; i++)
                campaign->map_ids[i] = source->map_ids[i];
        }
    }
    levels_critical_section_leave(22);
}

static __forceinline unsigned short levels_byte_swap_word(unsigned short value)
{
    return (unsigned short)((value << 8) | (value >> 8));
}

static __forceinline wchar_t *levels_map_ustrnzcpy(wchar_t *dest, const wchar_t *source, long size)
{
    for (long index = 0; index <= size - 1; index++)
    {
        wchar_t c = *source;
        dest[index] = c;
        if (c == 0)
            break;
        source++;
    }
    dest[size - 1] = 0;
    return dest;
}

static __forceinline const char *levels_get_default_map_directory()
{
    g_maps_directory = "d:\\maps\\";
    return "d:\\maps\\";
}

// @address 0x1e6b28 levels_add_map
extern "C" void levels_add_map(const s_levels_map_source *source, bool byte_swap, const char *map_directory, bool downloadable)
{
    internal_critical_section_enter(22);
    long language = get_current_language();
    unsigned long flags = source->flags;
    if (byte_swap)
        flags = byte_swap_long(flags);

    levels_map_datum *map = NULL;
    if (flags & (1 << 4))
    {
        map = &g_level_globals.default_map;
    }
    else if ((flags & (1 << 5)) || (flags & (1 << 6)))
    {
        s_data_array *levels_data = (flags & (1 << 5)) ? g_level_globals.map_levels : g_level_globals.multiplayer_levels;
        long map_index = datum_new(levels_data);
        if (map_index != -1)
            map = datum_get<levels_map_datum>(levels_data, map_index);
    }

    if (map != NULL)
    {
        map->flags = 0;
        operation_flag_set(map->flags, 0, downloadable);
        operation_flag_set(map->flags, 1, ((flags >> 1) & 1) != 0);
        operation_flag_set(map->flags, 2, ((flags >> 2) & 1) != 0);
        operation_flag_set(map->flags, 3, ((flags >> 3) & 1) != 0);
        if (flags & (1 << 4))
            map->flags |= (1 << 4);
        else if (flags & (1 << 5))
            map->flags |= (1 << 5);
        else if (flags & (1 << 6))
            map->flags |= (1 << 6);

        if (flags & (1 << 7))
            map->flags |= (1 << 7);
        else if (flags & (1 << 8))
            map->flags |= (1 << 8);
        else if (flags & (1 << 9))
            map->flags |= (1 << 9);

        if (byte_swap)
        {
            map->unique_id = byte_swap_long(source->map_id);
            levels_map_ustrnzcpy(map->display_name, source->names[language], 32);
            levels_byte_swap_wide_string(map->display_name, 32);
            levels_map_ustrnzcpy(map->description, source->descriptions[language], 128);
            levels_byte_swap_wide_string(map->description, 128);
            map->presence_context_id = byte_swap_long(source->presence_context_id);
            map->sort_order = byte_swap_long(source->sort_order);
            map->mp_minimum_desired_players = source->mp_minimum_desired_players;
            map->mp_maximum_desired_players = source->mp_maximum_desired_players;
            for (long i = 0; i < 11; i++)
                map->maximum_teams[i] = source->maximum_teams[i];
        }
        else
        {
            map->unique_id = source->map_id;
            levels_map_ustrnzcpy(map->display_name, source->names[language], 32);
            levels_map_ustrnzcpy(map->description, source->descriptions[language], 128);
            map->presence_context_id = source->presence_context_id;
            map->sort_order = source->sort_order;
            map->mp_minimum_desired_players = source->mp_minimum_desired_players;
            map->mp_maximum_desired_players = source->mp_maximum_desired_players;
            for (long i = 0; i < 11; i++)
                map->maximum_teams[i] = source->maximum_teams[i];
        }

        if (bit_test(map->flags, 5))
        {
            long insertion_datum_index = datum_new(g_level_globals.campaign_insertions);
            if (insertion_datum_index != -1)
            {
                levels_insertion_datum *insertion = datum_get<levels_insertion_datum>(
                    g_level_globals.campaign_insertions, insertion_datum_index);
                short insertion_count = 0;
                for (short insertion_index = 0; insertion_index < 4; insertion_index++)
                {
                    const s_levels_insertion_source *entry = &source->insertions[insertion_index];
                    if (entry->visible)
                    {
                        if (byte_swap)
                        {
                            wchar_t *name = insertion->names[insertion_index];
                            wchar_t *description = insertion->descriptions[insertion_index];
                            levels_map_ustrnzcpy(name, entry->names[language], 32);
                            levels_byte_swap_wide_string(name, 32);
                            levels_map_ustrnzcpy(description, entry->descriptions[language], 128);
                            levels_byte_swap_wide_string(description, 128);
                            insertion->zone_sets[insertion_index] = (unsigned char)levels_byte_swap_word(entry->zone_set_index);
                        }
                        else
                        {
                            levels_map_ustrnzcpy(insertion->names[insertion_index], entry->names[language], 32);
                            levels_map_ustrnzcpy(insertion->descriptions[insertion_index], entry->descriptions[language], 128);
                            insertion->zone_sets[insertion_index] = (unsigned char)entry->zone_set_index;
                        }
                        insertion_count++;
                    }
                }
                insertion->unique_id = map->unique_id;
                insertion->insertion_count = insertion_count;
            }
        }

        if (downloadable)
        {
            csnzprintf(map->name, 256, "%s\\%s", map_directory, source->scenario_file);
            csnzprintf(map->image_file_base, 256, "%s\\%s", map_directory, source->image_file_base);
        }
        else
        {
            csnzprintf(map->name, 256, "%s%s", levels_get_default_map_directory(), source->scenario_file);
            csnzprintf(map->image_file_base, 256, "%simages\\%s", levels_get_default_map_directory(), source->image_file_base);
        }
    }

    levels_critical_section_leave(22);
}
