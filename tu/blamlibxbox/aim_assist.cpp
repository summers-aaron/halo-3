// blamlibxbox/aim_assist.obj: from the records 0x99800 0x99f30 0x9b368 0x9c570 0x9d338 0xa2c28 0x278c28 0x279120 0x279258 0x279418 0x279580 0x279760 0x2798d0 0x279a20 0x279c68 0x27a098 0x27a2e8 0x27a490 0x27a5e0 0x27a740 0x27a9f8 0x27ac38 0x27ad98
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"
#include "objects/object_velocity_queries.h"
#include "game/game_allegiance.h"
#include "camera/observer_globals_types.h"
#include "objects/object_header.h"
// flags_test (mask form) keeps the accepted bytes here; the TEST_BIT spelling selects different bit-test forms.
#include "objects/object_hierarchy.h"
#include "objects/object_markers_inline.h"
#include "tag_files/tag_groups.h"
#include "units/biped_types.h"
#include "units/unit_definitions.h"
#include "units/vehicle_definitions.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "physics/collision_test_vector.h"
#include <stddef.h>
#include "objects/object_reference_types.h"
#include <string.h>
#include <stdlib.h>
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math_functions.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"
#include "cseries/cseries.h"

#include "game/game_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "units/unit_control.h"
#include "units/unit_types.h"
// @address 0x9d338 ?compute_composite_attenuation@@YAMMMMMM@Z
// @address 0x279760 aim_assist_teamless_vehicle_should_be_aimed_at
// @address 0x2798d0 aim_assist_teamless_vehicle_has_passenger
// @address 0x27a2e8 ?pill_compute_autoaim_target@@YAXPBTreal_point3d@@PBTreal_vector3d@@M01PAT1@@Z
// @address 0x27a098 ?aim_assist_evaluate_target@@YA_NPBUaim_assist_parameters@@MPAUs_aim_assist_target@@@Z
// @address 0x27a490 ?compare_targets@@YAHPBX0@Z
// @address 0x27a5e0 ?aim_assist_clear_line_of_sight@@YA_NPBTreal_point3d@@0MJJ_N@Z
#include <math.h>
#include "game/players.h"
#include "game/player_access.h"
#include "game/player_types.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "units/units.h"
#include "camera/first_person_camera.h"
#include "camera/following_camera.h"
#include "game/aim_assist.h"
#include "game/aim_assist_storage.h"
#include "memory/tls_globals.h"
#include "camera/saved_film_director_storage.h"
#include "math/real_math_constants.h"
#include "math/real_constant_pool_access.h"
#include "models/model_types.h"
#include "objects/object_definitions.h"
#include "ai/weapon_datum_types.h"
#include "items/weapon_definitions.h"
#include "creatures/creature_datum.h"
#include "units/biped_definitions.h"
#include "game/player_control_types.h"
#include "items/weapons.h"
#include "motor/motor_system.h"
#include "objects/object_early_movers.h"
#include "physics/havok_utilities.h"
#include "render/render_objects_static_lighting.h"
#include "units/bipeds.h"
#include "physics/havok_ray_test_types.h"
#include "game/game_globals.h"
#include "game/player_control_constants.h"
#include "memory/tls_data_arrays.h"
#include "items/projectiles.h"
#include "memory/bitstream.h"
#include "physics/collision_types.h"
#include "cache/cache_files.h"
#include "structures/structures.h"
#include "physics/havok_component.h"
#include "physics/havok_component_class.h"
#include "physics/collisions.h"
#include "game/game_time_types.h"
#include "game/player_mapping_types.h"

#include "units/unit_passenger_iterator_types.h"



#include "models/model_target_types.h"

#include "math/real_math_fraction_constants.h"
#include "items/projectile_definitions.h"
#include "camera/game_director.h"
#include "camera/director_globals_types.h"
#include "memory/data.h"
#include "scenario/cluster_reference_helpers.h"
#include "game/game_time_access.h"
#include "math/real_constant_pool.h"
#include "memory/data_typed_access.h"
#include "objects/object_type_access.h"

__declspec(thread) s_user_globals *player_mapping_globals;

struct s_aim_assist_control_result
{
    float yaw;
    float pitch;
    float scale;
};


static __forceinline void unit_passenger_iterator_new(long index,unit_passenger_iterator *iterator) {
 unit_datum *unit=(unit_datum *)object_get_and_verify_type(index,8195);
 iterator->unit_index=index; iterator->passenger_index=-1; iterator->seat_index=-1; iterator->next_object_index=unit->object.first_child_object_index;
}
static __forceinline bool game_team_is_friendly(long a, long b)
{
    return !game_team_is_enemy((e_game_team)a, (e_game_team)b) || game_team_is_ally((e_game_team)a, (e_game_team)b);
}



static __forceinline bool limit3d(real_vector3d *v, float limit)
{
    float magnitude_squared = magnitude_squared3d(v);
    if (magnitude_squared > limit * limit)
    {
        scale_vector3d(v, limit * reciprocal_square_root(magnitude_squared), v);
        return true;
    }
    return false;
}
static __forceinline real_point3d *point_from_point_minus_vector3d(const real_point3d *point, const real_vector3d *vector,
    real_point3d *result)
{
    result->x = point->x - vector->i;
    result->y = point->y - vector->j;
    result->z = point->z - vector->k;
    return result;
}

static __forceinline long object_root_parent(long object_index)
{
    long result=-1;
    while (object_index!=-1)
    {
        result=object_index;
        object_index=((object_datum *)object_get_and_verify_type(object_index, _object_mask_all))->parent_object_index;
    }
    return result;
}
static __forceinline float aim_assist_compute_attenuation(float value, float start, float end)
{
    if (value >= end)
        return 0.0f;
    else if (value <= start)
        return 1.0f;
    else
        return (end - value) / (end - start);
}

// @address 0x9c390 ?objects_in_clusters_by_indices@@YAJKKKJPBUs_cluster_reference@@JPAJ@Z
// @address 0x279a20 ?aim_assist_compute_targets_from_object@@YAJPBUaim_assist_parameters@@JJ_NPBTreal_point3d@@PBTreal_vector3d@@JAAV?$c_static_array@Us_aim_assist_target@@$0EA@@@@Z
static __forceinline long unit_inventory_weapon_from_datum(unit_datum *unit, long slot)
{
    long result=-1;
    if (slot!=-1) result=unit->unit.weapon_object_indices[slot];
    return result;
}
static __forceinline s_cluster_reference inl_scenario_cluster_reference_from_point(const real_point3d *point)
{
    long bsp_index;
    long leaf_index=scenario_leaf_index_from_point(point,&bsp_index);
    s_cluster_reference result;
    if (leaf_index!=-1) result=scenario_cluster_reference_from_leaf_index(bsp_index,leaf_index);
    else *(unsigned short *)&result=65535;
    return result;
}
static __forceinline bool aim_assist_weapon_is_looking_for_tracking_target(long weapon_index)
{
    weapon_datum *weapon=(weapon_datum *)object_get_and_verify_type(weapon_index,4);
    weapon_definition *definition=(weapon_definition *)tag_get('weap',weapon->definition_index);
    bool result=false;
    for (long trigger_index=0;trigger_index<definition->trigger_count;trigger_index++)
    {
        s_weapon_trigger_definition *trigger_definition = (s_weapon_trigger_definition *)tag_block_get_element_with_size(
            &definition->trigger_definitions, trigger_index, sizeof(s_weapon_trigger_definition));
        if (weapon->triggers[trigger_index].state==0 && trigger_definition->behavior==5)
            result=true;
        else if (weapon->triggers[trigger_index].state==0 && trigger_definition->behavior==1)
            result=true;
    }
    return result;
}

static __forceinline bool object_has_hidden_flags(long index)
{
    s_object_header *object_header=object_header_get(index);
    return (object_header->flags&16)!=0 || ((unsigned long)object_header->datum->object_flags&1)!=0;
}
static __forceinline bool aim_assist_object_in_cone(long index,const real_point3d *point,const real_vector3d *direction,float range,float sine,float cosine)
{
    object_datum *object=(object_datum *)object_get_and_verify_type(index, _object_mask_all);
    return sphere_intersects_cone3d(&object->bounding_center,object->bounding_radius,point,direction,range,sine,cosine);
}





