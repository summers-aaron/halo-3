// networkinglibxbox/replication_encoding.obj: from the records 0x40f268 0x40f458 0x40f690 0x40f9a8 0x40fb00 0x40fc68 0x40fe48 0x40ff80 0x410130
// @flags /TP /O2 /Oc
#include "math/real_math.h"
// @address 0x40ff80 ?player_prediction_compare_identical@@YA_NPBUs_player_prediction@@0@Z
// @address 0x40f268 ?player_motion_encode@@YAXPAVc_bitstream@@PBUs_player_motion@@@Z
// @address 0x40f458 ?player_motion_decode@@YA_NPAVc_bitstream@@PAUs_player_motion@@@Z
// @address 0x40f690 ?player_motion_valid@@YA_NPBUs_player_motion@@@Z
// @address 0x40f9a8 ?predicted_aim_assist_encode@@YAXPAVc_bitstream@@PBUs_player_predicted_aim_assist@@@Z
// @address 0x40fb00 ?predicted_aim_assist_decode@@YA_NPAVc_bitstream@@PAUs_player_predicted_aim_assist@@@Z
// @address 0x40fe48 ?player_prediction_decode@@YA_NPAVc_bitstream@@PAUs_player_prediction@@_N@Z
// @address 0x410130 ?player_prediction_valid@@YA_NPBUs_player_prediction@@@Z
// @address 0x205af8 ?valid_real_vector3d@@YA_NPBTreal_vector3d@@@Z
// @address 0x40fc68 ?player_prediction_encode@@YAXPAVc_bitstream@@PBUs_player_prediction@@_N@Z
#include <math.h>
#include <string.h>
#include "networking/replication/replication_encoding.h"
#include "simulation/simulation_encoding.h"
#include "render/render_lens_flares.h"
#include "memory/bitstream.h"



#include "networking/replication/replication_motion.h"
#include "math/real_math_types.h"



static __forceinline bool replication_encoding_valid_angles2d(const real_euler_angles2d *angles)
{
    bool valid = true;
    valid = valid && valid_real(angles->yaw);
    valid = valid && valid_real(angles->pitch);
    valid = valid && angles->yaw >= 0.0f && angles->yaw <= 6.2831855f;
    valid = valid && angles->pitch >= -1.5707964f && angles->pitch <= 1.5707964f;
    return valid;
}

static __forceinline bool replication_encoding_valid_vector2d(const real_vector2d *vector)
{
    return valid_real(vector->i) && valid_real(vector->j);
}

bool player_motion_valid(const s_player_motion *motion)
{
    bool valid = true;
    valid = valid && motion != 0;
    valid = valid && motion->type >= 0 && motion->type < 2;
    if (motion->type == 1)
        valid = valid && motion->identifier < 16;
    else
        valid = valid && motion->identifier == 255;
    valid = valid && replication_encoding_valid_angles2d(&motion->facing);
    valid = valid && replication_encoding_valid_vector2d(&motion->throttle);
    valid = valid && fabsf(motion->throttle.i) <= 1.0f;
    valid = valid && fabsf(motion->throttle.j) <= 1.0f;
    valid = valid && (unsigned int)(motion->flags & ~63) == 0;
    valid = valid && (motion->primary == -1 || (motion->primary >= 0 && motion->primary < 4));
    valid = valid && (motion->secondary == -1 || (motion->secondary >= 0 && motion->secondary < 4));
    valid = valid && (motion->primary == -1 || motion->primary != motion->secondary);
    valid = valid && (motion->zoom == -1 || (motion->zoom >= 0 && motion->zoom < 4));
    return valid;
}



const float k_predicted_aim_assist_lead_vector_minimum_magnitude = 0.01f;
const float k_predicted_aim_assist_lead_vector_maximum_magnitude = 15.0f;
const float k_predicted_aim_assist_minimum_autoaim_level = 0.0f;

static __forceinline void replication_encoding_entity_index_encode(c_bitstream *packet, long entity_index)
{
    packet->write_integer("entity-index", entity_index & 1023, 10);
    packet->write_integer("entity-salt", (unsigned long)entity_index >> 28, 4);
}

