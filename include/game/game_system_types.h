#ifndef GAME_GAME_SYSTEM_TYPES_H
#define GAME_GAME_SYSTEM_TYPES_H
#include <stddef.h>
struct s_game_cluster_bit_vectors;
class c_scenario_resource_registry;
// The designer and cinematic zone masks that make up a non-bsp zone set; scenario.cpp
// builds one on the stack and passes its address to fields 9, 10 and 11.
struct s_game_non_bsp_zone_set
{
    unsigned long designer_zone_mask;
    unsigned long cinematic_zone_mask;
};
// Shared 0x30-byte dispatch table, read by game.cpp and main_game.cpp.
struct s_game_system
{
    void (*initialize_proc)(void);
    void (*dispose_proc)(void);
    void (*initialize_for_new_map_proc)(void);
    void (*dispose_from_old_map_proc)(void);
    void (*prepare_for_new_zone_set)(unsigned long, unsigned long);
    void (*initialize_for_new_structure_bsp)(unsigned long);
    void (*dispose_from_old_structure_bsp_proc)(unsigned long);
    void (*change_pvs_proc)(const s_game_cluster_bit_vectors *, const s_game_cluster_bit_vectors *, bool);
    void (*change_activation_proc)(const s_game_cluster_bit_vectors *, const s_game_cluster_bit_vectors *);
    void (*prepare_for_designer_zone_set_switch)(const s_game_non_bsp_zone_set *, const s_game_non_bsp_zone_set *, c_scenario_resource_registry *);
    void (*initialize_for_new_non_bsp_zone_set)(const s_game_non_bsp_zone_set *);
    void (*dispose_from_old_non_bsp_zone_set)(const s_game_non_bsp_zone_set *);
};
typedef char game_system_size[(sizeof(void *) != 4 || ((sizeof(s_game_system) == 0x30))) ? 1 : -1];
typedef char game_system_dispose_bsp_offset[(sizeof(void *) != 4 || ((offsetof(s_game_system, dispose_from_old_structure_bsp_proc) == 0x18))) ? 1 : -1];
typedef char game_system_pvs_offset[(sizeof(void *) != 4 || ((offsetof(s_game_system, change_pvs_proc) == 0x1c))) ? 1 : -1];
extern "C" const s_game_system g_game_systems[92];
#endif
