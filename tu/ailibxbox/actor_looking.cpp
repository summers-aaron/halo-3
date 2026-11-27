// ailibxbox/actor_looking.obj: from the records 0xb8918 0xbc0d8 0xbcbd8 0xbcf00 0xbd0c0 0xc1688 0xd1370 0xd1508 0x568568 0x568650 0x5689b8 0x568a60 0x568c40 0x568cc0 0x568ed0 0x569028 0x5690d8 0x5694c8
// @compiler xdk6130
// @flags /TP /O2 /J /Oc
#include "ai/ai_collision_constants.h"
#include "ai/actor_looking_constants.h"
#include "memory/tls_globals.h"
#include "ai/actor_storage.h"
#include "objects/object_reference_types.h"
#include "physics/collision_types.h"
#include "physics/collision_test_flags.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "game/game_types.h"
#include "units/unit_seat.h"
#include "objects/object_header.h"
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "math/real_math_trigonometry.h"
#include "ai/ai_direction.h"
#include "math/matrix_math.h"
#include <math.h>
#include <stdlib.h>
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "ai/actor_looking.h"
#include "ai/actor_datum.h"
#include "ai/ai_player_types.h"
#include "ai/ai_player.h"
#include "ai/ai_player_storage.h"
#include "ai/behavior_posture_info_types.h"
#include "units/unit_types.h"
#include "units/unit_definitions.h"
#include "units/biped_types.h"
#include "tag_files/tag_groups.h"
#include "ai/ai_actions.h"
#include "render/render_objects_static_lighting.h"
#include "units/unit_animation.h"
#include "ai/character_definitions.h"
#include "game/game_allegiance.h"
#include "cache/cache_files.h"
#include "physics/collision_test_vector.h"
#include "ai/actor_posture.h"
#include "memory/data_typed_access.h"
#include "math/random_math_range.h"
#include "game/game_time_inlines.h"
#include "game/game_time_inline.h"
#include "ai/ai_direction_mode_types.h"
// @address 0xbcf00 ?actor_get_looking_bounds@@YA_NJPIAM000@Z
// @address 0xc1688 ?aiming_at_target@@YA_NJ@Z
// @address 0xd1370 ?actor_get_look_interest_time@@YAJJJ@Z
// @address 0x5689b8 ?actor_record_look_interest@@YAXJJ@Z
// @address 0x568a60 ?actor_update_look_interest@@YAXJ@Z
// @address 0x568ed0 ?idle_time_get@@YAJJ_N0@Z
// @address 0xae678 ?normalize2d@@YAMPATreal_vector2d@@@Z
// @address 0x1963b0 ?get@c_ai_direction@@QAA_NJW4e_ai_direction_mode@@PBTreal_point3d@@PATreal_vector3d@@PA_NPAT3@@Z
#define ACTOR_LOOKING_MIN(a, b) ((a) < (b) ? (a) : (b))
#define ACTOR_LOOKING_MAX(a, b) ((a) > (b) ? (a) : (b))
#define ACTOR_LOOKING_PIN(x, lo, hi) ACTOR_LOOKING_MIN(ACTOR_LOOKING_MAX(x, lo), hi)

union real_vector2d;


extern "C" {
// @address 0x7c5488 k_actor_idle_look_yaw_range
float k_actor_idle_look_yaw_range = 0.17453292f;
// @address 0x7c548c k_actor_idle_look_pitch_range
float k_actor_idle_look_pitch_range = 0.0872664601f;
// @address 0x7c5490 k_actor_look_interest_divisor
float k_actor_look_interest_divisor = 11.0f;
}

namespace {
enum e_ai_direction { _ai_direction_none = 0, _ai_direction_object = 1, _ai_direction_point = 2, _ai_direction_vector = 5 };
struct character_look_properties
{
    float aiming_yaw_range;
    float aiming_pitch_range;
    float looking_yaw_limit;
    float looking_pitch_limit;
    float maximum_aiming_deviation;
    float pad14;
    float maximum_looking_deviation;
    float pad1c;
    real_bounds noncombat_look_limits;
    real_bounds combat_look_limits;
    float idle_time_range[4][2];
};

static __forceinline character_look_properties *character_definition_look_properties_get(long character_index)
{
    character_look_properties *result = 0;
    while (character_index != -1)
    {
        character_definition *definition = (character_definition *)tag_get('char', character_index);
        if (definition->look_properties.count > 0)
        {
            result = (character_look_properties *)definition->look_properties.address;
            break;
        }
        else
        {
            character_index = definition->parent_character.index;
            continue;
        }
    }
    return result;
}
}


namespace {
}

namespace {
// match: 0x568cc0: retain this validator access shape with its integer-shaped interface.
static __forceinline actor_datum *actor_looking_affect_actor_get(long actor_index)
{
    long absolute_index = actor_index & 0xffff;
    actor_datum *actor = &((actor_datum *)tls_actor_data()->data)[absolute_index];
    return actor;
}
}

// NOTE: direction mode 3 has no enumerator name (the header only names 0-2), so the casts in this file stay literal.
// @address 0xb8918 ?actor_look_affect_movement@@YAXJ@Z
void actor_look_affect_movement(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    if (actor->look_direction.type == _ai_direction_none && !actor->path.valid)
    {
        actor->look_mode = 3;
        actor->look_direction.to_vector(&actor->facing_vector);
        actor->flags &= ~2;
    }
    else if (actor->look_mode >= 4 && actor->type != 20 &&
        actor->look_direction.get(actor_index, (e_ai_direction_mode)3, 0, &actor->steering_vector, 0, 0))
    {
        actor->flags |= 2;
    }
    else
    {
        actor->flags &= ~2;
    }
}

