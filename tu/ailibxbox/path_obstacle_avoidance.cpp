// ailibxbox/path_obstacle_avoidance.obj: from the records 0xc92d8 0x196b98 0x575d28 0x575e38 0x5761c8 0x576438 0x576740 0x576e08 0x577480
// @flags /TP /O2 /Oc
#include "memory/data_typed_access.h"
#include "ai/props.h"
#include "ai/path_state_types.h"
#include "ai/sector_geometry.h"
#include "ai/path_types.h"
#include "units/biped_types.h"
#include "ai/clump_importance_types.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "ai/sector_ref.h"
#include "game/game_types.h"
#include "objects/object_placement.h"
#include "ai/actor_datum.h"
#include "ai/prop_ref_datum.h"
#include "ai/recent_obstacle.h"
#include "ai/path_obstacle_types.h"
#include <float.h>
#include <math.h>
#include "math/matrix_math.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "ai/actors.h"
#include "objects/objects.h"
#include "ai/ai_hint_definitions.h"
#include "ai/path_obstacles.h"
#include "ai/path_smoothing.h"
#include "devices/device_machines.h"
#include "ai/path_obstacle_avoidance.h"
#include "physics/character_physics_component.h"
#include "ai/sector.h"
#include "game/game_allegiance.h"
#include "physics/havok_component.h"
#include "ai/special_movement_types.h"
#include "ai/pathfinding_pill_result.h"
#include "devices/device_datum.h"
#include "devices/device_definitions.h"
#include "models/model_types.h"
#include "ai/obstacle_path_types.h"
#include "devices/machine_datum.h"
#include "ai/obstacle_profile.h"
#include "objects/object_ai_properties.h"
#include "items/equipment_definitions.h"
#include "physics/collision_model_definition_types.h"
#include "ai/pathfinding_collision.h"
#include "math/real_math.h"
#include "physics/havok_component_class.h"
#include "math/projection_math.h"
#include "math/integer_math.h"
// @address 0x575d28 ?heap_down@@YAXPAUobstacle_path@@F@Z
// @address 0x5761c8 path_new
// @address 0xc92d8 ?actor_test_ignorable_obstacles@@YA_NJPBUobstacles@@F_NPBTreal_point3d@@Vc_sector_ref@@PBTreal_vector3d@@MMPAJPAF@Z


struct obstacles;
union real_point3d;
union real_vector3d;









// match: 0x196b98: tuned against the old c_flags_no_init::set (a named `mask` local); 7 words differ without it.
// A member function, as the old set was: a free function taking the flags by pointer or reference leaves 1 word.
struct c_obstacle_disc_flags : c_flags_no_init<long, short, 16>
{
    __forceinline void old_set(long bit, bool enable)
    {
        short mask = static_cast<short>(1) << static_cast<long>(bit);
        if (enable)
            m_storage |= mask;
        else
            m_storage &= ~mask;
    }
};



struct path_add_steps_stack_entry
{
    short disc_index;
    short sides;
};

void heap_down(obstacle_path *path, short index)
{
    volatile short *home = &index;
    short idx = 0;
    if (path->heap_count > 0) {
        for (;;) {
            short best = idx;
            short left = (short)(idx * 2 + 1);
            short right = (short)(idx * 2 + 2);
            if (left < path->heap_count && path->nodes[path->heap[best]].cost > path->nodes[path->heap[left]].cost)
                best = left;
            if (right < path->heap_count && path->nodes[path->heap[best]].cost > path->nodes[path->heap[right]].cost)
                best = right;
            if (best == idx)
                return;
            short t = path->heap[best];
            path->heap[best] = path->heap[idx];
            path->heap[idx] = t;
            idx = best;
        }
    }
}

static __forceinline const obstacle_disc *path_obstacle_avoidance_get_disc(const obstacles *o, short i) { return &o->discs[i]; }
static __forceinline short path_obstacle_avoidance_test_circle(const obstacles *o, short ignore, const real_point2d *center, float radius) {
    for (short i = 0; i < o->disc_count; i++)
        if (i != ignore)
        {
            const obstacle_disc *d = path_obstacle_avoidance_get_disc(o, i);
            if (point_in_circle(center, &d->center, radius + d->radius))
                return i;
        }
    return -1;
}
static __forceinline short path_obstacle_avoidance_obstacle_from_disc(const obstacles *o, short i)
{
    return i != -1 ? path_obstacle_avoidance_get_disc(o, i)->obstacle_index : -1;
}

extern "C" bool path_new(obstacle_path *path, const obstacles *o, bool flag, float radius, const real_point3d *start,
    c_sector_ref start_sector, const real_point3d *goal, c_sector_ref goal_sector, short projection_axis, bool projection_sign, bool partial,
    bool skip_flagged)
{
    bool success = true;
    path->radius = radius; path->obstacles_ref = o; path->flag = flag; path->complete = false;
    path->goal = *goal; path->goal_sector = goal_sector;
    path->goal_obstacle = path_obstacle_avoidance_obstacle_from_disc(o, path_obstacle_avoidance_test_circle(o, -1, &goal->xy, radius));
    path->last_node = -1; path->closest_distance = FLT_MAX; path->closest_step = -1;
    path->projection_axis = projection_axis; path->projection_sign = projection_sign; path->skip_flagged = skip_flagged;
    path->step_count = 0; path->heap_count = 0; path->partial_step = -1;
    path_add_step(path, start, start_sector, -1, false, false, -1, 0.0f, -1);
    for (short i = 0; i < o->disc_count; i++) {
        const obstacle_disc *d = path_obstacle_avoidance_get_disc(o, i);
        float penetration = (d->radius + radius) - distance2d(&goal->xy, &d->center);
        if (penetration > 0.0f) {
            if (partial && penetration > 2.0f * path->radius) { success = false; break; }
            d->flags |= 2;
        }
    }
    return success;
}

