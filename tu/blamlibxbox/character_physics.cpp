// blamlibxbox/character_physics.obj: from the records 0xabe40 0xe1640 0xe24a0 0xe29b8 0xe6e60 0x10b600 0x10b880 0x2b1610
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "physics/character_physics_globals.h"
// @address 0x11df6bc game_time_globals
// @address 0x11df6a8 object_header_data
// @address 0x3aa90 k_real_math_zero_vector3d_storage
// @address 0x2490c k_real_math_epsilon_storage
#include "math/real_math_constants.h"
#include "physics/character_physics_mode_melee.h"
#include "math/matrix_math_types.h"
#include "game/game_globals.h"
#include "game/player_information_definition.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_types.h"
#include "objects/object_reference_types.h"
#include "physics/character_physics_output.h"
#include "physics/character_physics_input.h"
#include "phys/phys_collide.h"
#include <ppcintrinsics.h>
#include <stdlib.h>
#include <string.h>
#include "objects/objects.h"
#include "physics/character_physics.h"
#include "physics/havok.h"
#include "objects/object_datum.h"
#include "objects/object_early_movers.h"
#include "animations/animation_manager.h"
#include "physics/havok_contact_points.h"
#include "scenario/scenario.h"
#include "physics/havok_component.h"
#include <math.h>
#include "physics/character_physics_component.h"
#include "math/matrix_math_constants.h"


#include "physics/character_physics_move_input.h"
#include "physics/havok_component_class.h"
#include "physics/havok_component_datum_access.h"
#include "objects/object_header_access.h"
#include "game/game_time_inline.h"
#include "physics/character_physics_move_output.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/character_physics_mode_flying.h"
#include "physics/character_physics_mode_sentinel.h"
#include "math/real_math_types.h"

#include "physics/character_physics_definition_types.h"
#include "math/real_math_functions.h"
#include "math/real_math.h"
#include "memory/datum_index.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "physics/physics_model_definition_types.h"
#include "physics/havok_constants_types.h"


// match: 0xe29b8: returning the threshold through a one-field struct keeps the comparison operand in its own register.
struct s_melee_magnitude_threshold { float value; };



static __forceinline real_vector3d *character_physics_scale_vector(const real_vector3d *const &vector, float scale, real_vector3d *result) {
    result->i = scale * vector->i;
    result->j = scale * vector->j;
    result->k = scale * vector->k;
    return result;
}
static __forceinline float character_physics_dot(const real_vector3d *a, const real_vector3d *const &b) {
    return a->i * b->i + a->j * b->j + a->k * b->k;
}
static __forceinline real_vector3d *character_physics_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = a->j * b->k - a->k * b->j;
    result->j = j;
    result->k = k;
    return result;
}


static __forceinline short character_physics_controlling_body(const c_havok_component *component) {
    long index = component->m_object_postion_controlling_rigid_body_index;
    return (short)((index >= 0 && index < component->m_rigid_bodies.getSize()) ? index : -1);
}
static __forceinline long character_physics_finite_inertia(const c_havok_component *component, short index) {
    return _rotl(~(unsigned long)component->m_rigid_bodies[index].m_flags, 31) & 1;
}


static __forceinline bool character_physics_test_flags(unsigned long flags, long bit) { return (bool)((flags >> bit) & 1); }
static __forceinline long character_physics_object_local_physics_get(long index) {
    long result = -1;
    object_datum *object = OBJECT_GET(index, object_datum, NONE);
    if (character_physics_test_flags(object->physics_flags, 1) || character_physics_test_flags(object->physics_flags, 2))
        result = object->local_physics_object_index;
    return result;
}
static __forceinline long character_physics_object_accepted_physics_get(long index) {
    long result = -1;
    object_datum *object = OBJECT_GET(index, object_datum, NONE);
    if (character_physics_test_flags(object->physics_flags, 3) &&
        (character_physics_test_flags(object->physics_flags, 1) || character_physics_test_flags(object->physics_flags, 2)))
        result = object->local_physics_object_index;
    return result;
}
#define character_physics_flags_set(flags, bit, value) ((value) ? (*(flags) |= (1 << (bit))) : (*(flags) &= ~(1 << (bit))))
static __forceinline bool character_physics_early_mover_velocity(
    long index, real_vector3d *linear, real_vector3d *angular, bool linear_velocity, bool angular_velocity)
{
    real_point3d origin;
    ::object_get_center_of_mass(index, &origin);
    return ::object_get_early_mover_local_space_velocity(index, &origin,
        linear, angular, linear_velocity, angular_velocity);
}
static __forceinline long character_physics_phantom_count(c_havok_component *component) {
    long result;
    if (!component->m_bodies_in_phantom) result = 0; else result = component->m_bodies_in_phantom->getSize();
    return result;
}
static __forceinline void character_physics_phantom_info(c_havok_component *component, long index, long *object, long *body, long *material) {
    const c_havok_component::s_body_in_phantom *phantom = &(*component->m_bodies_in_phantom)[index];
    *object = phantom->object_index; *body = phantom->rigid_body_index; *material = phantom->material_index;
}


