// blamlibxbox/cache_files_windows.obj: from the records 0x81ac8 0x81c70 0x81dc8 0x81e90 0x81f88 0x84310 0x1b3790 0x1f3018 0x1f31c0 0x1f3228 0x1f33e8 0x1f3500 0x1f36d8 0x1f3828 0x1f38b8 0x1f39b8 0x1f3a00 0x1f3b00 0x1f3bf8 0x1f3df8 0x1f43d0 0x1f4460 0x1f44c8 0x1f4530 0x1f4698 0x1f46e8 0x1f4778 0x1f5280 0x1f5390 0x1f54c8 0x1f5750 0x1f5828 0x1f5970 0x1f5aa0 0x1f5af0 0x1f5c40 0x1f5ca8 0x1f5d50 0x70b8c0 0x1f4848
#include "cache/cache_files_windows_async.h"
#include "cache/cache_file_byte_swap.h"
#include "memory/basic_buffer.h"
#include "memory/tls_globals.h"
#include "main/thread_globals_lock.h"
#include "tag_files/file_open_disposition_types.h"
#include "tag_files/files_windows_types.h"
#include <xtl.h>
#include "multithreading/thread_tls.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "memory/static_arrays.h"
#include "multithreading/synchronization.h"
#include "cache/cache_files.h"
#include "cache/cache_file_header.h"
#include "cache/cache_files_windows.h"
#include "tag_files/files_windows_globals.h"
#include "saved_games/game_state_procs.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "saved_games/game_state_xenon.h"
#include "cseries/cseries.h"
#include "cseries/async.h"
#include "cseries/async_queue.h"
#include "cseries/file_handle_types.h"
#include "cache/map_file_index_types.h"
#include "cache/cache_file_progress_types.h"
#include "cseries/create_file_task_types.h"
#include "multithreading/threads.h"
#include "main/game_preferences_types.h"
#include "main/thread_globals_lock_inlines.h"
#include "main/game_preferences_inlines.h"
#include "cseries/async_globals.h"
#include "multithreading/synchronization_types.h"
#include "cseries/async_helpers.h"
#include "cseries/language_types.h"
#include "cseries/language.h"
#include "shell/utility_drive.h"
#include "math/real_constant_pool_access.h"
#include "saved_games/game_state_procs_externs.h"
#include "interface/gui_custom_bitmap_storage_hash_types.h"
#include "multithreading/synchronized_value.h"
#include "saved_games/game_state_xenon_types.h"
#include "cache/cache_files_pending_load_types.h"
#include "cache/cache_files_windows_paths.h"
#include "memory/lruv_cache_internal.h"
#include "cache/cache_files_windows_dependencies.h"
#include "cseries/async_block.h"
#include "cache/indirect_cache_file_reader_types.h"
#include "cache/cache_file_bulk_reader.h"
#include "cseries/async_file_requests.h"
#include "cseries/async_io_error.h"
#include "saved_games/game_state_procs_types.h"
#include "networking/logic/network_storage_cache_services.h"
#include "main/loading.h"

// @flags /TP /O2 /Oc
// @address 0x7db1d4 g_async_file_io_error
// @address 0x8a1ead g_utility_drive_mounted
// @address 0x84310 ?cache_file_block_until_not_busy@@YAXXZ
// @address 0x1f5aa0 ?cached_map_file_get_size@@YAKW4e_map_file_index@@@Z

enum e_map_file_index;


// 0x1b3790
struct tag_header;
struct s_cache_files_windows_load_status
{
    // Runtime manager prefix: nine interface vptrs, header, gestalt, and zone state.
    const void *interface_vtables[9];
    tag_header *header;
    long file_index;
    struct {
        unsigned char flags;
        unsigned long mask4;
        unsigned long mask8;
        unsigned long maskc;
        unsigned long mask10;
        unsigned long mask14;
        unsigned long mask18;
        unsigned long mask1c;
    } active_state;
    long zone_state_index;
    long zone_name_index;
    long map_name_index;
    c_static_string<256> map_name;
    long progress;
    c_static_string<256> displayed_name;
};
typedef char cache_load_status_file_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_load_status, file_index) == 0x28))) ? 1 : -1];
typedef char cache_load_status_name_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_load_status, map_name) == 0x58))) ? 1 : -1];
typedef char cache_load_status_progress_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_load_status, progress) == 0x158))) ? 1 : -1];
typedef char cache_load_status_display_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_load_status, displayed_name) == 0x15c))) ? 1 : -1];


// 0x1f39b8


typedef char cache_preferences_language_offset[
    (sizeof(void *) != 4 || ((offsetof(s_global_preferences, primary_file) +
        offsetof(s_game_preferences_file_image, data) + offsetof(s_global_preferences_data, last_language) == 0x24))) ? 1 : -1];
typedef char cache_preferences_build_offset[
    (sizeof(void *) != 4 || ((offsetof(s_global_preferences, primary_file) +
        offsetof(s_game_preferences_file_image, data) + offsetof(s_global_preferences_data, map_count) == 0x34))) ? 1 : -1];


struct s_async_close_file_task : s_async_task
{
    __forceinline s_async_close_file_task() {}
};

enum e_cache_file_level
{
    _cache_file_level_mainmenu = 0,
    _cache_file_level_multiplayer = 1,
    _cache_file_level_campaign = 2
};

struct s_cache_files_windows_copy_task : s_async_task
{
    __forceinline s_cache_files_windows_copy_task() {}
};

struct s_cache_files_windows_write_header_task : s_async_task
{
    __forceinline s_cache_files_windows_write_header_task() {}
};

struct s_cache_files_windows_create_task : s_create_file_task
{
    __forceinline s_cache_files_windows_create_task() {}
};

struct s_cache_files_windows_copy_request : s_cache_files_windows_cached_map
{
    __forceinline s_cache_files_windows_copy_request() {}
};

static __forceinline s_file_handle cache_files_windows_handle_from_slot(void *handle)
{
    s_file_handle file;
    file.handle = handle;
    return file;
}

static __forceinline bool async_category_in_queue(long category)
{
bool result=false;
s_mutex_handle *mutex=&g_synch_globals.mutexes[1];
internal_mutex_take(1);
for(s_async_queue_element *task=g_async_globals.work_list;task;task=task->next) {
if(task->category==category) { result=true; break; }
}
_InterlockedDecrement(&mutex->lock_count.value);
ReleaseMutex(mutex->handle);
return result;
}

void cache_file_block_until_not_busy()
{
long categories[]={2,3,4,5,6};
for(unsigned long i=0;i<5;i++)
while(async_category_in_queue(categories[i])) SwitchToThread();
}


// @address 0x1b3790 cache_files_windows_update_load_status
extern "C" void cache_files_windows_update_load_status(s_cache_files_windows_load_status *status, short progress, const char *path)
{
    if (status->file_index != -1) {
        c_static_string<256> name;
        name.m_string[0] = 0;
        cache_files_windows_get_canonical_map_path(path, &name);
        if (strncmp(status->map_name.m_string, name.m_string, 256) != 0 || status->progress != progress) {
            status->progress = progress;
            status->displayed_name = name;
        }
    }
}


// 0x1f31c0

// @address 0x1f31c0 ?finish@s_cache_files_windows_stream_state@@UAA_NPAV?$c_basic_buffer@X@@@Z
bool s_cache_files_windows_stream_state::finish(c_basic_buffer<void> *result)
{
  unsigned char done = cache_files_windows_test_wait_request(this);
  while (done == 0) {
    done = cache_files_windows_test_wait_request(this);
  }
  result->m_address = 0;
  result->m_size = 0;
  return valid;
}

// @address 0x1f3228 cache_files_windows_test_wait_request
extern "C" unsigned char cache_files_windows_test_wait_request(s_cache_files_windows_stream_state *request)
{
  if (request->io_pending.peek() != 0) {
    if (request->completion.result.done.peek() == 0)
      SleepEx(0,1);
    if (request->completion.result.done.peek() != 0) {
      request->io_pending.set(0);
      if ((unsigned long)request->completion.result.result_a == 0) {
        long size=request->completion.result.result_b;
        request->valid &= ((size - request->completion.size) == 0);
        __lwsync();
        _InterlockedExchangeAdd(&request->bytes_done.value,size);
      }
    }
  }
  return request->io_pending.peek() == 0;
}

// 0x1f36d8

static __forceinline bool cache_files_windows_valid_index(long index)
{
    return index >= 0 && index < 15;
}

// @address 0x1f36d8 cached_map_file_exists
extern "C" bool cached_map_file_exists(const char *path)
{
    bool result = false;
    long index = game_state_xenon_find_file_by_path(path);
    if (index != -1 && cache_files_windows_valid_index(index))
    {
        result = true;
    }
    return result;
}