// @address 0xa2c28 ?aim_assist_targeting_encode@@YAXPAVc_bitstream@@PBUs_aim_assist_targeting_result@@@Z
void aim_assist_targeting_encode(c_bitstream *bitstream, const s_aim_assist_targeting_result *aim_assist)
{
    if ((aim_assist->flags & 2) || aim_assist->target_object_index != -1)
    {
        bitstream->write_bool("autoaim-exists", true);
        float zero = real_constant_pool_zero();
        float primary = aim_assist->primary_autoaim_level > zero ? aim_assist->primary_autoaim_level : zero;
        float secondary = aim_assist->secondary_autoaim_level > zero ? aim_assist->secondary_autoaim_level : zero;
        float one = real_constant_pool_one();
        bitstream->write_quantized_real("primary-autoaim-level", primary, zero, one, 4, false, false);
        bitstream->write_quantized_real("secondary-autoaim-level", secondary, zero, one, 4, false, false);
        bitstream->write_integer("flags", aim_assist->flags, 3);
        if (aim_assist->flags & 4)
#if defined(_XBOX) && !defined(__clang__)
            // match: 0xa2c28: 15.0f is read at +0x9c0 off the r29 pool base the zero loads share
            // (a literal drops r29 and shrinks the frame)
            bitstream->write_vector("lead-vector", &aim_assist->lead_vector, k_real_hundredth_storage,
                k_real_constant_pool_one[624], 8);
#else
            bitstream->write_vector("lead-vector", &aim_assist->lead_vector, k_real_hundredth_storage, 15.0f, 8);
#endif
        bitstream->write_integer("target-object", aim_assist->target_object_index, 32);
        if (aim_assist->target_object_index != -1)
        {
            bitstream->write_bool("model-target-exists", aim_assist->model_target != -1);
            if (aim_assist->model_target != -1)
                bitstream->write_integer("model-target", aim_assist->model_target, 5);
        }
    }
    else
    {
        bitstream->write_bool("autoaim-exists", false);
    }
    bitstream->write_bool("target-player-exists", aim_assist->target_player_index != -1);
    if (aim_assist->target_player_index != -1)
    {
        long absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(aim_assist->target_player_index);
        bitstream->write_integer("target-player", absolute_index, 4);
    }
}

// @address 0x99f30 ?aim_assist_get_target_position@@YAXJJPATreal_point3d@@@Z
void aim_assist_get_target_position(long object_index, long target_index, union real_point3d * point)
{
    bool found = false;

    memcpy(point, &k_real_math_origin3d, sizeof(*point));
    if (target_index != -1 && object_index != -1)
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
        s_object_definition *definition = (s_object_definition *)tag_get(0x6f626a65, object->definition_index);
        if (definition->model.index != -1)
        {
            s_model_definition *model = (s_model_definition *)tag_get(0x686c6d74, definition->model.index);
            s_model_target *target = (s_model_target *)tag_block_get_element_with_size(&model->targets, target_index,
    sizeof(s_model_target));
            object_marker markers[2];
            short model_target_marker_count = object_get_markers_by_string_id(object_index, target->marker_name, markers, 2);
            if (model_target_marker_count > 0)
            {
                object_marker *marker = &markers[0];
                *point = marker->matrix.position;
                found = true;
            }
        }
    }

    if (object_index != -1 && !found)
    {
        object_get_origin(object_index, point);
    }
}

// 0x9d338
inline float compute_composite_attenuation(float distance, float max_distance, float angle, float min_angle, float max_angle)
{
    float start_angle = min_angle > max_angle - 0.01f ? max_angle - 0.01f : min_angle;
    return aim_assist_compute_attenuation(distance, max_distance * 0.5f, max_distance) *
        aim_assist_compute_attenuation(angle, start_angle, max_angle);
}

// @address 0x8c4270 g_aim_assist_parameters
const aim_assist_parameters *g_aim_assist_parameters;