struct s_melee_clamp_data;
// @address 0x2251c k_real_zero_storage
static __forceinline const real_vector3d &character_physics_init_zero() {
    return *global_zero_vector3d;
}



__forceinline void c_character_physics_mode_melee_datum::initialize() {
    m_maximum = k_real_zero_storage;
    m_flags.set_unsafe(0);
    m_target_unit_index = -1;
    m_count = -1;
    m_initial_aiming_vector = character_physics_init_zero();
    m_initial_velocity = character_physics_init_zero();
    m_exit_invisible_crouched_uber_melee_offset = 0;
}

// @address 0x8c426b g_character_physics_flying_noise
bool g_character_physics_flying_noise;

static __forceinline float character_physics_magnitude_squared(const real_vector3d *vector) {
    return vector->i * vector->i + vector->j * vector->j + vector->k * vector->k;
}
static __forceinline float character_physics_magnitude(const real_vector3d *vector) {
    return (float)sqrt(character_physics_magnitude_squared(vector));
}
static __forceinline real_vector3d *character_physics_point_from_line(
    const real_vector3d *point, const real_vector3d *direction, const float &distance, real_vector3d *result)
{
    result->i = point->i + direction->i * distance;
    result->j = point->j + direction->j * distance;
    result->k = point->k + direction->k * distance;
    return result;
}
static __forceinline real_vector3d *character_physics_add_vectors(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    result->i = a->i + b->i;
    result->j = a->j + b->j;
    result->k = a->k + b->k;
    return result;
}
static __forceinline real_vector3d *character_physics_multiply_vectors(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    result->i = a->i * b->i;
    result->j = a->j * b->j;
    result->k = a->k * b->k;
    return result;
}


static __forceinline void character_physics_set_animation(s_character_physics_update_output_datum *output, int animation)
{
    output->animation_state = (s_character_physics_update_output_datum::e_animation_state)animation;
}

#if defined(_XBOX) && !defined(__clang__)
// match: 0xe29b8: 0.0f compares load at -0x23f0 off the shared k_real_math_epsilon_storage base; 0.0f adds a lis each
#define k_character_physics_real_zero (&k_real_math_epsilon_storage)[-2300]
#else
#define k_character_physics_real_zero 0.0f
#endif

static __forceinline void character_physics_animate_ground(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input)
{
    if (input->movement.i != k_character_physics_real_zero ||
        input->movement.j != k_character_physics_real_zero ||
        input->movement.k != k_character_physics_real_zero)
    {
        if (fabs(input->movement.i) < fabs(input->movement.j))
        {
            if (input->movement.j < k_character_physics_real_zero)
                character_physics_set_animation(output, 2);
            else
                character_physics_set_animation(output, 3);
        }
        else if (input->movement.i < k_character_physics_real_zero)
            character_physics_set_animation(output, 5);
        else
            character_physics_set_animation(output, 4);
    }
}

static __forceinline void character_physics_animate_flying(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input)
{
    character_physics_set_animation(output, 1);
    if (fabs(input->movement.i) < fabs(input->movement.j))
    {
        if (input->movement.j < rasterizer_states_mode_7_depth_bias)
            character_physics_set_animation(output, 2);
        else if (input->movement.j > k_real_math_epsilon_storage)
            character_physics_set_animation(output, 3);
    }
    else if (input->movement.i < rasterizer_states_mode_7_depth_bias)
        character_physics_set_animation(output, 5);
    else if (input->movement.i > k_real_math_epsilon_storage)
        character_physics_set_animation(output, 4);
}


static __forceinline float character_physics_tenth_float(void)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0xe29b8: 0.1f loads at -0x1b50 off the shared k_real_math_epsilon_storage base; 0.1f adds a lis
    return (&k_real_math_epsilon_storage)[-1748];
#else
    return 0.1f;
