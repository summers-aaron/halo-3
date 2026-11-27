// networkinglibxbox/network_storage_cache.obj: from the records 0x404620 0x404800 0x404948 0x404c08 0x404eb0 0x404f18 0x405030 0x4050b8 0x405210 0x4052c0 0x405358 0x4054e8 0x4057c8 0x4059c8 0x405a18 0x492960 0x492ba8 0x4a64e8
#include "interface/gui_custom_bitmap_storage.h"
#include "shell/utility_drive.h"
#include "multithreading/synchronization_types.h"
#include <stdarg.h>
#include "multithreading/synchronization.h"
#include "cache/security_functions.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "text/font_cache.h"
#include "saved_games/content_catalogue.h"
#include "networking/logic/storage/network_storage_cache.h"
#include "cseries/async.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "multithreading/synchronized_value.h"
#include "cseries/async_helpers.h"
#include "networking/logic/storage/network_storage_cache_key_types.h"
#include "networking/logic/storage/network_storage_bitmap_load_task_types.h"
#include "networking/tools/network_blf.h"
#include "cseries/crt_format.h"
#include "tag_files/files_windows_types.h"
#include "networking/logic/storage/network_storage_manifest_internals.h"
#include "networking/logic/storage/network_storage_cache_types.h"
#include "memory/data_types.h"
#include "networking/tools/blf_authentication_types.h"
#include <string.h>
#include "networking/logic/storage/network_http_request_hash.h"
#include "cseries/async_file_write.h"
#include "text/text_group.h"
#include <ppcintrinsics.h>
#include "tag_files/file_reference.h"
#include "cseries/async_completion.h"
#include "networking/logic/network_storage_cache_services.h"
#include "interface/gui_custom_bitmap_storage_decode.h"
#include "cseries/byte_swapping.h"

// @flags /TP /O2 /Oc
// @address 0x4059c8 ??$sprintf_s@$0BAE@@@YAHAAY0BAE@DPBDZZ

c_network_storage_cache g_network_storage_cache = { false, 1, 0 };

// @address 0x591fe8 vsprintf_s
struct s_file_reference;

struct c_static_string41 {
  char s[41];
  inline long length() const {
    const char *p = s;
    unsigned long length = 0;
    do {
      char c = *p++;
      if (c == 0)
        break;
      length++;
    } while (length < 41);
    return length;
  }
  const char *append_print(const char *format, ...);
};

// clear the manifest's expiry and data, re-point its download at the base url
struct s_network_storage_cache_url
{
    c_static_string<256> name;
    long service_type;
    long cache_mode;
    long lifetime_seconds;
    long queue_type;
};

// 0x4a64e8: bitmap storage slot lookup + load

// 0x492960: cache load operation state machine

struct s_cache_load_operation {
    long state;
    s_file_reference *file;
    unsigned long file_size;
    char *buffer;
    unsigned long capacity;
    long destination;
    long context;
    long *cancelled;
    long *completed;
    unsigned char mounted;
};
struct s_cache_load_chunk {
    long version;
    long size;
    char data[1];
};

namespace {
static __forceinline void network_storage_cache_leave_add_buffer_lock(long index);
class c_network_storage_cache_add_buffer_scope_lock {
public:
 __forceinline c_network_storage_cache_add_buffer_scope_lock() { internal_critical_section_enter(50); }
 __forceinline ~c_network_storage_cache_add_buffer_scope_lock() { network_storage_cache_leave_add_buffer_lock(50); }
};
struct s_network_storage_cache_scope_lock {
 __forceinline s_network_storage_cache_scope_lock() { internal_critical_section_enter(50); }
 __forceinline ~s_network_storage_cache_scope_lock() { internal_critical_section_leave(50); }
};
static __forceinline void network_storage_cache_leave_add_buffer_lock(long index)
{
 s_synchronization_globals *globals = &g_synch_globals;
 _InterlockedDecrement(&globals->critical_sections[index].lock_count.value);
 RtlLeaveCriticalSection(&globals->critical_sections[index].critical_section);
}

// @address 0x19f058 ?security_calculate_hash@@YA_NPBDK_NPAUs_network_http_request_hash@@@Z
// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x847d0 ?internal_critical_section_leave@@YAXJ@Z

}