// @address 0x279120 ?aim_assist_compute@@YA_NPBUaim_assist_parameters@@PBTreal_point3d@@PBTreal_vector3d@@JW4e_game_team@@JPAUs_aim_assist_target@@@Z
bool aim_assist_compute(const aim_assist_parameters *parameters, const real_point3d *point, const real_vector3d *direction,
    long object_index, e_game_team team, long flag, s_aim_assist_target *target)
{
    bool result=false;
    s_cluster_reference cluster=inl_scenario_cluster_reference_from_point(point);
    c_static_array<s_aim_assist_target,64> targets;
    if (cluster_reference_valid(&cluster))
    {
        long target_count=find_aim_assist_targets(parameters,cluster,point,direction,object_index,team,flag,targets);
        if (target_count>0)
        {
            g_aim_assist_parameters=parameters;
            qsort(targets.get_elements(),target_count,sizeof(s_aim_assist_target),compare_targets);
            g_aim_assist_parameters=0;
            for (long target_index=0;target_index<target_count;target_index++)
            {
                if (aim_assist_clear_line_of_sight(point, &targets[target_index].position, targets[target_index].radius, team,
                        targets[target_index].object_index, parameters->melee_only))
                {
                    memcpy(target,&targets[target_index],sizeof(*target));
                    result=true;
                    break;
                }
            }
        }
    }
    return result;
}
// @address 0x279258 ?find_aim_assist_targets@@YAJPBUaim_assist_parameters@@Us_cluster_reference@@PBTreal_point3d@@PBTreal_vector3d@@JW4e_game_team@@JAAV?$c_static_array@Us_aim_assist_target@@$0EA@@@@Z
long find_aim_assist_targets(const aim_assist_parameters *parameters, s_cluster_reference cluster, const real_point3d *point,
    const real_vector3d *direction, long object_index, e_game_team team, long flag, c_static_array<s_aim_assist_target, 64> &targets)
{
    long target_count=0;
    float range=MAX(parameters->autoaim_range,MAX(parameters->secondary_autoaim_range,parameters->magnetism_range));
    float angle=MAX(parameters->autoaim_angle,MAX(parameters->secondary_autoaim_angle,parameters->magnetism_angle));
    if (range>0.0f && angle>0.0f)
    {
        float angle_sine=(float)sin(angle);
        float angle_cosine=(float)cos(angle);
        s_cluster_reference clusters[256];
        long cluster_count=structure_clusters_in_cone(cluster,point,direction,range,angle_sine,angle_cosine,256,clusters,0);
        long objects[2048];
        long object_count=objects_in_clusters_by_indices(1,1,512,cluster_count,clusters,2048,objects);
        for (long object_number=0;object_number<object_count;object_number++)
        {
            long target_object_index=objects[object_number];
            target_count = find_aim_assist_targets_recursive(parameters, target_object_index, point, direction, range, angle_sine,
                angle_cosine, object_index, team, flag, target_count, targets);
            if (target_count>=64) break;
        }
    }
    return target_count;
}
// @address 0x279418 ?find_aim_assist_targets_recursive@@YAJPBUaim_assist_parameters@@JPBTreal_point3d@@PBTreal_vector3d@@MMMJW4e_game_team@@JJAAV?$c_static_array@Us_aim_assist_target@@$0EA@@@@Z
long find_aim_assist_targets_recursive(const aim_assist_parameters *parameters, long object_index, const real_point3d *point,
    const real_vector3d *direction, float range, float sine, float cosine, long source_index, e_game_team team, long flag,
    long target_count, c_static_array<s_aim_assist_target, 64> &targets)
{
    do
    {
        object_datum *object=(object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
        bool friendly=false;
        if (aim_assist_object_in_cone(object_index,point,direction,range,sine,cosine))
        {
            if (aim_assist_object_should_be_aimed_at(object_index,source_index,team,(e_game_team)flag,&friendly))
                target_count=aim_assist_compute_targets_from_object(parameters,object_index,source_index,friendly,point,direction,target_count,targets);
            if (target_count>=64) break;
            if (object->first_child_object_index!=-1)
            {
                target_count = find_aim_assist_targets_recursive(parameters, object->first_child_object_index, point, direction, range,
                    sine, cosine, source_index, team, flag, target_count, targets);
                if (target_count>=64) break;
            }
        }
        object_index=object->next_object_index;
    } while (object_index!=-1);
    return target_count;
}
// @address 0x279580 ?aim_assist_object_should_be_aimed_at@@YA_NJJJW4e_game_team@@PA_N@Z
bool aim_assist_object_should_be_aimed_at(long object_index,long source_index,long ignored_index,e_game_team team,bool *friendly)
{
    bool result=true;
    *friendly=false;
    result=result && object_index!=ignored_index;
    result=result && !object_has_hidden_flags(object_index);
    if (result)
    {
        long type_mask=1<<object_get_type(object_index);
        if (type_mask&0x1003)
        {
            unit_datum *unit=(unit_datum *)object_get_and_verify_type(object_index,0x1003);
            unit_definition *definition=(unit_definition *)tag_get('unit',unit->object.definition_index);
            long object_team=unit->unit.team;
            result=result && !flags_test(unit->object.damage_flags,2);
            result=result && !flags_test(definition->flags,21);
            result=result && unit->unit.active_camouflage<0.9f;
            if (((1<<unit->object.object_type)&2) && object_team==-1)
                result=aim_assist_teamless_vehicle_should_be_aimed_at(object_index,source_index,ignored_index,team,friendly);
            else
                *friendly=game_team_is_friendly(team,(e_game_team)object_team);
        }
        else if (type_mask&0x800)
        {
            creature_datum *object=(creature_datum *)object_get_and_verify_type(object_index,0x800);
            result=result && !flags_test(object->object.damage_flags,2);
            *friendly=game_team_is_friendly(team,(e_game_team)object->creature.team);
        }
    }
    return result;
}

extern "C" bool aim_assist_teamless_vehicle_should_be_aimed_at(long vehicle_index,long source_index,long extra,long team,bool *friendly)
{
    bool result=false;
    object_datum *vehicle=(object_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    vehicle_definition *definition=(vehicle_definition *)tag_get(0x76656869,vehicle->definition_index);
    *friendly=false;
    if (flags_test(definition->flags,2)) {
        result=true;
    }
    else if (vehicle->parent_object_index != -1 && flags_test(vehicle->flags, 26) &&
        ((1 << object_get_type(vehicle->parent_object_index)) & 0x1003) &&
        !((1 << object_get_type(vehicle->parent_object_index)) & 0x1000))
    {
        result=aim_assist_object_should_be_aimed_at(vehicle->parent_object_index,source_index,extra,(e_game_team)team,friendly);
    } else if (aim_assist_teamless_vehicle_has_passenger(vehicle_index,team,friendly)) {
        result=true;
    } else {
        for (long child=vehicle->first_child_object_index; child!=-1;) {
            object_datum *object=(object_datum *)object_get_and_verify_type(child, _object_mask_all);
            if (flags_test(object->flags, 26) && ((1 << object->object_type) & 0x1003) &&
                aim_assist_teamless_vehicle_has_passenger(child, team, friendly))
            {
                result=true;
                break;
            }
            child=object->next_object_index;
        }
    }
    return result;
}

// match: 0x2798d0: the seat flag is read the way the unit's former private c_flags::test did (bool from shift-and-1);
// match: 0x2798d0: preserve the shift-and-1 instruction sequence for the seat flag.
static __forceinline bool aim_assist_seat_flag_test(const unit_seat *seat, long bit) { return ((seat->flags >> bit) & 1) != 0; }

extern "C" {
extern "C" bool aim_assist_teamless_vehicle_has_passenger(long vehicle_index,long team,bool *friendly_passenger) {
 unit_datum *vehicle=(unit_datum *)object_get_and_verify_type(vehicle_index,2);
 unit_definition *vehicle_definition=(unit_definition*)tag_get('vehi',vehicle->object.definition_index);
 bool result=false;
 *friendly_passenger=false;
 unit_passenger_iterator iterator;
 unit_passenger_iterator_new(vehicle_index,&iterator);
 while(::unit_passenger_iterator_next(&iterator)!=0) {
  unit_datum *passenger=(unit_datum *)object_get_and_verify_type(iterator.passenger_index,8195);
  if(iterator.seat_index!=-1) {
   if(game_team_is_friendly(team,passenger->unit.team) && iterator.seat_index!=-1) {
    unit_seat *s=(unit_seat*)tag_block_get_element_with_size(&vehicle_definition->seats,iterator.seat_index,sizeof(unit_seat));
    if(!aim_assist_seat_flag_test(s, 11)) { result=true; *friendly_passenger=true; break; }
   } else result=true;
  }
 }
 return result;
}
}

// 0x27a098
bool aim_assist_evaluate_target(const aim_assist_parameters *parameters, float strength, s_aim_assist_target *target)
{
    bool eligible = true;
    if (target->cone_angle > 0.0f) {
        real_vector3d reverse_direction;
        negate_vector3d(&target->direction, &reverse_direction);
        float cosine = dot_product3d(&reverse_direction, &target->forward);
// match: 0x27a098: keeps the call to arccosine at 0x27a128 after the dot product is clamped.
#pragma inline_depth(0)
        eligible = ::arccosine(PIN(cosine, -1.0f, 1.0f)) <= target->cone_angle;
// match: 0x27a5e0: leaving inlining disabled moves the collision line test from 0x27a718 to 0x27a730.
#pragma inline_depth()
    }
    if (eligible && strength == 0.0f)
        eligible = false;
    if (eligible) {
        unsigned long weapon_flags = parameters->weapon_flags;
        if ((weapon_flags & (1 << 3)) || (weapon_flags & (1 << 5))) {
            if (target->flags & 1) {
                float tracking_range = target->tracking_distance != 0.0f ? target->tracking_distance : parameters->tracking_range;
                if (tracking_range < target->distance)
                    eligible = false;
            } else if (!(weapon_flags & (1 << 5))) {
                eligible = false;
            }
        }
        if (target->flags & 2) {
            float tracking_range = target->tracking_distance != 0.0f ? target->tracking_distance : parameters->tracking_range;
            if (tracking_range < target->distance)
                target->flags &= ~2;
        }
    }
    if (eligible) {
        if (!target->friendly) {
            float distance = target->distance;
            float angle = target->angle;
            target->primary_score = compute_composite_attenuation(
                angle, parameters->autoaim_angle, distance, parameters->autoaim_falloff_range, parameters->autoaim_range);
            target->secondary_score = compute_composite_attenuation(angle, parameters->secondary_autoaim_angle, distance,
                parameters->secondary_autoaim_falloff_range, parameters->secondary_autoaim_range);
            target->tertiary_score = compute_composite_attenuation(angle, parameters->magnetism_angle, distance,
                                         parameters->magnetism_falloff_range, parameters->magnetism_range) *
                strength;
            if (target->primary_score == 0.0f && target->secondary_score == 0.0f && target->tertiary_score == 0.0f)
                return false;
        } else {
            float score = compute_composite_attenuation(
                target->angle, parameters->autoaim_angle, target->distance, parameters->autoaim_falloff_range,
    parameters->autoaim_range);
            target->primary_score = 0.0f;
            target->secondary_score = 0.0f;
            target->tertiary_score = 0.0f;
            if (score < 1.0f)
                return false;
        }
    }
    return eligible;
}


void pill_compute_autoaim_target(union real_point3d const *pill_base, union real_vector3d const *pill_axis, float pill_radius,
    union real_point3d const *aim_point, union real_vector3d const *aim_vector, union real_point3d *target)
{
    real_vector3d normal;
    cross_product3d(pill_axis, aim_vector, &normal);
    float normal_squared = magnitude_squared3d(&normal);
    if (normal_squared > 0.0f)
    {
        real_vector3d to_aim_point;
        real_vector3d perpendicular;
        vector_from_points3d(pill_base, aim_point, &to_aim_point);
        cross_product3d(&to_aim_point, aim_vector, &perpendicular);
        float t = dot_product3d(&perpendicular, &normal) / normal_squared;
        point_from_line3d(pill_base, pill_axis, PIN(t, 0.0f, 1.0f), target);
    }
    else
    {
        *target = *pill_base;
    }

    real_vector3d offset;
    real_vector3d perpendicular;
    vector_from_points3d(aim_point, target, &offset);
    point_from_line3d(&offset, aim_vector, -dot_product3d(&offset, aim_vector), &perpendicular);
    limit3d(&perpendicular, pill_radius);
    point_from_line3d(target, &perpendicular, -1.0f, target);
}


int compare_targets(const void *first, const void *second)
{
    const s_aim_assist_target *a = static_cast<const s_aim_assist_target *>(first);
    const s_aim_assist_target *b = static_cast<const s_aim_assist_target *>(second);
    float first_score = a->primary_score;
    float second_score = b->primary_score;

    if (g_aim_assist_parameters)
    {
        if ((g_aim_assist_parameters->weapon_flags & 32))
        {
            if (!(a->flags & 1)) first_score -= 0.5f;
            if (!(b->flags & 1)) second_score -= 0.5f;
        }
        if ((g_aim_assist_parameters->weapon_flags & 16))
        {
            if (!(a->flags & 2)) first_score -= 0.5f;
            if (!(b->flags & 2)) second_score -= 0.5f;
        }
    }

    if (first_score > second_score) return -1;
    if (first_score < second_score) return 1;
    if (a->secondary_score > b->secondary_score) return -1;
    if (a->secondary_score < b->secondary_score) return 1;
    if (a->tertiary_score > b->tertiary_score) return -1;
    if (a->tertiary_score < b->tertiary_score) return 1;
    if (a->distance < b->distance) return -1;
    if (a->distance > b->distance) return 1;
    if (a->angle < b->angle) return -1;
    if (a->angle > b->angle) return 1;
    if (!a->friendly && b->friendly) return -1;
    if (a->friendly && !b->friendly) return 1;
    return static_cast<unsigned short>(a->object_index) - static_cast<unsigned short>(b->object_index);
}


// match: noalias declaration preserves caller live registers

// match: noalias declaration preserves caller live registers

bool aim_assist_clear_line_of_sight(const real_point3d *source_point, const real_point3d *destination_point, float target_radius,
    long shooter_object_index, long destination_object_index, bool is_melee_only)
{
    int walk_object_index = destination_object_index;
    if (is_melee_only)
    {
        long shooter_havok_index = shooter_object_index == -1 ? -1 : object_get(shooter_object_index)->havok_component_index;
        long destination_havok_index = destination_object_index == -1 ? -1 : object_get(destination_object_index)->havok_component_index;
        s_havok_ray_test ray_test;
        ray_test.start = *source_point;
        vector_from_points3d(source_point, destination_point, &ray_test.vector);
        return !havok_ray_cast_in_world(&ray_test, 11, shooter_havok_index, destination_havok_index);
    }
    collision_result line_collision;
    // match: 0x27a5e0: share invalid-material initialization with the root sentinel.
    int destination_root_index = line_collision.global_material.m_index = -1;
    while (walk_object_index != -1)
    {
        destination_root_index = walk_object_index;
        walk_object_index = object_get(walk_object_index)->parent_object_index;
    }
    return !collision_test_line(k_aim_assist_los_collision_flags, source_point, destination_point,
        shooter_object_index, destination_root_index, &line_collision);
}
// @address 0x27a740 ?aim_assist_object_get_translational_velocity@@YAXJ_NPATreal_vector3d@@PIAJ22@Z
void aim_assist_object_get_translational_velocity(long object_index, bool subtract_inherited, real_vector3d *velocity,
    // match: 0x27a740: pointer __restrict qualifier prevents alias reload
    long *__restrict early_mover_index, long *__restrict support_object_index, long *__restrict support_rigid_body_index)
{
    bool success=false;
    *support_rigid_body_index=-1;
    *early_mover_index=-1;
    *support_object_index=-1;
    if ((1<<object_get_type(object_index))&1)
    {
        biped_datum *biped=(biped_datum *)object_get_and_verify_type(object_index,1);
        if (biped->object.parent_object_index==-1 && !flags_test(biped->object.physics_flags,9))
        {
            real_vector3d inherited_velocity;
            real_vector3d inherited_angular_velocity;
            memcpy(&inherited_velocity, &k_real_math_origin3d, sizeof(inherited_velocity));
            memcpy(&inherited_angular_velocity, &k_real_math_origin3d, sizeof(inherited_angular_velocity));
            real_point3d origin;
            object_get_center_of_mass(object_index,&origin);
            if (object_get_early_mover_local_space_velocity(object_index,&origin,&inherited_velocity,&inherited_angular_velocity,false,false))
                *early_mover_index=object_early_mover_local_space_index(object_index);
            else if (biped->biped.physics.get_mode()==1)
            {
                long local_object_index=biped->biped.physics.get_mode_ground()->get_support_object_index();
                if (local_object_index!=-1)
                {
                    object_datum *support=(object_datum *)object_try_and_get_and_verify_type(local_object_index,-1);
                    if (support && support->havok_component_index!=-1)
                    {
                        c_havok_component *world = datum_get_typed<c_havok_component>((s_data_array *)g_havok_component_data,
                            support->havok_component_index);
                        long local_rigid_body_index=biped->biped.physics.get_mode_ground()->get_support_rigid_body_index();
                        if (local_rigid_body_index>=0 && local_rigid_body_index<world->get_rigid_body_count())
                        {
                            object_get_origin(object_index,&origin);
                            ((c_havok_component *)world)
                                ->get_rigid_body_linear_velocity_at_point(local_rigid_body_index, &inherited_velocity,
                                    &origin);
                            *support_object_index=local_object_index;
                            *support_rigid_body_index=local_rigid_body_index;
                        }
                    }
                }
            }
            object_get_velocities(object_index,velocity,0);
            if (biped->biped.physics.get_mode()==1 && !motor_system_in_airborne_state(object_index))
            {
                const real_vector3d *ground_plane=biped->biped.physics.get_mode_ground()->get_ground_plane();
                real_vector3d normal_velocity;
                scale_vector3d(ground_plane,dot_product3d(ground_plane,velocity),&normal_velocity);
                real_vector3d inherited_normal_velocity;
                scale_vector3d(ground_plane,dot_product3d(ground_plane,&inherited_velocity),&inherited_normal_velocity);
                subtract_vectors3d(velocity,&normal_velocity,velocity);
                add_vectors3d(velocity,&inherited_normal_velocity,velocity);
            }
            if (subtract_inherited)
                subtract_vectors3d(velocity,&inherited_velocity,velocity);
            success=true;
        }
    }
    if (!success) object_get_velocities(object_index,velocity,0);
}


long aim_assist_compute_targets_from_object(const aim_assist_parameters *parameters, long object_index, long source_index,
    bool friendly,
    const real_point3d *point, const real_vector3d *direction, long target_count, c_static_array<s_aim_assist_target, 64> &targets)
{
    float strength = 1.0f;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
    s_object_definition *definition = (s_object_definition *)tag_get(0x6f626a65, object->definition_index);
    if (!parameters->melee_only || ((1 << object_get_type(object_index)) & 2) == 0)
    {
        if (definition->model.index != -1)
        {
            s_model_definition *model = (s_model_definition *)tag_get(0x686c6d74, definition->model.index);
            if ((model->flags & 0x10) != 0)
            {
                s_player_control_constants *information = (s_player_control_constants *)
                    tag_block_get_element_with_size(&((s_game_globals *)global_game_globals)->player_control, 0,
                        sizeof(s_player_control_constants));
                strength = information->aim_assist_scale;
            }
        }
        if (target_count < 64 &&
            aim_assist_compute_target_from_model(
                parameters, object_index, source_index, friendly, -1, 0, 0, 0, point, direction, &targets[target_count]) &&
            aim_assist_evaluate_target(parameters, strength, &targets[target_count]))
        {
            target_count++;
        }
        if (definition->model.index != -1)
        {
            s_model_definition *model = (s_model_definition *)tag_get(0x686c6d74, definition->model.index);
            for (long model_target_index = 0; model_target_index < model->targets.count; model_target_index++)
            {
                s_model_target *model_target = (s_model_target *)tag_block_get_element_with_size(&model->targets,
    model_target_index, sizeof(s_model_target));
                object_marker markers[2];
                short marker_count = object_get_markers_by_string_id(object_index, model_target->marker_name, markers, 2);
                if (marker_count != 0)
                {
                    object_marker *second_marker = marker_count == 2 ? &markers[1] : 0;
                    if (target_count < 64)
                    {
                        s_aim_assist_target *target = &targets[target_count];
                        if (aim_assist_compute_target_from_model(parameters, object_index, source_index, friendly,
    model_target_index, model_target,
                                &markers[0].matrix, second_marker ? &second_marker->matrix : 0, point, direction, target) &&
                            aim_assist_evaluate_target(parameters, model_target->targeting_relevance * strength, target))
                        {
                            target_count++;
                        }
                    }
                }
            }
        }
    }
    return target_count;
}

static __forceinline bool object_has_hidden_flags(const s_object_header *object_header, const unit_datum *object)
{
    return (object_header->flags & 0x10) != 0 || (object->object.flags & 1) != 0;
}
static __forceinline bool aim_assist_object_can_be_considered(long object_index)
{
    s_object_header *object_header = object_header_get(object_index);
    unit_datum *object = (unit_datum *)object_header->datum;
    long type_mask = 1 << object_header->object_type;
    bool result = true;
    result = result && (type_mask & 0x188b) != 0;
    result = result && !object_has_hidden_flags(object_header, object);
    result = result && ((type_mask & 0x1003) == 0 || object->unit.active_camouflage < 0.9f);
    return result;
}

// @address 0x27a9f8 ?aim_assist_from_targeting@@YA_NPBUaim_assist_parameters@@PBTreal_point3d@@PBTreal_vector3d@@JJW4e_game_team@@PBUs_aim_assist_targeting_result@@PAUs_aim_assist_target@@@Z
bool aim_assist_from_targeting(const aim_assist_parameters *parameters, const real_point3d *point, const real_vector3d * vector, long source_index,
    long ignored_index, e_game_team team, const s_aim_assist_targeting_result *targeting_result, s_aim_assist_target *target)
{
    long object_index;
    long model_target_index;
    const s_model_target *model_target;
    real_matrix4x3 matrix;
    real_matrix4x3 matrix2;
    bool has_second;
    bool result = false;
    if ((targeting_result->primary_autoaim_level > 0.0f || targeting_result->secondary_autoaim_level > 0.0f) &&
        aim_assist_retrieve_targeting_info(targeting_result,&object_index,&model_target_index,&model_target,&matrix,&matrix2,&has_second))
    {
        bool friendly = false;
        if (aim_assist_object_should_be_aimed_at(object_index,source_index,ignored_index,team,&friendly) &&
            aim_assist_object_can_be_considered(object_index))
        {
            const real_matrix4x3 *second_matrix = (model_target_index != -1 && has_second) ? &matrix2 : 0;
            const real_matrix4x3 *target_matrix;
            if (model_target_index != -1)
                target_matrix = &matrix;
            else
            {
                target_matrix = 0;
                model_target = 0;
            }
            if (aim_assist_compute_target_from_model(parameters, object_index, source_index,
    friendly, model_target_index, model_target,
                    target_matrix, second_matrix, point, vector, target))
            {
                if (!friendly)
                {
                    target->primary_score = targeting_result->primary_autoaim_level;
                    target->secondary_score = targeting_result->secondary_autoaim_level;
                }
                else
                {
                    target->primary_score = 0.0f;
                    target->secondary_score = 0.0f;
                }
                target->tertiary_score = 0.0f;
                if (aim_assist_clear_line_of_sight(point,&target->position,target->radius,ignored_index,target->object_index,false))
                    result = true;
            }
        }
    }
    return result;
}

// @address 0x27ac38 aim_assist_retrieve_targeting_info
extern "C" bool aim_assist_retrieve_targeting_info(const s_aim_assist_targeting_result * targeting_result,
    // match: 0x27ac38: pointer __restrict qualifier prevents alias reload
    long *__restrict object_index, long *__restrict model_target_index, const s_model_target **__restrict model_target,
    real_matrix4x3 * matrix, real_matrix4x3 * matrix2, bool * has_second)
{
    bool result = false;

    if (targeting_result->target_object_index != -1 &&
        object_try_and_get_unsafe_and_verify_type(targeting_result->target_object_index, (unsigned long)-1) != 0 &&
        (targeting_result->flags & 2) == 0)
    {
        *object_index = targeting_result->target_object_index;
        *model_target_index = targeting_result->model_target;
        if (*model_target_index == -1)
        {
            result = true;
        }
        else
        {
            object_datum *object = (object_datum *)object_get_and_verify_type(*object_index, _object_mask_all);
            s_object_definition *definition = (s_object_definition *)tag_get(0x6f626a65, object->definition_index);
            if (definition->model.index != -1)
            {
                s_model_definition *model = (s_model_definition *)tag_get(0x686c6d74, definition->model.index);
                if (*model_target_index >= 0 && *model_target_index < model->targets.count)
                {
                    *model_target = (const s_model_target *)tag_block_get_element_with_size(&model->targets, *model_target_index,
    sizeof(s_model_target));
                    object_marker markers[2];
                    short marker_count = object_get_markers_by_string_id(*object_index, (*model_target)->marker_name, markers, 2);
                    if (marker_count != 0)
                    {
                        *matrix = markers[0].matrix;
                        result = true;
                        if (marker_count == 2)
                        {
                            *matrix2 = markers[1].matrix;
                            *has_second = true;
                        }
                        else
                        {
                            *has_second = false;
                        }
                    }
                }
            }
        }
    }

    return result;
}

// @address 0x27ad98 ?unit_ignore_object_index_get@@YAJJ@Z
long unit_ignore_object_index_get(long unit_index)
{
    long aiming_unit_index=unit_get_aiming_unit_index(unit_index);
    long result=object_root_parent(aiming_unit_index);
    long weapon_index=unit_get_active_primary_weapon(aiming_unit_index,0);
    if (weapon_index!=-1)
    {
        long parent_index=((object_datum *)object_get_and_verify_type(weapon_index, _object_mask_all))->parent_object_index;
        if (parent_index!=unit_index)
        {
            long previous_object_index=weapon_index;
            long object_index=parent_index;
            while (object_index!=-1)
            {
                if (!((1<<object_get_type(object_index)) & 2)) break;
                object_datum *object=(object_datum *)object_get_and_verify_type(object_index, _object_mask_vehicle);
                vehicle_definition *definition=(vehicle_definition *)tag_get(0x76656869,object->definition_index);
                if (flags_test(definition->flags,11))
                {
                    result=previous_object_index;
                    break;
                }
                previous_object_index=object_index;
                object_index=object->parent_object_index;
            }
        }
    }
    return result;
}

// @address 0x278c28 ?unit_get_aim_assist_parameters@@YA_NJV?$c_flags@W4e_aim_assist_flags@@E$01@@FPAUaim_assist_parameters@@@Z
bool unit_get_aim_assist_parameters(long unit_index, c_flags<e_aim_assist_flags, unsigned char, 2> flags,
    short zoom_level, aim_assist_parameters *parameters)
{
    unit_datum *unit;
    long primary_weapon_index;
    bool success = false;
    do
    {
    memset(parameters, 0, sizeof(*parameters));
    if (unit_index == -1)
        break;
    s_object_header *object_headers = ((s_object_header *)object_header_data->data);
    unit = (unit_datum *)object_headers[DATUM_INDEX_TO_ABSOLUTE_INDEX(unit_index)].datum;
    primary_weapon_index = unit_inventory_weapon_from_datum(unit, unit->unit.current_weapon_indices[0]);
    if (primary_weapon_index == -1)
        break;
    weapon_datum *weapon = (weapon_datum *)object_headers[DATUM_INDEX_TO_ABSOLUTE_INDEX(primary_weapon_index)].datum;
    weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->definition_index);
    parameters->flags = flags;
    parameters->melee_only = weapon_is_melee_only(primary_weapon_index);
    if (object_get_type(unit_index) != 1
        || !weapon_get_trigger_position(primary_weapon_index, 0, &parameters->position))
        object_get_origin(unit_index, &parameters->position);
    if (parameters->melee_only)
    {
        if (!weapon_can_be_fired(primary_weapon_index) || ((object_datum *)unit)->parent_object_index != -1)
            break;
    }
    if (zoom_level == -1 && flags_test(definition->flags, 5) && !flags.test(_aim_assist_secondary_bit))
        break;
    float magnification = weapon_get_zoom_magnification(primary_weapon_index, zoom_level);
    float autoaim_angle = definition->autoaim_angle;
    float inverse_magnification = 1.0f / magnification;
    parameters->autoaim_angle = autoaim_angle * inverse_magnification;
    parameters->autoaim_range = definition->autoaim_range * magnification;
    parameters->autoaim_falloff_range = definition->autoaim_falloff_range * magnification;
    parameters->magnetism_angle = definition->magnetism_angle * inverse_magnification;
    parameters->magnetism_range = definition->magnetism_range * magnification;
    parameters->magnetism_falloff_range = definition->magnetism_falloff_range * magnification;
    if ((definition->secondary_flags & 1) != 0 && magnification == 1.0f)
    {
        parameters->magnetism_angle = 0.0f;
        parameters->magnetism_range = 0.0f;
        parameters->magnetism_falloff_range = 0.0f;
    }
    float deviation_angle = definition->deviation_angle;
    if (!flags_test(definition->flags, 31))
        deviation_angle = MAX(deviation_angle, definition->autoaim_angle);
    parameters->deviation_angle = deviation_angle * inverse_magnification;
    if (flags_test(definition->flags, 31))
        parameters->weapon_flags |= 0x40;
    else
        parameters->weapon_flags &= ~0x40;
    if (flags.test(_aim_assist_secondary_bit))
    {
        parameters->autoaim_angle = 0.17453292f;
        parameters->autoaim_range = MAX(parameters->autoaim_range, 1000.0f);
        parameters->autoaim_falloff_range = 0.0f;
    }
    if (aim_assist_weapon_is_looking_for_tracking_target(primary_weapon_index))
    {
        if (definition->tracking_type == 1)
        {
            parameters->weapon_flags |= 0x20;
            parameters->tracking_range = parameters->autoaim_range;
        }
    }
    else if (weapon_is_tracking(primary_weapon_index))
    {
        short tracking_type = (short)definition->tracking_type;
        if (tracking_type == 1)
        {
            parameters->weapon_flags |= 8;
            parameters->tracking_range = parameters->autoaim_range;
        }
        else if (tracking_type == 2)
        {
            parameters->weapon_flags |= 0x10;
            parameters->tracking_range = parameters->autoaim_range;
        }
    }
    unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    long secondary_weapon_index = unit_inventory_weapon_from_datum(unit, unit->unit.current_weapon_indices[1]);
    if (secondary_weapon_index != -1)
    {
        weapon_datum *secondary_weapon = (weapon_datum *)object_get_and_verify_type(secondary_weapon_index, 4);
        weapon_definition *secondary_definition = (weapon_definition *)tag_get('weap', secondary_weapon->definition_index);
        float secondary_magnification = weapon_get_zoom_magnification(secondary_weapon_index, zoom_level);
        float secondary_inverse_magnification = 1.0f / secondary_magnification;
        parameters->secondary_autoaim_angle = secondary_definition->autoaim_angle;
        parameters->secondary_autoaim_range = secondary_definition->autoaim_range;
        parameters->secondary_autoaim_falloff_range = secondary_definition->autoaim_falloff_range;
        parameters->magnetism_angle = MAX(parameters->magnetism_angle,
    secondary_definition->magnetism_angle * secondary_inverse_magnification);
        parameters->magnetism_range = MAX(parameters->magnetism_range,
    secondary_definition->magnetism_range * secondary_magnification);
        parameters->magnetism_falloff_range = MAX(parameters->magnetism_falloff_range,
    secondary_definition->magnetism_falloff_range * secondary_magnification);
        if ((secondary_definition->secondary_flags & 1) != 0 && secondary_magnification == 1.0f)
        {
            parameters->magnetism_angle = 0.0f;
            parameters->magnetism_range = 0.0f;
            parameters->magnetism_falloff_range = 0.0f;
        }
        if (flags_test(secondary_definition->flags, 31))
            parameters->deviation_angle = secondary_definition->deviation_angle * secondary_inverse_magnification;
        else
            parameters->deviation_angle = MAX(parameters->deviation_angle,
                MAX(secondary_definition->deviation_angle, secondary_definition->autoaim_angle) * secondary_inverse_magnification);
    }
    success = true;
    } while (false);
    return success;
}

// @address 0x9b368 ?player_aim_projectile@@YA_NJPBUs_aim_assist_targeting_result@@J_NPBTreal_point3d@@2PATreal_vector3d@@@Z
// @address 0x783394 k_aim_assist_seated_min_angle
float k_aim_assist_seated_min_angle = 0.366519153f;






// Keep the two-argument shape: player_aim_projectile needs this inline temp structure.
static __forceinline player_datum *player_from_data(s_data_array *data, long player_index)
{
    player_datum *datum = &((player_datum *)data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)];
    return datum;
}
static __forceinline bool collision_test_vector_enter(s_collision_test_flags flags, const real_point3d *point,
    const real_vector3d *vector, long first_ignore_object_index, long second_ignore_object_index, collision_result *collision)
{
    bool hit = collision_test_vector(flags, false, point, vector, first_ignore_object_index, second_ignore_object_index, -1,
    collision);
    return hit;
}
static __forceinline float aim_assist_projectile_get_autoaim_max_lead_time(long definition_index)
{
    const void *tag_definition = tag_get('proj', definition_index);
    const projectile_definition *definition = (const projectile_definition *)tag_definition;
    return definition->autoaim_leading_max_lead_time;
}
static __forceinline void aim_assist_flags_set(unsigned long *flags, long bit, bool enable)
{
    unsigned long mask = 1UL << bit;
    if (enable)
        *flags |= mask;
    else
        *flags &= ~mask;
}
// match: 0x9b368: the unit's former private c_flags::set named its mask; with the canonical set the float ties of
// player_aim_projectile flip (check 3). This copy of the old set keeps that local for the one call there.
static __forceinline void aim_assist_parameter_flags_set(c_flags<e_aim_assist_flags, unsigned char, 2> *flags,
    e_aim_assist_flags bit, bool enable)
{
    unsigned char mask = (unsigned char)(1 << bit);
    if (enable)
        flags->set_unsafe(flags->get_unsafe() | mask);
    else
        flags->set_unsafe(flags->get_unsafe() & ~mask);
}

