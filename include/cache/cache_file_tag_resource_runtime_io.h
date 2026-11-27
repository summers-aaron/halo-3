#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_IO_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_IO_H

#include "cseries/async_types.h"
#include "cache/cache_files_windows_async.h"

class c_cache_file_tag_resource_runtime_manager_storage;
struct s_sound_cache_hash_state;
class c_sound_cache_reader;

extern "C" void cache_file_bulk_reader_update_reads(s_sound_cache_hash_state *state, c_sound_cache_reader *reader);
extern "C" void cache_files_windows_update_load_status(c_cache_file_tag_resource_runtime_manager_storage *manager, short progress, const char *path);

#endif // CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_IO_H