// @address 0x1f3828 cached_map_file_read_header
extern "C" long cached_map_file_read_header(long index, long offset, long size, void *buffer,
    c_synchronized_long *bytes_read, c_synchronized_long *done)
{
    long file_offset = xenon_game_state_globals.files[xenon_game_state_globals.active_file_index].header.map_file_offsets[index];
    void *file_handle = xenon_game_state_globals.files[xenon_game_state_globals.active_file_index].file_handle;
    long result = -1;
    if (file_offset != -1) {
        result = async_read_position(file_handle, buffer, size, file_offset + offset, 2, 9, bytes_read, done);
    }
    return result;
}



// @address 0x1f38b8 ?cached_map_files_delete@@YAXW4e_map_file_index@@0@Z
void cached_map_files_delete(enum e_map_file_index first_index, enum e_map_file_index last_index)
{
bool deleted=false;
int index=((int)first_index);
if(index<((int)last_index)) {
const char *cache0="cache0:\\";
const char *cache1="cache1:\\";
const char *format="%scache%03d%s";
const char *extension=".map";
char path[256];
do {
csnzprintf(path,256,format,cache0,index,extension);
deleted=DeleteFileA(path)!=0||deleted;
csnzprintf(path,256,format,cache1,index,extension);
deleted=DeleteFileA(path)!=0||deleted;
++index;
} while(index<((int)last_index));
}
SetLastError(0);
if(deleted&&g_utility_drive_mounted!=0) {
XFlushUtilityDrive();
}
return;
}

// @address 0x8a1eaf g_cache_files_globals


#if !defined(_XBOX) || defined(__clang__)
unsigned char g_cache_files_enabled;
bool g_cache_files_override;
#endif

// @address 0x1f39b8 cache_files_windows_is_caching_enabled
extern "C" bool cache_files_windows_is_caching_enabled()
{
    return CACHE_FILES_OVERRIDE || (CACHE_FILES_ENABLED && !g_utility_drive_mounted);
}


static __forceinline unsigned char cache_files_requests_blocked(void)
{
    return !(!((g_thread_system_initialized.value == 0)) && ((get_current_thread_index() != 3)));
}

static __forceinline int cache_files_thread_language_get(void)
{
    int language;
    unsigned char blocked;
    int result;
    result = -1;
    blocked = cache_files_requests_blocked();
    if (blocked == 0)
    {
        c_thread_globals_lock lock;
        language = global_preferences_thread_storage()->primary_file.data.last_language;
        result = language;
    }
    return result;
}

static __forceinline void cache_files_thread_language_set(long language)
{
    s_global_preferences *prefs;
    unsigned char blocked;
    blocked = cache_files_requests_blocked();
    if (blocked == 0)
    {
        c_thread_globals_lock lock;
        prefs = global_preferences_thread_storage();
        prefs->primary_file.data.last_language = language;
        prefs->dirty = 1;
    }
    return;
}

// @address 0x1f3a00 cache_files_windows_update_language
void cache_files_windows_update_language(void)
{
    int language;
    if (g_current_language == -1) {
        g_current_language = ::get_e_language_from_xbox_language(XTLGetLanguage());
    }
    language = g_current_language;
    if (cache_files_thread_language_get() != language) {
        ::cached_map_files_delete(_map_file_index_mainmenu, k_number_of_map_files);
        cache_files_thread_language_set(language);
    }
}

// @address 0x1f3b00 cache_files_windows_update_build_number
void cache_files_windows_update_build_number(void)
{
    int build_number;
    unsigned char blocked = cache_files_requests_blocked();
    if ((((unsigned char)blocked) == 0)) {
        {
            c_thread_globals_lock lock;
            build_number = global_preferences_thread_storage()->primary_file.data.map_count;
        }
        if (build_number == 11855)
            return;
    }
    ::cached_map_files_delete(_map_file_index_mainmenu, k_number_of_map_files);
    blocked = cache_files_requests_blocked();
    if ((((unsigned char)blocked) == 0)) {
        c_thread_globals_lock lock;
        s_global_preferences *prefs = global_preferences_thread_storage();
        prefs->primary_file.data.map_count = 11855;
        prefs->dirty = 1;
    }
}


extern "C" {
s_cache_files_windows_pending_load g_cache_files_windows_pending_load;
}

// @address 0x1f43d0 cache_files_windows_cancel_map_request
extern "C" void cache_files_windows_cancel_map_request(const char *path)
{
    long status = cache_files_windows_get_map_status(path);
    if (status == 2 || status == 1)
    {
        if (cache_files_windows_test_map_paths_equal(path, g_cache_files_windows_pending_load.m_cached_maps[0].name.m_string))
        {
            cache_files_windows_cancel_active_copy();
        }
        else if (cache_files_windows_test_map_paths_equal(path, g_cache_files_windows_pending_load.m_cached_maps[1].name.m_string))
        {
            g_cache_files_windows_pending_load.m_cached_maps[1].level = 0;
        }
    }
}

// @address 0x1f4460 cache_files_windows_get_active_map_level
extern "C" long cache_files_windows_get_active_map_level(const char *path)
{
    long result = -1;
    c_static_string<256> path_name;
    path_name.m_string[0] = 0;
    cache_files_windows_get_canonical_map_path(path, &path_name);
    c_static_string<256> current_name;
    current_name.m_string[0] = 0;
    cache_files_windows_get_canonical_map_path(g_cache_files_windows_pending_load.m_current_map_name.m_string, &current_name);
    if (::csstricmp((char const *)(path_name.m_string), (char const *)(current_name.m_string)) == 0)
        result = g_cache_files_windows_pending_load.m_cached_maps[0].level;
    return result;
}

// @address 0x1f44c8 cache_files_windows_get_pending_map_level
extern "C" long cache_files_windows_get_pending_map_level(const char *path)
{
    long result = -1;
    c_static_string<256> path_name;
    path_name.m_string[0] = 0;
    cache_files_windows_get_canonical_map_path(path, &path_name);
    c_static_string<256> cached_name;
    cached_name.m_string[0] = 0;
    cache_files_windows_get_canonical_map_path(g_cache_files_windows_pending_load.m_cached_maps[1].name.m_string, &cached_name);
    if (::csstricmp((char const *)(path_name.m_string), (char const *)(cached_name.m_string)) == 0)
        result = g_cache_files_windows_pending_load.m_cached_maps[1].level;
    return result;
}

// @address 0x1f4530 ?set_mode@c_cache_files_pending_load_listener@@UAAXJPAUs_game_state_buffer_description@@@Z
void c_cache_files_pending_load_listener::set_mode(long mode, s_game_state_buffer_description *description)
{
    if (mode == 1) {
        description->type = 1;
        description->size = 0x100000;
        return;
    }
    bool known = mode == 4 || mode == 5 || mode == 6;
    if (known) {
        description->type = 1;
        description->size = 0x40000;
        return;
    }
    description->type = 0;
    description->size = 0;
}

// @address 0x75de68 g_cache_files_pending_load_listener
// PC build only: on Xbox update() is the shared empty body at 0x5bec98 and the image holds
// the object's vtable word (0x3d560).
#if !defined(_XBOX) || defined(__clang__)
void c_cache_files_pending_load_listener::update(void *)
{
}

c_cache_files_pending_load_listener g_cache_files_pending_load_listener;
#endif

// @address 0x1f4698 cache_files_windows_call_pending_load_procs
extern "C" void cache_files_windows_call_pending_load_procs()
{
    if (g_cache_files_windows_pending_load.copy_buffer != 0) {
        ::game_state_call_after_load_procs(0, g_cache_files_windows_pending_load.copy_buffer);
        g_cache_files_windows_pending_load.copy_buffer = 0;
        g_cache_files_windows_pending_load.copy_buffer_size = 0;
    }
}

// @address 0x1f46e8 ?find_failed_map@@YAPAUs_failed_map@@PBD@Z

s_failed_map *find_failed_map(const char *path)
{
s_failed_map *result=0;
c_static_string<256> canonical_path;
cache_files_windows_get_canonical_map_path(path,&canonical_path);
for(unsigned long i=0;i<8;i++) {
s_failed_map *failed_map=&xenon_game_state_globals.failed_maps[i];
if(strcmp(canonical_path.m_string,failed_map->name.m_string)==0) { result=failed_map; break; }
}
return result;
}


