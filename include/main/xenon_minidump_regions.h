#ifndef MAIN_XENON_MINIDUMP_REGIONS_H
#define MAIN_XENON_MINIDUMP_REGIONS_H

extern "C" { bool xenon_minidump_get_game_state_region(void *); }
extern "C" { bool network_shared_memory_get_bounds(void *); }
extern "C" { bool xenon_minidump_get_geometry_data(void *); }
extern "C" { bool cache_file_try_and_get_memory_region(void *); }

#endif // MAIN_XENON_MINIDUMP_REGIONS_H