static __forceinline void replication_encoding_entity_index_decode(c_bitstream *packet, long *entity_index)
{
    long absolute_index = packet->read_integer("entity-absolute-index", 10);
    unsigned char seed = (unsigned char)packet->read_integer("entity-seed", 4);
    *entity_index = absolute_index | seed << 28;
}

void predicted_aim_assist_encode(c_bitstream *packet, const s_player_predicted_aim_assist *aim)
{
    if (aim->target != -1)
    {
        packet->write_bool("predicted-aim-assist", true);
        float primary = aim->primary_autoaim_level > k_predicted_aim_assist_minimum_autoaim_level
            ? aim->primary_autoaim_level
            : k_predicted_aim_assist_minimum_autoaim_level;
        float secondary = aim->secondary_autoaim_level > k_predicted_aim_assist_minimum_autoaim_level
            ? aim->secondary_autoaim_level
            : k_predicted_aim_assist_minimum_autoaim_level;
        packet->write_quantized_real("primary-autoaim-level", primary, k_predicted_aim_assist_minimum_autoaim_level, 1.0f, 4, false, false);
        packet->write_quantized_real("secondary-autoaim-level", secondary, k_predicted_aim_assist_minimum_autoaim_level, 1.0f, 4, false, false);
        replication_encoding_entity_index_encode(packet, aim->target);
        packet->write_bool("model-target-exists", aim->model_target != -1);
        if (aim->model_target != -1)
            packet->write_integer("model-target", aim->model_target, 5);
        packet->write_bool("only-lead-vector-valid", aim->only_lead_vector_valid);
        packet->write_bool("lead-vector-valid", aim->lead_vector_valid);
        if (aim->lead_vector_valid)
            packet->write_vector("lead-vector", &aim->lead_vector, k_predicted_aim_assist_lead_vector_minimum_magnitude,
                k_predicted_aim_assist_lead_vector_maximum_magnitude, 8);
    }
    else
    {
        packet->write_bool("predicted-aim-assist", false);
    }
}

bool predicted_aim_assist_decode(c_bitstream *packet, s_player_predicted_aim_assist *aim)
{
    bool success = true;
    memset(aim, 0, sizeof(*aim));
    aim->target = -1;
    aim->model_target = -1;
    if (packet->read_bool("predicted-aim-assist"))
    {
        aim->primary_autoaim_level = packet->read_quantized_real("primary-autoaim-level", k_predicted_aim_assist_minimum_autoaim_level, 1.0f, 4, false);
        aim->secondary_autoaim_level = packet->read_quantized_real("secondary-autoaim-level", k_predicted_aim_assist_minimum_autoaim_level, 1.0f, 4, false);
        replication_encoding_entity_index_decode(packet, &aim->target);
        success = success && aim->target != -1;
        if (packet->read_bool("model-target-exists"))
            aim->model_target = packet->read_integer("model-target", 5);
        aim->only_lead_vector_valid = packet->read_bool("only-lead-vector-valid");
        aim->lead_vector_valid = packet->read_bool("lead-vector-valid");
        if (aim->lead_vector_valid)
            packet->read_vector("lead-vector", &aim->lead_vector, k_predicted_aim_assist_lead_vector_minimum_magnitude,
                k_predicted_aim_assist_lead_vector_maximum_magnitude, 8);
    }
    return success;
}


const float k_player_prediction_minimum_magnitude = 0.03f;

static __forceinline void replication_encoding_gamestate_index_encode(c_bitstream *packet, long gamestate_index)
{
    long identifier = (unsigned short)(gamestate_index >> 16);
    long absolute_index = (unsigned short)gamestate_index;
    packet->write_integer("gamestgate-index-id", identifier, 16);
    packet->write_integer("gamestate-index-absolute", absolute_index, 11);
}

static __forceinline bool limit3d_with_limit_squared(real_vector3d *v, float limit)
{
    float magnitude_squared = magnitude_squared3d(v);
    if (magnitude_squared > limit * limit)
    {
        scale_vector3d(v, limit * reciprocal_square_root(magnitude_squared), v);
        return true;
    }
    return false;
}

