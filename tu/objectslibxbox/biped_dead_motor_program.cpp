// objectslibxbox/biped_dead_motor_program.obj: from the records 0x3865b0 0x386800 0x386900 0x3869c8 0x386eb8 0x386f90 0x387060 0x3870a0 0x387330 0x387480 0x387920 0x387a90 0x387f50 0x3884d0 0x388628 0x388668 0x388740 0x388820 0x388960 0x388d68 0x388e48 0x389220 0x3893b8 0x389468 0x3895a0 0x389770 0x389930 0x389ae8 0x389e20 0x389ea0 0x389f48 0x389f80
// @flags /TP /O2 /J /Oc
#include "cseries/cseries_types.h"
#include "animations/animation_state_change_settings.h"
#include "physics/havok_group_types.h"
#include "objects/ragdoll_source_types.h"
#include <string.h>
#include <math.h>
#include "game/game_time_types.h"
#include "objects/object_placement.h"
#include "objects/object_header.h"
#include "game/game_globals_storage.h"
#include <float.h>
#include "cseries/cseries.h"
#include "math/matrix_math.h"
#include "math/real_math.h"
#include "math/real_constant_pool.h"
#include "phys/phys_base.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "ai/ai_actions.h"
#include "motor/motor_system.h"
#include "motor/motor_system_biped.h"
#include "objects/damage.h"
#include "objects/object_early_movers.h"
#include "physics/havok.h"
#include "physics/havok_utilities.h"
#include "units/unit_animation.h"
#include "game/game_time.h"
#include "game/game_time_inline.h"
#include "physics/physics_model_definitions.h"
#include "motor/biped_dead_motor_program.h"
#include "units/motor_request.h"
#include "animations/animation_id.h"
#include "motor/motor_programs.h"
#include "motor/action_system.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/character_physics_mode_flying.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "units/unit_animation_key_frame_status_types.h"
#include "cache/cache_files.h"
#include "physics/havok_component.h"
#include "physics/character_physics_move_output.h"
#include "physics/havok_component_class.h"
#include "phys/phys_ragdoll_data.h"
#include "units/biped_types.h"
#include "animations/animation_control_data.h"
#include "physics/havok_constants_types.h"
#include "units/biped_definitions.h"
#include "models/model_types.h"
#include "memory/tls_globals.h"
#include "objects/damage_globals.h"
#include "models/model_variant_types.h"
#include "physics/physics_model_types.h"
#include "phys/phys_limited_hinge_data.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "game/game_time_conversion.h"
#include "physics/physics_model_constraint_reference.h"
#include "math/real_math_constants.h"
#include "units/bipeds.h"
#include "math/real_math_functions.h"
#include "physics/character_physics_ground_helpers.h"
#include "units/motor_enums.h"
#include "animations/animation_search_path_iterator.h"
#include "game/game_ragdoll.h"
#include "units/units.h"
#include "animations/animation_interpolation.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"
#include <new>
#include "physics/character_physics_shape_types.h"
#include "units/motor_program_state.h"
#include "units/unit_types.h"
#include "math/random_math_range.h"
#include "physics/havok_component_datum_access.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "units/unit_animation_types.h"
// @address 0x3aac8 k_real_math_identity_matrix_storage

// @address 0x17d750 ?matrix4x3_inverse_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z

enum e_biped_relaxation_state
{
    _biped_relaxation_state_none = 0,
    _biped_relaxation_state_ragdoll = 1,
    _biped_relaxation_state_frozen = 2
};
enum e_biped_relaxation_pose { _biped_relaxation_pose_none = 0 };
typedef s_object_block_reference biped_dead_animation_block_reference;
typedef c_animation_id biped_dead_animation_id;
struct s_dead_flag_holder { char prefix[8]; unsigned long flags; };

struct s_biped_dead_holder {
    e_motor_program motor_program;
    unsigned short program_state;
    union { unsigned long flags; c_flags_no_init<long, unsigned long, 32> state_flags; };
    signed char airborne_ticks;
    signed char ground_ticks;
    signed char pose;
    signed char ticks;
    volatile long last_motion_time;
    volatile long last_airborne_time;
    unsigned char state;

    volatile long time;
    __forceinline e_biped_relaxation_state get_relaxation_state() const { return (e_biped_relaxation_state)(signed char)state; }
    __forceinline void set_relaxation_state(e_biped_relaxation_state value)
    {
        time = game_time_get();
        state = (unsigned char)(signed char)value;
        if (value == 2) { pose = -1; last_motion_time = -1; }
    }
};
typedef char dead_state_pose_offset[(offsetof(s_biped_dead_holder, pose) == 14) ? 1 : -1];
typedef char dead_state_relaxation_offset[(offsetof(s_biped_dead_holder, state) == 24) ? 1 : -1];
typedef char dead_state_size[(sizeof(s_biped_dead_holder) == 32) ? 1 : -1];

// the object's datum: the block at +12 of its header entry

// The relaxation and recent-death readers retain their original nonvolatile
// accesses through const_cast; other holder readers intentionally use volatile time fields.
// the state byte at +24 of the object's holder (its datum plus the header offset)
static __forceinline unsigned char biped_dead_header_byte24(void *object)
{
    biped_datum *biped = (biped_datum *)object;
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    return holder->state;
}

static __forceinline real_vector3d *biped_dead_vector_from_line3d(const real_vector3d *a, const real_vector3d *b, float t, real_vector3d *result)
{
    result->i = a->i + t * b->i;
    result->j = a->j + t * b->j;
    result->k = a->k + t * b->k;
    return result;
}

// @address 0x389f48 ?biped_dead_relaxation_state@@YAFJ@Z
short biped_dead_relaxation_state(long biped_index)
{
    object_datum *object = object_get(biped_index);
    biped_datum *biped = (biped_datum *)object;
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    return (signed char)holder->state;
}

static __forceinline long biped_dead_seconds_to_ticks_round(float seconds)
{
    return (long)((float)game_time_globals->ticks_per_second * seconds + 0.5f);
}
static __forceinline float biped_dead_ragdoll_transition_fraction(c_character_physics_mode_dead_datum *physics)
{
    float result=1.0f;
    if (physics->m_ragdoll_start_time != -1) {
        long ticks=game_time_get()-physics->m_ragdoll_start_time;
        float seconds=game_ticks_to_seconds((float)ticks);
        result=seconds / 1.25f;
        result=(result > 0.0f ? result : 0.0f) < 1.0f ? (result > 0.0f ? result : 0.0f) : 1.0f;
    }
    return result;
}

extern "C" {
// @address 0x388668 ?biped_relaxation_finish@@YAXJ@Z
// @address 0x3aa90 k_real_math_origin3d
}
// @address 0xa9338 ?normalize3d@@YAMPATreal_vector3d@@@Z

static __forceinline void biped_dead_scale_constraint_limit_tau(c_havok_component *component, long index, float tau)
{
    c_havok_component::c_constraint *item = &component->m_component_constraints[index];
    long type = item->m_header.constraint_type;
    switch (type)
    {
    case 1:
        ((phys_limited_hinge_data *)component->m_component_constraints[index].m_contact_constraint->get_chain_data())
            ->m_data_atoms.m_ang_limit.m_ang_limits_tau_factor = tau;
        break;
    case 2:
        phys_ragdoll_data *data = (phys_ragdoll_data *)component->m_component_constraints[index].m_contact_constraint->get_chain_data();
        data->m_data_atoms.m_twist_limit.m_ang_limits_tau_factor = tau;
        data->m_data_atoms.m_cone_limit.m_ang_limits_tau_factor = tau;
        data->m_data_atoms.m_planes_limit.m_ang_limits_tau_factor = tau;
        break;
    }
}
static __forceinline short biped_dead_controlling_rigid_body_index(const c_havok_component *component)
{
    long result = component->m_object_postion_controlling_rigid_body_index;
    if (result < 0 || result >= component->m_rigid_bodies.m_size) result = -1;
    return result;
}

static __forceinline const short biped_dead_object_position_controlling_rigid_body_index(const c_havok_component *component)
{
    return (short)((component->m_object_postion_controlling_rigid_body_index >= 0 &&
        component->m_object_postion_controlling_rigid_body_index < component->m_rigid_bodies.m_size)
        ? component->m_object_postion_controlling_rigid_body_index : -1);
}

// 0x389e20: the flag word of the holder at +8 (bit 3) and the dead-flag count in the game globals
// @address 0x389e20 biped_dead_set_ragdoll_flag
extern "C" void biped_dead_set_ragdoll_flag(s_dead_flag_holder *dead_state,bool set)
{
    if (set)
        game_globals->ragdoll_count++;
    else {
        unsigned char flagged=(unsigned char)((dead_state->flags >> 3) & 1);
        if (flagged != 0)
            game_globals->ragdoll_count--;
    }
    if (set)
        dead_state->flags |= 0x8;
    else
        dead_state->flags &= ~0x8ul;
}