// @address 0x1f4778 ?cache_file_map_has_failed@@YAXPBDJ@Z
void cache_file_map_has_failed(const char *path, long load_action)
{
    c_static_string<256> canonical_path;
    canonical_path.m_string[0] = 0;
    cache_files_windows_get_canonical_map_path(path, &canonical_path);

    s_failed_map *failed_map = find_failed_map(canonical_path.m_string);
    if (!failed_map)
    {
        unsigned long offset = 0;
        s_failed_map *candidate = xenon_game_state_globals.failed_maps;
        do
        {
            if (candidate->name.m_string[0] == 0)
            {
                failed_map = candidate;
                break;
            }
            if (!failed_map || candidate->timestamp < failed_map->timestamp)
            {
                failed_map = candidate;
            }
            offset += sizeof(s_failed_map);
            candidate++;
        } while (offset < sizeof(xenon_game_state_globals.failed_maps));

        if (!failed_map)
            return;

        failed_map->name = canonical_path;
        failed_map->failure_count = 0;
    }

    failed_map->timestamp = GetTickCount();
    failed_map->failure_count++;
    find_failed_map(path);
}

// @address 0x75de6c g_cache_files_windows_stream_state
// PC build only: on Xbox open() is the shared ICF body at 0x42d380 (li r3,1; blr: returns true) and game_state_procs
// constructs the object.
#if !defined(_XBOX) || defined(__clang__)
#ifdef HALO3_64BIT
bool s_cache_files_windows_stream_state::open(c_basic_buffer<void> source_position)
#else
bool s_cache_files_windows_stream_state::open(unsigned __int64 source_position)
#endif
{
    return true;
}

s_cache_files_windows_stream_state g_cache_files_windows_stream_state;
#endif


// @address 0x1f5280 cache_files_windows_get_active_copy_progress
extern "C" bool cache_files_windows_get_active_copy_progress(float *progress_out)
{
    bool result = false;
    float progress = 0.0f;
    if (g_cache_files_windows_pending_load.countdown != 0) {
        if (g_cache_files_windows_pending_load.countdown < 15) {
            progress = 0.0f;
        } else {
            if (g_cache_files_windows_stream_state.active) {
                unsigned long done = g_cache_files_windows_stream_state.bytes_done.value;
                __lwsync();

                progress = (float)done / (float)g_cache_files_windows_pending_load.bytes_total;
            } else if (g_cache_files_windows_pending_load.countdown > 12) {
                progress = 1.0f;
            } else if (g_cache_files_windows_pending_load.bytes_total != 0) {
                progress = (float)g_cache_files_windows_pending_load.bytes_done / (float)g_cache_files_windows_pending_load.bytes_total;
            }
        }
        result = true;
    } else {
        if (g_cache_files_windows_pending_load.m_cached_maps[1].name.m_string[0] != 0)
            result = true;
        progress = 0.0f;
    }
    progress = MIN(MAX(progress, 0.0f), 1.0f);
    if (progress_out)
        *progress_out = progress;
    return result;
}

// 0x1f5390
/* __real@3f800000 (a literal pool owned by another object): 1.0f at [0] */

// @address 0x1f5390 cache_files_windows_get_map_copy_progress
extern "C" float cache_files_windows_get_map_copy_progress(const char *path)
{
    float result = real_constant_pool_zero();
    float progress = real_constant_pool_zero();
    long status = cache_files_windows_get_map_status(path);
    if (status == 3) {
        result = real_constant_pool_one();
    } else if (status == 2 && cache_files_windows_test_map_paths_equal(g_cache_files_windows_pending_load.m_current_map_name.m_string, path)) {
        cache_files_windows_get_active_copy_progress(&progress);
        result = progress;
    }
    return result;
}

// @address 0x1f54c8 cache_files_windows_cancel_active_copy
void cache_files_windows_cancel_active_copy(void)
{
    long countdown = g_cache_files_windows_pending_load.countdown;
    if (countdown == 0)
        return;
    unsigned char at_limit = (countdown >= 20);
    if (at_limit != 0)
        return;
    __lwsync();
    _InterlockedExchange(&g_cache_files_windows_pending_load.armed.value, 1);
    g_cache_files_windows_pending_load.phase = _cache_file_copy_cancelled;
    g_cache_files_windows_pending_load.countdown = _cache_file_copy_cancel;
}


static __forceinline void cache_files_windows_current_file_time(s_file_last_modification_date *date)
{
FILETIME time;
GetSystemTimeAsFileTime(&time);
memcpy(date, &time, sizeof(*date));
}

static __forceinline s_xenon_game_state_file *cache_files_windows_map_file_get(long index)
{
s_xenon_game_state_file *result=&xenon_game_state_globals.files[index];
return result;
}


// @address 0x1f5750 new_slot_is_better_fit
extern "C" bool new_slot_is_better_fit(long best_index,long new_index)
{
bool result=false;
s_file_last_modification_date current_date;
cache_files_windows_current_file_time(&current_date);
if(best_index==-1) result=true;
else if(new_index==-1) result=false;
else {
s_xenon_game_state_file *best=cache_files_windows_map_file_get(best_index);
s_xenon_game_state_file *next=cache_files_windows_map_file_get(new_index);
if(CompareFileTime((const FILETIME *)&best->header.cached_map_date, (const FILETIME *)&current_date)>0) result=false;
else if(CompareFileTime((const FILETIME *)&next->header.cached_map_date, (const FILETIME *)&current_date)>0) result=true;
else if ((unsigned long)::cached_map_file_get_size((e_map_file_index)best_index) <
    (unsigned long)::cached_map_file_get_size((e_map_file_index)new_index)) result = true;
else if(CompareFileTime((const FILETIME *)&next->header.cached_map_date, (const FILETIME *)&best->header.cached_map_date)<0) result=true;
}
return result;
}

static __forceinline bool cache_files_windows_map_files_enabled()
{
    bool enabled = CACHE_FILES_OVERRIDE || (CACHE_FILES_ENABLED && !g_utility_drive_mounted);
    return enabled;
}

// @address 0x1f5828 ?cached_map_files_find_free_map@@YA?AW4e_map_file_index@@JF@Z
enum e_map_file_index cached_map_files_find_free_map(long size, short type)
{
    long best = -1;
    long first, last;
    switch (type) {
    case _scenario_type_shared: first = 1; last = 1; break;
    case _scenario_type_single_player_shared: first = 2; last = 2; break;
    case _scenario_type_multiplayer: first = 3; last = 12; break;
    case _scenario_type_campaign: first = 13; last = 14; break;
    case _scenario_type_mainmenu: first = 0; last = 0; break;
    // match: 0x1f5828: without it the type switch gains cmplwi r11,4 / bgt to a default that spills first and last to the stack
    default: __assume(0);
    }
    for (long index = first; index <= last; ++index) {
        if (xenon_game_state_globals.active_file_index != index && xenon_game_state_globals.pending_file_index != index) {
            if (::cached_map_file_get_size((e_map_file_index)index) >= (unsigned long)size && new_slot_is_better_fit(best,
        index)) best = index;
        }
    }
    if (cache_files_windows_map_files_enabled()) cached_map_file_close(best);
    return (e_map_file_index)best;
}


// @address 0x75deb0 g_asynchronous_io_arena

// 0x1f3bf8


static __forceinline bool cache_files_windows_override_enabled()
{
    return CACHE_FILES_OVERRIDE || (CACHE_FILES_ENABLED && !g_utility_drive_mounted);
}

static __forceinline bool cache_files_windows_file_handle_valid(void *file_handle)
{
    return file_handle != 0 && file_handle != (void *)-1;
}

// @address 0x1f3bf8 cached_map_file_open
extern "C" bool cached_map_file_open(s_xenon_game_state_file *file, const char *path)
{
    bool result = false;
    unsigned long access = cache_files_windows_override_enabled() ? 1 : 11;
    e_file_open_disposition disposition = (e_file_open_disposition)(cache_files_windows_override_enabled() ? 0 : 3);
    unsigned long attributes = cache_files_windows_override_enabled() ? 1 : 17;
    c_synchronized_long done(0);
    s_create_file_task task;
    async_create_file(path, access, disposition, attributes, _async_category_none,
            _async_priority_blocking_below_sound, (s_file_handle
        *)&file->file_handle, &task);
    while (async_create_file_callback((s_async_task *)&task) != _async_completion_done)
        ;
    if (cache_files_windows_file_handle_valid((void *)file->file_handle))
    {
        for (long file_index = 31; file_index >= 0; file_index--)
        {
            if (!g_asynchronous_io_arena.valid_files.test(file_index))
            {
                if (asynchronous_io_arena_open_file(&g_asynchronous_io_arena, file_index, 0, path))
                    file->io_file_index = file_index;
                break;
            }
        }
        async_create_file(path, access, _file_open_disposition_open_existing, attributes | 8, _async_category_none,
            _async_priority_blocking_below_sound,
            (s_file_handle *)&file->overlapped_handle, &task);
        while (async_create_file_callback((s_async_task *)&task) != _async_completion_done)
            ;
        result = true;
    }
    return result;
}