template<unsigned int buffer_size>
int sprintf_s(char (&buffer)[buffer_size], const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int result = vsprintf_s(buffer, buffer_size, format, arguments);
    return result;
}

template int sprintf_s<260>(char (&buffer)[260], const char *format, ...);

extern "C" {
// @address 0x82370 _vsnprintf

}

static __forceinline bool network_storage_cache_generate_file_name(char *file_name,const s_network_storage_cache_key *key)
{
 s_network_http_request_hash hash;
 bool result=security_calculate_hash(key->path,strlen(key->path),true,&hash);
 if(result) {
  c_static_string41 name;
  name.s[0]=0;
  for(unsigned int i=0;i<20;i++) name.append_print("%02x",hash.hash[i]);
  strncpy(file_name,name.s,41);
  file_name[40]=0;
 }
 return result;
}

static __forceinline long network_storage_cache_synchronized_get(c_synchronized_long *value)
{
 long result=value->value;
 __lwsync();
 return result;
}
// @address 0x404948 network_storage_cache_update
extern "C" void network_storage_cache_update(c_network_storage_cache *cache)
{
 c_network_storage_cache_add_buffer_scope_lock lock;
 if(cache->m_storage.m_write_handle!=-1) {
  if(network_storage_cache_synchronized_get(&cache->m_storage.m_write_ready)) cache->m_storage.m_write_handle=-1;
 } else if(cache->m_storage.m_dirty) {
  s_network_storage_cache_index *index=(s_network_storage_cache_index *)cache->m_storage.m_write_buffer;
  long *count=&index->entry_count;
  char *out=index->entries;
  *count=0;
  for(unsigned int i=0;i<128;i++) {
   if(cache->m_storage.entries[i].state==1 || cache->m_storage.entries[i].state==3) {
    (*count)++;
    memcpy(out,&cache->m_storage.entries[i].key,
        sizeof(cache->m_storage.entries[i].key) + sizeof(cache->m_storage.entries[i].hash)
            + sizeof(cache->m_storage.entries[i].buffer_size) + sizeof(cache->m_storage.entries[i].file_name));
    out+=148;
   }
  }
  cache->m_storage.m_write_size=out-cache->m_storage.m_write_buffer;
  cache->m_storage.m_write_handle = async_write_buffer_to_file("cache1:\\webcache\\dir_001", cache->m_storage.m_write_buffer,
        cache->m_storage.m_write_size, _async_category_cache_files, _async_priority_blocking_below_sound,
      (void *)5, &cache->m_storage.m_write_overlapped, &cache->m_storage.m_write_ready);
  cache->m_storage.m_dirty=false;
 }
 for(unsigned int i=0;i<128;i++) {
  s_network_storage_cache_entry *entry=&cache->m_storage.entries[i];
  if(entry->state==2 && network_storage_cache_synchronized_get(&entry->async_done)) {
   if(network_storage_cache_synchronized_get(&entry->async_success)) {
    bool ok=text_group_verify_digest(entry->buffer,entry->buffer_size,true,&entry->hash,0);
    entry->buffer=0;
    entry->state=ok!=0;
   } else {
    entry->state=0;
    entry->buffer=0;
   }
   entry->async_task_id=-1;
  } else if(entry->state==3 && network_storage_cache_synchronized_get(&entry->async_done)) {
   entry->async_task_id=-1;
   entry->state=network_storage_cache_synchronized_get(&entry->async_success)!=0;
  } else if(entry->state==4 && network_storage_cache_synchronized_get(&entry->async_done)) {
   entry->state=0;
   entry->async_task_id=-1;
  }
 }
}
// @address 0x404c08 ?add_buffer@c_network_storage_cache@@QAA_NPBUs_network_storage_cache_key@@PBDJPBUs_network_http_request_hash@@PAJ@Z
bool c_network_storage_cache::add_buffer(
    const s_network_storage_cache_key *key, const char *buffer, long buffer_size, const s_network_http_request_hash *hash, long *cookie)
{
 c_network_storage_cache_add_buffer_scope_lock lock;
 bool result=false;
 if(m_initialized && strlen(key->path)<80 && resize_cache(buffer_size)) {
  long index=contains_internal(key,hash);
  if(index!=-1) {
   cancel_io_internal(index);
   result=true;
  } else {
   for(unsigned int i=0;i<128;i++) {
    if(m_storage.entries[i].state==0) {
     index=i;
     break;
    }
   }
   if(index!=-1) {
    strcpy_s(m_storage.entries[index].key.path,80,key->path);
    result=network_storage_cache_generate_file_name(m_storage.entries[index].file_name,key);
   }
  }
  if(result) {
   char path[260];
   sprintf_s(path,"%s\\%s","cache1:\\webcache",m_storage.entries[index].file_name);
   m_storage.entries[index].hash=*hash;
   result=false;
   m_storage.entries[index].buffer_size=buffer_size;
   GetSystemTimeAsFileTime(&m_storage.entries[index].file_time);
   m_storage.entries[index].request_cookie=m_next_request_cookie;
   m_next_request_cookie++;
   m_storage.entries[index].async_task_id=async_write_buffer_to_file(path, buffer, buffer_size, 1, 9,
       (void *)5, &m_storage.entries[index].async_success, &m_storage.entries[index].async_done);
   m_storage.entries[index].state=3;
   if(m_storage.entries[index].async_task_id!=-1) {
    if(cookie) *cookie=m_storage.entries[index].request_cookie;
    result=true;
    m_storage.m_dirty=true;
   }
  }
  if(!result && index!=-1) remove(&m_storage.entries[index].key);
 }
 return result;
}