// @rdata 0x820221fc
static const float actor_looking_friendly_interest = 1.0f;
// @rdata 0x8202a1a4
static const float actor_looking_interest_distance = 4.0f;
static __forceinline short actor_looking_player_index_get(long player)
{
    short result = -1;
    ai_player_state *players = tls_ai_player_states();
    for (long index = 0; index < 4; ++index, ++players)
    {
        if (players->id == player)
        {
            result = (short)index;
            break;
        }
    }
    return result;
}
static __forceinline unsigned long actor_looking_classify(
    actor_datum *actor, unit_datum *object, int type, float &type_score, float &friend_score, float &activity_score)
{
    if (type & 0x1003) {
        short player_index = -1;
        if (object->object.body_vitality == 0.0f && object->object.shield_vitality == 0.0f) return false;
        unit_datum *occupant = object;
        if (type & 2) {
            if (object->unit.driver_object_index == -1) return false;
            occupant = OBJECT_GET(object->unit.driver_object_index, unit_datum, _object_mask_all);
            if (occupant->unit.actor_index != -1) type_score = 0.4f;
            else if (occupant->unit.player_index != -1) {
                type_score = 1.0f;
                player_index = actor_looking_player_index_get(occupant->unit.player_index);
            }
        }
        else {
            if (occupant->unit.actor_index != -1) type_score = 0.6f;
            else if (occupant->unit.player_index != -1) {
                type_score = 1.0f;
                player_index = actor_looking_player_index_get(occupant->unit.player_index);
            }
        }
        if (player_index != -1 && actor->boarding_dialogue_active &&
            actor->boarding_dialogue_player_index == player_index)
            activity_score = 1.0f;
        if (!game_team_is_enemy((e_game_team)actor->team, (e_game_team)object->unit.team)) friend_score = actor_looking_friendly_interest;
    } else if ((type & 0x20) != 0 || (type & 0x400) != 0) type_score = 0.3f;

    return true;
}
// @address 0xbcbd8 actor_compute_look_interest
extern "C" float actor_compute_look_interest(long actor_index, const real_vector3d *reference, long object_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    object_header_datum *header = object_header_get(object_index);
    unit_datum *object = (unit_datum *)header->datum;
    float interest = 0.0f;
    float type_score = 0.0f, friend_score = 0.0f, activity_score = 0.0f;
    int object_type = header->object_type;
    if (actor->root_vehicle_index == -1 || object_index != actor->root_vehicle_index)
    {
    int type = 1 << object_type;
    if ((type & 0x20) == 0 || (object->object.ai_flags & 2) != 0)
    {
    real_vector3d direction = {
        object->object.position.i - actor->position.body.i,
        object->object.position.j - actor->position.body.j,
        object->object.position.k - actor->position.body.k };
    float distance = normalize3d(&direction);
    if (object_type == 0 || actor_looking_valid_idle_vector(actor_index, reference, 0, &direction))
    {
    if (actor_looking_classify(actor, object, type, type_score, friend_score, activity_score))
    {
    type_score *= 3.0f;
    activity_score *= 2.0f;
    float speed = sqrtf(object->object.linear_velocity.i*object->object.linear_velocity.i +
        object->object.linear_velocity.j*object->object.linear_velocity.j +
        object->object.linear_velocity.k*object->object.linear_velocity.k);
    if (!(speed <= 3.0f)) speed = 3.0f;
    float speed_score = (0.1f + 0.9f * (speed / 3.0f)) * 3.0f;
    float range = actor_looking_interest_distance - distance;
    if (!(range > 0.0f)) range = 0.0f;
    float range_score = (range * 0.225f + 0.1f) * 2.0f;
    return (type_score + (friend_score + (activity_score + range_score)) + speed_score) / k_actor_look_interest_divisor;
    }
    }
    }
    }
    return interest;
}

// match: 0xbcf00: pointer __restrict qualifier prevents alias reload
bool actor_get_looking_bounds(long actor_index, float *__restrict looking_deviation,
    // match: 0xbcf00: pointer __restrict qualifier prevents alias reload
    float *__restrict aiming_deviation, float *__restrict left_bound, float *__restrict right_bound)
{
    bool result = false;
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    character_look_properties *look_properties = character_definition_look_properties_get(actor->character_index);
    if (look_properties != 0)
    {
        result = true;
        *aiming_deviation = look_properties->maximum_aiming_deviation;
        if (actor->driver)
        {
            unit_datum *vehicle = OBJECT_GET(actor->vehicle_index, unit_datum, _object_mask_vehicle);
            character_vehicle_properties *vehicle_properties = character_vehicle_properties_get(actor_index, vehicle->object.definition_index);
            if (vehicle_properties != 0)
                *aiming_deviation = cosine(vehicle_properties->maximum_aiming_deviation);
        }
        else if (actor->passenger)
        {
            unit_datum *unit = OBJECT_GET(actor->unit_index, unit_datum, _object_mask_unit);
            if (unit->unit.parent_seat_index != -1)
            {
                unit_datum *parent = OBJECT_GET(unit->object.parent_object_index, unit_datum, _object_mask_unit);
                unit_definition *definition = (unit_definition *)tag_get('unit', parent->object.definition_index);
                unit_seat *seat = &((unit_seat *)definition->seats.address)[unit->unit.parent_seat_index];
                *aiming_deviation = cosine(-seat->left_aiming_angle > seat->right_aiming_angle ?
                    -seat->left_aiming_angle : seat->right_aiming_angle);
            }
        }
        *looking_deviation = look_properties->maximum_looking_deviation;
        if (actor->behavior_mode == 4)
        {
            *left_bound = cosine(look_properties->combat_look_limits.lower);
            *right_bound = cosine(look_properties->combat_look_limits.upper);
        }
        else
        {
            *left_bound = cosine(look_properties->noncombat_look_limits.lower);
            *right_bound = cosine(look_properties->noncombat_look_limits.upper);
        }
    }
    return result;
}


