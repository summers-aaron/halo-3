#ifndef CACHE_TAG_RESOURCE_CACHE_EXTERNAL_ACCESS_DEPENDENCIES_H
#define CACHE_TAG_RESOURCE_CACHE_EXTERNAL_ACCESS_DEPENDENCIES_H

class c_tag_resource_cache_new;
class c_tag_resource_error_receiver;
extern "C" void tag_resource_cache_new_commit_dirty(c_tag_resource_cache_new *cache);
extern "C" void tag_resource_cache_new_update_zones(c_tag_resource_cache_new *cache, long unused, bool synchronous,
    c_tag_resource_error_receiver *flag_receiver);
extern "C" void tag_resource_cache_new_update(c_tag_resource_cache_new *cache);
extern "C" void tag_resource_cache_new_update_pages(c_tag_resource_cache_new *cache, bool force);
extern "C" void tag_resource_cache_new_handle_space_demand(c_tag_resource_cache_new *cache, unsigned long size);

#endif // CACHE_TAG_RESOURCE_CACHE_EXTERNAL_ACCESS_DEPENDENCIES_H