bool actor_test_ignorable_obstacles(long actor, struct obstacles const *obs, short type, bool test, union real_point3d const *point,
    // match: 0xc92d8: without __restrict on object the *object store moves above the obstacle->flags lhz/extsh and object stays in r9 across it.
    class c_sector_ref sector, union real_vector3d const *direction, float radius, float height, long *__restrict object, short *flags)
{
    bool result = false;
    pathfinding_test_pill2d_result hit;
    pathfinding_test_pill2d(true, obs, -1, type, test, point, sector, g_sector_none, direction, radius, height, true, false, false, 0, &hit);
    if (hit.disc_index >= 0 && hit.disc_index < obs->disc_count) {
        const obstacle_disc *obstacle = path_obstacle_avoidance_get_disc(obs, hit.disc_index);
        *object = obstacle->object_index;
        if (obstacle->flags & 16) *flags |= 4;
        else if (obstacle->flags & 512) *flags |= 128;
        else if (obstacle->flags & 64) *flags |= 8;
        else if (obstacle->flags & 32) *flags |= 2;
        else if (obstacle->flags & 256) *flags |= 16;
        else if (obstacle->flags & 128) *flags |= 32;
        else if (obstacle->flags & 8) *flags |= 64;
        result = true;
    }
    return result;
}

static __forceinline obstacle_node *path_obstacle_avoidance_get_step(obstacle_path *path, short i)
{
    return &path->nodes[i];
}
static __forceinline bool path_obstacle_avoidance_heap_insert(obstacle_path *path, short value)
{
    if (path->heap_count < 64) {
        short heap_index = path->heap_count++;
        path->heap[heap_index] = value;
        while (heap_index > 0) {
            short parent_index = (short)((heap_index - 1) >> 1);
            short child_value = path->heap[heap_index];
            short parent_value = path->heap[parent_index];
            if (path->nodes[parent_value].cost <= path->nodes[child_value].cost)
                break;
            path->heap[parent_index] = child_value;
            path->heap[heap_index] = parent_value;
            heap_index = parent_index;
        }
        return true;
    }
    return false;
}
// @address 0x575e38 ?path_add_step@@YAFPAUobstacle_path@@PBTreal_point3d@@Vc_sector_ref@@F_N3FMF@Z
short path_add_step(obstacle_path *path, const real_point3d *point, c_sector_ref sector, short obstacle_index, bool side, bool turning_point, short disc_index,
    float base_cost, short parent_index)
{
    short result = -1;
    short next = path->step_count;
    if (next < 64) {
    bool goal = obstacle_index == path->goal_obstacle && path->goal_obstacle != -1;
    bool success = true;
    real_vector2d direction;
    vector_from_points2d(&point->xy, &path->goal.xy, &direction);
    float distance = normalize2d(&direction);
    for (short i = parent_index; i != -1; ) {
        obstacle_node *step = path_obstacle_avoidance_get_step(path, i);
        if (obstacle_index != -1) {
            if (step->obstacle_index == obstacle_index) {
                if (step->side != side || (goal && step->distance < distance)) {
                    success = false;
                    break;
                }
            }
        } else if (step->obstacle_index == path->goal_obstacle && step->obstacle_index != -1 && step->distance < distance) {
            success = false;
            break;
        }
        i = step->parent;
    }
    if (success && goal) {
        for (short i = parent_index; i != -1; ) {
            obstacle_node *step = path_obstacle_avoidance_get_step(path, i);
            if (step->obstacle_index != obstacle_index) {
                short other = step->children[!side];
                if (other != -1) {
                    obstacle_node *other_step = path_obstacle_avoidance_get_step(path, other);
                    if (dot_product2d(&other_step->direction, &direction) > 0.0f &&
                        cross_product2d(&other_step->direction, &path_obstacle_avoidance_get_step(path, parent_index)->direction) *
                        cross_product2d(&other_step->direction, &direction) < 0.0f)
                        success = false;
                }
                if (success) {
                    short *child = &step->children[side];
                    if (*child == parent_index || *child == -1)
                        *child = next;
                }
                break;
            }
            i = step->parent;
        }
    }
    if (success) {
    result = path->step_count;
    obstacle_node *step = path_obstacle_avoidance_get_step(path, path->step_count);
    path->step_count = result + 1;
    step->point = *point;
    step->sector = sector;
    step->direction = direction;
    step->distance = distance;
    step->obstacle_index = obstacle_index;
    step->side = side;
    step->disc_index = disc_index;
    step->cost = step->distance + base_cost;
    step->parent = parent_index;
    step->a = 0;
    step->b = 0;
    step->c = -1;
    step->visited = false;
    step->turning_point = turning_point;
    memset(step->children, -1, sizeof(step->children));
    if (goal && step->distance < path->closest_distance) {
        path->closest_distance = step->distance;
        path->closest_step = result;
    }
    path_obstacle_avoidance_heap_insert(path, result);
    }
    }
    return result;
}