void player_prediction_encode(c_bitstream *packet, const s_player_prediction *prediction, bool predicted_entity)
{
    if (predicted_entity)
    {
        replication_encoding_entity_index_encode(packet, prediction->entity);
    }
    else
    {
        packet->write_bool("predicted-gamestate-valid", prediction->identifier + 1 != 0);
        if (prediction->identifier != -1)
            replication_encoding_gamestate_index_encode(packet, prediction->identifier);
    }
    packet->write_integer("flags", prediction->flags, 4);
    simulation_write_quantized_position(packet, &prediction->origin, 16, false, 0);
    packet->write_axes("prediction-axes", &prediction->forward, &prediction->up);
    real_vector3d velocity = prediction->velocity;
    limit3d_with_limit_squared(&velocity, 350.0f);
    packet->write_vector("prediction-translational-velocity", &velocity, k_player_prediction_minimum_magnitude, 350.0f, 10);
    real_vector3d angular = prediction->angular;
    limit3d_with_limit_squared(&angular, 30.0f);
    packet->write_vector("prediction-angular-velocity", &angular, k_player_prediction_minimum_magnitude, 30.0f, 8);
}

void player_motion_encode(c_bitstream *packet, const s_player_motion *motion)
{
    packet->write_integer("type", motion->type, 1);
    if (motion->type == 1)
        packet->write_integer("identifier", motion->identifier, 4);
    packet->write_quantized_real("yaw", motion->facing.yaw, 0.0f, 6.2831855f, 13, false, false);
    packet->write_quantized_real("pitch", motion->facing.pitch, -3.1415927f, 3.1415927f, 12, false, false);
    packet->write_quantized_real("throttle-x", motion->throttle.i, -1.0f, 1.0f, 5, true, false);
    packet->write_quantized_real("throttle-y", motion->throttle.j, -1.0f, 1.0f, 5, true, false);
    packet->write_bool("motion-flags-exist", motion->flags != 0);
    if (motion->flags != 0)
        packet->write_integer("motion-flags", motion->flags, 6);
    packet->write_integer("weapon-set-identifier", motion->weapon_set + 1, 5);
    packet->write_integer("primary-weapon-index", motion->primary + 1, 3);
    packet->write_integer("secondary-weapon-index", motion->secondary + 1, 3);
    packet->write_integer("zoom-level", motion->zoom + 1, 2);
    packet->write_bool("primary-predicted-fire-primary", motion->fire[0]);
    packet->write_bool("primary-predicted-fire-secondary", motion->fire[1]);
    packet->write_bool("secondary-predicted-fire-primary", motion->fire[2]);
    packet->write_bool("secondary-predicted-fire-secondary", motion->fire[3]);
    packet->write_bool("predicted-aim-assist-exists", motion->aim_exists);
    if (motion->aim_exists)
        predicted_aim_assist_encode(packet, &motion->aim_assist);
    packet->write_bool("motion-sensor-velocity-threshold-exceeded", motion->exceeded);
}