// match: 0x1f5970: out-of-line memset keeps the file handle in r30 across the call (also 0x1f4848); the intrinsic reloads it into r11 after the zeroing stores
#pragma function(memset)
static __forceinline long cache_files_windows_async_close_file(
    void *file_handle, e_async_category category, e_async_priority priority, c_synchronized_long *done)
{
    s_async_close_file_task task;
    memset(&task, 0, sizeof(task));
    task.flush_file.handle = file_handle;
    return async_task_add_ex(priority, &task, category, async_close_file_callback, done, true);
}

// @address 0x1f5970 cached_map_file_close
extern "C" void cached_map_file_close(long index)
{
    s_xenon_game_state_file *file = cache_files_windows_map_file_get(index);
    if (cache_files_windows_file_handle_valid((void *)file->file_handle))
    {
        c_synchronized_long done(0);
        cache_files_windows_async_close_file(file->file_handle, _async_category_none,
            _async_priority_blocking_below_sound, &done);
        async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
        file->file_handle = (void *)-1;
        g_asynchronous_io_arena.close_file(file->io_file_index);
        cache_files_windows_async_close_file(file->overlapped_handle, _async_category_none,
            _async_priority_blocking_below_sound, &done);
        async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
        file->overlapped_handle = (void *)-1;
    }
    memset(&file->header, 0, sizeof(file->header));
}


static __forceinline const cache_files_windows_range *cache_files_windows_range_find(long x)
{
  const cache_files_windows_range *result = 0;
  for (unsigned long i = 0; i < 5; i++) {
    const cache_files_windows_range *range = &k_cache_file_ranges[i];
    if (x >= range->minimum && x <= range->maximum) {
      result = range;
      break;
    }
  }
  return result;
}

unsigned long cached_map_file_get_size(enum e_map_file_index index)
{
  long result = 0;
  const cache_files_windows_range *range = cache_files_windows_range_find(((long)index));
  if (range)
    result = range->value;
  return (unsigned long)(result);
}

// @address 0x1f5af0 ?cached_map_file_get_path@@YAXW4e_map_file_index@@PADI@Z

static __forceinline const char *cached_map_file_get_directory(e_map_file_index index)
{
    const char *directory = 0;
    if ((unsigned long)index <= 9)
        directory = "cache0:\\";
    else if ((unsigned long)(index - 10) <= 4)
        directory = "cache1:\\";
    return directory;
}

void cached_map_file_get_path(e_map_file_index index, char *path, unsigned int size)
{
    // match: 0x1f5af0: strncpy gets li r5,256 and the terminator is stb 0xff(path); without it size stays in r29 and path+size-1 is computed
    __assume(size == 256);
    const cache_files_windows_range *range = cache_files_windows_range_find(index);
    strncpy(path, "", size);
    path[size - 1] = 0;
    if (range)
        csnzprintf(path, size, "%scache%03d%s", cached_map_file_get_directory(index), index, ".map");
}

// 0x1f5c40


// @address 0x1f5c40 cache_files_windows_strip_extension_lowercase
extern "C" void cache_files_windows_strip_extension_lowercase(const char *src, char *dst)
{
    int length = strcspn(src, ".");
    unsigned int count = length + 1;
    if (count < 256)
        count = length + 1;
    else
        count = 256;
    strncpy(dst, src, count);
    dst[count - 1] = 0;
    ::csstrnlwr((char *)(dst), 256);
}

// @address 0x81f88 cache_files_windows_is_external_map_path
extern "C" bool cache_files_windows_is_external_map_path(const char *path)
{
    return strstr(path, ":\\") != NULL && strstr(path, cache_files_map_directory()) == NULL;
}

// @address 0x1f5ca8 cache_files_windows_get_canonical_map_path
extern "C" void cache_files_windows_get_canonical_map_path(const char *path, c_static_string<256> *buffer)
{
    char clean_path[256];
    char clean_name[256];
    const char *name;
    const char *last;

    if (cache_files_windows_is_external_map_path(path)) {
        clean_path[0] = 0;
        cache_files_windows_strip_extension_lowercase(path, clean_path);
        buffer->print("%s.map", clean_path);
    } else {
        last = strrchr(path, '\\');
        name = last ? last + 1 : path;
        clean_name[0] = 0;
        cache_files_windows_strip_extension_lowercase(name, clean_name);
        buffer->print("d:\\maps\\%s.map", clean_name);
    }
}

// @address 0x1f5d50 cache_files_windows_test_map_paths_equal
extern "C" bool cache_files_windows_test_map_paths_equal(const char *path_a, const char *path_b)
{
    bool result = false;
    c_static_string<256> canonical_a;
    canonical_a.m_string[0] = 0;
    c_static_string<256> canonical_b;
    canonical_b.m_string[0] = 0;
    cache_files_windows_get_canonical_map_path(path_a, &canonical_a);
    cache_files_windows_get_canonical_map_path(path_b, &canonical_b);
    if (strcmp(canonical_a.m_string, canonical_b.m_string) == 0)
        result = true;
    return result;
}


// 0x1f3018


static __forceinline void cache_files_windows_report_io_error(bool success) { if (!success) g_async_file_io_error = GetLastError(); }

// @address 0x1f3018 ?read@s_cache_files_windows_stream_state@@UAA_NV?$c_basic_buffer@X@@PAV2@@Z
bool s_cache_files_windows_stream_state::read(c_basic_buffer<void> source, c_basic_buffer<void> *out)
{
    unsigned char done = cache_files_windows_test_wait_request(this);
    while (done == 0) {
        done = cache_files_windows_test_wait_request(this);
    }
    if (valid != 0) {
        long offset = bytes_done.peek();
        s_lruv_completion_context *context = &overlapped;
        s_cache_files_windows_completion *write_completion = &completion;
        long size = (long)source.m_size;
        context->offset = offset;
        context->completion = &write_completion->result;
        context->offset_high = 0;
        write_completion->size = size;
        write_completion->result.done.set(0);
        c_basic_buffer<void> destination = buffer;
        memcpy((void *)destination.m_address, source.m_address, source.m_size);
        bool success = WriteFileEx(file_handle, (const void *)destination.m_address, size, (OVERLAPPED *)context,
        (LPOVERLAPPED_COMPLETION_ROUTINE)lruv_complete_request) != 0;
        if (success) {
            io_pending.set(1);
        } else {
            cache_files_windows_report_io_error(success);
            valid = 0;
        }
        done = cache_files_windows_test_wait_request(this);
        while (done == 0) {
            done = cache_files_windows_test_wait_request(this);
        }
        if (valid != 0) {
            union { unsigned __int64 value; struct { unsigned long high, low; } words; } hash;
            hash.value = 0;
            *out = source;
            unsigned long count = (unsigned long)size >> 3;
            for (unsigned long i = 0; i < count; i++) {
                hash.value ^= ((const unsigned __int64 *)source.m_address)[i];
            }
            checksum = checksum ^ hash.words.high ^ hash.words.low;
        }
    }
    return valid;
}

// 0x1f33e8
// @address 0x1f33e8 cache_files_windows_initialize
extern "C" void cache_files_windows_initialize(void)
{
    CACHE_FILES_OVERRIDE = !CACHE_FILES_ENABLED || !g_utility_drive_mounted;
    memset(&xenon_game_state_globals, 0, sizeof(xenon_game_state_globals));
    memset(&g_cache_files_windows_pending_load, 0, sizeof(g_cache_files_windows_pending_load));
    xenon_game_state_globals.active_file_index = -1;
    xenon_game_state_globals.pending_file_index = -1;
    xenon_game_state_globals.cached_map_file_index = -1;
    g_cache_files_windows_pending_load.task_done.set(1);
    g_cache_files_windows_pending_load.task_id = -1;
    g_cache_files_windows_pending_load.destination_file_index = -1;
    cache_files_windows_update_language();
    cache_files_windows_update_build_number();
    bool unused = false;
    cache_files_windows_scan_cached_maps(&unused);
    g_game_state_procs.entries[0].handler = &g_cache_files_pending_load_listener;
}


// @address 0x1f3500 cache_files_windows_set_active_map
extern "C" bool cache_files_windows_set_active_map(const char *path, s_cache_file_header *header)
{
    bool result = false;
    long file_index = game_state_xenon_find_file_by_path(path);
    if (cache_files_windows_is_external_map_path(path))
        content_catalogue_make_ready_for_path(path, true);
    bool enabled = CACHE_FILES_OVERRIDE || (CACHE_FILES_ENABLED && !g_utility_drive_mounted);
    if (enabled)
    {
        if (file_index == -1)
        {
            file_index = 3;
            if (!game_state_xenon_open_file(file_index, path))
                file_index = -1;
        }
    }
    if (file_index != -1)
    {
        xenon_game_state_globals.active_file_index = file_index;
        *header = xenon_game_state_globals.files[file_index].header;
        result = true;
    }
    return result;
}