#endif
}

static __forceinline s_melee_magnitude_threshold melee_magnitude_threshold_make(float value)
{
    s_melee_magnitude_threshold result = {value};
    return result;
}
static __forceinline void character_physics_melee_animation(
    s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input)
{
    if (input->melee_movement == 0)
        return;
    
    if (input->movement_magnitude >= melee_magnitude_threshold_make(character_physics_tenth_float()).value)
        character_physics_set_animation(output, 4);
}



static const float k_character_physics_ground_max = 3.402823466e+38f;
static const float k_character_physics_ground_one = 1.0f;
static const float k_character_physics_ground_zero = 0.0f;
static __forceinline float character_physics_ground_one() { return k_character_physics_ground_one; }
static __forceinline float character_physics_snap_movement(float value) {
    long sign;
    if (value != k_character_physics_ground_zero) {
        if (!(value >= k_character_physics_ground_zero)) sign = -1;
        else sign = 1;
    } else sign = 0;
    return (float)__fsel((float)fabs(value) - 0.92f, (double)sign, value);
}


// @address 0xe29b8 ?update_animation@c_character_physics_component@@AAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_component::update_animation(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input)
{
    if ((input->flags & 256) != 0)
    {
        output->animation_state_valid = true;
        character_physics_set_animation(output, 1);
        return;
    }
    switch (m_mode)
    {
    case _mode_ground:
        character_physics_animate_ground(output, input);
        return;
    case _mode_flying:
        character_physics_animate_flying(output, input);
        return;
    case _mode_dead:
        return;
    case _mode_melee:
        character_physics_melee_animation(output, input);
        return;
    case _mode_sentinel:
    case _mode_sentinel_b:
        if (!get_mode_sentinel()->is_climbing())
            return;
        {
            float vertical = input->movement.k;
            if (!(vertical >= k_character_physics_real_zero))
                character_physics_set_animation(output, 11);
            else if (!(vertical <= k_character_physics_real_zero))
                character_physics_set_animation(output, 10);
            else
                character_physics_set_animation(output, 1);
        }
        return;
    default:
        // match: 0xe29b8: unreachable hint omits dead branch check
        __assume(0);
    }
}

// @address 0x10b600 ?character_physics_move_input_datum_initialize@@YAXPAUs_character_physics_move_input_datum@@PAVc_character_physics_component@@PBUs_character_physics_definition@@JJ_N3333333PBTreal_point3d@@PBTreal_vector3d@@55@Z
void character_physics_move_input_datum_initialize(s_character_physics_move_input_datum *input, c_character_physics_component *component,
    const s_character_physics_definition *definition, long object_index, long rigid_body_index,
    bool at_rest, bool airborne, bool interpolating, bool recently_jumped, bool ignored, bool force_ground,
    bool ignore_climbable_surfaces, bool unused,
    const real_point3d *position, const real_vector3d *velocity, const real_vector3d *forward, const real_vector3d *up)
{
    input->accepted_physics = character_physics_object_accepted_physics_get(object_index);
    input->local_physics = character_physics_object_local_physics_get(object_index);
    input->mode = component->m_mode;
    input->sub_initialized = false;
    input->definition = definition;
    input->object_index = object_index;
    input->havok_component_index = rigid_body_index;
    input->flags = 0;
    character_physics_flags_set(&input->flags, 0, at_rest);
    character_physics_flags_set(&input->flags, 1, airborne);
    character_physics_flags_set(&input->flags, 2, interpolating);
    character_physics_flags_set(&input->flags, 3, recently_jumped);
    character_physics_flags_set(&input->flags, 4, false);
    character_physics_flags_set(&input->flags, 5, force_ground);
    character_physics_flags_set(&input->flags, 6, force_ground);
    character_physics_flags_set(&input->flags, 8, ignore_climbable_surfaces);
    real_vector3d angular;
    if (character_physics_early_mover_velocity(object_index, &input->early_velocity, &angular, true, true)) character_physics_flags_set(&input->flags, 7, true);
    else input->early_velocity = *global_zero_vector3d;
    input->position = *position;
    input->velocity = *velocity;
    input->forward = *forward;
    input->up = *up;
}