// @address 0x404eb0 ?contains@c_network_storage_cache@@QAA_NPBUs_network_storage_cache_key@@PBUs_network_http_request_hash@@@Z
bool c_network_storage_cache::contains(const s_network_storage_cache_key *key,const s_network_http_request_hash *hash) {
 bool result=false;
 s_network_storage_cache_scope_lock lock;
 if(m_initialized) result=contains_internal(key,hash)!=-1;
 return result;
}
// @address 0x404f18 ?begin_get_buffer@c_network_storage_cache@@QAA_NPBUs_network_storage_cache_key@@PBUs_network_http_request_hash@@PADJPAJ3@Z
bool c_network_storage_cache::begin_get_buffer(const s_network_storage_cache_key *cache_key, const s_network_http_request_hash *hash,
    char *buffer, long buffer_size, long *file_size_out, long *request_cookie_out)
{
    s_network_storage_cache_scope_lock lock;
    bool result = false;
    if (m_initialized)
    {
        long index = contains_internal(cache_key, hash);
        if (index != -1 && buffer_size >= m_storage.entries[index].buffer_size)
        {
            char path[260];
            sprintf_s(path, "%s\\%s", "cache1:\\webcache", m_storage.entries[index].file_name);
            cancel_io_internal(index);
            m_storage.entries[index].request_cookie = m_next_request_cookie;
            m_next_request_cookie++;
            if (request_cookie_out)
                *request_cookie_out = m_storage.entries[index].request_cookie;
            m_storage.entries[index].async_task_id = async_read_entire_file(path, buffer, buffer_size, (unsigned long *)file_size_out,
                _async_category_online_files, _async_priority_blocking_generic, &m_storage.entries[index].async_success, &m_storage.entries[index].async_done);
            if (m_storage.entries[index].async_task_id != -1)
            {
                m_storage.entries[index].buffer = buffer;
                result = true;
                m_storage.entries[index].state = 2;
                GetSystemTimeAsFileTime(&m_storage.entries[index].file_time);
            }
        }
    }
    return result;
}
// @address 0x1aec40 ?file_reference_create_from_path@@YAPAUs_file_reference@@PAU1@PBD_N@Z
// @address 0x1ad930 ?file_exists@@YA_NPBUs_file_reference@@@Z
// @address 0x8a1ead g_utility_drive_mounted
// @address 0x1adbf8 ?file_open@@YA_NPAUs_file_reference@@KPAK@Z
// @address 0x1ae008 ?file_close@@YA_NPAUs_file_reference@@@Z
// @address 0x1ae220 ?file_read@@YA_NPAUs_file_reference@@K_NPAX@Z
// @address 0x1ae380 ?file_get_size@@YA_NPBUs_file_reference@@PAK@Z