bool player_motion_decode(c_bitstream *packet, s_player_motion *motion)
{
    bool success = true;
    memset(motion, 0, sizeof(*motion));
    motion->identifier = 255;
    motion->aim_assist.primary_autoaim_level = 0.0f;
    motion->aim_assist.secondary_autoaim_level = 0.0f;
    motion->aim_assist.target = -1;
    motion->aim_assist.model_target = -1;
    motion->type = packet->read_integer("type", 1);
    if (motion->type == 1)
        motion->identifier = (unsigned char)packet->read_integer("identifier", 4);
    motion->facing.yaw = packet->read_quantized_real("yaw", 0.0f, 6.2831855f, 13, false);
    motion->facing.pitch = packet->read_quantized_real("pitch", -3.1415927f, 3.1415927f, 12, false);
    motion->throttle.i = packet->read_quantized_real("throttle-x", -1.0f, 1.0f, 5, true);
    motion->throttle.j = packet->read_quantized_real("throttle-y", -1.0f, 1.0f, 5, true);
    if (packet->read_bool("motion-flags-exist"))
        motion->flags = (unsigned short)packet->read_integer("motion-flags", 6);
    motion->weapon_set = (short)(packet->read_integer("weapon-set-identifier", 5) - 1);
    motion->primary = (signed char)(packet->read_integer("primary-weapon-index", 3) - 1);
    motion->secondary = (signed char)(packet->read_integer("secondary-weapon-index", 3) - 1);
    motion->zoom = (short)(packet->read_integer("zoom-level", 2) - 1);
    motion->fire[0] = packet->read_bool("primary-predicted-fire-primary");
    motion->fire[1] = packet->read_bool("primary-predicted-fire-secondary");
    motion->fire[2] = packet->read_bool("secondary-predicted-fire-primary");
    motion->fire[3] = packet->read_bool("secondary-predicted-fire-secondary");
    motion->aim_exists = packet->read_bool("predicted-aim-assist-exists");
    if (motion->aim_exists)
    {
        bool decoded = predicted_aim_assist_decode(packet, &motion->aim_assist);
        success = success && decoded;
    }
    motion->exceeded = packet->read_bool("motion-sensor-velocity-threshold-exceeded");
    success = success && player_motion_valid(motion);
    return success;
}


static __forceinline void replication_encoding_gamestate_index_decode(c_bitstream *packet, long *gamestate_index)
{
    long identifier = packet->read_integer("gamestgate-index-id", 16);
    long absolute_index = packet->read_integer("gamestate-index-absolute", 11);
    *gamestate_index = identifier << 16 | absolute_index;
}

bool player_prediction_decode(c_bitstream *packet, s_player_prediction *prediction, bool predicted_entity)
{
    bool success = true;
    if (predicted_entity)
    {
        replication_encoding_entity_index_decode(packet, &prediction->entity);
    }
    else if (packet->read_bool("predicted-gamestate-valid"))
    {
        replication_encoding_gamestate_index_decode(packet, &prediction->identifier);
    }
    else
    {
        prediction->identifier = -1;
    }
    prediction->flags = (unsigned char)packet->read_integer("flags", 4);
    ::simulation_read_quantized_position(packet, &prediction->origin, 16, 0);
    packet->read_axes("prediction-axes", &prediction->forward, &prediction->up);
    packet->read_vector("prediction-translational-velocity", &prediction->velocity, k_player_prediction_minimum_magnitude, 350.0f, 10);
    packet->read_vector("prediction-angular-velocity", &prediction->angular, k_player_prediction_minimum_magnitude, 30.0f, 8);
    success = success && player_prediction_valid(prediction);
    return success;
}

static __forceinline bool vector3d_equal(const real_vector3d *left, const real_vector3d *right)
{
    return left->i == right->i && left->j == right->j && left->k == right->k;
}

static __forceinline bool point3d_equal(const real_point3d *left, const real_point3d *right)
{
    return left->x == right->x && left->y == right->y && left->z == right->z;
}

bool player_prediction_compare_identical(const s_player_prediction *left, const s_player_prediction *right)
{
    bool identical = true;
    identical = identical && vector3d_equal(&left->angular, &right->angular);
    identical = identical && vector3d_equal(&left->forward, &right->forward);
    identical = identical && point3d_equal(&left->origin, &right->origin);
    identical = identical && vector3d_equal(&left->velocity, &right->velocity);
    identical = identical && vector3d_equal(&left->up, &right->up);
    return identical;
}

bool player_prediction_valid(const s_player_prediction *prediction)
{
    bool valid = true;
    valid = valid && prediction != 0;
    valid = valid && prediction->entity != -1;
    valid = valid && (unsigned int)(prediction->flags & -16) == 0;
    valid = valid && valid_real_point3d(&prediction->origin);
    valid = valid && valid_real_vector3d_axes2(&prediction->forward, &prediction->up);
    valid = valid && valid_real_vector3d(&prediction->velocity);
    valid = valid && valid_real_vector3d(&prediction->angular);
    return valid;
}

bool valid_real_vector3d(const real_vector3d *vector)
{
    return valid_real(vector->i) && valid_real(vector->j) && valid_real(vector->k);
}