// @address 0x10b880 ?move@c_character_physics_component@@QAAXPAUs_character_physics_move_output_datum@@PBUs_character_physics_move_input_datum@@@Z
void c_character_physics_component::move(s_character_physics_move_output_datum *output, const s_character_physics_move_input_datum *in) {
    // match: 0x10b880: pointer __restrict qualifier prevents alias reload
    s_character_physics_move_output_datum * __restrict out = output;
    out->flags = 0;
    out->contact = k_invalid_contact_point_reference;
    out->material = -1;
    out->object_index = -1;
    out->flags |= 1;
    switch (m_mode) {
    case 1: get_mode_ground()->move(out, in); break;
    case 2: break;
    case 3: get_mode_dead()->move(out, in); break;
    case 4: case 5: get_mode_sentinel()->move(out, in); break;
    case 6: break;
    // match: 0x10b880: unreachable hint omits dead branch check
    default: __assume(0);
    }
    long object_index = out->object_index != -1 ? out->object_index : in->accepted_physics;
    if (in->local_physics != -1 && object_index == in->local_physics) {
        object_datum *object = OBJECT_GET(in->local_physics, object_datum, NONE);
        if (object->component_index != -1) {
            c_havok_component *component = &(
                (c_havok_component *)g_havok_component_data->data)[datum_index_to_absolute_index(object->component_index)];
            s_object_definition *definition = (s_object_definition *)tag_get(NONE, object->definition_index);
            s_model_definition *model = (s_model_definition *)tag_get(NONE, definition->model.index);
            s_physics_model_definition *physics = (s_physics_model_definition *)tag_get(NONE, model->physics_model_index);
            long count = character_physics_phantom_count(component);
            for (long index = 0; index < count; index++) {
                long body_object, body_index, material_index;
                character_physics_phantom_info(component, index, &body_object, &body_index, &material_index);
                if (body_object == in->object_index) {
                    s_physics_model_phantom *material = &((s_physics_model_phantom *)physics->phantoms.address)[material_index];
                    s_physics_model_phantom_type *phantom = &((s_physics_model_phantom_type *)physics->phantom_types.address)[material->phantom_type_index];
                    if ((phantom->flags >> 24) & 1) { out->flags |= 4; break; }
                }
            }
        }
    }
    if (out->flags & 1) m_flags |= 2; else m_flags &= ~2;
}

// @address 0x2b1610 ?set_mode@c_character_physics_component@@QAAXW4e_mode@1@@Z
void c_character_physics_component::set_mode(e_mode new_mode) {
    e_mode previous_mode = (e_mode)m_mode;
    bool melee_rebuild;
    if (m_mode == _mode_melee && ((c_character_physics_mode_melee_datum *)m_storage)->has_suppressed_camera()) melee_rebuild = true;
    else melee_rebuild = false;
    if (m_mode != _mode_none) {
        switch ((e_mode)m_mode) {
        case _mode_ground: get_mode_ground()->dispose(); break;
        case _mode_flying: get_mode_flying()->dispose(); break;
        case _mode_dead: get_mode_dead()->dispose(); break;
        case _mode_melee: object_clamp_melee_velocity((const s_melee_clamp_data *)m_storage, m_object_index); break;
        case _mode_sentinel: case _mode_sentinel_b: get_mode_sentinel()->dispose(m_object_index); break;
        // match: 0x2b1610: unreachable hint omits dead branch check
        default: __assume(0);
        }
    }
    m_mode = (unsigned char)new_mode;
    switch ((e_mode)m_mode) {
    case _mode_ground: get_mode_ground()->initialize(); break;
    case _mode_flying: get_mode_flying()->initialize(); break;
    case _mode_dead: get_mode_dead()->initialize(); break;
    case _mode_melee: ((c_character_physics_mode_melee_datum *)m_storage)->initialize(); break;
    case _mode_sentinel: case _mode_sentinel_b: {
        real_point3d origin;
        object_get_origin(m_object_index, &origin);
        get_mode_sentinel()->initialize(m_mode == _mode_sentinel_b, &origin);
        break;
    }
    // match: 0x2b1610: unreachable hint omits dead branch check
    default: __assume(0);
    }
    if (previous_mode != (e_mode)m_mode &&
        (melee_rebuild || previous_mode == _mode_dead || m_mode == _mode_dead || previous_mode == _mode_sentinel ||
            previous_mode == _mode_sentinel_b || m_mode == _mode_sentinel || m_mode == _mode_sentinel_b))
        ::havok_object_rebuild(m_object_index, 0);
}