// @address 0x84f90 ?simple_yield_function@@YA_NPAVc_synchronized_long@@@Z
// @address 0x1ef790 ?delete_file_callback@@YA?AW4e_async_completion@@PAUs_async_task@@@Z
static __forceinline void network_storage_cache_synchronized_set(c_synchronized_long *value, long new_value)
{
    __lwsync();
    _InterlockedExchange(&value->value, new_value);
}
static __forceinline long network_storage_cache_async_delete_file(
    const char *file_path, bool is_directory, long priority, c_synchronized_long *success, c_synchronized_long *done)
{
    if (success)
        network_storage_cache_synchronized_set(success, 0);
    s_async_task task;
    memset(&task, 0, sizeof(task));
    strncpy(task.delete_file.path, file_path, sizeof(task.delete_file.path));
    task.delete_file.path[sizeof(task.delete_file.path) - 1] = 0;
    task.delete_file.is_directory = is_directory;
    task.delete_file.success_out = success;
    return ::async_task_add_ex(static_cast<e_async_priority>(priority), &task, _async_category_online_files,
        delete_file_callback, done, true);
}

// @address 0x405030 ?cancel_io_by_cookie@c_network_storage_cache@@QAAXJ@Z
void c_network_storage_cache::cancel_io_by_cookie(long request_cookie)
{
    s_network_storage_cache_scope_lock lock;
    if (m_initialized)
    {
        long index = -1;
        for (unsigned int i = 0; i < 128; i++)
        {
            if (m_storage.entries[i].request_cookie == request_cookie)
            {
                index = i;
                break;
            }
        }
        if (index != -1)
            cancel_io_internal(index);
    }
}

// @address 0x4050b8 ?remove@c_network_storage_cache@@QAA_NPBUs_network_storage_cache_key@@@Z
bool c_network_storage_cache::remove(const s_network_storage_cache_key *key)
{
    s_network_storage_cache_scope_lock lock;
    bool result = false;
    if (m_initialized)
    {
        long index = contains_internal(key, 0);
        result = index != -1;
        if (result)
        {
            cancel_io_internal(index);
            char path[260];
            sprintf_s(path, "%s\\%s", "cache1:\\webcache", m_storage.entries[index].file_name);
            s_file_reference file;
            // match: 0x4050b8: first-byte stb + 271B memset; one memset emits a 272B call
            *(char *)&file = 0;
            memset((char *)&file + 1, 0, sizeof(file) - 1);
            file_reference_create_from_path(&file, path, false);
            m_storage.entries[index].async_task_id = network_storage_cache_async_delete_file(path, false, _async_priority_blocking_generic,
        &m_storage.entries[index].async_success, &m_storage.entries[index].async_done);
            if (m_storage.entries[index].async_task_id != -1)
                m_storage.entries[index].state = 4;
            result = true;
            m_storage.m_dirty = result;
        }
    }
    return result;
}
// @address 0x405210 ?contains_internal@c_network_storage_cache@@AAAJPBUs_network_storage_cache_key@@PBUs_network_http_request_hash@@@Z
long c_network_storage_cache::contains_internal(const s_network_storage_cache_key *key,const s_network_http_request_hash *hash) {
 long result=-1;
 c_static_string<80> key_string(key->path);
 for(unsigned int i=0;i<128;i++) {
  if(m_storage.entries[i].state==1 && key_string.is_equal(m_storage.entries[i].key.path)) {
   if(!hash || memcmp(&m_storage.entries[i].hash,hash,sizeof(*hash))==0) result=i;
  }
 }
 return result;
}
// @address 0x4052c0 ?cancel_io_internal@c_network_storage_cache@@AAAXJ@Z
void c_network_storage_cache::cancel_io_internal(long task_index)
{
    if (m_storage.entries[task_index].async_task_id != -1)
    {
        async_block_until_done(&m_storage.entries[task_index].async_done, simple_yield_function, true, false, 0, 0);
        m_storage.entries[task_index].async_task_id = -1;
        if (m_storage.entries[task_index].state == 2)
        {
            m_storage.entries[task_index].load_size = 0;
        }
    }
    m_storage.entries[task_index].state = 0;
}

// @address 0x405a18 ?append_print@c_static_string41@@QAAPBDPBDZZ
inline const char *c_static_string41::append_print(const char *format, ...)
{
  long current_length = this->length();
  long remaining = 41 - current_length;
  char *end = this->s + current_length;
  va_list args;
  va_start(args, format);
  _vsnprintf(end, remaining - 1, format, args);
  end[remaining - 1] = 0;
  return this->s;
}


