#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_DAMAGE_OPERATIONS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_DAMAGE_OPERATIONS_H

struct s_damage_owner;
union real_point3d;
struct s_breakable_surfaces_event_material;
extern "C" void object_set_shield_depleted(long object_index, bool depleted, const s_damage_owner *owner);
extern "C" void breakable_surface_break_from_record(long bsp_index, long instance_index, long surface_index, const real_point3d *position,
    const s_breakable_surfaces_event_material *material);

#endif
