#ifndef TAG_FILES_TAG_RESOURCE_CACHE_CONTROL_H
#define TAG_FILES_TAG_RESOURCE_CACHE_CONTROL_H

#include "memory/wrapped_arrays.h"
#include "memory/allocation_description.h"

#include "memory/contiguous_allocation.h"

// Functions defined in tu/blamlibxbox/tag_resource_cache_control.cpp.

class c_tag_resource_address_cache_control_interface;
class c_tag_resource_cache_controller;
class c_tag_resource_runtime_active_set;

extern "C" void tag_resource_cache_update_resource_address(c_tag_resource_cache_controller *controller, bool required, long resource_handle,
    bool *fixed_up, c_tag_resource_address_cache_control_interface *address_cache_control);
extern "C" void tag_resource_cache_update_resources_from_active_set(c_tag_resource_cache_controller *controller, c_tag_resource_runtime_active_set *active_set,
    c_tag_resource_address_cache_control_interface *address_cache_control, bool *changed);

template<typename t_type> class c_basic_buffer;
struct s_tag_resource_fixup_setup;
void tag_resource_fixup_cache_data_references(s_tag_resource_fixup_setup *setup, c_basic_buffer<void> *optional_data);

#endif // TAG_FILES_TAG_RESOURCE_CACHE_CONTROL_H