static __forceinline bool character_physics_uses_player_control(const s_character_physics_definition *definition)
{
    return ((definition->flags >> 2) & 1) != 0;
}
// match: 0xe1640: retain the accessor locals and inline boundaries used by the rotation operand ties.
static __forceinline s_object_header *character_physics_object_header_get(long object_index) {
    long absolute_index = object_index & 0xffff;
    s_object_header *header = &((s_object_header *)object_header_data->data)[absolute_index];
    return header;
}
static __forceinline object_datum *character_physics_object_get_and_verify_type(long object_index) {
    s_object_header *header = character_physics_object_header_get(object_index);
    object_datum *object = (object_datum *)header->datum;
    return object;
}
static __forceinline c_havok_component *character_physics_havok_component_get(long havok_component_index) {
    long absolute_index = havok_component_index & 0xffff;
    c_havok_component *component = &((c_havok_component *)g_havok_component_data->data)[absolute_index];
    return component;
}
static __forceinline short character_physics_controlling_rigid_body_index(const c_havok_component *component) {
    return (short)((component->m_object_postion_controlling_rigid_body_index >= 0 
        && component->m_object_postion_controlling_rigid_body_index < component->m_rigid_bodies.getSize())
            ? component->m_object_postion_controlling_rigid_body_index
            : -1);
}
static __forceinline long character_physics_rigid_body_finite_inertia(const c_havok_component *component, long rigid_body_index) {
    const c_havok_component::c_rigid_body *rigid_body = &component->m_rigid_bodies[rigid_body_index];
    return _rotl(~(unsigned long)rigid_body->m_flags, 31) & 1;
}
static __forceinline bool character_physics_physics_flag_test(const unsigned long *flags, long bit) { return (bool)((*flags >> bit) & 1); }
static __forceinline long character_physics_accepted_local_physics_object_get(long object_index) {
    object_datum *object = character_physics_object_get_and_verify_type(object_index);
    long result = -1;
    bool accepted = character_physics_physics_flag_test(&object->physics_flags, 3);
    bool bit1 = character_physics_physics_flag_test(&object->physics_flags, 1);
    bool bit2 = character_physics_physics_flag_test(&object->physics_flags, 2);
    if (accepted && (bit1 || bit2)) result = object->local_physics_object_index;
    return result;
}
static __forceinline long character_physics_local_physics_object_get(long object_index) {
    object_datum *object = character_physics_object_get_and_verify_type(object_index);
    long result = -1;
    bool bit1 = character_physics_physics_flag_test(&object->physics_flags, 1);
    bool bit2 = character_physics_physics_flag_test(&object->physics_flags, 2);
    if (bit1 || bit2) result = object->local_physics_object_index;
    return result;
}
static __forceinline float character_physics_dot_product(const real_vector3d *a, const real_vector3d *b) {
    return a->i * b->i + a->j * b->j + a->k * b->k;
}
static __forceinline real_vector3d *character_physics_cross_product(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}