namespace {
static __forceinline actor_datum *aiming_actor_get(s_data_array *array, long actor_index)
{
    long absolute_index = actor_index & 0xffff;
    actor_datum *actor = &((actor_datum *)array->data)[absolute_index];
    return actor;
}
}
bool aiming_at_target(long actor_index)
{
    bool result = false;
    actor_datum *actor = aiming_actor_get(tls_actor_data(), actor_index);
    if ((actor->flags & 0x800) != 0 && actor->look_mode >= 2 &&
        (actor->look_direction.type == _ai_direction_point ||
         actor->look_direction.type == _ai_direction_object &&
         actor->look_direction.object_index == actor->target_prop_index))
    {
        result = true;
    }
    return result;
}

long actor_get_look_interest_time(long actor_index, long object_index)
{
    object_header_datum *header = object_header_get(object_index);
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    long object_type_bit = 1 << header->object_type;
    float scale = 1.0f;
    if ((object_type_bit & 0x1003) != 0)
    {
        unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
        if (object->unit.actor_index != -1)
            scale = 2.0f;
        else if (object->unit.player_index != -1)
            scale = 4.0f;
    }
    else if ((object_type_bit & 32) != 0)
        scale = 3.0f;
    else if ((object_type_bit & 0x400) != 0)
        scale = 1.5f;
    if (actor->combat_status >= 3)
        scale *= 0.5f;
    return (long)((float)tls_game_time_globals()->ticks_per_second * scale + 0.5f);
}

namespace {
}

// @address 0x568568 ?actor_look_valid_aim_vector@@YA_NMPATreal_vector3d@@0_N@Z
bool actor_look_valid_aim_vector(float minimum_dot, real_vector3d *vector, real_vector3d *vector2, bool use_3d)
{
    bool result = false;
    if (use_3d)
    {
        result = vector->i * vector2->i + (vector->k * vector2->k + vector->j * vector2->j) > minimum_dot;
    }
    else
    {
        real_vector2d direction = vector2->xy;
        real_vector2d aim = vector->xy;
        if (::normalize2d(&direction) > 0.0f && ::normalize2d(&aim) > 0.0f)
        {
            if (aim.j * direction.j + direction.i * aim.i > minimum_dot)
                result = true;
        }
    }
    return result;
}


void actor_record_look_interest(long actor_index, long object_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    long oldest_time = 0x7fffffff;
    short oldest_index = -1;
    for (short index = 0; index < 4; ++index)
    {
        if (actor->dialogue_entries[index].index == object_index)
        {
            oldest_index = index;
            break;
        }
        if (actor->dialogue_entries[index].time < oldest_time)
        {
            oldest_time = actor->dialogue_entries[index].time;
            oldest_index = index;
        }
    }
    actor->dialogue_entries[oldest_index].index = object_index;
    actor->dialogue_entries[oldest_index].time = tls_game_time_globals()->time;
}

long idle_time_get(long actor_index, bool scale_up, bool combat)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    long result = game_seconds_integer_to_ticks(3);
    character_look_properties *look_properties = character_definition_look_properties_get(actor->character_index);
    if (look_properties != 0)
    {
        float minimum;
        float maximum;
        if (actor->combat_status >= 2)
        {
            if (combat)
            {
                minimum = look_properties->idle_time_range[3][0];
                maximum = look_properties->idle_time_range[3][1];
            }
            else
            {
                minimum = look_properties->idle_time_range[2][0];
                maximum = look_properties->idle_time_range[2][1];
            }
        }
        else if (combat)
        {
            minimum = look_properties->idle_time_range[1][0];
            maximum = look_properties->idle_time_range[1][1];
        }
        else
        {
            minimum = look_properties->idle_time_range[0][0];
            maximum = look_properties->idle_time_range[0][1];
        }
        result = game_seconds_to_ticks_round(
            _real_random_range(tls_random_seed(), minimum, maximum));
    }
    return result;
}

namespace {
}

// @address 0x5694c8 ?allow_idle_looking@@YA_NJ_N@Z
bool allow_idle_looking(long actor_index, bool combat)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    bool result = true;
    if (actor->airborne ||
        OBJECT_GET(actor->unit_index, unit_datum, _object_mask_unit)->object.object_type == 0 &&
        OBJECT_GET(actor->unit_index, biped_datum, _object_mask_biped)->biped.physics.m_mode == 5)
    {
        result = false;
    }
    else if ((short)actor->behavior_mode >= 4 && (actor->flags & 1) != 0)
    {
        result = false;
    }
    else if ((short)actor->behavior_mode <= 1)
    {
        result = false;
    }
    else if (motor_animation_busy(actor->unit_index))
    {
        result = false;
    }
    else if (idle_time_get(actor_index, true, combat) == 0)
    {
        result = false;
    }
    return result;
}

// match: 0x568cc0: the fully typed validator rewrite changes one FMA operand pair.
// @address 0x568cc0 actor_looking_valid_idle_vector
bool actor_looking_valid_idle_vector(long actor_index, const real_vector3d *reference_in, unsigned char aiming, real_vector3d *direction_in)
{
    bool valid = true;
    actor_datum * actor = actor_looking_affect_actor_get(actor_index);
    character_look_properties *look_properties = character_definition_look_properties_get(actor->character_index);
    if (aiming != 0 && (actor->flags & 0x200) != 0)
    {
        valid = true;
    }
    else if (look_properties == 0)
    {
        valid = true;
    }
    else
    {
        real_vector3d *direction = direction_in;
        const real_vector3d *reference = aiming != 0 ? reference_in : &actor->forward8ec;
        real_vector3d flat_reference = *reference;
        flat_reference.k = 0.0f;
        real_vector3d flat_direction = *direction;
        flat_direction.k = 0.0f;
        if (normalize3d(&flat_direction) != 0.0f && normalize3d(&flat_reference) != 0.0f)
        {
            float angle = flat_direction.k * direction->k;
            angle = (direction->j * flat_direction.j + direction->i * flat_direction.i) + angle;
            angle = ACTOR_LOOKING_PIN(angle, -1.0f, 1.0f);
            angle = ::arccosine(angle);
            if (direction->k < 0.0f)
                angle = -angle;

            float skew = signed_angle_between_vectors2d(&flat_reference.xy, &flat_direction.xy);
            const float *limits = (const float *)look_properties;
            if (aiming != 0)
            {
                if (fabs(skew) > limits[0] + 0.01)
                    valid = false;
                else if (fabs(angle) > limits[1] + 0.01)
                    valid = false;
            }
            else
            {
                if (fabs(skew) > limits[2] + 0.01)
                    valid = false;
                else if (fabs(angle) > limits[3] + 0.01)
                    valid = false;
            }
        }
    }
    return valid;
}