static __forceinline long aim_assist_director_camera_deterministic(long unit_index, real_point3d *position, real_vector3d *forward)
{
    long seat_state;
    long perspective = director_game_desired_perspective(unit_index, (e_seat_state *)&seat_state);
    if (perspective == 0)
        first_person_camera_deterministic(unit_index, position, forward);
    else
        following_camera_deterministic(unit_index, position, forward);
    return perspective;
}
static __forceinline float sine_f64(float angle) { return (float)sin((double)angle); }
static __forceinline float cosine_f64(float angle) { return (float)cos((double)angle); }

static __forceinline long inl_game_time_get()
{
    const game_time_globals_definition *game_time = TLS_GAME_TIME_GLOBALS;
    return game_time->time;
}
static __forceinline s_object_header *inl_object_header_get(long object_index)
{
    s_object_header *object_headers = ((s_object_header *)object_header_data->data);
    return &object_headers[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)];
}
static __forceinline void *inl_object_get(long object_index)
{
    s_object_header *object_header = inl_object_header_get(object_index);
    return object_header->datum;
}
static __forceinline void *inl_object_get_and_verify(long object_index, unsigned long type_mask)
{
    void *object = inl_object_get(object_index);
    return object;
}
static __forceinline short unit_zoom_level(long unit_index)
{
    const unit_datum *unit = (const unit_datum *)inl_object_get_and_verify(unit_index, 8195);
    return unit->unit.zoom_level;
}
static __forceinline bool aim_assist_player_is_seated(long player_index)
{
    bool result = true;
    player_datum *player = player_get(player_index);
    long unit_index = player->unit_index;
    if (unit_index != -1)
    {
        const unit_datum *unit = (const unit_datum *)inl_object_get_and_verify(unit_index, 8195);
        if (unit->object.parent_object_index == -1 || unit->unit.parent_seat_index == -1)
            result = false;
    }
    return result;
}
bool player_aim_projectile(long player_index, const s_aim_assist_targeting_result *targeting, long projectile_definition_index,
    bool use_primary, const real_point3d *position, const real_point3d *target_position, real_vector3d *aim_direction)
{
    player_datum *player = player_from_data(player_data, player_index);
    long aiming_unit_index = unit_get_aiming_unit_index(player->unit_index);
    long source_object_index;
    if (aiming_unit_index != -1)
        source_object_index = object_accepted_local_physics_object_get(object_root_parent(aiming_unit_index));
    else
        source_object_index = -1;
    long best_target_index = -1;
    bool valid = false;
    aim_assist_parameters parameters;
    c_flags<e_aim_assist_flags, unsigned char, 2> flags;
    if ((targeting->flags & 1) != 0)
        aim_assist_parameter_flags_set(&flags, _aim_assist_secondary_bit, true);
    if (unit_get_aim_assist_parameters(aiming_unit_index, flags, unit_zoom_level(aiming_unit_index), &parameters))
    {
        real_point3d camera_position;
        real_vector3d camera_forward;
        aim_assist_director_camera_deterministic(player->unit_index, &camera_position, &camera_forward);
        if ((targeting->flags & 4) != 0 && targeting->target_object_index != -1 &&
            object_try_and_get_and_verify_type(targeting->target_object_index, (unsigned long)-1) != 0)
        {
            real_point3d target_point;
            real_point3d led_target_point;
            aim_assist_get_target_position(targeting->target_object_index, targeting->model_target, &target_point);
            point_from_line3d(&target_point, &targeting->lead_vector, 1.0f, &led_target_point);
            vector_from_points3d(&camera_position, &led_target_point, &camera_forward);
            normalize3d(&camera_forward);
        }
        real_vector3d seek_direction = *aim_direction;
        real_vector3d lead_direction = seek_direction;
        float blend_factor = 0.0f;
        long target_index = -1;
        s_aim_assist_target target;
        if (aim_assist_from_targeting(&parameters, &camera_position, &camera_forward, source_object_index,
                unit_ignore_object_index_get(player->unit_index), (e_game_team)player->configuration.host.team_index, targeting, &target) &&
            !target.friendly)
        {
            vector_from_points3d(position, &target.position, &seek_direction);
            if (normalize3d(&seek_direction) == 0.0f)
                seek_direction = *aim_direction;
            blend_factor = use_primary ? target.primary_score : target.secondary_score;
            target_index = target.object_index;
            if (projectile_definition_index != -1)
            {
                float time_to_target = projectile_estimate_time_to_target(projectile_definition_index, target.distance);
                float max_lead_time = aim_assist_projectile_get_autoaim_max_lead_time(projectile_definition_index);
                float lead_time = max_lead_time > time_to_target ? time_to_target : max_lead_time;
                real_point3d lead_point;
                point_from_line3d(&target.position, &target.velocity, lead_time, &lead_point);
                vector_from_points3d(position, &lead_point, &lead_direction);
                if (normalize3d(&lead_direction) == 0.0f)
                    lead_direction = seek_direction;
            }
        }
        s_collision_test_flags test_flags;
        test_flags.object_flags = k_projectiles_collision_test_flags.object_flags;
        test_flags.collision_flags = k_projectiles_collision_test_flags.collision_flags;
        aim_assist_flags_set(&test_flags.collision_flags, 21, true);
        aim_assist_flags_set(&test_flags.collision_flags, 13, true);
        collision_result collision;
        real_vector3d to_parameter_position;
        vector_from_points3d(&camera_position, &parameters.position, &to_parameter_position);
        float start_distance = 0.9f * dot_product3d(&to_parameter_position, &camera_forward);
        start_distance = MAX(start_distance, 0.0f);
        real_point3d test_point;
        real_vector3d test_vector;
        point_from_line3d(&camera_position, &camera_forward, start_distance, &test_point);
        scale_vector3d(&camera_forward, k_aim_assist_camera_test_distance, &test_vector);
        long ignore_object_index = unit_ignore_object_index_get(player->unit_index);
        collision_test_vector_enter(test_flags, &test_point, &test_vector, ignore_object_index, -1, &collision);
        float collision_distance = collision.fraction * k_aim_assist_camera_test_distance;
        float near_distance = 2.0f;
        float far_distance = 3.0f;
        real_point3d origin = *position;
        if (collision_distance < far_distance)
        {
            float t = (collision_distance - near_distance) / (far_distance - near_distance);
            t = PIN(t, 0.0f, 1.0f);
            real_vector3d origin_vector;
            vector_from_points3d(position, target_position, &origin_vector);
            point_from_line3d(&origin, &origin_vector, 1.0f - t, &origin);
        }
        real_vector3d to_hit;
        vector_from_points3d(&origin, &collision.point, &to_hit);
        if (normalize3d(&to_hit) == 0.0f)
            to_hit = *aim_direction;
        real_vector3d lead_delta;
        subtract_vectors3d(&lead_direction, &seek_direction, &lead_delta);
        real_vector3d lead_normal = lead_delta;
        float lead_length = normalize3d(&lead_normal);
        real_vector3d clamped_direction;
        if (lead_length > 0.0001f)
        {
            real_vector3d hit_delta;
            subtract_vectors3d(&to_hit, &seek_direction, &hit_delta);
            float projection = dot_product3d(&hit_delta, &lead_normal);
            if (projection <= 0.0f)
                clamped_direction = seek_direction;
            else if (projection >= lead_length)
                clamped_direction = lead_direction;
            else
            {
                point_from_line3d(&seek_direction, &lead_delta, projection / lead_length, &clamped_direction);
                if (normalize3d(&clamped_direction) == 0.0f)
                    clamped_direction = seek_direction;
            }
        }
        else
        {
            clamped_direction = seek_direction;
        }
        seek_direction = clamped_direction;
        real_vector3d result_direction;
        fast_normals_interpolate(&to_hit, &seek_direction, blend_factor, &result_direction);
        bool aim_valid;
        if (aim_assist_player_is_seated(player_index))
        {
            float max_angle = MAX(k_aim_assist_seated_min_angle, parameters.deviation_angle);
            aim_valid = !pin_normal_to_cone3d(&result_direction, aim_direction, sine_f64(max_angle), cosine_f64(max_angle), aim_direction);
        }
        else
        {
            if ((parameters.weapon_flags & 0x40) != 0)
            {
                float deviation_angle = parameters.deviation_angle;
                pin_normal_to_cone3d(&result_direction, &to_hit, sine_f64(deviation_angle), cosine_f64(deviation_angle), &result_direction);
            }
            *aim_direction = result_direction;
            aim_valid = true;
        }
        if (blend_factor >= 1.0f && aim_valid)
        {
            valid = true;
            best_target_index = target_index;
        }
        if (valid)
        {
            player->last_shot_at_object_index = best_target_index;
            player->last_shot_at_time = inl_game_time_get();
        }
    }
    return valid;
}