// @address 0xe1640 character_physics_update_input_datum_initialize
extern "C" void character_physics_update_input_datum_initialize(long object_index, s_character_physics_update_input_datum *input,
        c_character_physics_component *component,
    const s_character_physics_definition *definition, long havok_component_index, const c_animation_manager *animation_manager,
    float scale, e_havok_group havok_group, bool connected, bool facing_constrained, bool facing_locked, bool animation_disabled,
    bool at_rest, bool airborne, bool reduced_adhesion, bool input_gravity, bool stun_active, bool snap_to_ground,
    bool reference_relative, bool limit_upward,
    float movement_scale, float acceleration_scale, const real_vector3d *movement, const real_point3d *position,
    const real_vector3d *forward, const real_vector3d *up, const real_vector3d *facing, const real_vector3d *aiming,
    const real_vector3d *external_velocity, const phys_shape *shape)
{
    c_havok_component *havok_component = character_physics_havok_component_get(havok_component_index);
    input->accepted_physics = character_physics_accepted_local_physics_object_get(object_index);
    input->local_physics = character_physics_local_physics_object_get(object_index);
    input->mode = component->m_mode;
    input->definition = definition;
    input->havok_component_index = havok_component_index;
    input->scale = scale;
    input->havok_group = havok_group;
    input->animation_manager = animation_manager;
    input->flags = 0;
    character_physics_flags_set(&input->flags, 0, connected);
    character_physics_flags_set(&input->flags, 1, at_rest);
    character_physics_flags_set(&input->flags, 2, airborne);
    character_physics_flags_set(&input->flags, 3, reduced_adhesion);
    character_physics_flags_set(&input->flags, 4, input_gravity);
    character_physics_flags_set(&input->flags, 5, stun_active);
    character_physics_flags_set(&input->flags, 6, snap_to_ground);
    character_physics_flags_set(&input->flags, 7, reference_relative);
    character_physics_flags_set(&input->flags, 9, character_physics_controlling_rigid_body_index(havok_component) != -1 &&
        character_physics_rigid_body_finite_inertia(havok_component, character_physics_controlling_rigid_body_index(havok_component)));
    character_physics_flags_set(&input->flags, 11, limit_upward);
    real_vector3d angular_velocity;
    if (character_physics_early_mover_velocity(object_index, &input->early_velocity, &angular_velocity, true, true))
        character_physics_flags_set(&input->flags, 10, true);
    else
        input->early_velocity = k_real_math_zero_vector3d_storage;
    input->movement_scale = movement_scale;
    input->acceleration_scale = acceleration_scale;
    input->movement = *movement;
    input->current_position = *position;
    input->forward = *forward;
    input->up = *up;
    input->aiming = *aiming;
    input->facing = *facing;
    input->external_velocity = *external_velocity;
    input->shape = shape;
    input->acceleration = k_real_math_zero_vector3d_storage;
    input->maximum_velocity = 4.8f;
    input->maximum_slope = 0.0f;
    input->sub_initialized = false;
    if (input->animation_manager->valid() && !animation_disabled)
    {
        real_vector3d motion;
        float yaw_rate;
        animation_manager->read_state_motion_data(object_index, &motion, &yaw_rate);
        character_physics_scale_vector(&motion, input->scale, &input->acceleration);
        if (fabs(yaw_rate) >= 0.0001f)
        {
            real_vector3d new_forward = input->forward;
            float angle = yaw_rate * game_time_globals->tick_length;
            rotate_vector_about_axis(&new_forward, &input->up, sin(angle), cos(angle));
            normalize3d(&new_forward);
            if (facing_locked && facing_constrained)
            {
                float facing_dot = character_physics_dot_product(facing, &input->forward);
                if (facing_dot > 0.5f)
                {
                    real_vector3d old_cross, new_cross;
                    character_physics_cross_product(facing, &input->forward, &old_cross);
                    character_physics_cross_product(facing, &new_forward, &new_cross);
                    bool overshoot = false;
                    if (facing_dot > 0.99f)
                    {
                        float new_dot = character_physics_dot_product(facing, &new_forward);
                        if (new_dot < facing_dot)
                            overshoot = true;
                    }
                    if (character_physics_dot_product(&old_cross, &input->up) * character_physics_dot_product(&new_cross, &input->up) <= 0.0f || overshoot)
                    {
                        new_forward = *facing;
                        character_physics_cross_product(&input->up, &new_forward, &old_cross);
                        character_physics_cross_product(&old_cross, &input->up, &new_forward);
                        normalize3d(&new_forward);
                        input->flags |= 0x100;
                    }
                }
            }
            input->forward = new_forward;
        }
    }
}