// @address 0x405358 ?resize_cache@c_network_storage_cache@@AAA_NJ@Z
bool c_network_storage_cache::resize_cache(long requested_size)
{
    bool result = true;
    long total_size;
    unsigned __int64 oldest_time;
    do {
        total_size = requested_size;
        long i = 0;
        do {
            if (m_storage.entries[i].state)
                total_size += m_storage.entries[i].buffer_size;
        } while ((unsigned long)++i < 128);
        if (total_size > m_maximum_size) {
            long oldest_index = -1;
            long i = 0;
            do {
                if (m_storage.entries[i].state) {
                    unsigned __int64 entry_time = ((unsigned __int64)m_storage.entries[i].file_time.dwHighDateTime << 32)
                        | m_storage.entries[i].file_time.dwLowDateTime;
                    if (i == 0 || entry_time < oldest_time) {
                        oldest_time = entry_time;
                        oldest_index = i;
                    }
                }
            } while (128 > (unsigned long)++i);
            if (oldest_index == -1)
                return true;
            char path[260];
            sprintf_s(path, "%s\\%s", "cache1:\\webcache", m_storage.entries[oldest_index].file_name);
            s_file_reference file;
            // match: 0x405358: first-byte stb + 271B memset; one memset emits a 272B call
            *(char *)&file = 0;
            memset((char *)&file + 1, 0, sizeof(file) - 1);
            memset(&file, 0, sizeof(file));
            file.signature = 0x66696c6f;
            file.location = -1;
            file.handle = (void *)-1;
            if (file.flags & 1)
                ::file_path_remove_name((char *)(file.path));
            file_path_add_name(file.path, 256, path);
            file.flags |= 1;
            result = ::file_delete(&file);
            if (!result)
                break;
            m_storage.entries[oldest_index].state = 0;
        }
    } while (total_size > m_maximum_size);
    return result;
}

static __forceinline bool network_storage_cache_valid_hex_filename_string(const char *filename,long buffer_length) {
 bool result=true;
 long i=0; if(buffer_length>1) do { result &= (filename[i]>='0' && filename[i]<='9') || (filename[i]>='a' && filename[i]<='f'); } while(++i<buffer_length-1);
 result &= filename[buffer_length-1]==0;
 return result;
}
static __forceinline long network_storage_cache_name_length(const char *text) {
 const char *s=text; unsigned long length=0; while(length<41 && *s++) ++length; return length;
}
struct s_find_file_data;
struct s_file_last_modification_date;
static __forceinline bool network_storage_cache_cache_entries_filename_valid(const char *filename) {
 bool result=true;
 const char *p=filename+1;
 for(long i=0;i<8;i++) {
  for(long j=-1;j<4;j++)
   result &= (p[j]>='0' && p[j]<='9') || (p[j]>='a' && p[j]<='f');
  p+=5;
 }
 result &= filename[40]==0;
 return result;
}
// @address 0x4054e8 ?read_cache_entries@c_network_storage_cache@@AAA_NXZ
bool c_network_storage_cache::read_cache_entries()
{
    bool result = false;
    long count;
    for (long i = 0; i < 128; i++)
        clear_entry(i);

    s_file_reference file;
    // match: 0x4054e8: first-byte stb + 271B memset; one memset emits a 272B call
    *(char *)&file = 0;
    memset((char *)&file + 1, 0, sizeof(file) - 1);
    file_reference_create_from_path(&file, "cache1:\\webcache\\dir_001", false);
    if (file_exists(&file))
    {
        if (file_open(&file, 1, (unsigned long *)&count))
        {
            if (file_get_size(&file, (unsigned long *)&count) && (unsigned long)count >= 4)
            {
                unsigned long file_size = count;
                count = 0;
                if (file_read(&file, 4, true, &count)
                    && count <= 128 && (unsigned long)(count * 148 + 4) <= file_size)
                {
                    result = true;
                    for (long i = 0; i < count; i++)
                    {
                        m_storage.entries[i].state = 0;
                        if (!file_read(&file, 148, true, &m_storage.entries[i].key))
                        {
                            result = false;
                            break;
                        }
                        m_storage.entries[i].key.path[79] = 0;
                        m_storage.entries[i].file_name[40] = 0;
                        if (!network_storage_cache_cache_entries_filename_valid(m_storage.entries[i].file_name))
                        {
                            result = false;
                            break;
                        }
                        m_storage.entries[i].state = 1;
                    }
                }
            }
            file_close(&file);
        }
    }
    return result;
}
// @address 0x4057c8 ?clean_cache_directory@c_network_storage_cache@@AAAXXZ
void c_network_storage_cache::clean_cache_directory()
{
    s_file_reference directory;
    file_reference_create_from_path(&directory, "cache1:\\webcache", true);
    s_find_file_data find;
    find_files_start(&find, 0, &directory, "*.*");
    s_file_reference file;
    while (find_files_next(&find, &file, 0)) {
        bool keep = true;
        c_static_string<41> name;
        char filename[256];
        file_reference_get_name(&file, 12, filename, 256);
        name.set(filename);
        if (name.is_equal("dir_001")) keep = true;
        else {
            keep &= strlen(filename) == network_storage_cache_name_length(name.get_string());
            keep &= network_storage_cache_valid_hex_filename_string(name.get_string(), 41);
            if (keep) {
                keep = false;
                for (unsigned long i = 0; i < 128; i++) {
                    if (m_storage.entries[i].state && name.is_equal(m_storage.entries[i].file_name)) { keep = true; break; }
                }
            }
        }
        if (!keep) file_delete(&file);
    }
    find_files_end(&find);
}