// @address 0x577480 ?path_find@@YA_NPBUpath_state@@PAUobstacle_path@@_NPBUobstacles@@MPBTreal_point3d@@Vc_sector_ref@@45F222@Z
bool path_find(const path_state *path_state, obstacle_path *path, bool flag, const obstacles *o, float radius, const real_point3d *start,
    c_sector_ref start_sector, const real_point3d *goal, c_sector_ref goal_sector, short projection_axis, bool projection_sign, bool partial,
    bool skip_flagged)
{
    if (path_new(path, o, flag, radius, start, start_sector, goal, goal_sector, projection_axis, projection_sign, partial, skip_flagged)) {
        do {
        } while (path_iterate(path, partial));
    }
    if (path->last_node != -1) {
        path->complete = true;
    }
    else {
        short step_index = path->closest_step;
        if (step_index != -1) {
            if (partial) {
                bool v = false;
                obstacle_node *step = path_obstacle_avoidance_get_step(path, step_index);
                if (path_state->goal.distance > 0.0f && step->distance < path_state->goal.distance)
                    v = true;
                if (!v && step->disc_index == -1)
                    v = true;
                if (!v) {
                    const obstacle_disc *d = path_obstacle_avoidance_get_disc(o, step->disc_index);
                    float goal_distance = distance2d(&d->center, &goal->xy);
                    float penetration = d->radius - goal_distance;
                    if (step->distance < d->radius && penetration < d->radius * 0.5f)
                        v = true;
                }
                if (v) {
                    real_point3d new_point = step->point;
                    point_from_line2d(&new_point.xy, &step->direction, step->distance, &new_point.xy);
                    path->last_node = path_add_step(path, &new_point, path->goal_sector, -1, false, false, -1, step->distance, step_index);
                }
            }
            else {
                path->last_node = step_index;
            }
        }
    }
    return path->last_node != -1;
}








static __forceinline actor_datum *obstacle_actor_get(long actor_index)
{
    return datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
}

static __forceinline prop_ref_datum *obstacle_prop_ref_get(long prop_ref_index)
{
    return datum_get_typed<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index);
}

static __forceinline c_havok_component *obstacle_havok_component_get(long havok_component_index)
{
    return datum_get_typed<c_havok_component>(g_havok_component_data, havok_component_index);
}

static __forceinline bool obstacle_object_flag_test(const object_datum *object, long bit)
{
    return (object->flags & (1UL << bit)) != 0;
}

static __forceinline bool obstacle_object_damage_flag_test(const object_datum *object, long bit)
{
    return (object->damage_flags & (1UL << bit)) != 0;
}

static __forceinline long obstacle_object_get_type(const object_datum *object)
{
    return object->object_identifier.m_type;
}

static __forceinline real_matrix4x3 *obstacle_object_get_node_matrix(long object_index, short node_index)
{
    object_datum *object = object_get(object_index);
    return (real_matrix4x3 *)((char *)object + object->node_matrices.offset + node_index * 52);
}

static __forceinline c_recent_obstacle *find_recent_obstacle(c_recent_obstacle *obstacles, short count, long object_index)
{
    c_recent_obstacle *result = 0;
    for (short slot_index = 0; slot_index < count; ++slot_index) {
        c_recent_obstacle *obstacle = &obstacles[slot_index];
        if (obstacle->valid() && obstacle->object_index == object_index) {
            result = obstacle;
            break;
        }
    }
    return result;
}

static __forceinline bool path_obstacle_avoidance_point_in_sphere(const real_point3d *point, const real_point3d *center, float radius)
{
    real_vector3d vector;
    vector_from_points3d(point, center, &vector);
    return vector.j * vector.j + vector.k * vector.k + vector.i * vector.i <= radius * radius;
}


static __forceinline bool ai_hint_marker_valid(const object_marker *marker)
{
    real_vector3d up;
    up.i = 0.0f;
    up.j = 0.0f;
    up.k = 1.0f;
    bool valid = false;
    if (dot_product3d(&marker->matrix.forward, &up) > 0.95f)
        valid = true;
    return valid;
}

static __forceinline float path_obstacle_avoidance_marker_distance(const object_marker *marker, const real_vector3d *offset)
{
    const real_vector3d *up = &marker->matrix.up;
    return dot_product3d(up, offset);
}