// @address 0x568c40 actor_looking_valid_idle_direction
unsigned char actor_looking_valid_idle_direction(long looking_actor_index, const real_vector3d *reference_vector,
    unsigned char aiming_flag, c_ai_direction *look_direction)
{
    real_vector3d resolved_vector;
    bool result = false;
    if (look_direction->get(looking_actor_index, (e_ai_direction_mode)((aiming_flag != 0) + 2), 0, &resolved_vector, 0, 0))
        result = actor_looking_valid_idle_vector(looking_actor_index, reference_vector, aiming_flag, &resolved_vector);
    return result;
}

// @rdata 0x82022518
static const float actor_looking_minimum_interest = 0.3f;
// @rdata 0x820224f4
static const float actor_looking_current_interest_scale = 1.1f;
// @rdata 0x82022b14
static const float actor_looking_recent_interest_scale = 0.75f;

// @address 0x568a60 ?actor_update_look_interest@@YAXJ@Z
void actor_update_look_interest(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    if (actor->uncovering == 0)
        return;
    long old_interest = actor->dialogue_object_index;
    float best = actor_looking_minimum_interest;
    long best_object = -1;
    long results[130];
    short count = objects_in_sphere(0, 0x1c23, &actor->position.location, &actor->position.head, 7.0f, results, 128);
    if (count > 0)
    {
        short i = 0;
        do
        {
            long object = results[i];
            if (object != actor->unit_index)
            {
                float value = actor_compute_look_interest(actor_index, &actor->forward8ec, object);
                if (object == old_interest && value > actor_looking_minimum_interest)
                    value *= actor_looking_current_interest_scale;
                short match_slot = 0;
                long scan_slot = 0;
                do
                {
                    if (actor->dialogue_entries[scan_slot].index == object)
                    {
                        long elapsed = tls_game_time_globals()->time - actor->dialogue_entries[match_slot].time;
                        long duration = actor_get_look_interest_time(actor_index, object);
                        if ((float)elapsed < (float)duration * 2.0f)
                            value = 0.0f;
                        else if ((float)elapsed < (float)duration * 3.0f)
                            value *= actor_looking_recent_interest_scale;
                        break;
                    }
                    match_slot = (short)(scan_slot + 1);
                    scan_slot = match_slot;
                } while (scan_slot < 4);
                if (value > best)
                {
                    best = value;
                    best_object = object;
                }
            }
            i = (short)(i + 1);
        } while (i < count);
    }
    actor->dialogue_object_index = best_object;
    if (best_object != -1)
    {
        actor->dialogue_object_distance = best;
        return;
    }
    actor->dialogue_object_distance = 0.0f;
}


static __forceinline real_vector3d *actor_looking_rotate_projected_vector(
    real_vector3d *vector, const real_vector3d *axis, float sine, float cosine, float scale)
{
    float cross_i = vector->j * axis->k - vector->k * axis->j;
    float cross_j = vector->k * axis->i - vector->i * axis->k;
    float cross_k = vector->i * axis->j - vector->j * axis->i;
    vector->i = axis->i * scale + vector->i * cosine - cross_i * sine;
    vector->j = axis->j * scale + vector->j * cosine - cross_j * sine;
    const float &cosine_k = vector->k * cosine;
    vector->k = axis->k * scale + cosine_k - cross_k * sine;
    return vector;
}

static inline void actor_looking_set_vector_k(real_vector3d *vector, float k)
{
    vector->k = k;
}

// @address 0x568650 ?actor_look_find_random_vector@@YA_NPBTreal_point3d@@PBTreal_vector3d@@_NMMMMPAT2@@Z
bool actor_look_find_random_vector(const real_point3d *point, const real_vector3d *vector, bool unused, float minimum_yaw,
    float maximum_yaw, float minimum_pitch, float maximum_pitch, real_vector3d *random_vector)
{
    real_vector3d axis;
    real_vector3d best_vector;
    float best_t = 0.0f;
    bool result = false;
    bool clear;
    axis.i = -vector->j;
    axis.j = vector->i;
    actor_looking_set_vector_k(&axis, 0.0f);
    best_vector = *global_forward3d;
    if (normalize3d(&axis) == 0.0f)
        axis = *global_left3d;
    long count = 0;
    do
    {
        float yaw = _real_random_range(tls_random_seed(), minimum_yaw, maximum_yaw);
        float pitch = _real_random_range(tls_random_seed(), minimum_pitch, maximum_pitch);
        float pitch_cosine = 0.0f;
        float pitch_sine = 0.0f;
        real_vector3d test_vector = *vector;
        float yaw_cosine = 0.0f;
        float yaw_sine = 0.0f;
        float scale = 0.0f;
        pitch_cosine = (float)cos(pitch);
        pitch_sine = (float)sin(pitch);
        float vector_i = test_vector.i;
        float axis_k = axis.k;
        scale = (axis.i * vector_i + axis.j * test_vector.j + axis_k * test_vector.k) * (1.0f - pitch_cosine);
        actor_looking_rotate_projected_vector(&test_vector, &axis, pitch_sine, pitch_cosine, scale);
        yaw_cosine = (float)cos(yaw);
        yaw_sine = (float)sin(yaw);
        real_vector3d *rotated = &test_vector;
        const real_vector3d *rotation_axis = global_up3d;
        float rotation_sine = yaw_sine;
        float rotation_cosine = yaw_cosine;
        float rotation_scale = (rotation_axis->i * rotated->i + rotation_axis->j * rotated->j + rotation_axis->k * rotated->k) * (1.0f - rotation_cosine);
        float rotation_cross_i = rotated->j * rotation_axis->k - rotated->k * rotation_axis->j;
        float rotation_cross_j = rotated->k * rotation_axis->i - rotated->i * rotation_axis->k;
        float rotation_cross_k = rotated->i * rotation_axis->j - rotated->j * rotation_axis->i;
        rotated->i = rotation_axis->i * rotation_scale + rotated->i * rotation_cosine - rotation_cross_i * rotation_sine;
        rotated->j = rotation_axis->j * rotation_scale + rotated->j * rotation_cosine - rotation_cross_j * rotation_sine;
        rotated->k = rotation_axis->k * rotation_scale + rotated->k * rotation_cosine - rotation_cross_k * rotation_sine;
        real_vector3d ray;
        ray.i = test_vector.i * 3.0f;
        ray.j = test_vector.j * 3.0f;
        ray.k = test_vector.k * 3.0f;
        collision_result collision;
        clear = !collision_test_vector(k_ai_line_of_sight_default_flags, false, point, &ray, -1, -1, -1, &collision);
        if (clear)
        {
            normalize3d(&test_vector);
            result = true;
            *random_vector = test_vector;
            break;
        }
        if (collision.t > best_t)
        {
            best_t = collision.t;
            best_vector = test_vector;
        }
        count = (short)(count + 1);
    } while (count < 10);
    if (!clear && best_t > 0.0f)
    {
        normalize3d(&best_vector);
        *random_vector = best_vector;
        result = true;
    }
    return result;
}

