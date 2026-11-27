#ifndef PHYSICS_HAVOK_GROUP_TYPES_H
#define PHYSICS_HAVOK_GROUP_TYPES_H

enum e_havok_group
{
    _havok_group_default = 0,
    _havok_group_environment = 1,
    _havok_group_ragdoll = 2,
    _havok_group_vehicle = 6,
    _havok_group_biped = 11,
    _havok_group_dead_biped = 12,
    _havok_group_recently_dead_biped = 13,
    _havok_group_ragdoll_biped = 14,
    _havok_group_vehicle_physics = 15,
    _havok_group_giant = 21
};

#endif // PHYSICS_HAVOK_GROUP_TYPES_H
