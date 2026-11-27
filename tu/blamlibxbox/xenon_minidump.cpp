// blamlibxbox/xenon_minidump.obj: from the records 0x219a80 0x219b68
// @flags /TP /O2
#include "memory/static_arrays.h"
#include <stdlib.h>
#include <string.h>
#include "main/main.h"
#include "simulation/simulation.h"
#include "cseries/xenon_minidump.h"

#include "cache/geometry_globals.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
// @address 0x74b898 g_geometry_globals


#include "memory/game_state_globals.h"

// @address 0x219a80 xenon_minidump_get_game_state_region
bool xenon_minidump_get_game_state_region(s_xenon_minidump_memory_region *region)
{
    unsigned long lock;
    __try
    {
        lock = ::_internal_halt_render_thread_and_lock_resources("..\\cseries\\xenon_minidump.cpp", 260);
        region->size = 0xfe0000;
        region->base_address = game_state_globals.base_address;
    }
    __finally
    {
        unlock_resources_and_resume_render_thread(lock);
    }
    return true;
}

// @address 0x219b68 xenon_minidump_get_geometry_data
bool xenon_minidump_get_geometry_data(xenon_minidump_geometry_buffer *out)
{
    bool result = false;
    bool has_data = false;
    c_basic_buffer<void> data(c_basic_buffer<void>::_uninitialized);

    c_cache_file_tag_resource_runtime_manager *manager = g_geometry_globals.manager;
    if (manager != NULL)
    {
        has_data = true;
        data = manager->m_listener_storage.m_second;
        out->address = data.m_address;
        out->size = data.m_size;
    }
    if (has_data)
        result = true;
    return result;
}
