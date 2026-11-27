#ifndef CACHE_TAG_RESOURCE_RUNTIME_ACCESS_H
#define CACHE_TAG_RESOURCE_RUNTIME_ACCESS_H

#include "cache/cache_file_tag_resource_runtime.h"

struct s_tag_resource_cache_external_access_state;
extern "C" void tag_resource_cache_reset_state(s_tag_resource_cache_external_access_state *state);
extern "C" void tag_resource_cache_new_handle_space_demand(c_tag_resource_cache_new *cache, unsigned long size);
extern "C" void tag_resource_cache_free_block(s_cache_file_tag_resource_slots *slots, long slot_index, void *resource);

#endif // CACHE_TAG_RESOURCE_RUNTIME_ACCESS_H
