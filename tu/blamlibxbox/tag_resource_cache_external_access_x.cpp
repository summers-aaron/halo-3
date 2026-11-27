// blamlibxbox/tag_resource_cache_external_access_x.obj: from the records 0x21d1c8
// @compiler xdk5632
// @flags /TP /O2 /Oc /noGL
// @address 0x21d1c8 tag_resource_cache_external_access_track_resource
#include <stddef.h>
#include <stdlib.h>
#include "cache/tag_resource_cache_address_cache.h"
#include <ppcintrinsics.h>
#include "cache/tag_resource_page_datum.h"
#include "game/tag_resource_cache_external_access_page_slot_types.h"
#include "tag_files/tag_resource_cache_external_access.h"
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_cache_new.h"

static __forceinline long tag_resource_cache_external_access_make_internal_handle(long resource, bool optional)
{
    int hi = resource >> 16;
    int low = (int)(_rotl((unsigned int)resource, 1) & 0x1fffe);
    unsigned short salt = (unsigned short)~hi;
    long base = (salt << 16) & 0x7fff0000;
    if (optional)
        return (0x80000000 | base) | low;
    return base | (low + 1);
}
static __forceinline void *tag_resource_cache_external_access_get_permanent_resource_root_address(
    c_tag_resource_cache_controller *controller, long handle)
{
    int hi = handle >> 16;
    bool optional = ((unsigned short)(handle >> 16) & 0x8000) != 0;
    c_tag_resource_cache_file_datum_handler *handler = controller->m_file_datum_handler;
    unsigned short salt = (unsigned short)~hi;
    long external = (0x80000000 | ((salt << 16) & 0x7fff0000)) | ((((unsigned long)handle >> 1)) & 0x7fff);
    return handler->get_permanent_resource_root_address(external, optional);
}
static __forceinline void tag_resource_cache_external_access_add_page_reference(c_tag_resource_cache_new *cache, long page_index, long handle)
{
    ((s_tag_resource_page_datum *)cache->m_page_table.m_pages->data)[(unsigned short)page_index].io_lock_count++;
    s_tag_resource_cache_external_access_page_slot *slot =
        &((s_tag_resource_cache_external_access_page_slot *)cache->m_inverse_page_table.table->data)[(unsigned short)page_index];
    cache->m_inverse_page_table.list[slot->m_read + slot->m_write] = handle;
    ++slot->m_write;
}
extern "C" void tag_resource_cache_external_access_track_resource(c_tag_resource_cache_new *cache, long resource)
{
    c_tag_resource_cache_controller *controller = &cache->m_controller;
    long handle = tag_resource_cache_external_access_make_internal_handle(resource, false);
    controller->begin_tracking_resource(handle, false);
    if (cache->m_resource_interface->any_pageable_data(resource)) {
        tag_resource_cache_external_access_add_page_reference(cache, cache->m_resource_interface->get_required_page_index(resource), handle);
    }
    void *address = tag_resource_cache_external_access_get_permanent_resource_root_address(controller, handle);
    c_tag_resource_address_cache *addresses = &cache->m_resource_address_cache;
    addresses->initialize_resource_address(handle, address);
    if (cache->m_resource_interface->any_optional_data(resource)) {
        long optional_handle = tag_resource_cache_external_access_make_internal_handle(resource, true);
        controller->begin_tracking_resource(optional_handle, false);
        tag_resource_cache_external_access_add_page_reference(cache, cache->m_resource_interface->get_optional_page_index(resource), optional_handle);
        void *optional_address = tag_resource_cache_external_access_get_permanent_resource_root_address(controller, optional_handle);
        addresses->initialize_resource_address(optional_handle, optional_address);
    }
}