// 0x196b98 obstacle_object_test
// @address 0x196b98 path_obstacle_avoidance_add_object_discs
extern "C" bool path_obstacle_avoidance_add_object_discs(long actor_index, long object_index, s_obstacle_profile *profile,
    short projection_axis, bool projection_sign, obstacles *obstacle_list, obstacles *ignorable_obstacle_list)
{
    actor_datum *actor = 0;
    object_datum *object = object_get(object_index);
    s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
    c_havok_component *havok_component = 0;
    c_recent_obstacle *recent_obstacle = 0;
    bool rigid_body_position_valid = false;
    real_point3d rigid_body_position;
    float radius_bonus = 0.0f;
    bool friendly = false;
    bool door_open = false;
    bool pathfinding_object = false;
    bool ignorable = false;
    c_obstacle_disc_flags disc_flags;
    c_obstacle_disc_flags sphere_flags;
    disc_flags.clear();
    sphere_flags.clear();
    bool result = false;
    if (actor_index != -1)
        actor = obstacle_actor_get(actor_index);

    if (object_index == profile->ignore_object_index || object_index == profile->ignore_object_index2)
        return false;
    if (definition->object_flags.test(3))
        return false;
    if (profile->movement->recent_obstacles) {
        recent_obstacle = find_recent_obstacle(profile->movement->recent_obstacles, 3, object_index);
        if (recent_obstacle && recent_obstacle->successful())
            return false;
    }
    if (obstacle_object_flag_test(object, 22))
        pathfinding_object = true;
    else if (obstacle_object_flag_test(object, 23))
        return false;
    if (object_is_hidden(object_index))
        return false;
    if (profile->biped && profile->biped->biped.physics.get_mode() == c_character_physics_component::_mode_ground &&
        profile->biped->biped.physics.get_mode_ground()->get_support_object_index() == object_index)
        return false;
    if (object->havok_component_index != -1)
        havok_component = obstacle_havok_component_get(object->havok_component_index);

    long object_type = obstacle_object_get_type(object);
    switch (object_type)
    {
    case 0:
    {
        unit_datum *unit = (unit_datum *)object;
        if (actor && actor_is_giant(actor))
            return false;
        if (obstacle_object_damage_flag_test(object, 2))
            return false;
        if (actor) {
            long prop_ref_index = prop_ref_get_by_object_index(actor_index, object_index, false, _clump_importance_none);
            if (prop_ref_index != -1) {
                prop_ref_datum *prop_ref = obstacle_prop_ref_get(prop_ref_index);
                if (prop_ref->knowledge_state < 1)
                    return false;
            }
        }
        if (havok_component && havok_component->get_rigid_body_count() > 0) {
            havok_component->get_rigid_body_position(0, &rigid_body_position);
            rigid_body_position_valid = true;
        }
        if (unit->unit.actor_index != -1) {
            actor_datum *unit_actor = obstacle_actor_get(unit->unit.actor_index);
            bool enemy = actor && game_team_is_enemy((e_game_team)unit_actor->team, (e_game_team)actor->team);
            bool facing = false;
            if (profile->flags & 0x100)
                return false;
            if (actor) {
                if (actor->root_vehicle_index == -1 && rigid_body_position_valid && (profile->flags & 0x80)) {
                    if (unit_actor->flags & 1) {
                        real_vector3d direction;
                        vector_from_points3d(&profile->position, &rigid_body_position, &direction);
                        if (normalize3d(&direction) > 0.0f) {
                            float cosine = dot_product3d(&direction, &unit_actor->throttle) / magnitude3d(&unit_actor->throttle);
                            if (cosine >= -0.70710677f)
                                facing = true;
                        }
                    }
                    else if (unit_actor->flags & 0x8000000) {
                        facing = true;
                    }
                }
                if (actor->root_vehicle_index != -1 && !enemy) {
                    if (unit_actor->flags & 1)
                        radius_bonus = 0.4f;
                    else
                        radius_bonus = 0.2f;
                }
                else if (enemy && actor->suppress_combat) {
                    radius_bonus = 0.5f;
                }
                else if (facing) {
                    disc_flags.old_set(9, true);
                    ignorable = true;
                    radius_bonus = 0.3f;
                }
            }
            friendly = !enemy;
        }
        else if (actor && unit->unit.player_index != -1) {
            bool enemy = game_team_is_enemy((e_game_team)actor->team, (e_game_team)unit->unit.team);
            if (actor->root_vehicle_index != -1 && !enemy)
                radius_bonus = 0.7f;
            else if (enemy && actor->suppress_combat)
                radius_bonus = 0.5f;
            friendly = !enemy;
        }
        break;
    }
    case 1:
    {
        unit_datum *unit = (unit_datum *)object;
        if (actor && actor->root_vehicle_index == -1 && !game_team_is_enemy((e_game_team)unit->unit.team, (e_game_team)actor->team))
            friendly = true;
        break;
    }
    case 7:
    {
        s_machine_datum *machine = (s_machine_datum *)object;
        s_machine_definition *machine_definition = (s_machine_definition *)tag_get('mach', object->definition_index);
        if (!(machine_definition->flags & 1))
            return false;
        if ((machine_definition->flags & 2) && machine_definition->type == 0) {
            if (machine->device_position == 1.0f) {
                door_open = true;
            }
            else if (!machine->machine_flags.test(_machine_flag_0) && machine->power > 0.0f && actor) {
                long objects = actor->root_vehicle_index != -1 ? actor->root_vehicle_index : actor->unit_index;
                door_open = machine_door_test_object(object_index, &objects, 1);
            }
        }
        break;
    }
    case _object_type_crate:
        if (havok_component && havok_component->get_maximum_rigid_body_size() <= 1)
            return false;
        break;
    case 3:
    {
        s_equipment_definition *equipment_definition = (s_equipment_definition *)tag_get('eqip', object->definition_index);
        if (!(equipment_definition->flags & 1))
            return false;
        break;
    }
    }

    if (!path_obstacle_avoidance_point_in_sphere(&profile->position, &object->bounding_center, profile->radius + object->bounding_radius))
        return false;

    object_ai_properties *ai_properties = 0;
    bool valid = true;
    if (!friendly && (!recent_obstacle || recent_obstacle->successful())) {
        short size = 0;
        if (definition->ai_properties.count > 0) {
            ai_properties = (object_ai_properties *)tag_block_get_element_with_size(&definition->ai_properties, 0, sizeof(object_ai_properties));
            if ((ai_properties->flags & 2) && !(object->body_vitality > 0.0f))
                valid = false;
            else
                size = ai_properties->size;
        }
        if (havok_component && size == 0)
            size = (short)havok_component->get_maximum_rigid_body_size();
        if (valid && size > 0) {
            if (profile->ignore_size >= size) {
                valid = false;
                if (object_type == 1) {
                    unit_datum *unit = (unit_datum *)object;
                    if (!game_team_is_enemy((e_game_team)unit->unit.team, (e_game_team)actor->team) && unit->unit.driver_object_index != -1)
                        valid = true;
                }
            }
            else if (profile->step_size >= size && profile->mount_allowed) {
                disc_flags.old_set(4, true);
                ignorable = true;
            }
            else if (profile->minimum_push_size <= size && profile->maximum_push_size >= size && obstacle_object_get_type(object) != 0) {
                disc_flags.old_set(5, true);
                ignorable = true;
            }
        }
        if (valid && profile->movement) {
            if ((profile->flags & 0x08) && profile->vault_allowed) {
                object_marker markers[32];
                short marker_count = ai_hint_get_vault_markers(object_index, markers, 32, (short)profile->movement->movement_hints);
                for (short marker_index = 0; marker_index < marker_count; marker_index++) {
                    object_marker *marker = &markers[marker_index];
                    if (ai_hint_marker_valid(marker)) {
                        const float tolerance = 0.2f;
                        bool vaultable = false;
                        real_vector3d offset;
                        vector_from_points3d(&marker->matrix.position, &profile->position, &offset);
                        float distance = path_obstacle_avoidance_marker_distance(marker, &offset);
                        if (distance >= -tolerance && distance < marker->length + tolerance) {
                            float alignment = fabsf(dot_product3d(&profile->forward, &marker->matrix.left));
                            if (alignment >= 0.9f)
                                vaultable = true;
                        }
                        if (vaultable) {
                            disc_flags.old_set(6, true);
                            ignorable = true;
                            break;
                        }
                    }
                }
            }
            if ((profile->flags & 0x10) && profile->mount_allowed) {
                object_marker markers[32];
                short marker_count = ai_hint_get_mount_markers(object_index, markers, 32, (short)profile->movement->movement_hints);
                for (short marker_index = 0; marker_index < marker_count; marker_index++) {
                    object_marker *marker = &markers[marker_index];
                    if (ai_hint_marker_valid(marker)) {
                        const float tolerance = 0.1f;
                        bool mountable = false;
                        real_vector3d offset;
                        vector_from_points3d(&marker->matrix.position, &profile->position, &offset);
                        float distance = dot_product3d(&offset, &marker->matrix.up);
                        if (distance >= tolerance && distance < marker->length - tolerance) {
                            float alignment = fabsf(dot_product3d(&profile->forward, &marker->matrix.left));
                            if (alignment >= 0.9f)
                                mountable = true;
                        }
                        if (mountable) {
                            disc_flags.old_set(8, true);
                            ignorable = true;
                            break;
                        }
                    }
                }
            }
            if ((profile->flags & 0x20) && profile->mount_allowed) {
                object_marker markers[32];
                short marker_count = ai_hint_get_hoist_markers(object_index, markers, 32, (short)profile->movement->movement_hints);
                for (short marker_index = 0; marker_index < marker_count; marker_index++) {
                    if (ai_hint_marker_valid(&markers[marker_index])) {
                        disc_flags.old_set(7, true);
                        ignorable = true;
                        break;
                    }
                }
            }
            if (actor && actor->root_vehicle_index != -1 && (profile->flags & 0x80) &&
                ((1 << obstacle_object_get_type(object)) & 0x1003)) {
                real_point3d origin;
                real_vector3d to_object;
                object_get_origin(object_index, &origin);
                vector_from_points3d(&profile->position, &origin, &to_object);
                unit_datum *unit = (unit_datum *)object;
                if (dot_product3d(&profile->forward, &to_object) > 0.0f && unit->unit.driver_object_index != -1) {
                    if (unit->unit.team == actor->team || !game_team_is_enemy((e_game_team)unit->unit.team, (e_game_team)actor->team)) {
                        unit_datum *driver = (unit_datum *)object_get(unit->unit.driver_object_index);
                        real_vector3d velocity = unit->object.linear_velocity;
                        if (driver->unit.actor_index != -1) {
                            actor_datum *driver_actor = obstacle_actor_get(driver->unit.actor_index);
                            if (driver_actor->flags & 1)
                                velocity = driver_actor->throttle;
                        }
                        if (normalize3d(&velocity) > 0.0f) {
                            bool approaching = false;
                            float dot = profile->forward_projection(&velocity);
                            if (dot > 0.70710677f) {
                                const real_vector3d *forward = &profile->forward;
                                float velocity_dot = dot_product3d(forward, &unit->object.linear_velocity);
                                if (velocity_dot > 0.0f)
                                    approaching = true;
                            }
                            else if (dot > -0.70710677f) {
                                approaching = true;
                            }
                            if (approaching) {
                                disc_flags.old_set(9, true);
                                ignorable = true;
                                radius_bonus = 4.0f;
                            }
                        }
                    }
                }
            }
        }
    }

    if (!valid)
        return false;
    if ((ignorable && ignorable_obstacle_list) || (!ignorable && obstacle_list)) {
        if (ai_properties && (profile->flags & 0x40) && (ai_properties->flags & 2))
            sphere_flags.old_set(3, true);
        if (definition->model.index != -1) {
            s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
            if (model->collision_model_index != -1) {
                collision_model_definition *collision_model = (collision_model_definition *)tag_get('coll', model->collision_model_index);
                if (collision_model->pathfinding_spheres.count > 0) {
                    real_matrix4x3 world_matrix;
                    object_get_world_matrix(object_index, &world_matrix);
                    if (rigid_body_position_valid)
                        world_matrix.position = rigid_body_position;
                    for (short sphere_index = 0; sphere_index < collision_model->pathfinding_spheres.count; sphere_index++) {
                        pathfinding_sphere *sphere = (pathfinding_sphere *)tag_block_get_element_with_size(
                            &collision_model->pathfinding_spheres, sphere_index, sizeof(pathfinding_sphere));
                        if (door_open && !(sphere->flags & 1))
                            continue;
                        if (pathfinding_object && !(sphere->flags & 4))
                            continue;
                        if (!profile->in_vehicle && (sphere->flags & 2))
                            continue;
                        real_point3d center;
                        float radius;
                        if (sphere->node != -1) {
                            real_matrix4x3 *node_matrix = obstacle_object_get_node_matrix(object_index, sphere->node);
                            matrix4x3_transform_point(node_matrix, &sphere->center, &center);
                            radius = sphere->radius * node_matrix->scale;
                        }
                        else {
                            matrix4x3_transform_point(&world_matrix, &sphere->center, &center);
                            radius = sphere->radius * world_matrix.scale;
                        }
                        if (center.z + radius < profile->lower || center.z - radius > profile->upper)
                            continue;
                        if (center.z + radius + 1.0f < profile->position.z && profile->forward.k > -0.2f)
                            continue;
                        if (center.z - radius - 1.0f > profile->position.z && profile->forward.k < 0.2f)
                            continue;
                        real_point3d projected_position;
                        real_vector3d delta;
                        project_point3d(&profile->position, projection_axis, projection_sign, &projected_position);
                        project_point3d(&center, projection_axis, projection_sign, &center);
                        vector_from_points3d(&projected_position, &center, &delta);
                        if (delta.i * delta.i + delta.j * delta.j + 4.0f * delta.k * delta.k > (profile->radius + radius) * (profile->radius + radius))
                            continue;
                        short flags = disc_flags.get_unsafe();
                        if (!ignorable) {
                            flags |= sphere_flags.get_unsafe();
                            if (obstacle_object_get_type(object) == 0 &&
                                dot_product3d(&delta, &profile->forward) > 0.0f &&
                                dot_product3d(&object->linear_velocity, &profile->forward) > 2.0f)
                                flags |= 1;
                        }
                        obstacles *list = ignorable ? ignorable_obstacle_list : obstacle_list;
                        obstacles_add_disc(list, object_index, flags, &center, radius + radius_bonus);
                    }
                }
            }
        }
    }
    return result;
}