// 0x389ea0
// @address 0x389ea0 ?biped_dead_in_airborne_state@@YA_NJ@Z
bool biped_dead_in_airborne_state(long object_index)
{
    biped_datum *biped=OBJECT_GET(object_index, biped_datum, 1);
    s_biped_dead_holder *header=(s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    unsigned char dead_bit=(unsigned char)((biped->object.physics_flags >> 9) & 1);
    bool result=dead_bit == 0 && header->airborne_ticks >= biped_dead_seconds_to_ticks_round(0.18f) && biped->object.parent_object_index == -1;
    return result;
}

// @address 0x389f80 biped_dead_test_recent_death
extern "C" bool biped_dead_test_recent_death(long object_index)
{
    biped_datum *biped=OBJECT_GET(object_index, biped_datum, 1);
    s_biped_dead_holder *header=(s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    bool result=false;
    unsigned char flagged=(unsigned char)((biped->object.damage_flags >> 2) & 1);
    if (flagged != 0) {
        if (const_cast<long &>(header->last_airborne_time) != -1) {
            if (game_time_get()-const_cast<long &>(header->last_airborne_time) < biped_dead_seconds_to_ticks_round(2.0f))
                result=true;
        }
    }
    return result;
}

// 0x3884d0: the object's rigid bodies take the input direction's share of the object's velocity
// @address 0x3884d0 ?biped_initialize_melee_death@@YAXJJPBTreal_vector3d@@@Z
void biped_initialize_melee_death(long object_index,long rigid_body_index,const real_vector3d *input)
{
    biped_datum *biped=OBJECT_GET(object_index, biped_datum, 1);
    if (biped->object.havok_component_index != -1) {
        c_havok_component *component=havok_component_get(biped->object.havok_component_index);
        real_vector3d velocity;
        real_vector3d localized_velocity[2];
        ::object_get_localized_velocities(object_index, &velocity, 0,
            localized_velocity, 0);
        real_vector3d direction=*input;
        float length=normalize3d(&direction);
        if (length != 0.0f) {
            float speed=dot_product3d(&velocity,&direction);
            speed=speed>0.0f?speed:0.0f;
            float acceleration=length-speed;
            if (acceleration>0.0f) {
                long rigid_body_count=component->m_rigid_bodies.m_size;
                long i=0;
                add_vectors3d(localized_velocity,&velocity,localized_velocity);
                biped_dead_vector_from_line3d(localized_velocity,&direction,acceleration,localized_velocity);
                if (rigid_body_count > 0) {
                    do {
                        component->set_rigid_body_linear_velocity(i, localized_velocity);
                        i++;
                    } while (i < component->m_rigid_bodies.m_size);
                }
            }
        }
    }
}

// 0x388628: the state-1 object's relaxation finishes
// @address 0x388628 ?biped_dead_motor_program_early_mover_localize_callback@@YAXJ@Z
void biped_dead_motor_program_early_mover_localize_callback(long object_index)
{
    object_datum *object = object_get(object_index);
    if (biped_dead_header_byte24(object) != 1)
        return;
    ::biped_relaxation_finish(object_index);
}

// 0x388740: the object's safe position, relative to the object, from its controlling body's world position

// 0x387060: the holder's dead flag is cleared when the object's state byte is 1
// @address 0x387060 ?biped_dead_clear_dead_flag@@YAPAUs_dead_flag_holder@@J@Z
s_dead_flag_holder *biped_dead_clear_dead_flag(long object_index)
{
    object_datum *object = object_get(object_index);
    s_dead_flag_holder *holder=(s_dead_flag_holder *)object_header_block_get(object, &((biped_datum *)object)->unit.motor_program);
    if (biped_dead_header_byte24(object) == 1)
        biped_dead_set_ragdoll_flag(holder,false);
    return holder;
}

// @address 0x387330 ?biped_update_constraint_limits@@YAXJ@Z
void biped_update_constraint_limits(long index) {
    biped_datum *biped=OBJECT_GET(index, biped_datum, 1);
    if ((unsigned char)biped->biped.physics.get_mode() == 3 && biped->object.havok_component_index != -1) {
        c_havok_component *component=havok_component_get(biped->object.havok_component_index);
        if (component->is_physics_model_havok_component()) {
            float fraction=biped_dead_ragdoll_transition_fraction(biped->biped.physics.get_mode_dead());
            for (long constraint_index=0; constraint_index < component->get_constraint_count(); ++constraint_index)
                biped_dead_scale_constraint_limit_tau(component, constraint_index, fraction);
        }
    }
}

// @rdata 0x82022dbc 4
static const float k_biped_relaxation_key_scale = 0.1f;

// @address 0x387920 ?biped_move_relaxation@@YAXJ@Z
void biped_move_relaxation(long object_index)
{
    biped_datum *biped=OBJECT_GET(object_index, biped_datum, 1);
    bool physics_disabled = ((biped->object.physics_flags >> 9) & 1) != 0;
    bool physics_flag4 = ((biped->object.physics_flags >> 4) & 1) != 0;
    if (!physics_disabled && !physics_flag4) {
        long period=biped_dead_seconds_to_ticks_round(0.3f);
        if (biped_dead_header_byte24(biped) == 1 && biped->object.havok_component_index != NONE) {
            c_havok_component *component=havok_component_get(biped->object.havok_component_index);
            long key=game_time_add_water_ripple(object_index,period,(float)(double)component->m_component_constraints.m_size * k_biped_relaxation_key_scale);
            if (key == -1)
                key=object_index & 0xffff;
            object_compute_node_matrices(object_index);
            if ((game_time_get() + key) % period == 0)
                biped_bring_ragdoll_inside_world(object_index,true);
            ::biped_update_constraint_limits(object_index);
        }
        object_in_early_mover_physics_move(object_index,biped_dead_motor_program_early_mover_localize_callback);
        return;
    }
    biped->object.high_frequency_rest_ticks=0;
    biped->object.low_frequency_rest_ticks=0;
}

// Finish relaxation by freezing the node orientations and rebuilding physics.
void biped_relaxation_finish(long object_index)
{
    object_datum *object = object_get(object_index);
    s_biped_dead_holder *holder=(s_biped_dead_holder *)object_header_block_get(object, &((biped_datum *)object)->unit.motor_program);
    bool finished=(signed char)holder->state == 1;
    ::biped_relaxation_calculate_object_relative_safe_position(object_index);
    if (finished) {
        object_compute_node_matrices(object_index);
        object_reverse_compute_node_orientations_from_matrices(object_index,0);
    }
    object_freeze_node_orientations(object_index,true);
    long time=game_time_get();
    holder->state=2;
    holder->pose=-1;
    const_cast<long &>(holder->last_motion_time)=-1;
    const_cast<long &>(holder->time)=time;
    object_set_velocities(object_index,global_zero_vector3d,global_zero_vector3d);
    ::havok_object_rebuild(object_index, 0);
    if (finished)
        biped_dead_set_ragdoll_flag((s_dead_flag_holder *)holder,false);
}

static __forceinline __vector4 biped_dead_vector_set(float x, float y, float z, float w)
{
    __vector4 result;
    result.v[0] = x;
    result.v[1] = y;
    result.v[2] = z;
    result.v[3] = w;
    return result;
}
static __forceinline long biped_dead_get_rigid_body_count(const c_havok_component *component) { return component->m_rigid_bodies.m_size; }
static __forceinline void biped_dead_get_rigid_body_linear_velocity(c_havok_component *component, long index, real_vector3d *linear_velocity)
{
    phys_rigid_body *rigid_body = component->m_rigid_bodies[index].m_rigid_body;
    if (rigid_body->m_motion.m_type != 7) {
        const phys_vec4 &velocity = rigid_body->m_motion.m_linear_velocity;
        linear_velocity->i = velocity(0);
        linear_velocity->j = velocity(1);
        linear_velocity->k = velocity(2);
    } else {
        *linear_velocity = *global_zero_vector3d;
    }
}
static __forceinline void biped_dead_set_rigid_body_linear_velocity(c_havok_component *component, long index, const real_vector3d *linear_velocity)
{
    phys_rigid_body *rigid_body = component->m_rigid_bodies[index].m_rigid_body;
    if (rigid_body->m_motion.m_type != 7) {
        phys_vec4 velocity;
        velocity.m_simd_quad = biped_dead_vector_set(linear_velocity->i, linear_velocity->j, linear_velocity->k, 0.0f);
        rigid_body->activate();
        rigid_body->m_motion.set_linear_velocity(velocity);
    }
}
// @address 0x388740 ?biped_relaxation_calculate_object_relative_safe_position@@YAXJ@Z
void biped_relaxation_calculate_object_relative_safe_position(long index) {
    biped_datum *biped=OBJECT_GET(index, biped_datum, 1);
    if (biped->object.havok_component_index != -1) {
        c_havok_component *component=havok_component_get(biped->object.havok_component_index);
        long body_index=biped_dead_controlling_rigid_body_index(component);
        real_point3d world_position;
        component->get_rigid_body_center_of_mass(body_index, &world_position);
        real_matrix4x3 world_matrix;
        ::object_get_world_matrix(index, &world_matrix);
        real_point3d relative_position;
        matrix4x3_inverse_transform_point(&world_matrix,&world_position,&relative_position);
        biped->biped.physics.get_mode_dead()->m_object_relative_safe_position=relative_position;
    }
}
// @address 0x388820 ?biped_update_out_of_map_status@@YAXJ@Z
void biped_update_out_of_map_status(long biped_index)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    bool physics_disabled = ((biped->object.physics_flags >> 9) & 1) != 0;
    bool out_of_map = ((biped->biped.flags >> 5) & 1) != 0;
    if (!physics_disabled && !out_of_map && biped->object.havok_component_index != -1 && ((unsigned long)(game_time_get() + biped_index) & 3) == 0) {
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        short rigid_body_index = biped_dead_controlling_rigid_body_index(component);
        if (rigid_body_index != -1) {
            float radius = g_havok_constants.minimum_radius;
            real_point3d center_of_mass;
            real_point3d fixed_position;
            component->get_rigid_body_center_of_mass(rigid_body_index, &center_of_mass);
            if (!havok_try_and_get_sphere_fixed_position(g_havok_constants.minimum_radius, radius, _havok_group_ragdoll_biped, 1, biped_index,
                    &center_of_mass, 0, -1, &fixed_position, false))
            {
                biped->biped.m_flags.set(_biped_out_of_map, true);
                biped->biped.m_flags.set(_biped_kill_volume_damaged, true);
            }
        }
    }
}
extern "C" {
// @address 0x79ddbc g_biped_relaxation_angular_velocity_threshold
}

const real biped_dead_relaxation_scale_maximum = 1.0f;

static __forceinline float biped_dead_normalize3d(real_vector3d *vector, float &inverse)
{
    float length = magnitude3d(vector);
    if (fabsf(length) >= 0.0001f) {
        inverse = 1.0f / length;
        vector->i /= length;
        vector->j *= inverse;
        vector->k *= inverse;
    } else
        length = 0.0f;
    return length;
}
static __forceinline real_point3d *biped_dead_point_from_line3d(const real_point3d *point, const real_point3d *vector, float t, real_point3d *result)
{
    result->x = point->x + vector->x * t;
    result->y = point->y + vector->y * t;
    result->z = point->z + vector->z * t;
    return result;
}
static __forceinline real_point3d *biped_dead_scale_point3d(const real_point3d *point, float scale, real_point3d *result)
{
    result->x = point->x * scale;
    result->y = point->y * scale;
    result->z = point->z * scale;
    return result;
}
// a head shot pushes the dead biped's rigid bodies sideways about its center of mass, scaled by the object's speed
// @address 0x387f50 ?biped_initialize_head_shot_death@@YAXJJPBTreal_vector3d@@@Z
void biped_initialize_head_shot_death(long biped_index, long rigid_body_index, const real_vector3d *direction)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    if (biped->object.havok_component_index != -1) {
        biped_definition *definition =
            TAG_GET('bipd', biped_definition, biped->object.definition_index);
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        const float minimum_acceleration = 1.5f;
        const float maximum_acceleration = 3.25f;
        const float minimum_up_acceleration = 0.75f;
        const float maximum_up_acceleration = 1.5f;
        const float back_acceleration_scale = 0.65f;
        const float hit_body_acceleration_scale = 2.0f;
        const float maximum_definition_scale = 0.7f;
        float acceleration_scale = _real_random_range(TLS_RANDOM_SEED, minimum_acceleration, maximum_acceleration);
        float up_scale = _real_random_range(TLS_RANDOM_SEED, minimum_up_acceleration, maximum_up_acceleration);
        float total_mass = 0.0f;
        real_point3d center_of_mass = *global_origin3d;
        const float maximum_speed = 2.4f;
        const float minimum_strength = 0.5f;
        const float velocity_weight = 0.5f;
        real_vector3d linear_velocity;
        object_get_localized_velocities(biped_index, &linear_velocity, 0, 0, 0);
        float speed = sqrtf(dot_product3d(&linear_velocity, &linear_velocity)) / maximum_speed;
        speed = PIN(speed, 0.0f, 1.0f);
        float scale = speed * velocity_weight;
        if (normalize3d(&linear_velocity) != 0.0f) {
            real_vector3d hit_direction = *direction;
            if (normalize3d(&hit_direction) != 0.0f) {
                float dot = linear_velocity.i * hit_direction.i + (hit_direction.k * linear_velocity.k + linear_velocity.j * hit_direction.j);
                if (dot < 0.0f)
                    scale += -dot * (1.0f - velocity_weight);
            }
        }
        float strength = scale * (1.0f - minimum_strength) + minimum_strength;
        float acceleration = strength * acceleration_scale;
        float up_acceleration = strength * up_scale;

        real_point3d head_position;
        component->get_rigid_body_center_of_mass(rigid_body_index, &head_position);
        for (long i = 0; i < biped_dead_get_rigid_body_count(component); i++) {
            long mass = (long)component->get_rigid_body_mass(i);
            real_point3d position;
            component->get_rigid_body_center_of_mass(i, &position);
            total_mass += mass;
            biped_dead_point_from_line3d(&center_of_mass, &position, mass, &center_of_mass);
        }
        biped_dead_scale_point3d(&center_of_mass, 1.0f / total_mass, &center_of_mass);

        float inverse_length;
        real_vector3d axis;
        vector_from_points3d(&center_of_mass, &head_position, &axis);
        float distance = biped_dead_normalize3d(&axis, inverse_length);
        if (distance > 0.001f) {
            real_vector3d projection;
            real_vector3d tangent;
            float dot = dot_product3d(direction, &axis);
            projection.i = axis.i * dot;
            projection.j = axis.j * dot;
            projection.k = axis.k * dot;
            subtract_vectors3d(direction, &projection, &tangent);
            if (normalize3d(&tangent) != 0.0f) {
                scale_vector3d(&tangent, MIN(definition->head_shot_acceleration_scale, maximum_definition_scale) * acceleration, &tangent);

                float maximum_distance = 0.0f;
                for (long i = 0; i < biped_dead_get_rigid_body_count(component); i++) {
                    real_point3d position;
                    component->get_rigid_body_center_of_mass(i, &position);
                    real_vector3d offset;
                    vector_from_points3d(&center_of_mass, &position, &offset);
                    float distance = dot_product3d(&offset, &axis);
                    maximum_distance = MAX(maximum_distance, distance);
                }
                for (long i = 0; i < biped_dead_get_rigid_body_count(component); i++) {
                    real_point3d position;
                    component->get_rigid_body_center_of_mass(i, &position);
                    real_vector3d offset;
                    vector_from_points3d(&center_of_mass, &position, &offset);
                    float distance = dot_product3d(&offset, &axis);
                    float fraction = (distance < 0.0f ? back_acceleration_scale : 1.0f) * (1.0f / maximum_distance) * distance;
                    if (i == rigid_body_index)
                        fraction *= hit_body_acceleration_scale;
                    real_vector3d acceleration_vector;
                    scale_vector3d(&tangent, fraction, &acceleration_vector);
                    acceleration_vector.k += up_acceleration;
                    real_vector3d velocity;
                    biped_dead_get_rigid_body_linear_velocity(component, i, &velocity);
                    add_vectors3d(&velocity, &acceleration_vector, &velocity);
                    biped_dead_set_rigid_body_linear_velocity(component, i, &velocity);
                }
            }
        }
    }
}