extern "C" {


}
static __forceinline void cache_files_windows_header_construct(s_cache_file_header *header)
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
static __forceinline e_map_file_index cached_map_file_index_from_level(long level)
{
    e_map_file_index index = _map_file_index_none;
    switch (level)
    {
    case _cache_file_level_mainmenu: index = _map_file_index_mainmenu; break;
    case _cache_file_level_multiplayer: index = _map_file_index_shared; break;
    case _cache_file_level_campaign: index = _map_file_index_single_player_shared; break;
    }
    return index;
}
static __forceinline s_xenon_game_state_file *cached_map_file_get_for_level(long level)
{
    s_xenon_game_state_file *file = 0;
    e_map_file_index index = cached_map_file_index_from_level(level);
    if (index != _map_file_index_none)
        file = cache_files_windows_map_file_get(index);
    return file;
}
static __forceinline bool cache_files_windows_read_header(const char *path, s_cache_file_header *header)
{
    bool result = false;
    c_static_string<256> map_name;
    map_name.m_string[0] = 0;
    cache_files_windows_get_canonical_map_path(path, &map_name);
    void *handle = CreateFileA(map_name.m_string, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (handle != (void *)-1)
    {
        unsigned long bytes_read;
        if (ReadFile(handle, header, sizeof(*header), &bytes_read, 0) != 0
            && bytes_read == sizeof(*header))
        {
            cache_file_header_to_host(header);
            if (cache_file_header_verify(header, map_name.m_string, true))
                result = true;
        }
        CloseHandle(handle);
    }
    return result;
}
// @address 0x1f3df8 cache_files_windows_scan_cached_maps
extern "C" void cache_files_windows_scan_cached_maps(bool *success_out)
{
    bool result = false;
    bool deleted_older = true;
    bool level_loaded[3];
    for (long level = 0; level < 3; level++)
        level_loaded[level] = true;

    if (!cache_files_windows_override_enabled())
    {
        cached_map_files_delete(k_number_of_map_files, k_number_of_legacy_map_files);
        for (long index = 0; index < 15; index++)
        {
            s_xenon_game_state_file *file = &xenon_game_state_globals.files[index];
            bool found = false;
            bool valid = false;
            c_synchronized_long read_done(0);
            read_done.value = 0;
            char map_path[256];
            cached_map_file_get_path((e_map_file_index)index, map_path, 256);
            unsigned long size = cached_map_file_get_size((e_map_file_index)index);
            bool open_ok = cached_map_file_open(file, map_path);
            if (open_ok)
            {
                __int64 free_space = 0;
                bool read_ok = async_allocate_file_space(cached_map_file_get_directory((e_map_file_index)index),
                                            (void *)file->file_handle, size, &free_space);
                if (free_space == 0 && read_ok)
                {
                    valid = true;
                    found = true;
                }
                else
                {
                    if (deleted_older)
                    {
                        cached_map_files_delete((e_map_file_index)(index + 1), k_number_of_map_files);
                        deleted_older = false;
                    }
                    if (!read_ok)
                        read_ok = async_allocate_file_space(cached_map_file_get_directory((e_map_file_index)index),
                                               (void *)file->file_handle, size, &free_space);
                    if (read_ok)
                    {
                        if (free_space != (__int64)size)
                        {
                            bool complete;
                            game_state_xenon_write_file(index, 0, 9, (unsigned char *)&complete, &read_done);
                            async_block_until_done(&read_done, simple_yield_function, 0, 0, 0, 0);
                            if (complete)
                            {
                                valid = false;
                                found = true;
                            }
                        }
                        else
                        {
                            valid = false;
                            found = true;
                        }
                    }
                }
            }

            if (valid)
            {
                if (game_state_read_from_file(index))
                {
                    c_synchronized_long size_done(0);
                    unsigned long async_size;
                    async_get_file_size(cache_files_windows_handle_from_slot((void *)file->file_handle), &async_size,
                                        _async_category_none,
            _async_priority_blocking_below_sound, &size_done);
                    async_block_until_done(&size_done, simple_yield_function, 0, 0, 0, 0);
                    if (async_size != cached_map_file_get_size((e_map_file_index)index))
                        valid = false;
                }
                else
                {
                    valid = false;
                }
            }

            if (valid)
            {
                bool verify_header = false;
                bool is_level_file = false;
                if ((unsigned long)index < 3)
                    is_level_file = true;
                if (is_level_file)
                    verify_header = true;
                if (verify_header)
                {
                    s_cache_file_header read_header;
                    cache_files_windows_header_construct((&read_header));
                    valid = cache_files_windows_read_header((&file->header)->source_path, &read_header);
                    if (valid)
                        valid = memcmp((&file->header)->digest.hashes,
                                       (&read_header)->digest.hashes, sizeof(read_header.digest.hashes)) == 0;
                    if (valid)
                        valid = CompareFileTime((const FILETIME *)&(&read_header)->timestamp,
                                             (const FILETIME *)&(&file->header)->timestamp) == 0;
                }

                if (valid && index != 0 && index != 1 && index != 2)
                {
                    for (long level = 0; valid && level < 3; level++)
                    {
                        if (((&file->header)->level_mask & (1 << level)) != 0)
                        {
                            valid = level_loaded[level];
                            if (valid)
                            {
                                s_xenon_game_state_file *level_file = cached_map_file_get_for_level(level);
                                valid = CompareFileTime(
                                    (const FILETIME *)&(&level_file->header)->timestamp,
                                    (const FILETIME *)&file->header.level_timestamps[level]) == 0;
                            }
                        }
                    }
                }
            }

            if (!found)
            {
                result = true;
                cached_map_file_close(index);
            }

            if (!valid)
            {
                memset(&file->header, 0, sizeof(file->header));
                switch (index)
                {
                case _cache_file_level_mainmenu: level_loaded[0] = false; break;
                case _cache_file_level_multiplayer: level_loaded[1] = false; break;
                case _cache_file_level_campaign: level_loaded[2] = false; break;
                }
            }
        }
    }
    else
    {
        for (long index = 0; index < 15; index++)
        {
            s_xenon_game_state_file *file = &xenon_game_state_globals.files[index];
            file->file_handle = (void *)-1;
            file->overlapped_handle = (void *)-1;
        }

        const struct
        {
            const char *path;
            e_cache_file_level level;
        } levels[] =
        {
            { "levels\\ui\\mainmenu\\mainmenu", _cache_file_level_mainmenu },
            { "levels\\shared\\shared\\shared", _cache_file_level_multiplayer },
            { "levels\\shared\\shared\\campaign", _cache_file_level_campaign },
        };
        const long level_count = NUMBEROF(levels);
        for (long index = 0; index < level_count; index++)
            level_loaded[index] = game_state_xenon_open_file(levels[index].level, levels[index].path);
    }

    *success_out = !result;
    if (g_utility_drive_mounted != 0)
        XFlushUtilityDrive();
}

// @rdata 0x820392dc..0x82039350
// @address 0x392dc k_cache_file_ranges
const cache_files_windows_range k_cache_file_ranges[5] =
{
    { 0, 0, 0x4100000 },
    { 1, 1, 0xbe00000 },
    { 2, 2, 0x22600000 },
    { 3, 12, 0x9600000 },
    { 13, 14, 0x1f400000 }
};
static const long k_cache_copy_priorities[7] = { 0, 2, 2, 2, 2, 2, 2 };
static const long k_cache_copy_buffer_sizes[7] = { 0x40000, 0x100000, 0x100000, 0x100000, 0x100000, 0x100000, 0x100000 };
// @rdata-end





class c_sound_cache_hash_source;


static __forceinline long cache_files_windows_async_create_file(
    const char *file_name, e_async_category category, e_async_priority priority, s_file_handle volatile *file, c_synchronized_long *done)
{
    s_cache_files_windows_create_task task;
    memset(&task, 0, sizeof(task));
    strncpy(task.file_name, file_name, 256);
    task.file_name[255] = 0;
    task.share_mode = FILE_SHARE_READ;
    task.create_directories = false;
    task.access = GENERIC_READ;
    task.disposition = OPEN_EXISTING;
    task.flags = FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED;
    task.file = file;
    return async_task_add_ex(priority, (s_async_task *)&task, category, async_create_file_callback, done, true);
}

static __forceinline long cache_files_windows_async_flush_file(
    void *file_handle, e_async_category category, e_async_priority priority, c_synchronized_long *done)
{
    s_async_close_file_task task;
    memset(&task, 0, sizeof(task));
    task.flush_file.handle = file_handle;
    return async_task_add_ex(priority, &task, category, async_flush_file_callback, done, true);
}

static __forceinline long cache_files_windows_async_close_source(
    void *file_handle, e_async_category category, e_async_priority priority, c_synchronized_long *done)
{
    s_async_close_file_task task;
    memset(&task, 0, sizeof(task));
    task.flush_file.handle = file_handle;
    return async_task_add_ex(priority, &task, category, async_close_file_no_flush_callback, done, true);
}

static __forceinline long cache_files_windows_async_copy_position(void *source, void *destination, void *buffer, unsigned long size,
    unsigned long source_position, unsigned long destination_position, bool extra_flag, e_async_priority priority,
    c_synchronized_long *progress, c_synchronized_long *done)
{
    s_cache_files_windows_copy_task task;
    progress->set(0);
    memset(&task, 0, sizeof(task));
    task.copy_position.source = source;
    task.copy_position.destination = destination;
    task.copy_position.buffer = buffer;
    task.copy_position.size = size;
    task.copy_position.source_position = source_position;
    task.copy_position.destination_position = destination_position;
    task.copy_position.extra_flag = extra_flag;
    task.copy_position.progress = progress;
    return async_task_add_ex(priority, &task, _async_category_cache_files, async_copy_position_callback, done, true);
}

static __forceinline long cache_files_windows_async_write_header(void *handle, const void *buffer, unsigned long size,
    unsigned long file_size, bool flush, bool *success_out, e_async_priority priority, c_synchronized_long *done)
{
    s_cache_files_windows_write_header_task task;
    task.write_header.handle = handle;
    task.write_header.buffer = buffer;
    task.write_header.flush = flush;
    task.write_header.size = size;
    task.write_header.file_size = file_size;
    task.write_header.success_out = success_out;
    return async_task_add_ex(priority, &task, _async_category_cache_files, async_write_header_callback, done, true);
}

static __forceinline long cache_files_windows_failure_count(const char *path)
{
    s_failed_map *failed_map = find_failed_map(path);
    long failure_count = 0;
    if (failed_map != 0)
        failure_count = failed_map->failure_count;
    return failure_count;
}

static __forceinline bool cache_files_windows_map_failed_too_often(const char *path)
{
    long maximum_failures = 3;
    return cache_files_windows_failure_count(path) >= maximum_failures;
}

static __forceinline void cache_files_windows_clear_failed_map(const char *path)
{
    s_failed_map *failed_map = find_failed_map(path);
    if (failed_map != 0)
        memset(failed_map, 0, sizeof(*failed_map));
}

static __forceinline bool cache_files_windows_request_pending()
{
    return g_cache_files_windows_pending_load.m_cached_maps[1].name.m_string[0] != 0;
}

static __forceinline unsigned long cache_files_windows_realtime_checksum(const void *buffer, long length)
{
    union { unsigned __int64 value; struct { unsigned long high, low; } words; } checksum;
    checksum.value = 0;
    unsigned long count = ((unsigned long)length) >> 3;
    if (count != 0)
    {
        const unsigned __int64 *entries = (const unsigned __int64 *)buffer;
        do
        {
            checksum.value ^= *entries++;
        } while (--count != 0);
    }

    return checksum.words.high ^ checksum.words.low;
}

static __forceinline void cache_files_windows_stream_begin(
    s_cache_files_windows_stream_state *source, s_file_handle file_handle, long offset, long checksum, bool flag,
        c_basic_buffer<void> buffer)
{
    source->checksum = checksum;
    source->valid = flag;
    source->file_handle = file_handle.handle;
    __lwsync();
    _InterlockedExchange(&source->bytes_done.value, offset);
    source->buffer = buffer;
    __lwsync();
    _InterlockedExchange(&source->io_pending.value, 0);
}

static __forceinline void cache_files_windows_stream_end(s_cache_files_windows_stream_state *source, volatile long *offset, long *checksum)
{
    long unused_progress = source->io_pending.value;
    __lwsync();
    source->buffer.m_address = 0;
    source->buffer.m_size = 0;
    source->active = false;
    long streamed = source->bytes_done.value;
    __lwsync();
    *offset = streamed;
    // match: 0x1f4848: volatile completion read is required; stream-begin writes stay nonvolatile for scheduling.
    const volatile long &completed_checksum = source->checksum;
    *checksum = completed_checksum;
}


// @address 0x1f4848 cache_files_windows_update_map_copy
extern "C" bool cache_files_windows_update_map_copy()
{
    bool success = true;
    long next_state = g_cache_files_windows_pending_load.countdown;
    long priority = k_cache_copy_priorities[g_cache_files_windows_pending_load.m_cached_maps[0].level];
    switch (g_cache_files_windows_pending_load.countdown)
    {
    case _cache_file_copy_idle:
        if (cache_files_windows_request_pending())
        {
            if (!cache_files_windows_map_failed_too_often(g_cache_files_windows_pending_load.m_cached_maps[1].name.m_string)
                && cache_files_windows_get_map_status(g_cache_files_windows_pending_load.m_cached_maps[1].name.m_string) != 3)
            {
                cache_files_windows_get_canonical_map_path(g_cache_files_windows_pending_load.m_cached_maps[1].name.m_string,
                    &g_cache_files_windows_pending_load.m_current_map_name);
                memcpy(&g_cache_files_windows_pending_load.m_cached_maps[0], &g_cache_files_windows_pending_load.m_cached_maps[1],
                    sizeof(g_cache_files_windows_pending_load.m_cached_maps[0]));
                next_state = _cache_file_copy_open_source;
            }
        }
        memset(&g_cache_files_windows_pending_load.m_cached_maps[1], 0, sizeof(g_cache_files_windows_pending_load.m_cached_maps[1]));
        g_cache_files_windows_pending_load.phase = _cache_file_copy_pending;
        break;
    case _cache_file_copy_open_source:
        g_cache_files_windows_pending_load.task_id = cache_files_windows_async_create_file(
            g_cache_files_windows_pending_load.m_current_map_name.m_string, _async_category_cache_files, (e_async_priority)priority,
            (s_file_handle *)&g_cache_files_windows_pending_load.source_file_handle,
            (c_synchronized_long *)&g_cache_files_windows_pending_load.task_done);
        next_state = _cache_file_copy_source_opened;
        break;
    case _cache_file_copy_source_opened:
        success = cache_files_windows_file_handle_valid(g_cache_files_windows_pending_load.source_file_handle);
        next_state = _cache_file_copy_read_header;
        break;
    case _cache_file_copy_read_header:
        memset(&g_cache_files_windows_pending_load.header, 0, sizeof(g_cache_files_windows_pending_load.header));
        g_cache_files_windows_pending_load.task_id = async_read_header(g_cache_files_windows_pending_load.source_file_handle,
            &g_cache_files_windows_pending_load.header, sizeof(g_cache_files_windows_pending_load.header), 0, 1, priority,
            (c_synchronized_long *)&g_cache_files_windows_pending_load.transfer_size,
            (c_synchronized_long *)&g_cache_files_windows_pending_load.task_done, false);
        next_state = g_cache_files_windows_pending_load.task_id != -1
            ? _cache_file_copy_verify_header : _cache_file_copy_read_header;
        break;
    case _cache_file_copy_verify_header:
        cache_file_header_to_host(&g_cache_files_windows_pending_load.header);
        cache_files_windows_current_file_time(&g_cache_files_windows_pending_load.header.cached_map_date);
        strncpy(g_cache_files_windows_pending_load.header.source_path, g_cache_files_windows_pending_load.m_current_map_name.m_string, 256);
        g_cache_files_windows_pending_load.header.source_path[255] = 0;
        success = g_cache_files_windows_pending_load.transfer_size.peek() == sizeof(g_cache_files_windows_pending_load.header);
        success = success &&
            cache_file_header_verify(
                &g_cache_files_windows_pending_load.header, g_cache_files_windows_pending_load.m_current_map_name.m_string, false);
        if (success)
        {
            unsigned long mismatch_mask;
            if (!game_state_xenon_compare_file_times(&g_cache_files_windows_pending_load.header, &mismatch_mask))
                success = false;
        }
        next_state = _cache_file_copy_get_size;
        break;
    case _cache_file_copy_get_size:
        g_cache_files_windows_pending_load.task_id = async_get_file_size(
            cache_files_windows_handle_from_slot(g_cache_files_windows_pending_load.source_file_handle),
            (unsigned long *)&g_cache_files_windows_pending_load.bytes_total, _async_category_cache_files, (e_async_priority)priority,
            (c_synchronized_long *)&g_cache_files_windows_pending_load.task_done);
        next_state = _cache_file_copy_verify_size;
        break;
    case _cache_file_copy_verify_size:
        success = g_cache_files_windows_pending_load.bytes_total != (unsigned long)-1;
        success = success && g_cache_files_windows_pending_load.bytes_total > sizeof(g_cache_files_windows_pending_load.header);
        g_cache_files_windows_pending_load.checksum = 0;
        next_state = _cache_file_copy_create_destination;
        break;
    case _cache_file_copy_create_destination:
        {
            s_cache_file_header *header = &g_cache_files_windows_pending_load.header;
            long free_map = cached_map_files_find_free_map(header->file_size, header->scenario_type);
            if (free_map != -1)
            {
                g_cache_files_windows_pending_load.destination_file_index = free_map;
                g_cache_files_windows_pending_load.destination_created = 0;
                g_cache_files_windows_pending_load.task_id = game_state_xenon_write_file(free_map, 1, priority,
                    &g_cache_files_windows_pending_load.destination_created, &g_cache_files_windows_pending_load.task_done);
                next_state = _cache_file_copy_destination_created;
            }
        }
        break;
    case _cache_file_copy_destination_created:
        success = g_cache_files_windows_pending_load.transfer_size.peek() == sizeof(g_cache_files_windows_pending_load.header);
        next_state = _cache_file_copy_flush_destination;
        break;
    case _cache_file_copy_flush_destination:
        g_cache_files_windows_pending_load.task_id = cache_files_windows_async_flush_file(
            xenon_game_state_globals.files[g_cache_files_windows_pending_load.destination_file_index].file_handle, _async_category_cache_files,
            (e_async_priority)priority, (c_synchronized_long *)&g_cache_files_windows_pending_load.task_done);
        next_state = _cache_file_copy_begin_stream;
        break;
    case _cache_file_copy_begin_stream:
        g_cache_files_windows_pending_load.transfer_size.set(0);
        g_cache_files_windows_pending_load.bytes_done = sizeof(g_cache_files_windows_pending_load.header);
        g_cache_files_windows_pending_load.copy_start_time = GetTickCount();
        next_state = _cache_file_copy_stream;
        break;
    case _cache_file_copy_stream:
        {
            long buffer_size = k_cache_copy_buffer_sizes[g_cache_files_windows_pending_load.m_cached_maps[0].level];
            cache_files_windows_update_copy_buffer(buffer_size);
        }
        if (g_cache_files_windows_pending_load.copy_buffer != 0)
        {
            __lwsync();
            _InterlockedExchange(&g_cache_files_windows_pending_load.armed.value, 0);
            cache_files_windows_stream_begin(&g_cache_files_windows_stream_state,
                cache_files_windows_handle_from_slot((void
        *)xenon_game_state_globals.files[g_cache_files_windows_pending_load.destination_file_index].file_handle),
                g_cache_files_windows_pending_load.bytes_done, g_cache_files_windows_pending_load.checksum, success,
                c_basic_buffer<void>(g_cache_files_windows_pending_load.copy_buffer,
                    g_cache_files_windows_pending_load.copy_buffer_size / 2));
            g_cache_files_windows_stream_state.active = success;
            long second_half = g_cache_files_windows_pending_load.copy_buffer_size / 2;
            g_cache_files_windows_pending_load.task_id = gui_custom_bitmap_storage_queue_async_read((e_async_priority)priority,
                g_cache_files_windows_pending_load.source_file_handle, (unsigned long)g_cache_files_windows_pending_load.bytes_done,
                g_cache_files_windows_pending_load.bytes_total - (unsigned long)g_cache_files_windows_pending_load.bytes_done,
                (const s_gui_custom_bitmap_storage_hash *)0,
                c_basic_buffer<void>(g_cache_files_windows_pending_load.copy_buffer + second_half, second_half),
                &g_cache_files_windows_stream_state, c_basic_buffer<void>(),
                &g_cache_files_windows_pending_load.stream_success, &g_cache_files_windows_pending_load.armed,
                (c_synchronized_long *)0, &g_cache_files_windows_pending_load.task_done);
            next_state = _cache_file_copy_stream_finished;
        }
        else
        {
            next_state = _cache_file_copy_stream;
        }
        break;
    case _cache_file_copy_stream_finished:
        {
            cache_files_windows_stream_end(&g_cache_files_windows_stream_state, &g_cache_files_windows_pending_load.bytes_done,
                &g_cache_files_windows_pending_load.checksum);
            if ((unsigned long)g_cache_files_windows_pending_load.bytes_done >= g_cache_files_windows_pending_load.bytes_total)
            {
                success = cache_file_checksum_to_host(g_cache_files_windows_pending_load.checksum)
                    == g_cache_files_windows_pending_load.header.cached_map_checksum;
                next_state = _cache_file_copy_flush_complete;
            }
            else
            {
                long armed_value = g_cache_files_windows_pending_load.armed.value;
                __lwsync();
                success = armed_value != 0;
                next_state = _cache_file_copy_stream;
            }
        }
        break;
    case _cache_file_copy_begin_copy:
        g_cache_files_windows_pending_load.transfer_size.set(0);
        g_cache_files_windows_pending_load.bytes_done = sizeof(g_cache_files_windows_pending_load.header);
        g_cache_files_windows_pending_load.copy_start_time = GetTickCount();
        next_state = _cache_file_copy_copy_chunk;
        break;
    case _cache_file_copy_copy_chunk:
        {
            long buffer_size = k_cache_copy_buffer_sizes[g_cache_files_windows_pending_load.m_cached_maps[0].level];
            cache_files_windows_update_copy_buffer(buffer_size);
        }
        if (g_cache_files_windows_pending_load.copy_buffer != 0)
        {
            long left = (long)(g_cache_files_windows_pending_load.bytes_total - (unsigned
        long)g_cache_files_windows_pending_load.bytes_done);
            g_cache_files_windows_pending_load.copy_size = g_cache_files_windows_pending_load.copy_buffer_size > left
                ? left
                : g_cache_files_windows_pending_load.copy_buffer_size;
            g_cache_files_windows_pending_load.task_id = cache_files_windows_async_copy_position(
                g_cache_files_windows_pending_load.source_file_handle,
                (void *)xenon_game_state_globals.files[g_cache_files_windows_pending_load.destination_file_index].file_handle,
                g_cache_files_windows_pending_load.copy_buffer, g_cache_files_windows_pending_load.copy_size,
                g_cache_files_windows_pending_load.bytes_done, g_cache_files_windows_pending_load.bytes_done, true,
                (e_async_priority)priority, &g_cache_files_windows_pending_load.transfer_size,
                (c_synchronized_long *)&g_cache_files_windows_pending_load.task_done);
            next_state = _cache_file_copy_chunk_copied;
        }
        else
        {
            next_state = _cache_file_copy_copy_chunk;
        }
        break;
    case _cache_file_copy_chunk_copied:
        success = g_cache_files_windows_pending_load.copy_size == g_cache_files_windows_pending_load.transfer_size.peek();
        g_cache_files_windows_pending_load.checksum ^= cache_files_windows_realtime_checksum(
            g_cache_files_windows_pending_load.copy_buffer, g_cache_files_windows_pending_load.transfer_size.peek());
        if (success)
        {
            g_cache_files_windows_pending_load.bytes_done += g_cache_files_windows_pending_load.transfer_size.peek();
            if ((unsigned long)g_cache_files_windows_pending_load.bytes_done >= g_cache_files_windows_pending_load.bytes_total)
            {
                success = cache_file_checksum_to_host(g_cache_files_windows_pending_load.checksum)
                    == g_cache_files_windows_pending_load.header.cached_map_checksum;
                next_state = _cache_file_copy_flush_complete;
            }
            else
            {
                next_state = _cache_file_copy_copy_chunk;
            }
        }
        break;
    case _cache_file_copy_flush_complete:
        g_cache_files_windows_pending_load.task_id = cache_files_windows_async_flush_file(
            xenon_game_state_globals.files[g_cache_files_windows_pending_load.destination_file_index].file_handle, _async_category_cache_files,
            (e_async_priority)priority, (c_synchronized_long *)&g_cache_files_windows_pending_load.task_done);
        next_state = _cache_file_copy_write_header;
        break;
    case _cache_file_copy_write_header:
        {
            s_xenon_game_state_file *file = cache_files_windows_map_file_get(g_cache_files_windows_pending_load.destination_file_index);
            cache_files_windows_current_file_time(&g_cache_files_windows_pending_load.header.cached_map_date);
            g_cache_files_windows_pending_load.header_written = 0;
            cache_file_header_to_disk(&g_cache_files_windows_pending_load.header);
            g_cache_files_windows_pending_load.task_id = cache_files_windows_async_write_header((void *)file->file_handle,
                &g_cache_files_windows_pending_load.header, sizeof(g_cache_files_windows_pending_load.header),
                cached_map_file_get_size((e_map_file_index)g_cache_files_windows_pending_load.destination_file_index), true,
                (bool *)&g_cache_files_windows_pending_load.header_written, (e_async_priority)priority,
                (c_synchronized_long *)&g_cache_files_windows_pending_load.task_done);
            next_state = _cache_file_copy_header_written;
        }
        break;
    case _cache_file_copy_header_written:
        cache_file_header_to_host(&g_cache_files_windows_pending_load.header);
        success = g_cache_files_windows_pending_load.header_written;
        next_state = _cache_file_copy_complete;
        break;
    case _cache_file_copy_complete:
        g_cache_files_windows_pending_load.phase = success;
        next_state = _cache_file_copy_cancel;
        break;
    case _cache_file_copy_cancel:
        next_state = _cache_file_copy_close_source;
        break;
    case _cache_file_copy_close_source:
        if (cache_files_windows_file_handle_valid(g_cache_files_windows_pending_load.source_file_handle))
            g_cache_files_windows_pending_load.task_id = cache_files_windows_async_close_source(
                g_cache_files_windows_pending_load.source_file_handle, _async_category_none,
            _async_priority_blocking_below_sound,
                (c_synchronized_long *)&g_cache_files_windows_pending_load.task_done);
        next_state = _cache_file_copy_finish;
        break;
    case _cache_file_copy_finish:
        {
            s_cache_files_windows_copy_request request;
            memcpy(&request, &g_cache_files_windows_pending_load.m_cached_maps[0], sizeof(request));
            switch (g_cache_files_windows_pending_load.phase)
            {
            case _cache_file_copy_succeeded:
                memcpy(&cache_files_windows_map_file_get(g_cache_files_windows_pending_load.destination_file_index)->header,
                    &g_cache_files_windows_pending_load.header, sizeof(g_cache_files_windows_pending_load.header));
                memset(&g_cache_files_windows_pending_load.header, 0, sizeof(g_cache_files_windows_pending_load.header));
                cache_files_windows_clear_failed_map(request.name.m_string);
                break;
            case _cache_file_copy_failed:
                if (!cache_files_windows_map_failed_too_often(request.name.m_string))
                    cache_files_windows_request_map_load(request.name.m_string, request.level);
                break;
            }
            cache_files_windows_call_pending_load_procs();
            g_cache_files_windows_pending_load.destination_file_index = -1;
            memset(&g_cache_files_windows_pending_load.header, 0, sizeof(g_cache_files_windows_pending_load.header));
            memset(&g_cache_files_windows_pending_load.m_cached_maps[0], 0, sizeof(g_cache_files_windows_pending_load.m_cached_maps[0]));
            g_cache_files_windows_pending_load.m_current_map_name.m_string[0] = 0;
            g_cache_files_windows_pending_load.source_file_handle = (void *)-1;
            next_state = _cache_file_copy_idle;
        }
        break;
    default:
        // match: 0x1f4848: without it the state switch gains cmplwi r9,22 / bgt before the jump table
        __assume(0);
    }
    if (g_cache_files_windows_pending_load.release_copy_buffer)
    {
        cache_files_windows_call_pending_load_procs();
        g_cache_files_windows_pending_load.release_copy_buffer = false;
    }
    if (success)
    {
        g_cache_files_windows_pending_load.previous_state = g_cache_files_windows_pending_load.countdown;
        g_cache_files_windows_pending_load.countdown = next_state;
    }
    else
    {
        cache_files_windows_call_pending_load_procs();
        cache_file_map_has_failed(g_cache_files_windows_pending_load.m_current_map_name.m_string,
        g_cache_files_windows_pending_load.m_cached_maps[0].level);
        g_cache_files_windows_pending_load.countdown = _cache_file_copy_cancel;
        g_cache_files_windows_pending_load.phase = _cache_file_copy_failed;
    }
    return success;
}

// @address 0x81c70 cache_files_windows_request_map_load
extern "C" long cache_files_windows_request_map_load(const char *path, long level)
{
    long result = 0;
    long status = cache_files_windows_get_map_status(path);
    c_static_string<256> canonical_path;
    cache_files_windows_get_canonical_map_path(path, &canonical_path);
    if (status == 0)
    {
        bool request = false;
        if (!cache_files_windows_is_caching_enabled())
        {
            if (!cache_files_windows_request_pending() || level >= g_cache_files_windows_pending_load.m_cached_maps[1].level)
                request = true;
        }
        else
        {
            request = level <= 2;
        }
        if (request)
        {
            memset(&g_cache_files_windows_pending_load.m_cached_maps[1], 0, sizeof(g_cache_files_windows_pending_load.m_cached_maps[1]));
            g_cache_files_windows_pending_load.m_cached_maps[1].name.set(canonical_path.get_string());
            g_cache_files_windows_pending_load.m_cached_maps[1].level = level;
            result = 1;
        }
    }
    else if (status == 2)
    {
        if (cache_files_windows_test_map_paths_equal(canonical_path.get_string(), g_cache_files_windows_pending_load.m_cached_maps[0].name.m_string))
            g_cache_files_windows_pending_load.m_cached_maps[0].level = level;
        result = 1;
    }
    else if (status == 1)
    {
        if (cache_files_windows_test_map_paths_equal(canonical_path.get_string(), g_cache_files_windows_pending_load.m_cached_maps[1].name.m_string))
            g_cache_files_windows_pending_load.m_cached_maps[1].level = level;
        result = 1;
    }
    else if (status == 3)
    {
        result = 1;
    }

    if (g_cache_files_windows_pending_load.m_cached_maps[0].name.m_string[0] != 0
        && cache_files_windows_request_pending()
        && g_cache_files_windows_pending_load.m_cached_maps[1].level >= g_cache_files_windows_pending_load.m_cached_maps[0].level)
    {
        cache_files_windows_cancel_active_copy();
    }
    return result;
}

// @address 0x81dc8 cache_files_windows_process_loading_requests
extern "C" void cache_files_windows_process_loading_requests(void)
{
    s_next_scenario_path next;
    if (!loading_get_next_scenario_path(&next))
        return;

    if (next.matchmaking)
    {
        c_static_string<256> shared_path;
        cache_files_windows_get_canonical_map_path("levels\\shared\\shared\\campaign", &shared_path);
        cache_files_windows_cancel_map_request(shared_path.get_string());
        c_static_string<256> intro_path;
        cache_files_windows_get_canonical_map_path("levels\\solo\\005_intro\\005_intro", &intro_path);
        cache_files_windows_cancel_map_request(intro_path.get_string());
        return;
    }

    if (next.has_path)
    {
        long type = next.type;
        if (cache_files_windows_get_map_status(next.path) == 0)
            cache_files_windows_request_map_load(next.path, type);
    }
    else
    {
        cache_files_windows_request_map_load(next.path, next.type);
    }
}

// @address 0x81e90 cache_files_windows_get_map_status
extern "C" long cache_files_windows_get_map_status(const char *path)
{
    c_static_string<256> canonical_path;
    cache_files_windows_get_canonical_map_path(path, &canonical_path);
    if (game_state_xenon_find_file_by_path(canonical_path.get_string()) != NONE)
        return 3;
    if (cache_files_windows_map_failed_too_often(path))
        return 4;
    if (cache_files_windows_get_active_map_level(path) != NONE)
        return 2;
    if (!cache_files_windows_is_caching_enabled() && cache_files_windows_get_pending_map_level(path) != NONE)
        return 1;
    return 0;
}

// @address 0x81ac8 cache_files_windows_idle
extern "C" void cache_files_windows_idle(void)
{
    if (!cache_files_windows_override_enabled())
    {
        if (g_cache_files_windows_pending_load.task_done.peek() != 0)
            cache_files_windows_update_map_copy();
        return;
    }

    s_cache_files_windows_cached_map *request = &g_cache_files_windows_pending_load.m_cached_maps[1];
    if (request->name.m_string[0] == 0)
        return;

    s_failed_map *failed_map = find_failed_map(request->name.m_string);
    bool failed_too_often = failed_map && failed_map->failure_count >= 3;
    if (!failed_too_often && cache_files_windows_get_map_status(request->name.m_string) != 3)
    {
        long level = request->level;
        if (xenon_game_state_globals.active_file_index != xenon_game_state_globals.cached_map_file_index)
        {
            if (xenon_game_state_globals.cached_map_file_index != NONE)
            {
                if (xenon_game_state_globals.cached_map_file_index == xenon_game_state_globals.pending_file_index
                    || xenon_game_state_globals.cached_map_level > level)
                {
                    memset(request, 0, sizeof(*request));
                    return;
                }
                cached_map_file_close(xenon_game_state_globals.cached_map_file_index);
                xenon_game_state_globals.cached_map_file_index = NONE;
                xenon_game_state_globals.cached_map_level = NONE;
            }
        }
        else
        {
            xenon_game_state_globals.cached_map_file_index = NONE;
            xenon_game_state_globals.cached_map_level = NONE;
        }

        long index = cached_map_files_find_free_map(0, 1);
        if (game_state_xenon_open_file(index, request->name.m_string))
        {
            xenon_game_state_globals.cached_map_file_index = index;
            xenon_game_state_globals.cached_map_level = level;
        }
        else
        {
            cache_file_map_has_failed(request->name.m_string, level);
        }
    }
    memset(request, 0, sizeof(*request));
}