static __forceinline void path_obstacle_avoidance_inverse_project_point3d(const real_point3d *point, short projection, bool sign, real_point3d *result)
{
    result->x = point->n[global_projection3d_inverse_mappings[projection][sign][0]];
    result->y = point->n[global_projection3d_inverse_mappings[projection][sign][1]];
    result->z = point->n[global_projection3d_inverse_mappings[projection][sign][2]];
}


// match: 0x576740: the typed vector projection rewrite changes one FMA operand pair.
static __forceinline real_vector3d *inverse_project_vector3d(const real_vector3d *vector, short projection, bool sign, real_vector3d *result)
{
    return (real_vector3d *)inverse_project_point3d((const real_point3d *)vector, projection, sign, (real_point3d *)result);
}


// @address 0x576438 ?path_add_turning_point_step@@YAFPAUobstacle_path@@FPBTreal_point3d@@FG_N2@Z
short path_add_turning_point_step(obstacle_path *path, short step_index, const real_point3d *point, short structure_bsp_index,
    unsigned short link_index, bool clockwise, bool flag)
{
    obstacle_node *step = path_obstacle_avoidance_get_step(path, step_index);
    short result = 0;
    real_vector3d direction;
    real_point3d turning_point;
    if (find_turning_point(structure_bsp_index, &step->point, path->radius, link_index, clockwise, path->projection_axis, path->projection_sign,
            path->flag, &turning_point))
    {
        real_point3d projected_point;
        real_vector3d projected_vector;
        path_collision_result collision;
        find_avoidance_point(&step->point, &turning_point, point, path->radius, clockwise, path->projection_axis, path->projection_sign, &turning_point);
        vector_from_points3d(&step->point, &turning_point, &direction);
        direction.k = 0.0f;
        float length = normalize3d(&direction);
        if (length > 0.0f)
        {
            path_obstacle_avoidance_inverse_project_point3d(&step->point, path->projection_axis, path->projection_sign, &projected_point);
            inverse_project_vector3d(&direction, path->projection_axis, path->projection_sign, &projected_vector);
            pathfinding_test_vector(&projected_point, step->sector, g_sector_none, &projected_vector, length, 0, &collision);
            project_point3d(&collision.point, path->projection_axis, path->projection_sign, &collision.point);
            if (!collision.collision)
            {
                pathfinding_test_pill2d_result pill;
                if (!pathfinding_test_pill2d(path->flag, path->obstacles_ref, -1, path->projection_axis, path->projection_sign, &projected_point,
                        step->sector, g_sector_none, &projected_vector, path->radius, length, true, false, path->skip_flagged, 0, &pill))
                {
                    if (path_add_step(path, &collision.point, collision.sector, -1, flag, true, -1, (step->cost - step->distance) + length, step_index) != -1)
                        result++;
                }
            }
        }
    }
    return result;
}


