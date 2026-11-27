#ifndef CACHE_TAG_RESOURCE_RUNTIME_INTERFACES_H
#define CACHE_TAG_RESOURCE_RUNTIME_INTERFACES_H


// March runtime-manager listener vtable at 0x820c93d4.
class c_tag_resource_runtime_listener
{
public:
    virtual bool register_resource(long owner, long resource_type, void *root) = 0;
    virtual void unregister_resource(long owner, long resource_type, void *root) = 0;
};

#if defined(_XBOX) && !defined(__clang__)
class c_tag_resource_runtime_active_set
#else
#include "cache/tag_resource_cache_loaded_query.h"
// The cache also uses this interface's first slot as its loaded query.
class c_tag_resource_runtime_active_set : public c_tag_resource_cache_new_loaded_query
#endif
{
public:
#if defined(_XBOX) && !defined(__clang__)
    virtual bool any_resources_active() const = 0;
#endif
    virtual bool is_resource_required(long owner, long resource_handle) const = 0;
    virtual bool is_resource_deferred(long owner, long resource_handle) const = 0;
    virtual bool is_resource_unneeded(long owner, long resource_handle) const = 0;
};

#endif
