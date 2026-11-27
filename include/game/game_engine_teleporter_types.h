#ifndef GAME_GAME_ENGINE_TELEPORTER_TYPES_H
#define GAME_GAME_ENGINE_TELEPORTER_TYPES_H

#include "game/game_engine_teleporters.h"
#include "game/game_engine_area_set_types.h"

class c_teleporter_area : public c_area
{
    friend float teleporter_is_channel_linked(long object_index);
    friend bool teleporter_are_points_linked(const real_point3d *sender_point, const real_point3d *receiver_point);
    friend void teleporter_compute_destination(c_teleporter_area *sender, c_teleporter_area *receiver, long object_index, real_point3d *destination);
    friend unsigned char teleporter_teleport_object(c_teleporter_area *area, c_teleporter_area *receiver, long object_index);
    friend unsigned char teleporter_area_is_blocked(c_teleporter_area *area, long unused);
public:
    long update_countdown;
    long refresh_countdown;
    unsigned char active;
    __forceinline const s_multiplayer_object_properties *get_multiplayer_properties() const;
    __forceinline bool can_teleport_players() const
    {
        return !get_multiplayer_properties()->passability.test(_multiplayer_object_boundary_passability_disallow_players);
    }
    __forceinline bool can_teleport_light_land_vehicles() const
    {
        return get_multiplayer_properties()->passability.test(_multiplayer_object_boundary_passability_light_land_vehicles);
    }
    __forceinline bool can_teleport_heavy_land_vehicles() const
    {
        return get_multiplayer_properties()->passability.test(_multiplayer_object_boundary_passability_heavy_land_vehicles);
    }
    __forceinline bool can_teleport_flying_vehicles() const
    {
        return get_multiplayer_properties()->passability.test(_multiplayer_object_boundary_passability_flying_vehicles);
    }
    __forceinline bool can_teleport_projectiles() const
    {
        return get_multiplayer_properties()->passability.test(_multiplayer_object_boundary_passability_projectiles);
    }
};

typedef char teleporter_area_size_check[(sizeof(c_teleporter_area) == 0x70) ? 1 : -1];

#endif
