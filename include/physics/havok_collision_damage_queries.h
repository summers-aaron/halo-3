#ifndef PHYSICS_HAVOK_COLLISION_DAMAGE_QUERIES_H
#define PHYSICS_HAVOK_COLLISION_DAMAGE_QUERIES_H

struct s_collision_damage_definition;
union real_point3d;
class c_global_material_type;

bool havok_collision_damage_apply_absolute_damage(const s_collision_damage_definition *collision_damage, const real_point3d *point,
    long object_index, float impulse, c_global_material_type material_type, float speed, float relative_speed, bool damage_owner_valid,
    long damage_owner_object_index);

#endif // PHYSICS_HAVOK_COLLISION_DAMAGE_QUERIES_H
