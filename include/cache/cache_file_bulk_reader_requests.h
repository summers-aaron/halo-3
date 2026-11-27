#ifndef CACHE_CACHE_FILE_BULK_READER_REQUESTS_H
#define CACHE_CACHE_FILE_BULK_READER_REQUESTS_H

struct s_sound_cache_hash_state;
struct s_cache_file_bulk_reader_slot_set;


extern "C" void cache_file_bulk_reader_update_reads(s_sound_cache_hash_state *, void *);
extern "C" long cache_file_tag_resource_find_ready_slot(const s_cache_file_bulk_reader_slot_set *);

#endif // CACHE_CACHE_FILE_BULK_READER_REQUESTS_H