// @rdata 0x820221fc 4
static const float aim_assist_one = 1.0f;
// @rdata 0x82022bbc 4
static const float aim_assist_maximum_lead_distance = 15.0f;
// @rdata 0x820265dc 4
static const float aim_assist_lead_distance_scale = 0.9f;
// @address 0x99800 aim_assist_compute_control
extern "C" void aim_assist_compute_control(long input_user_index, c_flags<e_aim_assist_flags, unsigned char, 2> flags,
    // match: 0x99800: pointer __restrict qualifier prevents alias reload
    const real_vector3d *aiming_vector, short zoom_level, s_aim_assist_control_result *__restrict out_aim_assist_control,
    s_aim_assist_targeting_result *out_aim_assist_targeting)
{
    memset(out_aim_assist_control, 0, sizeof(*out_aim_assist_control));
    aim_assist_targeting_clear(out_aim_assist_targeting);
    if (flags.test(_aim_assist_secondary_bit))
        out_aim_assist_targeting->flags |= 1;
    long player_index = -1;
    if (input_user_index != -1)
        player_index = player_mapping_globals->player_user_mapping[input_user_index];
    player_datum *player = datum_get_typed<player_datum>(player_data, player_index);
    long aiming_unit_index = unit_get_aiming_unit_index(player->unit_index);
    long source_object_index;
    if (aiming_unit_index != -1)
        source_object_index = object_accepted_local_physics_object_get(object_root_parent(aiming_unit_index));
    else
        source_object_index = -1;
    aim_assist_parameters parameters;
    if (unit_get_aim_assist_parameters(aiming_unit_index, flags, zoom_level, &parameters))
    {
        long ignore_object_index = unit_ignore_object_index_get(player->unit_index);
        real_point3d camera_position;
        real_vector3d camera_forward;
        aim_assist_director_camera_deterministic(player->unit_index, &camera_position, &camera_forward);
        real_vector3d camera_to_position;
        vector_from_points3d(&camera_position, &parameters.position, &camera_to_position);
        float distance = dot_product3d(&camera_to_position, aiming_vector) * aim_assist_lead_distance_scale;
        real_point3d aim_point;
        point_from_line3d(&camera_position, aiming_vector, distance, &aim_point);
        s_aim_assist_target target;
        if (aim_assist_compute(&parameters, &aim_point, aiming_vector, source_object_index,
                (e_game_team)ignore_object_index, player->configuration.host.team_index, &target))
        {
            out_aim_assist_targeting->primary_autoaim_level = target.primary_score;
            out_aim_assist_targeting->secondary_autoaim_level = target.secondary_score;
            out_aim_assist_control->scale = (parameters.weapon_flags & 2) != 0 ? aim_assist_one : target.tertiary_score;
            real_vector3d unit_velocity;
            long unit_early_mover_index;
            long unit_support_object_index;
            long unit_support_rigid_body_index;
            aim_assist_object_get_translational_velocity(player->unit_index, false, &unit_velocity, &unit_early_mover_index,
                &unit_support_object_index, &unit_support_rigid_body_index);
            real_vector3d target_velocity;
            long target_early_mover_index;
            long target_support_object_index;
            long target_support_rigid_body_index;
            aim_assist_object_get_translational_velocity(target.object_index, false, &target_velocity, &target_early_mover_index,
                &target_support_object_index, &target_support_rigid_body_index);
            if ((unit_early_mover_index != -1 || unit_support_object_index != -1) &&
                unit_early_mover_index == target_early_mover_index &&
                unit_support_object_index == target_support_object_index &&
                unit_support_rigid_body_index == target_support_rigid_body_index)
            {
                aim_assist_object_get_translational_velocity(player->unit_index, true, &unit_velocity, &unit_early_mover_index,
                    &unit_support_object_index, &unit_support_rigid_body_index);
                aim_assist_object_get_translational_velocity(target.object_index, true, &target_velocity, &target_early_mover_index,
                    &target_support_object_index, &target_support_rigid_body_index);
            }
            if (unit_early_mover_index != -1 && source_object_index == object_root_parent(target.object_index))
            {
                aim_assist_object_get_translational_velocity(player->unit_index, true, &unit_velocity, &unit_early_mover_index,
                    &unit_support_object_index, &unit_support_rigid_body_index);
                target_velocity.i = 0.0f;
                target_velocity.j = 0.0f;
                target_velocity.k = 0.0f;
            }
            real_vector3d relative_velocity;
            subtract_vectors3d(&target_velocity, &unit_velocity, &relative_velocity);
            const real_vector3d *direction = &target.direction_to_target;
            const real_vector3d *velocity = &relative_velocity;
            float horizontal_squared = direction->i * direction->i + direction->j * direction->j;
            if (horizontal_squared > 0.0001f)
            {
                float horizontal = square_root(horizontal_squared);
                out_aim_assist_control->yaw = (direction->i * velocity->j - direction->j * velocity->i) / horizontal_squared;
                out_aim_assist_control->pitch =
                    (horizontal * velocity->k - (direction->i * velocity->i + direction->j * velocity->j) / horizontal * direction->k) /
                    (horizontal_squared + direction->k * direction->k);
            }
            else
            {
                out_aim_assist_control->yaw = 0.0f;
                out_aim_assist_control->pitch = 0.0f;
            }
            if (out_aim_assist_targeting->primary_autoaim_level > 0.0f || out_aim_assist_targeting->secondary_autoaim_level > 0.0f || target.friendly)
            {
                out_aim_assist_targeting->target_object_index = target.object_index;
                out_aim_assist_targeting->model_target = target.model_target;
            }
            if (out_aim_assist_targeting->target_object_index != -1)
            {
                if (((1 << object_get_type(out_aim_assist_targeting->target_object_index)) & 0x1003) != 0)
                {
                    const unit_datum *unit = (const unit_datum *)object_get_and_verify_type(
                        out_aim_assist_targeting->target_object_index, 0x1003);
                    out_aim_assist_targeting->target_player_index = unit->unit.player_index;
                }
                if (flags.test(_aim_assist_multiplayer_bit))
                {
                    real_point3d target_position;
                    aim_assist_get_target_position(out_aim_assist_targeting->target_object_index,
    out_aim_assist_targeting->model_target, &target_position);
                    real_vector3d normal;
                    vector_from_points3d(&target_position, &aim_point, &normal);
                    normalize3d(&normal);
                    real_plane3d plane;
                    plane3d_from_point_and_normal(&plane, &target_position, &normal);
                    float intersection_time = vector_intersect_plane3d(&aim_point, aiming_vector, &plane);
                    real_point3d intersection;
                    point_from_line3d(&aim_point, aiming_vector, intersection_time, &intersection);
                    real_vector3d lead_vector;
                    vector_from_points3d(&target_position, &intersection, &lead_vector);
                    if (!(square_root(magnitude_squared3d(&lead_vector)) > aim_assist_maximum_lead_distance))
                    {
                        out_aim_assist_targeting->lead_vector = lead_vector;
                        out_aim_assist_targeting->flags |= 4;
                    }
                }
            }
        }
    }
}