// the relaxation constraints ease in from the last motion time
// @address 0x3895a0 ?biped_relaxation_pose_update_scale@@YAXJ@Z
void biped_relaxation_pose_update_scale(long biped_index)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    s_biped_dead_holder *relaxation = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    if ((unsigned char)relaxation->pose != 0xff && biped->object.havok_component_index != NONE) {
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        if (havok_component_has_active_entity(component) && component->is_physics_model_havok_component()) {
            float scale = 0.0f;
            {
                real_vector3d linear_velocity;
                real_vector3d angular_velocity;
                ::object_get_localized_velocities(biped_index, &linear_velocity,
                    &angular_velocity, 0, 0);
                if (linear_velocity.i * linear_velocity.i + linear_velocity.k * linear_velocity.k +
                        linear_velocity.j * linear_velocity.j > 1.0f * 1.0f ||
                    angular_velocity.i * angular_velocity.i + angular_velocity.k * angular_velocity.k +
                        angular_velocity.j * angular_velocity.j >
                        g_biped_relaxation_angular_velocity_threshold * g_biped_relaxation_angular_velocity_threshold)
                    const_cast<long &>(relaxation->last_motion_time) = game_time_get();
            }
            if (const_cast<long &>(relaxation->last_motion_time) != NONE) {
                float elapsed = game_ticks_to_seconds((float)(game_time_get() - const_cast<long &>(relaxation->last_motion_time)));
                if (elapsed < 1.0f) {
                    scale = 1.0f - elapsed / 1.0f;
                    scale = PIN(scale, 0.0f, biped_dead_relaxation_scale_maximum);
                }
            }
            for (long constraint_index = 0; constraint_index < component->get_constraint_count(); constraint_index++)
                component->scale_constraint_motor_power(constraint_index, scale);
        }
    }
}

static const float biped_relaxation_worst_dot = -FLT_MAX;
static const real_vector3d biped_dead_up3d = { 0.0f, 0.0f, 1.0f };
static const real_vector3d biped_dead_down3d = { 0.0f, 0.0f, -1.0f };
static __forceinline const real_vector3d *biped_dead_global_up3d_get() { return &biped_dead_up3d; }
static __forceinline const real_vector3d *biped_dead_global_down3d_get() { return &biped_dead_down3d; }
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z

// the facing direction (and airborne offset) with the best relaxation dot product
// @address 0x389930 ?biped_relaxation_calculate_best_relaxation_pose@@YA?AW4e_biped_relaxation_pose@@J@Z
e_biped_relaxation_pose biped_relaxation_calculate_best_relaxation_pose(long biped_index)
{
    float best = biped_relaxation_worst_dot;
    long pose;
    bool airborne = ::biped_dead_in_airborne_state(biped_index);
    const real_vector3d *directions[6] = {biped_dead_global_down3d_get(), biped_dead_global_up3d_get(), biped_dead_global_down3d_get(),
        biped_dead_global_up3d_get(), biped_dead_global_down3d_get(), biped_dead_global_up3d_get()};
    object_marker marker;
    real_matrix4x3 matrix;
    real_matrix3x3 *m;
    if (internal_object_get_markers_by_string_id(biped_index, _string_id_body, &marker, 1, false) == 1)
        m = (real_matrix3x3 *)&marker.matrix.forward;
    else {
        ::object_get_world_matrix(biped_index, &matrix);
        m = (real_matrix3x3 *)&matrix.forward;
    }
    const real_vector3d *vectors[6] = { &m->up, &m->up, &m->forward, &m->forward, &m->left, &m->left };
    for (unsigned long i = 0; i < 6; i++) {
        float dot = dot_product3d(vectors[i], directions[i]);
        if (dot > best) {
            best = dot;
            pose = (airborne ? 6 : 0) + i;
        }
    }
    return (e_biped_relaxation_pose)pose;
}

