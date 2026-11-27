#ifndef ITEMS_PROJECTILE_FLAGS_H
#define ITEMS_PROJECTILE_FLAGS_H
// Projectile state flags; the perception-attached flag uses
// bit 3; preserve that separate spelling until its storage offset is reconciled.
enum e_projectile_flags
{
    _projectile_rotating_bit = 0,
    _projectile_tracer_bit = 1,
    _projectile_grounded_bit = 2,
    _projectile_perception_attached_bit = 3,
    _projectile_stopped_bit = 4,
    _projectile_timer_started_bit = 5,
    _projectile_supercombine_consumed_bit = 6,
    _projectile_supercombine_excess_bit = 7,
    _projectile_disable_deceleration_bit = 8,
    _projectile_impact_effect_created_bit = 9,
    _projectile_attached_bit = 10,
    _projectile_collided_bit = 11,
    _projectile_detonated_bit = 12,
    _projectile_collision_recorded_bit = 13,
    _projectile_at_rest_bit = 14,
    _projectile_supercombine_processed_bit = 15,
    _projectile_supercombine_deferred_bit = 16,
    _projectile_supercombine_use_actual_count_bit = 17,
    _projectile_use_physics_bit = 18,
    _projectile_distributing_bit = 19,
    _projectile_oriented_bit = 20,
    _projectile_no_impact_material_bit = 21
};
#endif