static __forceinline bool aim_assist_target_from_biped(
    long biped_index, const real_point3d *aim_point, const real_vector3d *aim_vector, real_point3d *target_point)
{
    bool result = false;
    real_point3d pill_base;
    real_vector3d pill_axis;
    float pill_radius;
    biped_get_autoaim_pill(biped_index, &pill_base, &pill_axis, &pill_radius);
    if (pill_radius > 0.0001f)
    {
        pill_compute_autoaim_target(&pill_base, &pill_axis, pill_radius, aim_point, aim_vector, target_point);
        result = true;
    }
    return result;
}
static __forceinline void sphere_compute_autoaim_target(
    const real_point3d *center, float radius, const real_point3d *aim_point, const real_vector3d *aim_vector, real_point3d *target)
{
    *target = *center;
    real_vector3d offset;
    vector_from_points3d(aim_point, target, &offset);
    real_vector3d perpendicular;
    point_from_line3d(&offset, aim_vector, -dot_product3d(&offset, aim_vector), &perpendicular);
    limit3d(&perpendicular, radius);
    point_from_line3d(target, &perpendicular, -1.0f, target);
}

// @address 0x279c68 aim_assist_compute_target_from_model
extern "C" bool aim_assist_compute_target_from_model(const aim_assist_parameters *input_parameters, long object_index, long source_index,
    bool friendly,
    long model_target_index, const s_model_target *model_target, const real_matrix4x3 *matrix, const real_matrix4x3 *matrix2,
    const real_point3d *point, const real_vector3d *vector, s_aim_assist_target *target)
{
    // match: 0x279c68: retain the parameter local for the accepted FMA shape.
    const aim_assist_parameters *parameters = input_parameters;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
    bool result = false;
    target->object_index = object_index;
    target->model_target = model_target_index;
    target->tracking_distance = 0.0f;
    target->flags = 0;
    object_get_velocities(object_index, &target->velocity, 0);
    if (model_target_index == -1)
    {
        if (((1 << object->object_type) & 1) && aim_assist_target_from_biped(object_index, point, vector, &target->position))
        {
            biped_definition *definition = (biped_definition *)tag_get('bipd', object->definition_index);
            if (definition->autoaim_flags & 1)
                target->flags |= 1;
            else
                target->flags &= ~1;
            if ((definition->autoaim_flags & 2) == 0 && (definition->autoaim_flags & 4) == 0)
                target->flags &= ~2;
            else
                target->flags |= 2;
            target->tracking_distance = definition->autoaim_width;
            target->radius = object->bounding_radius;
            target->cone_angle = 0.0f;
            result = true;
        }
    }
    else if ((object_index != source_index || !(model_target->flags & 0x20)) &&
        (!(model_target->flags & 0x40) || parameters->flags.test(_aim_assist_secondary_bit)))
    {
        if (matrix2)
        {
            real_vector3d axis;
            vector_from_points3d(&matrix->position, &matrix2->position, &axis);
            pill_compute_autoaim_target(&matrix->position, &axis, model_target->size, point, vector, &target->position);
        }
        else
        {
            sphere_compute_autoaim_target(&matrix->position, model_target->size, point, vector, &target->position);
        }
        if (model_target->flags & 1)
            target->flags |= 1;
        else
            target->flags &= ~1;
        if ((model_target->flags & 2) == 0 && (model_target->flags & 0x10) == 0)
            target->flags &= ~2;
        else
            target->flags |= 2;
        target->tracking_distance = model_target->damage_modifier;
        target->radius = model_target->size;
        target->cone_angle = model_target->cone_angle;
        target->forward = matrix->forward;
        result = true;
    }

    if (result)
    {
        vector_from_points3d(point, &target->position, &target->direction_to_target);
        target->direction = target->direction_to_target;
        target->distance = normalize3d(&target->direction);
        target->friendly = friendly;
        float cosine = dot_product3d(&target->direction, vector);
        target->angle = (float)acos(PIN(PIN(cosine, -1.0f, 1.0f), -1.0f, 1.0f));
    }
    return result;
}