// ---- relaxation pose ----

// match: noalias declaration preserves caller live registers

// @address 0x389ae8 ?biped_keyframe_ragdoll_motors_to_nodes@@YAXJ@Z
void biped_keyframe_ragdoll_motors_to_nodes(long biped_index)
{
    biped_datum *biped = (biped_datum *)object_get(biped_index);
    long havok_index = biped->object.havok_component_index;
    if (havok_index == -1)
        return;
    c_havok_component *component = havok_component_get(havok_index);
    if (!component->is_physics_model_havok_component())
        return;
    char *node_base = (char *)biped + biped->object.node_orientations.offset;
    biped_definition *object_def = TAG_GET('bipd', biped_definition, biped->object.definition_index);
    s_model_definition *model_def = TAG_GET('hlmt', s_model_definition, object_def->unit.model.index);
    s_physics_model_definition *physics_def = TAG_GET('phmo', s_physics_model_definition, model_def->physics_model_index);
    for (long index = 0; index < component->get_constraint_count(); index++)
    {
        c_havok_component::c_constraint *c = &component->m_component_constraints[index];
        if ((c->m_header.flags & 1) == 0)
            continue;
        short type = c->m_header.constraint_type;
        short body = c->m_header.constraint_type_index;
        long node_a = -1;
        long node_b = -1;
        s_physics_model_constraint_reference reference;
        reference.type = type;
        reference.index = body;
        long body_b_index;
        long body_a_index;
        ((c_havok_component *)component)->get_constraint_rigid_bodies(index, &body_a_index, &body_b_index);
        c_havok_component::c_rigid_body *body_a = &component->m_rigid_bodies[body_a_index];
        if (body_a->m_physics_model_rigid_body_indices.m_size > 0)
            node_a = physics_def->rigid_bodies[(signed char)body_a->m_physics_model_rigid_body_indices.m_data[0]].node;
        c_havok_component::c_rigid_body *body_b = &component->m_rigid_bodies[body_b_index];
        if (body_b->m_physics_model_rigid_body_indices.m_size > 0)
            node_b = physics_def->rigid_bodies[(signed char)body_b->m_physics_model_rigid_body_indices.m_data[0]].node;
        if (node_a == -1 || node_b == -1)
            continue;
        s_model_node *anim = &((s_model_node *)model_def->nodes.address)[node_b];
        real_quaternion node_rotation = anim->default_rotation;
        node_rotation.w *= -1.0f;
        real_quaternion out_rotation;
        union { real_matrix3x3 rotation; __vector4 quad[3]; } basis;
        quaternions_multiply(&node_rotation, (const real_quaternion *)(node_base + (node_b << 5)), &out_rotation);
        matrix3x3_rotation_from_quaternion(&basis.rotation, &out_rotation);
        physics_model_try_and_get_constraint((s_physics_model_definition *)physics_def, &reference);
        if (type != 1)
        {
            if (type == 2)
            {
                real_matrix4x3 target_matrix = *global_identity4x3;
                phys_mat3 result_frame;
                target_matrix.rotation = basis.rotation;
                matrix4x3_inverse(&target_matrix, &target_matrix);
                c = &component->m_component_constraints[index];
                if ((c->m_header.flags & 1) == 0)
                    continue;
                phys_constraint_data *data = c->m_contact_constraint->get_chain_data();
                phys_mat3 *from_frame = (phys_mat3 *)&basis;
                ::phys_mat3_from_matrix3x3((phys_rotation *)from_frame, &target_matrix.rotation);
                phys_mat3 source_frame(((phys_ragdoll_data *)data)->m_data_atoms.m_transform_atom.m_transform_b.get_rotation());
                result_frame.set_mul(*from_frame, source_frame);
                ((phys_ragdoll_data *)data)->set_ragdoll_target(result_frame);
            }
        }
        else
        {
            float angle = (float)atan2((double)basis.rotation.left.i, (double)basis.rotation.left.j);
            c = &component->m_component_constraints[index];
            if ((c->m_header.flags & 1) == 0)
                continue;
            phys_constraint_data *data = c->m_contact_constraint->get_chain_data();
            ((phys_limited_hinge_data *)data)->m_data_atoms.m_ang_motor.m_target_angle = angle;
        }
    }
}

static __forceinline bool biped_dead_channel_valid(const c_animation_manager *channel)
{
    return channel->m_state_channel.m_enabled != 0 && channel->m_state_channel.m_graph_index != -1 &&
        channel->m_state_channel.m_animation_id.m_animation_index >= 0;
}

static __forceinline void biped_dead_channel_set_position(c_animation_manager *channel, float position)
{
    if (biped_dead_channel_valid(channel))
        ((c_animation_channel *)channel)->set_frame_position(position);
}

// the relaxation pose is applied to the animation and the ragdoll motors are keyframed to the nodes
// @address 0x389770 ?biped_relaxation_pose_set@@YAXJW4e_biped_relaxation_pose@@@Z
void biped_relaxation_pose_set(long biped_index, e_biped_relaxation_pose pose)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    s_biped_dead_holder *relaxation = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    c_animation_manager *animation = (c_animation_manager *)object_header_block_get(biped, &biped->object.animation);
    bool first = (signed char)relaxation->pose == NONE;
    relaxation->pose = (unsigned char)pose;
    relaxation->ticks = 0;
    if (biped->object.havok_component_index != NONE) {
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        if (component->is_physics_model_havok_component()) {
            biped_dead_animation_id id =
                TAG_GET('jmad', const c_model_animation_graph, animation->m_graph_tag_index)
                    ->find_animation(538);
            if (id.animation_index >= 0) {
                if (first)
                    const_cast<long &>(relaxation->last_motion_time) = game_time_get();
                biped->biped.flags |= 512;
                ((c_animation_manager *)animation)->set_custom_goal(id, 0);
                biped_dead_channel_set_position(animation, (float)pose);
                ((c_animation_manager *)animation)->pause_state_channel(true);
                object_freeze_node_orientations(biped_index, false);
                object_compute_node_matrices(biped_index);
                biped_keyframe_ragdoll_motors_to_nodes(biped_index);
                biped->biped.flags &= ~512;
                object_compute_node_matrices(biped_index);
                object_freeze_node_orientations(biped_index, true);
                biped_relaxation_pose_update_scale(biped_index);
            }
        }
    }
}

// @address 0x3893b8 ?biped_relaxation_pose_rebuild@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_relaxation_pose_rebuild(long biped_index, const c_motor_request *request)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    s_biped_dead_holder *relaxation = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    long pose = (signed char)relaxation->pose;
    if (pose != NONE && biped->object.havok_component_index != NONE) {
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        if (component->is_physics_model_havok_component()) {
            biped_relaxation_pose_set(biped_index, (e_biped_relaxation_pose)pose);
            biped_relaxation_pose_update_scale(biped_index);
        }
    }
    return _motor_result_pending;
}

// @address 0x389468 ?biped_relaxation_pose_update@@YAXJ@Z
void biped_relaxation_pose_update(long biped_index)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    s_biped_dead_holder *relaxation = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    long old_pose = (signed char)relaxation->pose;
    if (old_pose != NONE && biped->object.havok_component_index != NONE) {
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        if (component->is_physics_model_havok_component()) {
            long ticks = relaxation->ticks + 1;
            signed char new_ticks = (signed char)(ticks < 126 ? ticks : 126);
            relaxation->ticks = new_ticks;
            if (new_ticks > biped_dead_seconds_to_ticks_round(0.1f)) {
                long best_pose = biped_relaxation_calculate_best_relaxation_pose(biped_index);
                if (best_pose != old_pose)
                    biped_relaxation_pose_set(biped_index, (e_biped_relaxation_pose)best_pose);
            }
            biped_relaxation_pose_update_scale(biped_index);
        }
    }
}

// the dead-motor holder: the relaxation state (pose, ticks, last motion time) sits in it too

// @address 0x3476e8 ?biped_enter_dead_physics@@YAXJ@Z

// Reset the dead motor state, enter dead physics, and service a pending relaxation finish.
// @address 0x386f90 biped_dead_initialize_motor
extern "C" int biped_dead_initialize_motor(long object_index)
{
    biped_datum *biped = (biped_datum *)object_get(object_index);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    holder->flags = 0;
    holder->program_state = 0;
    long time = game_time_get();
    holder->state = 0;
    holder->airborne_ticks = 0;
    holder->ground_ticks = 127;
    holder->pose = -1;
    holder->time = time;
    holder->last_motion_time = -1;
    holder->last_airborne_time = -1;
    if ((unsigned char)biped->biped.physics.get_mode() != 3)
        biped_enter_dead_physics(object_index);
    unsigned char pending = (biped->biped.flags >> 12) & 1;
    if (pending != 0) {
        biped_dead_reset_airborne_state(object_index);
        biped->biped.flags &= 0xefff;
    }
    return 1;
}

#if defined(__clang__)
static __forceinline float biped_dead_velocity_magnitude_squared(const real_vector3d &velocity);
static __forceinline void biped_dead_apply_relaxation_velocity(long object_index, const real_vector3d *velocity, float speed_squared);
#endif

