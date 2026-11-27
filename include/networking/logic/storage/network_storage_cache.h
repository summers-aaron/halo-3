#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_CACHE_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_CACHE_H

// Functions defined in tu/networkinglibxbox/network_storage_cache.cpp that other units call.

struct s_network_storage_bitmap_load_task;
class c_network_storage_manifest;

extern "C" void network_storage_cache_reset_manifest(c_network_storage_manifest *manifest);
extern "C" int network_storage_cache_handle_bitmap_load_task(s_network_storage_bitmap_load_task *task);

#include <xtl.h>
#include "cseries/async_types.h"
#include "multithreading/synchronized_value.h"
#include "networking/logic/storage/network_storage_cache_key_types.h"
#include "networking/logic/storage/network_http_request_hash.h"

struct s_network_storage_cache_entry {
 int state;
 long request_cookie;
 long async_task_id;
 c_synchronized_long async_success;
 c_synchronized_long async_done;
 FILETIME file_time;
 union { long load_size; char *buffer; };
 s_network_storage_cache_key key;
 s_network_http_request_hash hash;
 long buffer_size;
 char file_name[44];
};


struct c_network_storage_cache {
 friend class c_http_buffer_downloader;
 bool m_initialized;
 long m_next_request_cookie;
 long m_maximum_size;
 struct s_storage {
  // match: 0x709ce8: aggregate header {false, 1, 0} + nested s_storage ctor matches static .data and 0x709ce8.
  __forceinline s_storage() : m_dirty(false), m_write_handle(-1) {}
  s_network_storage_cache_entry entries[128];
  bool m_dirty;
  char m_write_buffer[0xa410 - 0x5a0d];
  long m_write_size;
  long m_write_handle;
  c_synchronized_long m_write_ready;
  c_synchronized_long m_write_overlapped;
 } m_storage;
 bool add_buffer(const s_network_storage_cache_key *key,const char *buffer,long buffer_size,const s_network_http_request_hash *hash,long *cookie);
 bool contains(const s_network_storage_cache_key *key,const s_network_http_request_hash *hash);
 bool begin_get_buffer(const s_network_storage_cache_key *cache_key, const s_network_http_request_hash *hash, char *buffer,
     long buffer_size, long *file_size_out, long *request_cookie_out);
 bool remove(const s_network_storage_cache_key *key);
 void cancel_io_by_cookie(long request_cookie);
 bool initialize(long maximum_size);
private:
 long contains_internal(const s_network_storage_cache_key *key, const s_network_http_request_hash *hash);
 void cancel_io_internal(long index);
 bool resize_cache(long requested_size);
 void clear_entry(long index) { m_storage.entries[index].state = 0; }
 bool read_cache_entries();
 void clean_cache_directory();
public:
};

extern "C" {
void network_storage_cache_update(c_network_storage_cache *cache);
}

extern c_network_storage_cache g_network_storage_cache;

struct s_cache_load_operation;
extern "C" e_async_completion network_storage_cache_load_bitmap_file(s_cache_load_operation *operation);

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_CACHE_H