// @address 0x576740 ?path_add_steps@@YAFPAUobstacle_path@@FF@Z
short path_add_steps(obstacle_path *path, short step_index, short seed_disc_index)
{
    short steps_added = 0;
    obstacle_node *step = path_obstacle_avoidance_get_step(path, step_index);
    c_static_flags<256> visited;
    short stack_top = 0;
    short seed_obstacle = seed_disc_index == -1 ? -1 : path_obstacle_avoidance_obstacle_from_disc(path->obstacles_ref, seed_disc_index);
    path_add_steps_stack_entry stack[256];

    bit_vector_clear_range(visited.get_writeable_bits_direct(), path->obstacles_ref->disc_count);
    visited.set(seed_disc_index, true);
    stack[0].disc_index = seed_disc_index;
    if (seed_obstacle == step->obstacle_index && seed_obstacle != -1)
        stack[0].sides = (short)(1 << (step->side != 0));
    else
        stack[0].sides = 3;
    stack_top++;

    while (stack_top > 0)
    {
        stack_top--;
        path_add_steps_stack_entry *entry = &stack[stack_top];
        short disc_index = entry->disc_index;
        short sides = entry->sides;
        short obstacle = path_obstacle_avoidance_obstacle_from_disc(path->obstacles_ref, disc_index);
        real_vector2d tangents[2];
        float tangent_distance;
        obstacles_disc_tangents(path->obstacles_ref, disc_index, &step->point.xy, path->radius, &tangents[0],
            &tangents[1], &tangent_distance);
        if (tangent_distance < path->radius)
            tangent_distance = path->radius;
        const obstacle_disc *current_disc = path_obstacle_avoidance_get_disc(path->obstacles_ref, disc_index);
        float length = current_disc->radius + path->radius + tangent_distance;

        for (short side = 0; side < 2; side++)
        {
            if ((sides & (1 << side)) == 0)
                continue;

            real_vector3d vector;
            vector.i = tangents[side].i;
            vector.j = tangents[side].j;
            vector.k = 0.0f;
            real_point3d origin;
            inverse_project_point3d(&step->point, path->projection_axis, path->projection_sign, &origin);
            inverse_project_vector3d(&vector, path->projection_axis, path->projection_sign, &vector);
            pathfinding_test_pill2d_result result;
            bool blocked = pathfinding_test_pill2d(path->flag, path->obstacles_ref, disc_index, path->projection_axis, path->projection_sign, &origin,
                step->sector, g_sector_none, &vector, path->radius, length, false, false, path->skip_flagged, 0,
                &result);
            if (result.disc_index != -1)
            {
                if (!visited.test(result.disc_index))
                {
                    stack[stack_top].disc_index = result.disc_index;
                    if (result.obstacle_index == current_disc->obstacle_index)
                        stack[stack_top].sides = (short)(1 << side);
                    else
                        stack[stack_top].sides = 3;
                    visited.set(result.disc_index, true);
                    stack_top++;
                }
            }
            else if (blocked && result.link_index != -1 && result.link_index != 0xffff && result.distance < tangent_distance)
            {
                real_point3d point = step->point;
                bool clockwise;
                if (!result.left_side_blocked)
                    clockwise = false;
                else if (!result.right_side_blocked)
                    clockwise = true;
                else
                    clockwise = side == 1;
                point_from_line2d(&step->point.xy, &tangents[side], length, &point.xy);
                steps_added += path_add_turning_point_step(path, step_index, &point, result.structure_bsp_index,
                    (unsigned short)result.link_index, clockwise, side > 0);
            }

            path_collision_result collision;
            if (result.distance >= tangent_distance && result.obstacle_index != obstacle)
            {
                short added = -1;
                float distance = (result.distance + tangent_distance) * 0.5f;
                real_vector3d line_vector;
                line_vector.i = tangents[side].i;
                line_vector.j = tangents[side].j;
                line_vector.k = 0.0f;
                real_point3d line_origin;
                inverse_project_point3d(&step->point, path->projection_axis, path->projection_sign, &line_origin);
                inverse_project_vector3d(&line_vector, path->projection_axis, path->projection_sign, &line_vector);
                pathfinding_test_vector(&line_origin, step->sector, g_sector_none, &line_vector, distance, 0, &collision);
                project_point3d(&collision.point, path->projection_axis, path->projection_sign, &collision.point);
                added = path_add_step(path, &collision.point, collision.sector, obstacle, side > 0, false, seed_disc_index,
                    (step->cost - step->distance) + distance, step_index);
                if (added != -1)
                    steps_added++;
            }
        }
    }
    return steps_added;
}