// @address 0x388d68 biped_dead_reset_airborne_state
extern "C" void biped_dead_reset_airborne_state(long object_index)
{
    biped_datum *biped = OBJECT_GET(object_index, biped_datum, 1);
    short header_offset = biped->unit.motor_program.offset;
    unsigned short physics_flags = biped->biped.physics.m_flags;
    s_biped_dead_holder *holder = (s_biped_dead_holder *)((char *)biped + header_offset);
    biped->biped.physics.m_flags = physics_flags | 2;
    short tick_rate = game_time_globals->ticks_per_second;
    holder->ground_ticks = 0;
    holder->airborne_ticks = (unsigned char)(long)((float)tick_rate * 0.18f + 0.5f);
    object_set_at_rest(object_index, false);
    biped->biped.physics.get_mode_dead()->m_contact_ticks = 0;
    biped->biped.physics.get_mode_dead()->m_ground_ticks = 0;
    if (holder->state == 1) {
        game_time_globals_definition *time_globals = game_time_globals;
        holder->state_flags.set(4, true);
        holder->last_airborne_time = time_globals->time;
#if defined(__clang__)
        // Melee death applies knockback directly to the live rigid bodies.
        real_vector3d object_velocity = biped->object.linear_velocity;
        real_vector3d body_velocity = *global_zero_vector3d;
        if (biped->object.havok_component_index != NONE) {
            c_havok_component *component = havok_component_get(biped->object.havok_component_index);
            short rigid_body_index = biped_dead_controlling_rigid_body_index(component);
            if (rigid_body_index != NONE)
                biped_dead_get_rigid_body_linear_velocity(component, rigid_body_index, &body_velocity);
        }
#endif
        havok_object_rebuild(object_index, 0);
#if defined(__clang__)
        float body_speed_squared = biped_dead_velocity_magnitude_squared(body_velocity);
        float object_speed_squared = biped_dead_velocity_magnitude_squared(object_velocity);
        if (!(object_speed_squared <= body_speed_squared))
            biped_dead_apply_relaxation_velocity(object_index, &object_velocity, object_speed_squared);
        else
            biped_dead_apply_relaxation_velocity(object_index, &body_velocity, body_speed_squared);
#endif
    }
}

// @address 0x386800 ?biped_dead_force_airborne_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
enum e_motor_result biped_dead_force_airborne_submit(long biped_index, class c_motor_request const *request)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    enum e_motor_result result = _motor_result_failed;
    bool exit_relaxation = true;
    if (biped->object.parent_object_index != -1 && (unsigned char)((biped->object.physics_flags >> 4) & 1))
        object_in_early_mover_detach(biped_index);
    if (biped->object.parent_object_index == -1) {
        bool accepted;
        switch ((signed char)holder->state) {
        case 0:
            accepted = true;
            break;
        case 1:
            accepted = true;
            exit_relaxation = false;
            break;
        case 2:
            if (game_can_create_additonal_ragdoll(_ragdoll_source_death) && biped_start_relaxation(biped_index, false)) {
                accepted = true;
                exit_relaxation = false;
            } else {
                accepted = true;
                exit_relaxation = true;
            }
            break;
        default:
            // match: 0x386800: unreachable hint omits dead branch check
            __assume(0);
            break;
        }
        if (accepted) {
            if (exit_relaxation)
                biped_exit_relaxation(biped_index, true);
            biped_dead_reset_airborne_state(biped_index);
            result = _motor_result_pending;
        }
        holder->last_motion_time = game_time_get();
    }
    return result;
}

// @address 0x386900 ?biped_dead_force_relaxation_state_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_dead_force_relaxation_state_submit(long object_index, const c_motor_request *request)
{
    biped_datum *biped = OBJECT_GET(object_index, biped_datum, 1);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);

    if ((unsigned long)request->m_force_relaxation_state.relaxation_type >= 1) {
        if ((unsigned long)request->m_force_relaxation_state.relaxation_type != 1) {
            if (holder->state == 2)
                biped_exit_relaxation(object_index, request->m_force_relaxation_state.interrupt);
            if (game_can_create_additonal_ragdoll(_ragdoll_source_death) && holder->state != 1)
                biped_start_relaxation(object_index, false);
        } else {
            biped_exit_relaxation(object_index, request->m_force_relaxation_state.interrupt);
        }
    } else {
        biped_relaxation_finish(object_index);
    }
    return _motor_result_pending;
}

// @address 0x389220 ?biped_dead_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
enum e_motor_program biped_dead_move(long object_index, class c_motor_request *request)
{
    
    object_datum *datum = object_get(object_index);
    biped_datum *object = (biped_datum *)datum;
    biped_definition *definition = TAG_GET('bipd', biped_definition, object->object.definition_index);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(datum, &object->unit.motor_program);
    e_motor_program result = _motor_program_none;
    biped_move_relaxation(object_index);
    signed char relaxation_state = (signed char)holder->state;
    bool airborne = ::biped_dead_in_airborne_state(object_index);
    s_character_physics_move_output_datum output;
    s_character_physics_move_output_datum *output_pointer = &output;
    motor_system_biped_move(object_index, airborne, false, relaxation_state != 0, false, output_pointer);
    if (object->biped.stun_ticks == 1 && object->biped.physics.m_mode == 3 &&
        (bool)((definition->flags >> 10) & 1) && !(bool)((object->object.damage_flags >> 2) & 1)) {
        biped_enter_default_physics(object_index);
        motor_animation_set_mode_weapons_and_state(object_index, _string_id_default, _string_id_current, _string_id_current,
            _string_id_default, 0.0f, 2, 31, false);
        object_freeze_node_orientations(object_index, false);
        if (object->biped.physics.m_mode == 2) {
            c_character_physics_mode_flying_datum *flying = object->biped.physics.get_mode_flying();
            flying->m_turning_disabled_counter = biped_dead_seconds_to_ticks_round(1.5f);
            flying->m_axis = *global_zero_vector3d;
        }
        result = mover_default_program(object_index);
    }
    return result;
}

extern "C" {
// @address 0x79ddbc g_biped_relaxation_angular_velocity_threshold
float g_biped_relaxation_angular_velocity_threshold = 4.712389f;
// @address 0x8c443a g_biped_ragdoll_force_physics
unsigned char g_biped_ragdoll_force_physics;
// @address 0x8c443b g_biped_ragdoll_skip_rebuild
unsigned char g_biped_ragdoll_skip_rebuild;
}

