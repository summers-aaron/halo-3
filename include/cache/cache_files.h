#ifndef CACHE_CACHE_FILES_H
#define CACHE_CACHE_FILES_H

#include "cache/cache_file_ancestor_list_types.h"

// Functions defined in tu/blamlibxbox/cache_files.cpp.

struct s_cache_file_header;

void *tag_try_and_get_unsafe(unsigned long group_tag, long tag_index);
bool cache_file_blocking_read(long file, long offset, long size, void *destination);
bool cache_file_header_verify(const s_cache_file_header *game_state_header, const char *file_name, bool p2);
bool scenario_tags_load(const char *scenario_path);
extern "C" int cache_file_compare_digests(const unsigned char *a, unsigned int size, const unsigned char *b, unsigned char full);
extern "C" unsigned char cache_file_try_and_get_map_digest(unsigned long *digest_size, const void **digest_data);
extern "C" void scenario_tags_unload(void);
extern "C" float cache_file_get_load_progress(int scenario_type, short map_index, const char *path);
extern "C" int tag_group_get_ancestor_depth(int initial_group_tag, unsigned int ancestor_group_tag);

struct tag_iterator;

long tag_iterator_next(tag_iterator *iterator);

struct cache_file_tag_instance;
struct s_cache_file_globals;

extern "C" int tag_group_get_min_ancestor_depth(int initial_group_tag, s_cache_file_ancestor_list ancestors);

// Data defined in tu/blamlibxbox/cache_files.cpp that other units use.
extern const cache_file_tag_instance *global_tag_instances;
extern s_cache_file_globals g_cache_file_globals;
extern "C" const char *g_maps_directory;

inline const char *cache_files_map_directory()
{
    g_maps_directory = "d:\\maps\\";
    return g_maps_directory;
}

#endif // CACHE_CACHE_FILES_H