static __forceinline short path_iterate_heap_pop(obstacle_path *path)
{
    if (path->heap_count > 0)
    {
        short result = path->heap[0];
        path->heap_count--;
        path->heap[0] = path->heap[path->heap_count];
        heap_down(path, 0);
        return result;
    }
    return -1;
}

static __forceinline void path_iterate_step_failed(obstacle_path *path, short step_index, short obstacle)
{
    do
    {
        obstacle_node *step = path_obstacle_avoidance_get_step(path, step_index);
        step_index = -1;
        step->visited = true;
        if (step->c == -1 && obstacle != -1)
            step->c = obstacle;
        if (step->parent != -1)
        {
            obstacle_node *parent = path_obstacle_avoidance_get_step(path, step->parent);
            short completed = ++parent->b;
            if (completed >= parent->a)
                step_index = step->parent;
        }
        else
            path->partial_step = step->c;
    } while (step_index != -1);
}

// @address 0x576e08 ?path_iterate@@YA_NPAUobstacle_path@@_N@Z
bool path_iterate(obstacle_path *path, bool partial)
{
    short step_index = path_iterate_heap_pop(path);
    if (step_index != -1)
    {
        pathfinding_test_pill2d_result result;
        obstacle_node *step = path_obstacle_avoidance_get_step(path, step_index);
        bool root_step = step->parent == -1;
        short steps_added = 0;
        real_vector3d direction;
        direction.i = step->direction.i;
        direction.j = step->direction.j;
        direction.k = 0.0f;
        real_point3d origin;
        inverse_project_point3d(&step->point, path->projection_axis, path->projection_sign, &origin);
        inverse_project_vector3d(&direction, path->projection_axis, path->projection_sign, &direction);
        pathfinding_test_pill2d(path->flag, path->obstacles_ref, -1, path->projection_axis, path->projection_sign, &origin, step->sector,
            path->goal_sector, &direction, path->radius, step->distance, root_step, partial, path->skip_flagged, 0,
            &result);
        if (result.link_index == -1 || result.link_index == 0xffff)
        {
            if (result.disc_index == -1)
            {
                if (result.sector == path->goal_sector || step->distance < 0.2f)
                {
                    real_point3d point = step->point;
                    point_from_line2d(&step->point.xy, &step->direction, result.distance, &point.xy);
                    path->last_node = path_add_step(
                        path, &point, path->goal_sector, -1, false, false, -1, (step->cost - step->distance) + result.distance, step_index);
                    steps_added = 1;
                }
            }
            else
            {
                const obstacle_disc *obstacle = path_obstacle_avoidance_get_disc(path->obstacles_ref, result.disc_index);
                bool partial_success = false;
                if (result.obstacle_index == path->goal_obstacle && partial)
                {
                    float remaining = step->distance - result.distance;
                    bool reached = false;
                    if (remaining < path->radius * 2.5f)
                        reached = true;
                    else if (obstacle->flags & 2)
                    {
                        real_vector2d goal_direction;
                        vector_from_points2d(&obstacle->center, &path->goal.xy, &goal_direction);
                        if (normalize2d(&goal_direction) > 0.0f)
                        {
                            real_vector2d step_direction;
                            vector_from_points2d(&obstacle->center, &step->point.xy, &step_direction);
                            if (normalize2d(&step_direction) > 0.0f && dot_product2d(&step_direction, &goal_direction) > 0.70710677f)
                                reached = true;
                        }
                    }
                    if (reached)
                    {
                        real_vector2d offset;
                        scale_vector2d(&step->direction, result.distance, &offset);
                        real_point3d point = step->point;
                        point_from_line2d(&point.xy, &offset, 1.0f, &point.xy);
                        path->last_node = path_add_step(path, &point, path->goal_sector, -1, false, false, -1, step->cost, step_index);
                        steps_added = 1;
                        partial_success = true;
                    }
                }
                if (!partial_success)
                {
                    if (result.obstacle_index == path->goal_obstacle && step->distance < path->radius)
                    {
                        path->last_node = step_index;
                    }
                    else
                    {
                        if (result.obstacle_index == path->goal_obstacle && step->distance < path->closest_distance)
                        {
                            path->closest_distance = step->distance;
                            path->closest_step = step_index;
                        }
                        steps_added = path_add_steps(path, step_index, result.disc_index);
                    }
                }
            }
        }
        else
        {
            if (step->turning_point || step->obstacle_index != -1)
            {
                bool clockwise = step->side;
                real_point3d point = step->point;
                point_from_line2d(&step->point.xy, &step->direction, step->distance, &point.xy);
                steps_added = path_add_turning_point_step(path, step_index, &point, result.structure_bsp_index,
                    (unsigned short)result.link_index, clockwise, step->side);
            }
            if (steps_added == 0 && step->disc_index != -1)
            {
                steps_added = path_add_steps(path, step_index, step->disc_index);
                result.disc_index = step->disc_index;
            }
        }
        step->a = steps_added;
        if (steps_added == 0 && path->last_node == -1)
        {
            short obstacle = -1;
            if (result.disc_index != -1)
                obstacle = path_obstacle_avoidance_obstacle_from_disc(path->obstacles_ref, result.disc_index);
            path_iterate_step_failed(path, step_index, obstacle);
        }
    }
    return path->last_node == -1 && path->heap_count > 0 ? 1 : 0;
}