// @address 0x387a90 ?biped_update_relaxation@@YA_NJ@Z
bool biped_update_relaxation(long object_index)
{
    biped_datum *biped = OBJECT_GET(object_index, biped_datum, 1);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    bool allow = (biped->unit.death_state != 0 && !(bool)((holder->flags >> 1) & 1)) ? true : false;
    bool result = false;
    switch ((signed char)holder->state) {
    case 0: {
        if (allow == 0) {
            bool both_set = (biped->biped.physics.get_mode_dead()->m_contact_on_relaxable_surface != 0
                && biped->biped.physics.get_mode_dead()->m_contact_ticks != 0);
            if (!both_set) {
                bool relaxation_locked = ((biped->biped.flags >> 11) & 1);
                if (!relaxation_locked)
                    break;
            }
        }
        do {
            biped_definition *definition =
                TAG_GET('bipd', biped_definition, biped->object.definition_index);
            s_model_definition *entry = TAG_GET('hlmt', s_model_definition, definition->unit.model.index);
            if (entry->physics_model_index != -1 && !(bool)((holder->flags >> 2) & 1)) {
                result = true;
                if (g_biped_ragdoll_skip_rebuild == 0) {
                    if ((bool)((definition->flags >> 11) & 1))
                        break;
                    if (game_can_create_additonal_ragdoll((e_ragdoll_source)!allow)) {
                        e_unit_animation_key_frame_status key_primary = unit_animation_get_key_frame_status(object_index, true);
                        e_unit_animation_key_frame_status key_secondary = unit_animation_get_key_frame_status(object_index, false);
                        bool either_after =
                            (key_primary == _unit_animation_key_frame_status_after || key_secondary == _unit_animation_key_frame_status_after)
                            ? true
                            : false;
                        bool both_none =
                            (key_primary == _unit_animation_key_frame_status_none && key_secondary == _unit_animation_key_frame_status_none)
                            ? true
                            : false;
                        float elapsed = (float)(game_time_globals->time - holder->time) * game_time_globals->tick_length;
                        bool relaxation_still_locked = ((biped->biped.flags >> 11) & 1);
                        if (allow == 0 && !relaxation_still_locked && (either_after || (both_none && elapsed < 0.3f))) {
                            if (!biped_dead_in_airborne_state(object_index) || elapsed <= 0.07f)
                                break;
                        }
                        float reconnect_elapsed = game_ticks_to_seconds((float)biped->object.physics_reconnect_time);
                        bool reconnect_expired = (reconnect_elapsed > 0.15f);
                        if (reconnect_expired)
                            break;
                        if (biped_start_relaxation(object_index, (holder->flags >> 1) & 1)) {
                            if (biped->object.havok_component_index != -1) {
                                c_havok_component *component = havok_component_get(biped->object.havok_component_index);
                                if ((bool)((component->m_flags >> 8) & 1)) {
                                    c_character_physics_mode_dead_datum *block = biped->biped.physics.get_mode_dead();
                                    long node_time = block->m_death_time;
                                    bool use_direction = false;
                                    long node_index;
                                    real_vector3d direction;
                                    if (node_time != -1) {
                                        long time_now = game_time_get();
                                        if (time_now - node_time <= biped_dead_seconds_to_ticks_round(0.1f)) {
                                            node_index = block->m_death_node_index;
                                            direction = block->m_death_node_acceleration;
                                            use_direction = true;
                                        }
                                    }
                                    if (use_direction) {
                                        long rigid_body = havok_object_compute_rigid_body_index_of_node(object_index, node_index);
                                        if (allow && rigid_body != -1) {
                                            switch (biped->unit.death_state) {
                                            case 1:
                                                biped_initialize_head_shot_death(object_index, rigid_body, &direction);
                                                break;
                                            case 2:
                                            case 4:
                                                biped_initialize_melee_death(object_index, rigid_body, &direction);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            holder->flags &= ~1ul;
                            break;
                        }
                        else {
                            long time = game_time_get();
                            holder->state = 0;
                            holder->time = time;
                        }
                    }
                    else {
                        holder->flags |= 2;
                        break;
                    }
                }
                holder->flags |= 1;
            }
        } while (false);
        biped->biped.flags &= 0xf7ff;
        break;
    }
    case 1: {
        if (g_biped_ragdoll_force_physics == 0 && (bool)((biped->object.physics_flags >> 7) & 1) && (bool)((biped->object.physics_flags >> 9) & 1)) {
            biped_relaxation_finish(object_index);
            return true;
        }
        if ((bool)((holder->flags >> 4) & 1) && !biped_dead_test_recent_death(object_index)) {
            havok_object_rebuild(object_index, 0);
            holder->flags &= ~16ul;
        }
        return true;
    }
    case 2: {
        if (!(bool)((biped->object.physics_flags >> 9) & 1) && !(bool)((biped->object.physics_flags >> 4) & 1))
            biped_exit_relaxation(object_index, true);
        break;
    }
    default:
        // match: 0x387a90: assume omits cmplwi r11,3 + bge default range check
        __assume(0);
    }
    return result;
}

// @address 0x3865b0 ?biped_dead_handle_simulation_warp@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_dead_handle_simulation_warp(long biped_index, const c_motor_request *request)
{
    (void)request;
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    if (biped->object.parent_object_index == -1) {
        s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
        bool accepted = false;
        signed char state = (signed char)holder->state;
        if (state == 1) {
            accepted = true;
        } else if (state == 2 && game_can_create_additonal_ragdoll(_ragdoll_source_death) && biped_start_relaxation(biped_index, false)) {
            accepted = true;
        }
        if (accepted && biped->object.havok_component_index != -1) {
            c_havok_component *component = havok_component_get(biped->object.havok_component_index);
            if (component->is_physics_model_havok_component() && component->m_rigid_bodies.m_size != 0) {
                real_point3d center_of_mass;
                real_point3d fixed_position;
                component
                    ->get_rigid_body_center_of_mass(biped_dead_controlling_rigid_body_index(component), &center_of_mass);
                if (havok_try_and_get_sphere_fixed_position(g_havok_constants.minimum_radius, k_biped_relaxation_key_scale,
                        (biped_dead_test_recent_death(biped_index)
                            ? _havok_group_recently_dead_biped : _havok_group_ragdoll_biped),
                        10, biped_index, &center_of_mass, 0, -1,
                        &fixed_position, false))
                {
                    float dy = fixed_position.y - center_of_mass.y;
                    float dz = fixed_position.z - center_of_mass.z;
                    float dx = fixed_position.x - center_of_mass.x;
                    float s = dx * dx;
                    // match: 0x3865b0: value range assumption guides branch lowering
                    __assume(s >= 0.0f);
                    s += dz * dz;
                    s += dy * dy;
                    if (s > 1.0000001e-06f) {
                        for (long body = 0; body < component->m_rigid_bodies.m_size; body++) {
                            real_matrix4x3 matrix;
                            component->get_rigid_body_transform(body, &matrix);
                            matrix.position.x += dx;
                            matrix.position.y += dy;
                            matrix.position.z += dz;
                            phys_transform transform;
                            phys_transform_from_matrix4x3(&transform, &matrix);
                            component->set_rigid_body_transform_internal(body, &transform);
                        }
                        biped_bring_ragdoll_inside_world(biped_index, false);
                    }
                }
            }
        }
    }
    return _motor_result_pending;
}

// the physics flag bit kept in shift-extract form through the helper boundary
static __forceinline bool biped_dead_physics_bit4_set(unsigned long physics_flags)
{
    return ((physics_flags >> 4) & 1) != 0;
}

// the frozen-orientations bit, read through a fresh lookup so the test keeps
// its own datum reload and shift-extract form
static __forceinline bool biped_dead_node_orientations_frozen(long object_index)
{
    biped_datum *frozen_biped = OBJECT_GET(object_index, biped_datum, 1);
    return ((frozen_biped->object.flags >> 29) & 1) != 0;
}

// 0x3870a0: exiting relaxation; the holder state is cleared, the object is rebuilt,
// and the biped is optionally repositioned with its velocity flattened
// @rdata 0x82028ef4
static const float biped_dead_exit_velocity_threshold = 1.4f;
// @address 0x3870a0 ?biped_exit_relaxation@@YAXJ_N@Z
void biped_exit_relaxation(long biped_index, bool reposition)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    e_biped_relaxation_state relaxation_state = holder->get_relaxation_state();
    if (relaxation_state == 1)
        biped_relaxation_calculate_object_relative_safe_position(biped_index);
    if (biped_dead_physics_bit4_set(biped->object.physics_flags)) {
        real_point3d center_of_mass;
        real_vector3d mover_offset;
        real_vector3d mover_axis;
        object_get_center_of_mass(biped_index, &center_of_mass);
        bool from_early_mover = object_get_early_mover_local_space_velocity(biped_index, &center_of_mass, &mover_offset, &mover_axis, false, true);
        object_detach(biped_index);
        if (from_early_mover)
            object_set_velocities(biped_index, &mover_offset, &mover_axis);
    }
    if (biped->object.parent_object_index != -1)
        object_detach(biped_index);
    if (biped_dead_node_orientations_frozen(biped_index)) {
        object_freeze_node_orientations(biped_index, false);
        object_start_interpolation(biped_index, k_standard_interpolation_time);
        motor_animation_set_mode_weapons_and_state(biped_index, _string_id_current, _string_id_current, _string_id_current,
            _string_id_airborne_dead, 0.0f, 0x210, 31, false);
    }
    holder->set_relaxation_state(_biped_relaxation_state_none);
    unsigned long *holder_flags_a = &holder->flags;
    *holder_flags_a &= ~1ul;
    holder->state_flags.set(1, false);
    holder->flags &= ~4ul;
    object_set_at_rest(biped_index, false);
    havok_object_rebuild(biped_index, 0);
    if (relaxation_state == 1)
        biped_dead_set_ragdoll_flag((s_dead_flag_holder *)holder, false);
    else if (relaxation_state == 0)
        return;
    if (reposition) {
        real_matrix4x3 world_matrix;
        real_vector3d linear_velocity;
        real_point3d transformed_point;
        object_get_world_matrix(biped_index, &world_matrix);
        matrix4x3_transform_point(&world_matrix, &biped->biped.physics.get_mode_dead()->m_object_relative_safe_position, &transformed_point);
        biped_fix_position(biped_index, -1, &transformed_point, 0, 0, 0.0f, 2.0f, false, true);
        object_get_velocities(biped_index, &linear_velocity, 0);
        float x = linear_velocity.i;
        float y = linear_velocity.j;
        float z = linear_velocity.k;
        if (!(z * z + (y * y + x * x) <= biped_dead_exit_velocity_threshold)) {
            real_vector3d fallback_direction = linear_velocity;
            fallback_direction.k = 0.0f;
            if (normalize3d(&fallback_direction) != 0.0f)
                object_set_position_direct(biped_index, 0, &fallback_direction, &k_real_math_identity_matrix_storage.up, 0, false);
        }
    }
}

static __forceinline c_animation_manager *biped_dead_animation_manager_get(long object_index)
{
    biped_datum *biped = OBJECT_GET(object_index, biped_datum, 1);
    return (c_animation_manager *)object_header_block_get(biped, &biped->object.animation);
}
static __forceinline bool biped_dead_physics_bit9_set(unsigned long physics_flags)
{
    return ((physics_flags >> 9) & 1) != 0;
}

static __forceinline bool biped_dead_animation_nodes_frozen(long object_index)
{
    return ((OBJECT_GET(object_index, biped_datum, 1)->object.flags >> 29) & 1) != 0;
}
static __forceinline bool biped_dead_animation_delete_finished(unsigned long flags)
{
    return ((flags >> 1) & 1) != 0;
}
static __forceinline bool biped_dead_animation_is_damaged(unsigned long flags)
{
    return ((flags >> 2) & 1) != 0;
}
static __forceinline bool biped_dead_animation_can_resurrect(unsigned long flags)
{
    return ((flags >> 10) & 1) != 0;
}
static __forceinline bool biped_dead_animation_resurrection_pending(const biped_datum *biped)
{
    return biped->biped.stun_ticks > 0;
}
static __forceinline bool biped_dead_animation_resurrection_requested(long object_index)
{
    bool requested = false;
    biped_datum *biped = OBJECT_GET(object_index, biped_datum, 1);
    if (biped->object.identifier_type == 0)
        requested = biped_dead_animation_resurrection_pending(biped);
    return requested;
}
static __forceinline void *biped_dead_animation_block_get(long object_index, const biped_dead_animation_block_reference *reference)
{
    return object_header_block_get(object_get(object_index), reference);
}

// @address 0x388960 ?biped_dead_update_animation@@YA_NJPAUs_animation_control_data@@PAUs_character_physics_update_input_datum@@@Z
bool biped_dead_update_animation(long object_index, s_animation_control_data *animation, s_character_physics_update_input_datum *input)
{
    biped_datum *biped = OBJECT_GET(object_index, biped_datum, 1);
    biped_definition *definition =
        TAG_GET('bipd', biped_definition, biped->object.definition_index);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)biped_dead_animation_block_get(object_index, &biped->unit.motor_program);
    c_animation_manager *manager = (c_animation_manager *)biped_dead_animation_block_get(object_index, &biped->object.animation);
    s_unit_animation *mover = (s_unit_animation *)biped_dead_animation_block_get(object_index, &biped->unit.animation_storage);
    bool result = false;
    animation->mode = 226;
    animation->state = 80;
    animation->moved = 0;
    animation->crouching = 0;
    c_animation_manager *manager_state = (c_animation_manager *)manager;
    if (biped_dead_in_airborne_state(object_index) && !biped_dead_physics_bit9_set(biped->object.physics_flags)
        && biped_dead_animation_nodes_frozen(object_index))
    {
        object_freeze_node_orientations(object_index, false);
        object_start_interpolation(object_index, k_standard_interpolation_time);
        animation->mode = 226;
        animation->state = 79;
        animation->flags = 0x210;
        result = true;
    }
    else if (manager->valid() && manager->state_channel_finished() && !biped_dead_animation_nodes_frozen(object_index)
        && manager_state->m_current_goal.state != 79)
    {
        if (biped_dead_animation_delete_finished(definition->unit.flags) && !simulation_query_object_is_predicted(object_index))
        {
            object_destroy_internal(object_index, -1, -1, 0, 0);
        }
        else
        {
            mover->flags.set(_unit_animation_disabled_bit, true);
            manager_state->m_flags |= 1;
            object_freeze_node_orientations(object_index, true);
        }
    }
    else if (holder->get_relaxation_state() != 0)
    {
        animation->state = manager->state_channel_finished() ? 833 : 80;
        animation->mode = 226;
        animation->moved = 0;
        animation->crouching = 0;
        if (holder->get_relaxation_state() == 1)
            result = true;
    }
    else
    {
        if (biped_dead_in_airborne_state(object_index) && biped_dead_animation_manager_get(object_index)->find_goal(226, 226, 226, 79, 0))
        {
            if ((manager_state->m_flags & 1) != 0)
                manager_state->m_flags &= ~1u;
            if (action_executing(object_index, _action_death_ping))
            {
                c_motor_task *task = motor_task_find_by_action(object_index, _action_death_ping);
                if (task)
                    motor_system_interrupted(object_index, task, true);
            }
            result = true;
            animation->state = 79;
        }
        else
        {
            if (manager_state->m_current_goal.state == 79)
            {
                c_character_physics_component *mode_holder = &biped->biped.physics;
                real_vector3d *up = &biped->object.up;
                real_vector3d *forward = &biped->object.forward;
                input->flying_friction = 0.0f;
                switch (mode_holder->m_mode - 1)
                {
                case 0:
                    ((c_character_physics_mode_ground_datum *)mode_holder->m_storage)->snap_facing(forward, up, input);
                    break;
                case 1:
                    ((c_character_physics_mode_flying_datum *)mode_holder->m_storage)->snap_facing(forward, up, input);
                    break;
                case 2:
                    ((c_character_physics_mode_dead_datum *)mode_holder->m_storage)->snap_facing(forward, up, input);
                    break;
                case 3:
                case 4:
                case 5:
                    break;
                default:
                    // match: 0x388960: unreachable hint omits dead branch check
                    __assume(0);
                    break;
                }
            }
            animation->state = 80;
        }
        if (!biped_dead_animation_is_damaged(biped->object.damage_flags) && biped_dead_animation_can_resurrect(definition->flags)
            && biped_dead_animation_resurrection_requested(object_index))
        {
            animation->moved = 1;
            animation->crouching = 1;
            animation->mode = 141;
            animation->state = 12;
            result = true;
        }
        else
        {
            animation->crouching = 0;
        }
    }
    return result;
}

static __forceinline float biped_dead_velocity_magnitude_squared(const real_vector3d &velocity)
{
    return velocity.i * velocity.i + velocity.k * velocity.k + velocity.j * velocity.j;
}

static __forceinline void biped_dead_apply_relaxation_velocity(long object_index, const real_vector3d *velocity, float speed_squared)
{
    object_set_velocities_internal(object_index, velocity, 0, 0);
    havok_object_set_velocities(object_index, velocity, 0, true);
    if (!(speed_squared <= 9.999999e-09f))
    {
        object_set_at_rest(object_index, false);
        object_wake(object_index);
        object_set_requires_motion(object_index);
    }
}

// Start relaxation, select the faster velocity, and bring the ragdoll inside the world.
// @address 0x387480 ?biped_start_relaxation@@YA_NJ_N@Z
bool biped_start_relaxation(long object_index, bool lift)
{
    bool relaxed = false;
    biped_datum *biped = OBJECT_GET(object_index, biped_datum, 1);
    real_vector3d body_velocity;
    // match: 0x387480: initialize the first component before assigning the zero vector.
    body_velocity.i = 0.0f;
    body_velocity = *global_zero_vector3d;
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    bool was_disconnected = ((biped->object.physics_flags >> 7) & 1) != 0;
    biped_definition *definition =
        TAG_GET('bipd', biped_definition, biped->object.definition_index);
    float radius = ((const s_character_physics_dead_sphere *)definition->physics.dead_spheres.address)
        [biped->biped.physics.get_mode_dead()->get_dead_size()].shape.get_convex_radius();
    holder->state_flags.set(4, true);
    holder->last_airborne_time = game_time_globals->time;
    real_point3d center_of_mass;
    object_get_center_of_mass(object_index, &center_of_mass);
    long saved_state = (signed char)holder->state;
    long time = game_time_globals->time;
    holder->state = 1;
    holder->time = time;
    object_set_at_rest(object_index, false);
    if (g_biped_ragdoll_force_physics != 0)
        biped->object.physics_flags |= 0x200;
    if (was_disconnected)
        object_disconnect_from_physics(object_index);
    if (biped->object.havok_component_index != -1) {
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        if (biped_dead_object_position_controlling_rigid_body_index(component) != -1)
            body_velocity = component->m_rigid_bodies[biped_dead_object_position_controlling_rigid_body_index(component)]
                .m_pre_simulation_linear_velocity;
    }
    if (lift) {
        biped_datum *lift_biped = OBJECT_GET(object_index, biped_datum, 1);
        long node_count = (unsigned long)lift_biped->object.node_matrices.size / sizeof(real_matrix4x3);
        real_matrix4x3 *node_matrices = (real_matrix4x3 *)object_header_block_get(lift_biped, &lift_biped->object.node_matrices);
        biped->object.position.z += 0.125f;
        for (long node_index = 0; node_index < node_count; node_index++)
            node_matrices[node_index].position.z += 0.125f;
    }
    havok_object_rebuild(object_index, 0);
    real_vector3d object_velocity = biped->object.linear_velocity;
    float body_speed_squared = biped_dead_velocity_magnitude_squared(body_velocity);
    float object_speed_squared = biped_dead_velocity_magnitude_squared(object_velocity);
    if (!(object_speed_squared <= body_speed_squared))
        biped_dead_apply_relaxation_velocity(object_index, &object_velocity, object_speed_squared);
    else
        biped_dead_apply_relaxation_velocity(object_index, &body_velocity, body_speed_squared);
    if (biped->object.havok_component_index != -1) {
        c_havok_component *component = havok_component_get(biped->object.havok_component_index);
        if (component->is_physics_model_havok_component() && biped_dead_object_position_controlling_rigid_body_index(component) != -1) {
            real_point3d target;
            if (saved_state == 0)
                target = center_of_mass;
            else
                component
                    ->get_rigid_body_center_of_mass(
                        biped_dead_object_position_controlling_rigid_body_index(component), &target);
            component->align_constraint_positions();
            biped_bring_ragdoll_inside_world(object_index, false, &target, radius);
            relaxed = true;
        }
    }
    if (was_disconnected)
        object_reconnect_to_physics(object_index);
    if (relaxed) {
        if ((unsigned char)biped->biped.physics.get_mode() == 3) {
            biped->biped.physics.get_mode_dead()->m_ragdoll_start_time = lift ? game_time_globals->time : -1;
            biped_update_constraint_limits(object_index);
        }
        object_freeze_node_orientations(object_index, true);
        biped_dead_set_ragdoll_flag((s_dead_flag_holder *)holder, true);
        biped_relaxation_pose_set(object_index, biped_relaxation_calculate_best_relaxation_pose(object_index));
    } else {
        holder->set_relaxation_state(_biped_relaxation_state_none);
    }
    return relaxed;
}

// @address 0x2251c biped_dead_update_pool
// @address 0x388e48 ?biped_dead_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_dead_update(long biped_index, s_animation_control_data *animation_control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, 1);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)object_header_block_get(biped, &biped->unit.motor_program);
    bool result = false;
    s_character_physics_update_output_datum output;
    if (!biped_dead_physics_bit4_set(biped->object.physics_flags) && holder->state == 1)
        object_in_early_mover_physics_update(biped_index);
    if (biped->object.havok_component_index != NONE) {
        long ancestor_index = biped->object.parent_object_index;
        while (ancestor_index != NONE) {
            biped_datum *ancestor = OBJECT_GET(ancestor_index, biped_datum, 1);
            if (!(bool)((ancestor->object.flags >> 26) & 1))
                break;
            ancestor_index = ancestor->object.parent_object_index;
        }
        if (ancestor_index == NONE || holder->state == 0) {
            biped_relaxation_pose_update(biped_index);
            if (game_globals->options.game_mode == 2)
                biped_update_out_of_map_status(biped_index);
            if ((biped->biped.physics.m_flags & 2) != 0) {
                signed char count = holder->airborne_ticks;
                if (count < 127)
                    holder->airborne_ticks = (unsigned char)(count + 1);
                short ticks = game_time_globals->ticks_per_second;
                if (holder->airborne_ticks >= (long)((float)(double)ticks * 0.18f + 0.5f))
                    holder->ground_ticks = 0;
                result = true;
            } else {
                float limit;
                if (holder->state == 1)
                    limit = 0.3f;
                else
                    limit = 0.066f;
                signed char count = holder->ground_ticks;
                if (count < 127)
                    holder->ground_ticks = (unsigned char)(count + 1);
                short ticks = game_time_globals->ticks_per_second;
                if (holder->ground_ticks >= (long)((float)(double)ticks * limit + 0.5f))
                    holder->airborne_ticks = 0;
            }
            if (biped->object.havok_component_index != NONE) {
                long saved_state = (signed char)holder->state;
                s_character_physics_update_input_datum input;
                bool not_first = saved_state != 0;
                bool (*const update)(long, s_character_physics_update_input_datum *,
                    s_character_physics_update_output_datum *, bool, bool, float, bool, bool) = motor_system_biped_physics_preupdate_moving;
                bool moved = update(biped_index, &input, &output, false, biped_dead_in_airborne_state(biped_index),
                    0.0f, not_first, false);
                biped_dead_update_animation(biped_index, &control, &input);
                if (moved) {
                    long child_index = biped->object.parent_object_index;
                    while (child_index != NONE) {
                        biped_datum *child = OBJECT_GET(child_index, biped_datum, 1);
                        if (!(bool)((child->object.flags >> 26) & 1))
                            break;
                        child_index = child->object.parent_object_index;
                    }
                    if (child_index == NONE) {
                        motor_system_biped_physics_postupdate_moving(biped_index, &input, &output, &control.crouching);
                        result = true;
                    }
                }
                result = biped_update_relaxation(biped_index) | result;
            }
        }
    }
    {
        s_state_changes_requested requested;
        s_animation_state_change_settings *settings = new (&output) s_animation_state_change_settings;
        settings->flags.clear();
        requested.goals.clear();
        requested.transition.clear();
        requested.primary.clear();
        motor_animation_build_state_change_settings_from_control_data(biped_index, &control, &requested, settings);
        if (settings->goals.state == _string_id_landing_dead)
            settings->primary.m_playback_flags.set(_animation_playback_looping_bit, false);
        c_animation_manager *channel = (c_animation_manager *)object_header_block_get(biped, &biped->object.animation);
        bool valid = channel->m_state_channel.m_enabled != 0 &&
            channel->m_state_channel.m_graph_index != NONE &&
            (unsigned short)channel->m_state_channel.m_animation_id.m_animation_index < 0x8000;
        if (!valid || settings->goals.state != _string_id_landing_dead || (bool)(((unsigned long)requested.goals >> 3) & 1))
            result = (0 != motor_task_execute_state_change_settings(biped_index, _motor_task_default,
                _action_none, settings, 1, control.moved)) | result;
        return result;
    }
}

// @address 0x386eb8 ?biped_dead_resurrect_update@@YA_NJPAVc_motor_task@@@Z
bool biped_dead_resurrect_update(long biped_index, c_motor_task *)
{
    bool result = true;
    long mode;
    long state;
    if (motor_animation_get_mode_and_state(biped_index, &mode, &state) && state != _string_id_resurrect_front
        && state != _string_id_resurrect_back)
    {
        if (ai_handle_resurrection(biped_index))
            result = true;
        else
            result = !motor_animation_set_mode_weapons_and_state(biped_index, _string_id_current, _string_id_current,
                _string_id_current, _string_id_resurrect_front, 0.1f, 0, 29, false);
    }
    return result;
}

static __forceinline void biped_dead_resurrect_object(long object_index)
{
    object_datum *object = object_get(object_index);
    object->damage_flags &= ~4;
    object_update_collision_culling(object_index);
    object_set_garbage(object_index, false, -1);
    object_damage_handle_resurrection(object_index);
}

static __forceinline void biped_dead_resurrect_matrix_from_point_and_vectors(real_matrix4x3 *matrix,
    const real_point3d *point, const real_vector3d *forward, const real_vector3d *up)
{
    matrix4x3_rotation_from_vectors(matrix, forward, up);
    matrix->position = *point;
}

static __forceinline bool biped_dead_resurrect_is_unarmed(long biped_index)
{
    unit_datum *unit = OBJECT_GET(biped_index, unit_datum, 1);
    bool unarmed = true;
    for (long weapon_index = 0; weapon_index < 2; weapon_index++)
    {
        if (unit->unit.current_weapon_indices[weapon_index] != -1)
            unarmed = false;
    }
    return unarmed;
}

// @address 0x3869c8
e_motor_result biped_dead_resurrect_submit(long biped_index, const c_motor_request *request)
{
    unit_datum *biped = OBJECT_GET(biped_index, unit_datum, 1);
    s_biped_dead_holder *holder = (s_biped_dead_holder *)((byte *)biped + biped->unit.motor_program_offset);
    e_motor_result result = _motor_result_failed;
    if (!biped_dead_in_airborne_state(biped_index) && !(bool)((biped->object.object_flags >> 7) & 1))
    {
        result = _motor_result_pending;
        c_flags_no_init<e_unit_animation_flags, unsigned short, 9> *animation_flags =
            (c_flags_no_init<e_unit_animation_flags, unsigned short, 9> *)((byte *)biped + biped->unit.animation_storage.offset);
        bool was_relaxing = holder->get_relaxation_state() != 0;
        biped_dead_resurrect_object(biped_index);
        animation_flags->set(_unit_animation_disabled_bit, false);
        biped_exit_relaxation(biped_index, false);
        biped_enter_default_physics(biped_index);
        if (!(bool)((biped->object.physics_flags >> 7) & 1))
            object_reconnect_to_physics(biped_index);
        if (was_relaxing)
        {
            real_point3d origin;
            real_point3d fixed_position;
            object_get_origin(biped_index, &origin);
            biped_fix_position(biped_index, -1, 0, &origin, &fixed_position, 0.0f, 0.2f, false, false);
        }

        bool animate = false;
        bool resurrect_ai = false;
        bool marker_valid = false;
        long state = -1;
        long marker_name = -1;
        switch (request->resurrect.first)
        {
        case 4:
            marker_name = _string_id_infection_back;
            state = _string_id_thrash_back;
            break;
        case 3:
            marker_name = _string_id_infection_front;
            state = _string_id_thrash_front;
            break;
        case 2:
            state = _string_id_resurrect_back;
            resurrect_ai = true;
            marker_name = _string_id_body;
            break;
        case 1:
            state = _string_id_resurrect_front;
            resurrect_ai = true;
            marker_name = _string_id_body;
            break;
        }

        real_matrix4x3 original_matrix;
        object_marker marker;
        object_get_world_matrix(biped_index, &original_matrix);
        if (state != -1)
        {
            if (biped_dead_resurrect_is_unarmed(biped_index))
                unit_animation_set_weapon(biped_index, _string_id_unarmed, 0, true);
            c_motor_task *task = motor_task_set_mode_and_state(biped_index, _motor_task_resurrect,
                request->action, _string_id_combat, state, k_biped_relaxation_key_scale, 15, 0, 31,
                request->force());
            if (task)
            {
                marker_valid = internal_object_get_markers_by_string_id(biped_index, marker_name, &marker, 1, false) > 0;
                animate = true;
            }
        }
        ((c_motor_program_state *)holder)->set_program(biped_index, _motor_program_ground);
        if (resurrect_ai || !animate)
            ai_handle_resurrection(biped_index);
        if (animate)
        {
            result = _motor_result_success;
            if (marker_valid && biped->object.parent_object_index == -1)
            {
                real_vector3d up;
                real_vector3d forward;
                real_vector3d left;
                scale_vector3d(&marker.matrix.up, -1.0f, &forward);
                up = *global_up3d;
                cross_product3d(&up, &forward, &left);
                if (!(normalize3d(&left) > 0.0f))
                {
                    left = *global_left3d;
                }
                cross_product3d(&left, &up, &forward);
                object_set_position_direct(biped_index, 0, &forward, &up, 0, false);
                object_compute_node_matrices(biped_index);
                real_orientation *original_orientations =
                    (real_orientation *)object_header_block_get(biped, &biped->object.original_node_orientations);
                real_matrix4x3 new_matrix;
                real_quaternion rotation;
                real_point3d position;
                real_quaternion new_rotation;
                real_quaternion original_rotation;
                biped_dead_resurrect_matrix_from_point_and_vectors(&new_matrix, &original_matrix.position, &forward, &up);
                matrix4x3_transform_point(&original_matrix, &original_orientations[0].translation, &position);
                matrix4x3_inverse_transform_point(&new_matrix, &position, &position);
                matrix3x3_rotation_to_quaternion(&original_matrix.rotation, &original_rotation);
                matrix3x3_rotation_to_quaternion(&new_matrix.rotation, &new_rotation);
                new_rotation.w *= -1.0f;
                quaternions_multiply(&original_rotation, &original_orientations[0].rotation, &rotation);
                quaternions_multiply(&new_rotation, &rotation, &rotation);
                original_orientations[0].translation = position;
                original_orientations[0].rotation = rotation;
            }
        }
    }
    return result;
}
