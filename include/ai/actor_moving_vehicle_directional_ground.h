#ifndef AI_ACTOR_MOVING_VEHICLE_DIRECTIONAL_GROUND_H
#define AI_ACTOR_MOVING_VEHICLE_DIRECTIONAL_GROUND_H

// Functions defined in tu/ailibxbox/actor_moving_vehicle_directional_ground.cpp that other units call.

struct character_vehicle_properties;
struct obstacles;
union real_point3d;
union real_vector3d;
struct turn_info;
struct unit_datum;

bool test_turn(long actor_index, turn_info *turn, unit_datum *unit, character_vehicle_properties *properties,
    real_point3d *position, real_vector3d *forward, real_vector3d *right, float radius, obstacles *obstacles);

#endif // AI_ACTOR_MOVING_VEHICLE_DIRECTIONAL_GROUND_H
