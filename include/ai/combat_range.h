#ifndef AI_COMBAT_RANGE_H
#define AI_COMBAT_RANGE_H

#include "math/real_math_types.h"

class c_combat_range {
public:
    __forceinline c_combat_range();
    __forceinline void add_actors_from_task(short objective_index, short task_index);
    __forceinline void add_actors_from_squad_task(long squad_index);
    __forceinline void set_custom_weapon_range(float minimum, float maximum, float distance);
    __forceinline void set_target_danger_radius(float radius);
    __forceinline void set_target(real_point3d *point);
    __forceinline void set_target(real_point3d *point, float radius);
    void add_actor(long actor_index);
    bool place_actor(long actor_index);
    bool set_constraints_from_task(short objective_index, short task_index);
    __forceinline bool set_target_from_actor(long actor_index);

    __forceinline bool set_weapon_range_from_actor(long actor_index);

    __forceinline void set_constraints_from_actor(long actor_index);

    __forceinline void get_constraints(short standing, float *minimum, float *maximum);


    __forceinline void set_constraints(float minimum, float maximum);

    __forceinline void set_leader_constraints(float minimum, float maximum);

    float m_weapon_minimum;
    float m_weapon_maximum;
    float m_weapon_distance;
    bool m_weapon_range_valid;
    float m_leader_distance;
    bool m_leader_distance_valid;
    real_point3d m_leader_position;
    bool m_leader_position_valid;
    float m_follower_minimum;
    float m_follower_maximum;
    bool m_target_valid;
    float m_leader_minimum;
    float m_leader_maximum;
    bool m_leader_valid;
    short m_actor_count;
    float m_maximum;
    float m_minimum;
    float m_distance;
    long m_leader_index;
    float m_range_minimum;
    float m_range_maximum;
    float m_range_ideal;
    float m_range_center;
    float m_range_spread;
    float m_range_vertical;
};

// Functions defined in tu/ailibxbox/combat_range.cpp that other units call.

extern "C" bool actor_get_weapon_range(long actor_index, float *minimum, float *maximum, float *distance);

#endif // AI_COMBAT_RANGE_H