// @address 0xe6e60 ?character_physics_update_input_datum_initialize_ground@@YAXPAUs_character_physics_update_input_datum@@PAVc_character_physics_component@@_N22MM@Z
void character_physics_update_input_datum_initialize_ground(s_character_physics_update_input_datum *input,
    c_character_physics_component *component, bool disable_player_control, bool sprinting, bool ground_contact,
    float friction, float crouch)
{
    input->support_component = component->get_mode_ground()->get_support_havok_component_index();
    input->support_body = component->get_mode_ground()->get_support_rigid_body_index();
    const real_matrix4x3 *support_matrix = component->get_mode_ground()->get_support_havok_component_matrix();
    memcpy(&input->support_matrix, support_matrix, sizeof(real_matrix4x3));
    input->ground_plane = *component->get_mode_ground()->get_ground_plane();
    input->ground_friction = friction;
    input->ground_adhesion = k_character_physics_ground_zero;
    input->ground_contact = ground_contact;
    input->sub_initialized = true;
    input->desired_velocity = input->forward;
    if (input->flags & 4)
        crouch = k_character_physics_ground_zero;
    bool player_control = character_physics_uses_player_control(input->definition) && !disable_player_control;
    if (player_control)
    {
        const s_player_information_definition *control =
            (const s_player_information_definition *)global_game_globals->player_information.address;
        real_vector2d movement = { input->movement.i, input->movement.j };
        float magnitude_squared = movement.i * movement.i + movement.j * movement.j;
        if (magnitude_squared > character_physics_ground_one())
        {
            float scale = character_physics_ground_one() / sqrtf(magnitude_squared);
            movement.i = scale * movement.i;
            movement.j = scale * movement.j;
        }
        movement.i = character_physics_snap_movement(movement.i);
        movement.j = character_physics_snap_movement(movement.j);
        if (movement.i > k_character_physics_ground_zero)
            input->acceleration.i = control->run_forward * (character_physics_ground_one() - crouch) + control->sneak_forward * crouch;
        else
            input->acceleration.i = control->run_backward * (character_physics_ground_one() - crouch) + control->sneak_backward * crouch;
        input->acceleration.j = control->run_sideways * (character_physics_ground_one() - crouch) + control->sneak_sideways * crouch;
        input->acceleration.k = k_character_physics_ground_zero;
        input->maximum_velocity = control->run_acceleration * (character_physics_ground_one() - crouch) + control->sneak_acceleration * crouch;
        input->maximum_slope = control->airborne_acceleration;
        if (sprinting)
        {
            input->acceleration.i = movement.i > k_character_physics_ground_zero ? control->walking_speed : k_character_physics_ground_zero;
            input->acceleration.j = k_character_physics_ground_zero;
        }
        input->acceleration.i *= movement.i;
        input->acceleration.j *= movement.j;
        input->desired_velocity = input->facing;
    }
    else if (!(input->flags & 16) && !(input->flags & 4))
        input->maximum_velocity = k_character_physics_ground_max;
}
static __forceinline void character_physics_projection_distance(float distance, real_vector3d *result)
{
    result->i = -distance;
}
// @address 0xe24a0 ?character_physics_update_input_datum_initialize_flying@@YAXJPAUs_character_physics_update_input_datum@@PAVc_character_physics_component@@MM@Z
void character_physics_update_input_datum_initialize_flying(
    long node_count, s_character_physics_update_input_datum *input, c_character_physics_component *component, float friction, float boost)
{
    input->flying_friction = friction;
    const s_character_physics_definition *definition = input->definition;
    input->sub_initialized = true;
    float throttle = 1.0f - MIN(character_physics_magnitude(&input->movement), 1.0f);
    float acceleration = 1.0f;
    if (definition->flying.crouch_velocity_modifier > 0.0f)
    {
        if (boost == 1.0f)
            acceleration = definition->flying.crouch_velocity_modifier;
        else if (boost > 0.0f)
            acceleration = 1.0f + boost * (definition->flying.crouch_velocity_modifier - 1.0f);
    }
    acceleration *= input->acceleration_scale;
    real_vector3d acceleration_scale;
    acceleration_scale.i = acceleration * input->scale * definition->flying.maximum_velocity;
    acceleration_scale.j = acceleration * input->scale * definition->flying.maximum_sidestep_velocity;
    acceleration_scale.k = acceleration * input->scale * definition->flying.maximum_sidestep_velocity;
    input->maximum_velocity = acceleration * input->scale *
        ((1.0f - throttle) * definition->flying.acceleration + throttle * definition->flying.deceleration);
    input->maximum_slope = input->maximum_velocity;
    input->acceleration.i = input->movement.i * acceleration_scale.i;
    input->acceleration.j = input->movement.j * acceleration_scale.j;
    input->acceleration.k = input->movement.k * acceleration_scale.k;

    real_vector3d animation_velocity;
    float animation_yaw_rate;
    input->animation_manager->read_state_motion_data(node_count, &animation_velocity, &animation_yaw_rate);
    scale_vector3d(&animation_velocity, input->scale, &animation_velocity);
    real_vector3d animation_direction = animation_velocity;
    normalize3d(&animation_direction);
    real_vector3d animation_offset;
    character_physics_projection_distance(character_physics_dot(&animation_direction, &input->acceleration), &animation_offset);
    character_physics_point_from_line(&input->acceleration, &animation_direction, animation_offset.i, &input->acceleration);
    character_physics_add_vectors(&animation_velocity, &input->acceleration, &input->acceleration);

    if (g_character_physics_flying_noise)
    {
        unsigned long noise_seed = game_time_globals->time * input->havok_component_index;
        real_vector3d noise;
        noise.i = TEST_BIT(noise_seed, 0) ? 1.0f : 0.0f;
        noise.j = TEST_BIT(noise_seed, 1) ? 1.0f : 0.0f;
        noise.k = TEST_BIT(noise_seed, 2) ? 1.0f : 0.0f;
        noise.i *= TEST_BIT(noise_seed, 3) ? 1.0f : -1.0f;
        noise.j *= TEST_BIT(noise_seed, 4) ? 1.0f : -1.0f;
        noise.k *= TEST_BIT(noise_seed, 5) ? 1.0f : -1.0f;
        noise.i *= TEST_BIT(noise_seed, 6) ? 1.0f : 0.5f;
        noise.j *= TEST_BIT(noise_seed, 7) ? 1.0f : 0.5f;
        noise.k *= TEST_BIT(noise_seed, 8) ? 1.0f : 0.5f;
        scale_vector3d(&noise, 0.2f, &noise);
        character_physics_multiply_vectors(&noise, &acceleration_scale, &noise);
        character_physics_add_vectors(&noise, &input->acceleration, &input->acceleration);
    }
}


