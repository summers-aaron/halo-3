#ifndef GAME_CACHED_RESOURCE_STATE_TYPES_H
#define GAME_CACHED_RESOURCE_STATE_TYPES_H

#include <stddef.h>

struct s_tag_resource_location_handle_struct;

struct s_cached_resource_state
{
    long handle;
    void *required_address;
    void *optional_address;
    s_tag_resource_location_handle_struct *required_page_handle;
    s_tag_resource_location_handle_struct *optional_page_handle;
};

typedef char cached_resource_state_page_handles_layout[(sizeof(void *) != 4 ||
    (offsetof(s_cached_resource_state, required_page_handle) == 0xc &&
     offsetof(s_cached_resource_state, optional_page_handle) == 0x10 &&
     sizeof(s_cached_resource_state) == 0x14)) ? 1 : -1];

#endif // GAME_CACHED_RESOURCE_STATE_TYPES_H