// @address 0x569028 ?actor_test_looking_direction@@YA_NJPBTreal_point3d@@PBTreal_vector3d@@MPAM@Z
bool actor_test_looking_direction(long actor_index, const real_point3d *point, const real_vector3d *direction, float distance, float *collision_t)
{
    float t = 1.0f;
    collision_result collision;
    real_vector3d vector;
    scale_vector3d(direction, distance, &vector);
    bool clear = !collision_test_vector(k_ai_line_of_sight_default_flags, false, point, &vector, -1, -1, -1, &collision);
    if (!clear)
        t = collision.t;
    if (collision_t)
        *collision_t = t;
    return clear;
}

// @rdata 0x82024264
static const float actor_looking_test_distance = 5.0f;

static __forceinline float actor_looking_test_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->j * b->j + a->k * b->k);
}

extern "C" {
// @address 0x2b210 k_actor_aiming_test_distance
const float k_actor_aiming_test_distance = 1.25f;
}

// match: 0xbd0c0: float_control pragma preserves single-precision rounding in the alignment blend
#pragma float_control(precise, on, push)
// match: 0xbd0c0: fenv_access pragma blocks fmadds contraction in the alignment blend
#pragma fenv_access(on)
static __forceinline float actor_looking_candidate_alignment(const real_vector3d *candidate, const real_vector3d *direction, float minimum)
{
    return actor_looking_test_dot(candidate, direction) > minimum ? actor_looking_test_dot(candidate, direction) : minimum;
}
// match: 0xbd0c0: fenv_access pragma blocks fmadds contraction in the alignment blend
#pragma fenv_access(off)
// match: 0xbd0c0: float_control pragma preserves single-precision rounding in the alignment blend
#pragma float_control(pop)

// @address 0xbd0c0 ?test_stupid_aiming_direction@@YA_NJ_NPBTreal_point3d@@PBTreal_vector3d@@PAT2@PA_N@Z
bool test_stupid_aiming_direction(long actor_index, bool allow_search, const real_point3d *point,
    const real_vector3d *direction, real_vector3d *result_direction, bool *result_adjusted)
{
    bool result = false;
    bool adjusted = false;
    actor_datum *actor = actor_looking_affect_actor_get(actor_index);
    if (actor->vehicle_index == -1 && !actor->combat_flag339 && !actor->airborne)
    {
        real_vector3d flat_direction = *direction;
        flat_direction.k = 0.0f;
        if (normalize3d(&flat_direction) > 0.0f)
        {
            float collision_t;
            if (actor_test_looking_direction(actor_index, point, &flat_direction, k_actor_aiming_test_distance, &collision_t))
            {
                if (result_direction)
                    *result_direction = flat_direction;
                result = true;
            }
            else
            {
                real_vector3d candidates[8];
                short best_index = -1;
                float best_score = 0.0f;
                for (short index = 0; index < 8; index++)
                {
                    candidates[index] = flat_direction;
                    float angle = (float)index * 0.7853982f;
                    float score = 1.0f;
                    float cosine = (float)cos(angle);
                    float sine = (float)sin(angle);
                    rotate_vector_about_axis(&candidates[index], global_up3d, sine, cosine);
                    if (!actor_test_looking_direction(actor_index, point, &candidates[index], actor_looking_test_distance, &collision_t))
                        score = collision_t * 0.2f;
                    score *= actor_looking_candidate_alignment(&candidates[index], &flat_direction, 0.6f);
                    if (score > best_score)
                    {
                        best_score = score;
                        best_index = index;
                    }
                }
                if (best_index != -1 && actor_looking_test_dot(&candidates[best_index], &flat_direction) < 0.8660254f)
                {
                    if (result_direction)
                        *result_direction = candidates[best_index];
                    adjusted = true;
                    result = true;
                }
            }
        }
    }
    if (result_adjusted)
        *result_adjusted = adjusted;
    return result;
}

