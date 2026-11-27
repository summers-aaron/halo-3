#ifndef CACHE_TAG_RESOURCE_CACHE_FILE_DATUM_HANDLER_H
#define CACHE_TAG_RESOURCE_CACHE_FILE_DATUM_HANDLER_H

#include "memory/basic_buffer.h"
#include "memory/wrapped_arrays.h"
class c_tag_resource_fixup;
struct s_tag_resource_fixup_location;
struct s_tag_resource_interop_location;

// March method names, ordered by the shipping datum-handler vtable at 0x1b88.
// Includes page-index and offset queries for pageable and optional data.
class c_tag_resource_cache_file_datum_handler
{
public:
    virtual c_basic_buffer<void> get_control_data(long resource_handle, bool optional) = 0;
    virtual c_tag_resource_fixup get_root_address_fixup(long resource_handle) = 0;
    virtual bool is_streamed(long resource_handle) = 0;
    virtual void *get_permanent_resource_root_address(long resource_handle, bool optional);
    virtual long get_resource_definition(long resource_handle) = 0;
    virtual long get_resource_owner(long resource_handle) = 0;
    virtual c_wrapped_array<const s_tag_resource_fixup_location> get_control_fixups(long resource_handle) = 0;
    virtual c_basic_buffer<void> get_interop_buffer(long resource_handle, bool optional) = 0;
    virtual c_wrapped_array<const s_tag_resource_interop_location> get_interop_locations(long resource_handle) = 0;
    virtual bool any_pageable_data(long resource_handle) = 0;
    virtual bool any_optional_data(long resource_handle) = 0;
    virtual long get_required_page_index(long resource_handle) = 0;
    virtual long get_optional_page_index(long resource_handle) = 0;
    virtual unsigned long get_pageable_offset(long resource_handle) = 0;
    virtual unsigned long get_optional_offset(long resource_handle) = 0;
#if !defined(_XBOX) || defined(__clang__)
    virtual bool is_optional(long resource_handle) = 0;
#endif
};

#endif