// @address 0x404620 network_storage_cache_reset_manifest
extern "C" void network_storage_cache_reset_manifest(c_network_storage_manifest *manifest_state)
{
    c_network_storage_manifest *manifest = manifest_state;
    s_network_storage_cache_url url;
    url.name.get_buffer()[0] = 0;
    url.service_type = 0;
    url.cache_mode = 0;
    url.lifetime_seconds = 600;
    url.queue_type = 0;
    online_url_set_base_path(&url);
    url.name.append_print("manifest_%03u.bin", 1);
    url.cache_mode = 0;
    url.queue_type = 0;
    manifest->m_expiration_time = 0;
    http_buffer_downloader_set_url(manifest->m_downloader.buffer_downloader(), &url);
    manifest->m_downloader.reset();
    manifest->m_manifest = 0;
    manifest->m_failed = false;
}
// @address 0x404800 ?initialize@c_network_storage_cache@@QAA_NJ@Z
bool c_network_storage_cache::initialize(long maximum_size)
{
    c_network_storage_cache_add_buffer_scope_lock lock;
    for (long i = 0; i < 128; i++) {
        m_storage.entries[i].async_task_id = -1;
        m_storage.entries[i].state = 0;
    }
    bool result = g_utility_drive_mounted;
    m_maximum_size = 0x3200000;
    if (result) {
        s_file_reference file;
        // match: 0x404800: first-byte stb + 271B memset; one memset emits a 272B call
        *(char *)&file = 0;
        memset((char *)&file + 1, 0, sizeof(file) - 1);
        file_reference_create_from_path(&file, "cache1:\\webcache", true);
        if (!file_exists(&file))
            result = file_create_parent_directories_if_not_present(&file);
        if (result) {
            result = read_cache_entries();
            if (result)
                clean_cache_directory();
            else if (file_delete_recursive(&file))
                result = file_create(&file);
            m_initialized = result;
        }
    }
    return result;
}


struct s_nsc_bitmap_item;
// @address 0x4ef9c8 gui_custom_bitmap_storage_decode_image
// @address 0x4a64e8 network_storage_cache_load_bitmap_slot
extern "C" bool network_storage_cache_load_bitmap_slot(
    s_gui_custom_bitmap_storage_globals *context, long index, const void *image_data, unsigned long image_size, long aspect_index)
{
    bool result = false;
    internal_critical_section_enter(20);
    s_data_array *array = context->data;
    gui_custom_bitmap_storage_slot *slot = 0;
    if (index != -1 && (index & 0xffff) < array->first_unallocated)
    {
        gui_custom_bitmap_storage_slot *candidate = (gui_custom_bitmap_storage_slot *)(array->data + array->size * (index & 0xffff));
        if (candidate->identifier != 0 && candidate->identifier == (int)index >> 16)
        {
            slot = candidate;
            slot->state = 1;
        }
    }
    internal_critical_section_leave(20);
    if (slot != 0)
    {
        long limit = context->buffer_size;
        void *base = context->buffer;
        result = gui_custom_bitmap_storage_decode_image((s_nsc_bitmap_item *)&slot->item, 0, image_data, image_size, base, limit, aspect_index);
        internal_critical_section_enter(20);
        slot->state = result ? 2 : 0;
        internal_critical_section_leave(20);
    }
    return result;
}