// @address 0xd1508 actor_find_idle_look_direction
bool actor_find_idle_look_direction(long actor_index, const real_vector3d *reference, bool direction_from_idle, bool aiming, c_ai_direction *direction_out)
{
    bool result = false;
    actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;
    float scale = 1.0f;
    real_vector3d direction;
    character_look_properties *look_properties = character_definition_look_properties_get(actor->character_index);
    if (look_properties != 0)
    {
        if (actor->vehicle_driver_type > 1)
            scale = 0.6f;

        if (aiming)
        {
            if (actor->dialogue_object_index != -1)
            {
                bool use_random;
                if (actor->controlling_other_unit)
                    use_random = false;
                else if (actor->dialogue_object_distance > 0.5f)
                    use_random = true;
                else
                    use_random = _real_random_range(tls_random_seed(), 0.0f, 1.0f) < 0.33f;

                if (use_random)
                {
                    bool valid;
                    if ((actor->flags & 1) == 0)
                    {
                        valid = true;
                    }
                    else
                    {
                        unit_datum *object = OBJECT_GET(actor->dialogue_object_index, unit_datum, _object_mask_all);
                        real_vector3d object_direction;
                        vector_from_points3d(&actor->position.body, &object->object.position, &object_direction);
                        valid = actor_looking_valid_idle_vector(actor_index, reference, aiming, &object_direction);
                    }

                    if (valid)
                    {
                        direction_out->to_object(actor->dialogue_object_index);
                        actor_record_look_interest(actor_index, actor->dialogue_object_index);
                        result = true;
                        // match: 0xd1508: goto joins the shared li r26,1 / mr r3,r26 exit; an early return emits a second __restfpr_26/__restgprlr_25 epilogue.
                        goto done;
                    }
                }
            }

            direction = *reference;
            direction.k = 0.0f;
            if (normalize3d(&direction) == 0.0f)
                direction = global_identity4x3->forward;
            x_min = -look_properties->aiming_yaw_range * scale;
            x_max = look_properties->aiming_yaw_range * scale;
            y_max = 0.0f;
            y_min = 0.0f;
        }
        else
        {
            if (direction_from_idle)
            {
                direction = actor->forward8ec;
            }
            else if (!actor->secondary_aim_direction.get(actor_index, _ai_direction_mode_object_point, 0, &direction, 0, 0))
            {
                direction = actor->facing_vector;
            }

            real_vector3d flat_direction = direction;
            flat_direction.k = 0.0f;
            if (normalize3d(&flat_direction) != 0.0f)
            {
                real_vector3d flat_idle = actor->forward8ec;
                flat_idle.k = 0.0f;
                if (normalize3d(&flat_idle) != 0.0f)
                {
                    const real_bounds *limits = (actor->behavior_mode == 4)
                        ? &look_properties->combat_look_limits
                        : &look_properties->noncombat_look_limits;
                    if (direction_from_idle)
                    {
                        x_min = -limits->lower * scale;
                        x_max = limits->upper * scale;
                        y_min = -10.0f * 0.017453292f * scale;
                        y_max = 10.0f * 0.017453292f * scale;
                    }
                    else
                    {
                        float skew = signed_angle_between_vectors2d(&flat_idle.xy, &flat_direction.xy);
                        float angle = direction.i * flat_direction.i + direction.j * flat_direction.j + direction.k * flat_direction.k;
                        angle = ACTOR_LOOKING_PIN(angle, -1.0f, 1.0f);
                        angle = ::arccosine(angle);
                        if (direction.k < 0.0f)
                            angle = -angle;

                        x_min = skew - k_actor_idle_look_yaw_range;
                        x_max = skew + k_actor_idle_look_yaw_range;
                        y_min = angle - k_actor_idle_look_pitch_range;
                        y_max = angle + k_actor_idle_look_pitch_range;
                        x_min = ACTOR_LOOKING_MAX(x_min, -look_properties->looking_yaw_limit * scale);
                        float yaw_limit = look_properties->looking_yaw_limit * scale;
                        x_max = x_max > yaw_limit ? yaw_limit : x_max;
                        y_min = ACTOR_LOOKING_MAX(y_min, -look_properties->looking_pitch_limit * scale);
                        float pitch_limit = look_properties->looking_pitch_limit * scale;
                        y_max = y_max > pitch_limit ? pitch_limit : y_max;
                        x_min -= skew;
                        x_max -= skew;
                        y_min -= angle;
                        y_max -= angle;
                    }
                }
            }
        }

        {
            real_vector3d new_vector;
            if (actor_look_find_random_vector(&actor->position.head, &direction, true, x_min, x_max, y_min, y_max, &new_vector))
            {
                direction_out->to_vector(&new_vector);
                result = true;
            }
        }
    }

done:
    if (!result)
    {
        direction_out->to_vector(&actor->facing_vector);
    }
    return result;
}

// @rdata 0x82022dc0
static const float actor_looking_idle_time_scale = 0.5f;



