#ifndef EFFECTS_LIGHT_VOLUMES_GLOBALS_H
#define EFFECTS_LIGHT_VOLUMES_GLOBALS_H

#include "effects/effect_runtime_storage.h"

// Storage and address annotations belong to light_volumes.cpp.
extern "C" {
extern bool g_light_volume_location_allocation_succeeded[];
extern __declspec(thread) s_data_array *g_light_volume_location_data;
extern char g_light_volume_system_allocation_succeeded[];
extern bool g_light_volume_allocation_succeeded[];
}

#endif // EFFECTS_LIGHT_VOLUMES_GLOBALS_H