static __forceinline void object_move_position(
    long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up, const s_location *location)
{
    object_set_position_internal(object_index, position, forward, up, location, false, true, false, false);
    object_set_requires_motion(object_index);
}

// @address 0xabe40 ?update@c_character_physics_component@@QAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_component::update(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input)
{
    output->flags = 0;
    output->up = input->up;
    output->facing = input->forward;
    output->velocity = *global_zero_vector3d;
    output->animation_state = s_character_physics_update_output_datum::_animation_state_none;
    output->animation_state_valid = false;
    output->origin_valid = false;
    output->origin = *global_origin3d;
    output->previous_external_velocity = *global_zero_vector3d;
    if (m_collision_damage_immunity_counter != 0)
        m_collision_damage_immunity_counter += 255;
    switch (m_mode - 1) {
    case 0:
        get_mode_ground()->update(output, input);
        break;
    case 1:
        get_mode_flying()->update(output, input);
        break;
    case 2:
        get_mode_dead()->update(output, input);
        break;
    case 5: {
        short tick = game_time_globals->ticks_per_second;
        m_collision_damage_immunity_counter = (int)((float)tick * 0.3f + 0.5f);
        get_mode_melee()->update(output, input);
        break;
    }
    case 3:
    case 4:
        get_mode_sentinel()->update(output, input);
        break;
    default:
        // match: 0xabe40: unreachable hint omits dead branch check
        __assume(0);
    }
    update_animation(output, input);
    const phys_shape *shape = input->shape;
    if (shape != 0) {
        c_havok_component *havok_component = havok_component_get(input->havok_component_index);
        const phys_rigid_body *body = havok_component->m_rigid_bodies[0].m_rigid_body;
        if (body->get_collider()->get_shape() != shape) {
            int type = shape->get_type_id();
            if (type == PHYS_SHAPE_CAPSULE) {
                float diameter = g_havok_constants.minimum_radius * 2.0f;
                const phys_capsule_shape *input_capsule = (const phys_capsule_shape *)shape;
                phys_capsule_shape capsule(input_capsule->get_vertex(0), input_capsule->get_vertex(1),
                    (input_capsule->get_convex_radius() - diameter > g_havok_constants.minimum_radius) ?
                        input_capsule->get_convex_radius() - g_havok_constants.minimum_radius * 2.0f :
                        g_havok_constants.minimum_radius);
                real_matrix4x3 matrix;
                ((c_havok_component *)havok_component)->get_rigid_body_transform(0, &matrix);
                matrix.position.z += diameter * 0.5f;
                if (!havok_world_shape_collides(&capsule, input->havok_component_index, &matrix, (e_havok_group)input->havok_group))
                    output->flags |= 2;
                else
                    output->flags &= ~2;
            }
            else if (type == PHYS_SHAPE_SPHERE || (type == PHYS_SHAPE_CONVEX_OFFSET 
        && ((const phys_convex_offset_shape *)shape)->get_child_shape()->get_type_id() == PHYS_SHAPE_SPHERE)) {
                output->flags |= 2;
            }
            else {
                output->flags &= ~2;
            }
        }
        else {
            output->flags |= 2;
        }
    }
    if (character_physics_test_flags(input->definition->flags, 5)) {
        object_set_velocities(m_object_index, &output->velocity, 0);
        real_point3d origin;
        point_from_line3d(&input->current_position, &output->velocity, game_tick_length(), &origin);
        object_move_position(m_object_index, &origin, 0, 0, 0);
    }
    if (TEST_BIT(output->flags, 0))
        m_flags |= 1;
    else
        m_flags &= ~1;
}