// @address 0x5690d8 actor_update_idle_look_direction
extern "C" unsigned char actor_update_idle_look_direction(long actor_index, const real_vector3d *reference,
    unsigned char looking, short combat_state, real_vector3d *direction_out,
    bool *valid_out, real_point3d *point_out)
{
    bool aim_adjusted;
    real_vector3d facing_vector;
    real_vector3d aim_vector;
    bool combat = combat_state != 0;
    unsigned char combat_flag = combat;
    bool changed = false;
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    if (combat_flag != 0 && actor->uncovering && ((actor->flags >> 9) & 1) != 0)
    {
        if (!actor->secondary_aim_direction.get(actor_index, (e_ai_direction_mode)3, 0, &aim_vector, 0, 0))
            aim_vector = actor->forward8e0;
        changed = test_stupid_aiming_direction(actor_index, true, &actor->position.head,
            &aim_vector, &facing_vector, &aim_adjusted);
        if (aim_adjusted)
        {
            actor->attention_prop_index = idle_time_get(actor_index, true, combat);
            actor->attention_actor_index = idle_time_get(actor_index, false, combat);
            actor->secondary_aim_direction.to_vector(&facing_vector);
            actor->previous_aim_direction = actor->secondary_aim_direction;
            actor->flags |= 0x400;
        }
    }
    if ((changed & 0xff) == 0)
    {
        unsigned char idle_valid;
        if (actor->attention_prop_index == 0)
            idle_valid = 0;
        else if ((actor->flags & 1) == 0 && actor->secondary_aim_direction.type == 7)
            idle_valid = 1;
        else
            idle_valid = actor_looking_valid_idle_direction(actor_index, reference, 1, &actor->secondary_aim_direction);
        if ((idle_valid & 0xff) == 0)
        {
            c_ai_direction *idle_direction = &actor->secondary_aim_direction;
            changed = actor_find_idle_look_direction(actor_index, reference, true, true, idle_direction);
            if ((changed & 0xff) != 0)
            {
                if (idle_direction->type == 7)
                {
                    unsigned long *seed = tls_random_seed();
                    unsigned long random_value = random_seed_next(seed);
                    long interest_time = actor_get_look_interest_time(actor_index, actor->secondary_aim_direction.object_index);
                    unsigned long random_high = random_value >> 16;
                    float fraction = (float)random_high * (1.0f / 65535.0f);
                    float scale = fraction * actor_looking_idle_time_scale + 0.75f;
                    actor->attention_actor_index = actor->attention_prop_index = (long)((float)interest_time * scale);
                }
                else
                    actor->attention_prop_index = idle_time_get(actor_index, true, combat);
                actor->previous_aim_direction = *idle_direction;
            }
        }
        if ((looking & 0xff) != 0)
        {
            if (actor->attention_actor_index == 0 ||
                (actor_looking_valid_idle_direction(actor_index, reference, 0, &actor->previous_aim_direction) & 0xff) == 0)
                changed = actor_find_idle_look_direction(actor_index, reference, false, false, &actor->previous_aim_direction);
        }
        if ((changed & 0xff) != 0)
        {
            if (actor->previous_aim_direction.type == 7)
                actor->attention_actor_index = actor_get_look_interest_time(actor_index, actor->previous_aim_direction.object_index);
            else
                actor->attention_actor_index = idle_time_get(actor_index, false, combat);
            long previous_time = actor->attention_prop_index;
            long best_time = actor->attention_actor_index;
            if (best_time <= previous_time)
                best_time = previous_time;
            actor->attention_prop_index = best_time;
        }
    }
    long mode = 2 + (combat_flag != 0);
    bool got_direction;
    if (looking)
        got_direction = actor->previous_aim_direction.get(actor_index, (e_ai_direction_mode)mode,
            0, direction_out, valid_out, point_out);
    else
        got_direction = actor->secondary_aim_direction.get(actor_index, (e_ai_direction_mode)mode,
            0, direction_out, valid_out, point_out);
    if (!got_direction)
        *direction_out = actor->facing_vector;
    return 1;
}

extern "C" {
// @address 0x239e0 k_behavior_posture_info
}

static __forceinline long actor_looking_motor_program(long unit_index)
{
    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
    return *(const long *)((const char *)unit + unit->unit.motor_program.offset);
}

static __forceinline bool actor_looking_currently_firing_burst(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    bool firing = false;
    if (actor->current_fire_target_type > 0)
        firing = actor->target_state == 2;
    return firing;
}

static __forceinline void actor_looking_reset_idle_timers(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    short time = (short)game_seconds_to_ticks_round(1.0f);
    actor->attention_prop_index = time;
    actor->attention_actor_index = time;
    actor->secondary_aim_direction.to_vector(&actor->facing_vector);
    actor->previous_aim_direction.to_vector(&actor->facing_vector);
}

static __forceinline bool actor_looking_variable_camera_up(long unit_index)
{
    long program = actor_looking_motor_program(unit_index);
    bool variable_up = false;
    if (program == 7 || program == 8)
        variable_up = true;
    return variable_up;
}

static __forceinline void actor_looking_exact_facing(long actor_index, bool exact)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    if (exact)
        actor->control_flags |= 8;
    else
        actor->control_flags &= ~8ul;
}