struct s_data_array;

// @address 0x492ba8 network_storage_cache_handle_bitmap_load_task
extern "C" int network_storage_cache_handle_bitmap_load_task(s_network_storage_bitmap_load_task *task)
{
    if (network_storage_cache_load_bitmap_slot(
            &g_gui_custom_bitmap_storage_data, task->bitmap_index, task->image_data, task->image_size,
        task->aspect_index))
        network_storage_cache_synchronized_set(task->success, 1);
    return 1;
}

// @address 0x3ada60 ?network_blf_find_chunk_in_file@@YAPAXPBDKJFW4e_blf_file_authentication_type@@PAK@Z


// @address 0x492960 network_storage_cache_load_bitmap_file
extern "C" e_async_completion network_storage_cache_load_bitmap_file(s_cache_load_operation *operation)
{
    bool progress = false;
    bool complete = false;
    long cancelled = *operation->cancelled;
    __lwsync();
    bool abort = cancelled != 0;
    if (operation->state == 0) {
        if (!abort) {
            char path[256];
            operation->mounted = content_catalogue_make_ready_for_path(file_reference_get_name(operation->file, 13, path, 256), true);
            unsigned long error;
            if (file_open(operation->file, 1, &error)) {
                if (file_get_size(operation->file, &operation->file_size) && operation->file_size < operation->capacity) {
                    progress = true;
                    operation->state = 1;
                }
                if (!progress)
                    file_close(operation->file);
            }
        }
    } else if (operation->state == 1) {
        if (!abort && file_read(operation->file, operation->file_size, true, operation->buffer)) {
            file_close(operation->file);
            progress = true;
            operation->state = 2;
        }
        if (!progress)
            file_close(operation->file);
    } else if (operation->state == 2) {
        if (!abort) {
            unsigned long chunk_size = 0;
            s_cache_load_chunk *chunk = static_cast<s_cache_load_chunk *>(network_blf_find_chunk_in_file(
                operation->buffer, operation->file_size, 0x6d617069, 1, _blf_file_authentication_signature, &chunk_size));
#if !defined(_XBOX) || defined(__clang__)
            // the chunk payload's version and size are big-endian file data, which the Xbox loads natively;
            // a little-endian host must swap them (0x59c2 would read as 0xc2590000 and reject the image)
            const long chunk_version = chunk ? (long)byte_swap_long((unsigned long)chunk->version) : 0;
            const long chunk_data_size = chunk ? (long)byte_swap_long((unsigned long)chunk->size) : 0;
            if (chunk && (long)chunk_size > 8 && chunk_data_size == (long)chunk_size - 8 &&
                chunk_version >= 0 && chunk_version < 2 &&
                network_storage_cache_load_bitmap_slot(&g_gui_custom_bitmap_storage_data, operation->destination,
                    chunk->data, chunk_data_size, operation->context))
#else
            if (chunk && (long)chunk_size > 8 && chunk->size == (long)chunk_size - 8 &&
                chunk->version >= 0 && chunk->version < 2 &&
                network_storage_cache_load_bitmap_slot(&g_gui_custom_bitmap_storage_data, operation->destination,
                    chunk->data, chunk->size, operation->context))
#endif
            {
                progress = true;
                complete = true;
                operation->state = 3;
            }
        }
    }
    long *completed = operation->completed;
    __lwsync();
    _InterlockedExchange(completed, complete);
    e_async_completion result;
    if (progress && !complete)
        result = _async_completion_pending;
    else {
        result = _async_completion_done;
        if (operation->mounted)
            content_catalogue_update_active_map_items(true);
    }
    return result;
}