static __forceinline short user_zoom_level(long user_index)
{
    short zoom_level = -1;
    if (user_index != -1)
        zoom_level = player_control_globals->states[user_index].action.zoom;
    return zoom_level;
}
static __forceinline long aim_assist_crosshair_player_index(long user_index)
{
    long player_index = -1;
    if (user_index != -1)
        player_index = player_mapping_globals->player_indices[user_index];
    return player_index;
}

static __forceinline void aim_assist_player_control_get_aiming_vector(long user_index, real_vector3d *aiming_vector)
{
    player_build_aiming_vector_from_facing(aim_assist_crosshair_player_index(user_index),
        &player_control_globals->states[user_index].action.look, aiming_vector);
}


// @address 0x9c570 ?player_aim_assist_get_player_under_crosshair@@YAJJ@Z
long player_aim_assist_get_player_under_crosshair(long user_index)
{
    long result = -1;
    short perspective = (short)g_director_globals->slots[user_index].perspective;
    if (perspective == 0 || perspective == 1) {
        long player_index = aim_assist_crosshair_player_index(user_index);
        player_datum *player = datum_get_typed<player_datum>(player_data, player_index);
        if (player->unit_index != -1) {
            long aiming_unit_index = unit_get_aiming_unit_index(player->unit_index);
            const real_point3d *camera_position = &g_observer_globals->blocks[user_index].camera.position;
            real_vector3d aiming_vector;
            aim_assist_player_control_get_aiming_vector(user_index, &aiming_vector);
            s_cluster_reference cluster_reference = scenario_cluster_reference_from_point(camera_position);
            float zoom = aiming_unit_index == -1 ? 1.0f : unit_get_zoom_magnification(aiming_unit_index,
    user_zoom_level(user_index));
            const double far_angle = 3.0 * 0.017453292f;
            float far_range = zoom * 40.0f;
            float far_sine = (float)sin(far_angle);
            float far_cosine = (float)cos(far_angle);
            float near_range = zoom * 10.0f;
            const double near_angle = 20.0f * 0.017453292f;
            float near_sine = (float)sin(near_angle);
            float near_cosine = (float)cos(near_angle);
            if (cluster_reference_valid(&cluster_reference)) {
                long best_index = -1;
                long best_friendly_index = -1;
                float best_distance = 3.402823466e+38f;
                float best_friendly_distance = best_distance;
                long ignore_index = aiming_unit_index != -1 ? object_root_parent(aiming_unit_index) : -1;
                c_data_iterator<player_datum> iterator;
                iterator.begin((s_data_array *)player_data);
                while (iterator.next()) {
                    long unit_index = iterator.get_datum()->unit_index;
                    if (unit_index != -1 && iterator.get_index() != player_index && ignore_index != object_root_parent(unit_index)) {
                        bool friendly = game_team_is_friendly((e_game_team)player->configuration.host.team_index,
                            (e_game_team)iterator.get_datum()->configuration.host.team_index);
                        bool in_far_cone = aim_assist_object_in_cone(unit_index, camera_position, &aiming_vector, far_range,
    far_sine, far_cosine);
                        bool in_near_cone = friendly &&
                            aim_assist_object_in_cone(unit_index, camera_position, &aiming_vector, near_range, near_sine, near_cosine);
                        if (in_far_cone || in_near_cone) {
                            real_point3d center;
                            object_get_center_of_mass(unit_index, &center);
                            float dy = 0.0f, dz = 0.0f, dx = 0.0f;
                            dx = camera_position->x - center.x;
                            dz = camera_position->z - center.z;
                            dy = camera_position->y - center.y;
                            float distance_squared = dx * dx + dz * dz + dy * dy;
                            if (in_far_cone && distance_squared < best_distance &&
                                aim_assist_clear_line_of_sight(camera_position, &center, 0.0f, aiming_unit_index, unit_index, false))
                            {
                                best_index = iterator.get_index();
                                best_distance = distance_squared;
                            }
                            if (best_index != -1 && in_near_cone && distance_squared < best_friendly_distance &&
                                aim_assist_clear_line_of_sight(camera_position, &center, 0.0f, aiming_unit_index, unit_index, false))
                            {
                                best_friendly_index = iterator.get_index();
                                best_friendly_distance = distance_squared;
                            }
                        }
                    }
                }
                result = best_index == -1 ? best_friendly_index : best_index;
            }
        }
    }
    return result;
}

#if !defined(_WIN64) && !defined(__LP64__)
typedef char aim_assist_target_direction_offset[(offsetof(s_aim_assist_target, direction_to_target) == 0x38) ? 1 : -1];
typedef char aim_assist_collision_plane_offset[(offsetof(collision_result, plane) == 0x24) ? 1 : -1];
typedef char aim_assist_player_team_offset[(offsetof(player_datum, configuration) + offsetof(s_player_configuration, host) +
    offsetof(s_player_configuration_from_host, team_index) == 0x138) ? 1 : -1];
typedef char aim_assist_player_target_offset[(offsetof(player_datum, last_shot_at_object_index) == 0x290) ? 1 : -1];
#endif

// @address 0x3aa90 k_real_math_origin3d

// @address 0x11df660 player_data
// @address 0x11df6d8 player_mapping_globals

extern "C" {
// @address 0x33558 k_aim_assist_camera_test_distance
const float k_aim_assist_camera_test_distance = 128.0f;
}