// @address 0xbc0d8 ?actor_look_update@@YAXJ@Z
void actor_look_update(long actor_index)
{
    actor_datum *actor = datum_get_typed<actor_datum>(tls_actor_data(), actor_index);
    bool used_idle = false;
    bool exit_posture = false;
    bool has_look_point = false;
    real_point3d look_point;
    long posture = actor_get_posture(actor_index);
    long peek_type = 0;
    if (posture != 0)
        peek_type = k_behavior_posture_info[posture].peek_type;

    actor->attention_prop_index = (long)MAX((float)(actor->attention_prop_index - 1), 0.0f);
    actor->attention_actor_index = (long)MAX((float)(actor->attention_actor_index - 1), 0.0f);
    actor_update_look_interest(actor_index);
    actor->forward8ec = actor->facing_vector;
    actor->forward8f8 = actor->facing_vector;
    float looking_deviation;
    float aiming_deviation;
    float bounds[2];
    bool unrestricted_aim = false;
    if (actor_get_looking_bounds(actor_index, &looking_deviation, &aiming_deviation, &bounds[0], &bounds[1]))
    {
        bool has_override = false;
        real_vector3d reference = actor->facing_vector;
        bool used_override = false;
        if (actor->attention_ticks > 0)
            has_override = true;
        if (TEST_BIT(actor->flags, 9) && TEST_BIT(0x93, actor_looking_motor_program(actor->unit_index)))
        {
            bool got_facing = false;
            if (posture == 0)
            {
                if (has_override && actor->attention_level >= 2)
                {
                    if (actor->attention_direction.get(actor_index, _ai_direction_mode_vector, 0, &reference, 0, 0))
                    {
                        used_override = true;
                        got_facing = true;
                    }
                }
                else if ((unsigned long)(actor->look_mode - 3) <= 1)
                {
                    if (actor->look_direction.get(actor_index, _ai_direction_mode_vector, 0, &reference, 0, 0))
                        got_facing = true;
                }
            }
            if (!got_facing)
                reference = actor->facing_vector;
            actor->forward8e0 = reference;
            actor->flags &= ~0x200;
        }

        {
            real_vector3d direction;
            bool got_aim = false;
            if (actor_looking_currently_firing_burst(actor_index) && !actor->suppress_burst_aiming)
            {
                direction = actor->attack_vector;
                got_aim = normalize3d(&direction) > 0.0f;
            }
            else if (has_override && (actor->attention_level >= 2 ||
                                      (actor->attention_level >= 1 && actor->look_mode < 4)))
            {
                if (actor->attention_direction.get(actor_index, (e_ai_direction_mode)3, 0, &direction, 0, 0))
                {
                    got_aim = true;
                    unrestricted_aim = true;
                    used_override = true;
                }
            }
            else
            {
                switch (actor->look_mode)
                {
                case 2:
                    if (actor->look_direction.get(actor_index, (e_ai_direction_mode)3, 0, &direction, 0, 0))
                    {
                        got_aim = true;
                        break;
                    }
                case 0:
                case 3:
                    if ((short)actor->behavior_mode >= 4 && TEST_BIT(actor->flags, 0))
                    {
                        c_ai_direction movement = {};
                        movement.to_movement();
                        if (movement.get(actor_index, (e_ai_direction_mode)3, 0, &direction, 0, 0))
                        {
                            got_aim = true;
                            unrestricted_aim = true;
                        }
                        break;
                    }
                case 1:
                    if (allow_idle_looking(actor_index, true) &&
                        actor_update_idle_look_direction(actor_index, &reference, false, 1, &direction, 0, 0))
                    {
                        if (!TEST_BIT(actor->flags, 0))
                            unrestricted_aim = true;
                        used_idle = true;
                        got_aim = true;
                    }
                    break;
                case 4:
                    if (actor->look_direction.get(actor_index, (e_ai_direction_mode)3, 0, &direction, 0, 0))
                    {
                        got_aim = true;
                        if (peek_type == 1)
                        {
                            exit_posture = true;
                            aiming_deviation = 0.70710677f;
                        }
                    }
                    break;
                }
            }

            if (got_aim && (TEST_BIT(actor->flags, 9) || unrestricted_aim ||
                            (posture == 0 && TEST_BIT(actor->flags, 1)) ||
                            actor_look_valid_aim_vector(aiming_deviation, &actor->facing_vector, &direction,
                                                        actor->combat_flag339)))
            {
                actor->forward8ec = direction;
                actor->flags |= 0x800;
            }
            else if (exit_posture)
            {
                if (!motor_animation_busy(actor->unit_index))
                    actor_exit_posture(actor_index);
                actor->forward8ec = direction;
                actor->flags |= 0x800;
            }
            else
            {
                if (allow_idle_looking(actor_index, true) &&
                    actor_update_idle_look_direction(actor_index, &reference, false, 2, &direction, 0, 0))
                {
                    got_aim = true;
                    used_idle = true;
                    actor->forward8ec = direction;
                    actor->flags |= 0x800;
                }
                if (!got_aim)
                {
                    actor->flags &= ~0x800;
                    actor->forward8ec = actor->forward8e0;
                }
            }
        }
        {
            real_vector3d direction;
            bool got_look = false;
            if (has_override && (actor->look_mode < 4 || actor->attention_level >= 2))
            {
                if (actor->attention_direction.get(actor_index, _ai_direction_mode_object_point, 0, &direction,
                                                       &has_look_point, &look_point))
                {
                    used_override = true;
                    got_look = true;
                }
            }
            else
            {
                switch (actor->aim_mode)
                {
                case 0:
                case 1:
                    if (actor->look_mode < 4 && actor->situation.best_prop_class < 5 &&
                        !((short)actor->behavior_mode >= 4 && TEST_BIT(actor->flags, 0)) &&
                        allow_idle_looking(actor_index, false))
                    {
                        if (actor_update_idle_look_direction(actor_index, &reference, true, 0, &direction, &has_look_point, &look_point))
                        {
                            used_idle = true;
                            got_look = true;
                        }
                    }
                    else
                    {
                        direction = actor->forward8ec;
                        got_look = true;
                    }
                    break;
                case 2:
                    if (actor->aim_direction.get(actor_index, _ai_direction_mode_object_point, 0, &direction,
                                                             &has_look_point, &look_point))
                        got_look = true;
                    break;
                }
            }
            if (got_look)
                actor->forward8f8 = direction;
            else
                actor->forward8f8 = actor->facing_vector;
        }
        if (!used_idle)
            actor_looking_reset_idle_timers(actor_index);
        if (used_override && actor->attention_direction.type == 7)
            actor_record_look_interest(actor_index, actor->attention_direction.object_index);
    }
    if (!actor->combat_flag339 && !actor_looking_variable_camera_up(actor->unit_index))
    {
        actor->forward8e0.k = 0.0f;
        if (normalize3d(&actor->forward8e0) == 0.0f)
            actor->forward8e0 = actor->facing_vector;
    }
    if (actor->attention_ticks > 0)
    {
        if (--actor->attention_ticks <= 0)
            actor->attention_level = 0;
    }
    if (actor->emotion_type > 0)
    {
        if (--actor->emotion_type <= 0)
            unit_set_emotional_state(actor->unit_index, -1, 1.0f, 2.5f);
    }
    actor->output_facing = actor->forward8e0;
    actor->output_aiming = actor->forward8ec;
    actor->output_looking = actor->forward8f8;
    actor_looking_exact_facing(actor_index, TEST_BIT(actor->flags, 10));
    if (has_look_point)
    {
        actor->control_flags |= 0x8000;
        actor->output_gaze = look_point;
    }
    if (actor->combat_status < 2 && (short)actor->behavior_mode < 4 && actor->output_animation_state != 138 &&
        actor->output_animation_state != 139)
        actor->output_aiming_speed = 1;
    else
        actor->output_aiming_speed = 0;
}
