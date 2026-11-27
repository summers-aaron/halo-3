// blamlibxbox/havok_proxies.obj: from the records 0x102c08 0x1031c0 0x1032b8 0x1075a0 0x242070 0x2420b8 0x2420c8 0x2420e0 0x242238 0x2422e0 0x242650 0x242710 0x2427c8 0x242868 0x2428b8 0x242920 0x242938 0x242970 0x242978 0x2429d8 0x242a28
// @flags /TP /O2 /Oc
// @address 0x2428b8 ?get_rotation_axis@phys_quat@@QBAXAAVphys_vec4@@@Z
#include "math/matrix_math.h"
#include "memory/data.h"
#include "objects/object_iterator.h"
#include "phys/phys_base_types.h"
#include <vectorintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "physics/havok_proxies.h"
#include "physics/havok_proxy_types.h"
#include "objects/widgets/muffins.h"
#include "physics/havok.h"
#include "physics/havok_utilities.h"
#include "units/units.h"
#include "cache/restricted_memory_layout.h"
#include "physics/havok_component.h"
#include "objects/object_flags.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "game/game_time_types.h"
#include "game/game_time_access.h"
#include "units/unit_passenger_iterator_types.h"
#include "phys/phys_math_constants.h"

struct unit_datum;
struct unit_passenger_iterator;
struct s_data_iterator;
struct s_data_array;
typedef unsigned int uint;
struct s_data_array;
class c_allocation_base;

#include "physics/havok_proxies_globals.h"
// @address 0x11df7a0 g_havok_proxy_data
__declspec(thread) s_data_array *g_havok_proxy_data;

#include "cache/restricted_memory_registration.h"
class c_restricted_memory;

#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"



#define PHYS_RIGID_BODY_OUT_OF_LINE
#include "physics/havok_component_class.h"
#undef PHYS_RIGID_BODY_OUT_OF_LINE
#include "physics/havok_math.h"
#include "physics/havok_component_datum_access.h"

struct s_havok_proxies_tolerances { float angle; };

struct s_data_array;
namespace {
}

// @address 0x783210 g_havok_proxy_data_allocator
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_havok_proxy_data_allocator> g_havok_proxy_data_allocator;

// @address 0x8c29cf ?g_havok_proxy_rigid_bodies_active@@3_NA
bool g_havok_proxy_rigid_bodies_active;

// @address 0x14d90 ?phys_simd_vec_minus_half@@3U__vector4@@B
// @rdata 0x82014d90..0x82014da0
const __vector4 phys_simd_vec_minus_half = { -0.5f, -0.5f, -0.5f, -0.5f };
// @rdata-end

// @address 0x242070 ?havok_proxies_initialize@@YAXXZ
void havok_proxies_initialize(void)
{
    g_havok_proxy_data = data_new("havok proxies", 16, sizeof(s_havok_proxy_datum), 0, (c_allocation_base *)&g_havok_proxy_data_allocator);
}

// @address 0x2420b8 ?havok_proxies_dispose@@YAXXZ
void havok_proxies_dispose(void)
{
    data_dispose(g_havok_proxy_data);
}

// @address 0x2420c8 ?havok_proxies_initialize_for_new_map@@YAXXZ
void havok_proxies_initialize_for_new_map(void)
{
    data_make_valid(g_havok_proxy_data);
}

// @address 0x1031c0 ?havok_proxies_dispose_from_old_map@@YAXXZ
void havok_proxies_dispose_from_old_map(void)
{
    g_havok_proxy_data->valid = false;
}





// @address 0x2420e0 ?havok_proxy_aquire_internal@@YA_NJ_N@Z
bool havok_proxy_aquire_internal(long object_index, bool create_record)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
    bool result = false;
    if (object->havok_component_index != NONE) {
        c_havok_component *component = havok_component_get(object->havok_component_index);
        short rigid_body_index = component->get_object_postion_controlling_rigid_body_index();
        if (rigid_body_index != NONE) {
            long havok_proxy_index;
            if (create_record)
                havok_proxy_index = datum_new(g_havok_proxy_data);
            else
                havok_proxy_index = object_get_havok_proxy_index(object_index);
            if (havok_proxy_index != NONE) {
                s_havok_proxy_datum *havok_proxy = datum_get<s_havok_proxy_datum>(g_havok_proxy_data, havok_proxy_index);
                phys_transform transform;
                if (create_record) {
                    ASSERT(!object->m_physics_flags.test(_object_physics_has_havok_proxy_bit));
                    component->get_rigid_body_transform(rigid_body_index, &havok_proxy->matrix);
                    havok_proxy->creation_time = game_time_get();
                }
                phys_transform_from_matrix4x3(&transform, &havok_proxy->matrix);
                havok_proxy->body = 0;
                havok_proxy->object_index = object_index;
                havok_proxy->body = component->construct_proxy_rigid_body(rigid_body_index);
                havok_proxy->body->set_transform(transform);
                object->m_physics_flags.set(_object_physics_has_havok_proxy_bit, true);
                havok_world_add_entity(havok_proxy->body, object->m_physics_flags.test(_object_physics_bit9));
                result = true;
            }
        }
    }
    return result;
}

// @address 0x242238 ?havok_proxy_release_internal@@YAXJ_N@Z
void havok_proxy_release_internal(long object_index, bool delete_record)
{
    long havok_proxy_index = object_get_havok_proxy_index(object_index);
    if (havok_proxy_index != NONE) {
        s_havok_proxy_datum *havok_proxy = datum_get<s_havok_proxy_datum>(g_havok_proxy_data, havok_proxy_index);
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
        ASSERT(object_index == havok_proxy->object_index);
        if (havok_proxy->body) {
            havok_world_remove_entity(havok_proxy->body);
            havok_proxy->body->object_remove_ref();
            havok_proxy->body = 0;
        }
        if (delete_record) {
            object->m_physics_flags.set(_object_physics_has_havok_proxy_bit, false);
            ::datum_delete(g_havok_proxy_data, havok_proxy_index);
        }
    }
}

// @address 0x242650 ?is_player_controlled_object@@YA_NJ@Z
bool is_player_controlled_object(long object_index)
{
    bool result = false;
    s_object_header *header = object_header_get(object_index);
    if (((1 << header->object_type) & 0x1003) != 0) {
        unit_datum *unit = (unit_datum *)header->datum;
        if (unit->unit.player_index != -1) {
            result = true;
        } else {
            unit_passenger_iterator iterator;
            iterator.unit_index = object_index;
            iterator.passenger_index = -1;
            iterator.seat_index = -1;
            iterator.next_object_index = unit->object.first_child_object_index;
            unit_datum *passenger;
            while ((passenger = ::unit_passenger_iterator_next(&iterator)) != 0) {
                if (passenger->unit.player_index != -1) {
                    result = true;
                    break;
                }
            }
        }
    }
    return result;
}


// @address 0x242710 ?object_get_havok_proxy_index@@YAJJ@Z
long object_get_havok_proxy_index(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
    long havok_proxy_index = NONE;
    if (object->m_physics_flags.test(_object_physics_has_havok_proxy_bit)) {
        c_data_iterator<s_havok_proxy_datum> iterator;
        iterator.begin(g_havok_proxy_data);
        while (iterator.next()) {
            if (iterator.get_datum()->object_index == object_index) {
                havok_proxy_index = iterator.get_index();
                break;
            }
        }
        ASSERT(havok_proxy_index != NONE);
    }
    return havok_proxy_index;
}

template<> void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_havok_proxy_data_allocator>::allocate(
    unsigned int size, const char *name)
{
    m_member_index = restricted_region_register_member(1, name, "", size, 0,
        __tls_set_g_muffin_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}


// @address 0x1032b8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_havok_proxy_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_havok_proxy_data_allocator>::deallocate(void *allocation)
{
    g_havok_proxy_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x242868 ?set_angular_velocity@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// match: 0x242868: plain inline preserves the summary consumed by havok_component's velocity setter.
#if defined(_XBOX) && !defined(__clang__)
inline void phys_rigid_body::set_angular_velocity(const phys_vec4 &body_new_lin_vel)
#else
void phys_rigid_body::set_angular_velocity(const phys_vec4 &body_new_lin_vel)
#endif
{
    activate();
    get_rigid_motion()->set_angular_velocity(body_new_lin_vel);
}

// @address 0x242938 ?get_rotation_angle@phys_quat@@QBAMXZ
float phys_quat::get_rotation_angle() const
{
    return phys_math::acos(phys_math::fabs(m_quat_vec(3))) * 2.0f;
}

namespace phys_math {

// @address 0x242970 ?fabs@phys_math@@YAMM@Z
// @address 0x242920 ?inv_sqrt@phys_math@@YAMM@Z
// @address 0x242978 ?acos@phys_math@@YAMM@Z
float acos(float value)
{
    if (!(phys_math::fabs(value) < 1.0f)) {
        value = value > 0.0f ? 0.0f : 3.1415927f;
        return value;
    } else {
        return (float)::acos(value);
    }
}

}
// @address 0x242a28 ??Bphys_simd_scalar@@QBAMXZ
// @address 0x2429d8 ?set_linear_velocity@phys_rigid_body@@QAAXABVphys_vec4@@@Z
// match: 0x2429d8: plain inline preserves the summary consumed by havok_component's velocity setter.
#if defined(_XBOX) && !defined(__clang__)
inline void phys_rigid_body::set_linear_velocity(const phys_vec4 &body_new_lin_vel)
#else
void phys_rigid_body::set_linear_velocity(const phys_vec4 &body_new_lin_vel)
#endif
{
    activate();
    get_rigid_motion()->set_linear_velocity(body_new_lin_vel);
}

float havok_proxies_simd_real_to_float(const phys_simd_scalar &value)
{
    return value;
}

// match: 0x1075a0: preload each column before its three stores, preserving the column copy boundary.
static __forceinline void havok_proxies_rotation_from_columns(real_matrix3x3 *matrix, const phys_vec4 *columns)
{
    const float *c0 = (const float *)&columns[0];
    const float *c1 = (const float *)&columns[1];
    const float *c2 = (const float *)&columns[2];
    set_real_vector3d(&matrix->forward, c0[0], c0[1], c0[2]);
    set_real_vector3d(&matrix->left, c1[0], c1[1], c1[2]);
    set_real_vector3d(&matrix->up, c2[0], c2[1], c2[2]);
}

static __forceinline void havok_proxies_matrix_from_transform(real_matrix4x3 *matrix, const phys_transform *transform)
{
    matrix->scale = 1.0f;
    havok_proxies_rotation_from_columns(&matrix->rotation, &transform->m_rotation.m_column0);
    const float *position = (const float *)&transform->m_translation;
    set_real_point3d(&matrix->position, position[0], position[1], position[2]);
}

// @address 0x1075a0 ?havok_proxies_move@@YAXXZ
void havok_proxies_move() {
 c_data_iterator<s_havok_proxy_datum> iterator;
 iterator.begin(g_havok_proxy_data);
 while(iterator.next()) {
  s_havok_proxy_datum *proxy=iterator.get_datum();
  if(proxy->body) havok_proxies_matrix_from_transform(&proxy->matrix,&proxy->body->get_transform());
 }
}



// @rdata 0x82024f00
static const float havok_proxies_maximum_angular_speed = 3.1415927f;
// @rdata 0x8202490c
static const s_havok_proxies_tolerances havok_proxies_tolerances = { 0.0001f };
static __forceinline float havok_proxies_get_angle_epsilon() { return havok_proxies_tolerances.angle; }

static __forceinline float havok_proxies_dot3(const phys_vec4 &a, const phys_vec4 &b)
{
    float value;
    __stvewx(__vmsum3fp(a.m_simd_quad, b.m_simd_quad), &value, 0);
    return value;
}

static __forceinline void havok_proxies_limit_velocity(phys_vec4 &velocity, float maximum, float maximum_squared)
{
    float length_squared = havok_proxies_dot3(velocity, velocity);
    if (length_squared > maximum_squared) {
        float scale = maximum / sqrtf(length_squared);
        velocity.m_simd_quad = __vmulfp(velocity.m_simd_quad, __vspltw(__lvlx(&scale, 0), 0));
    }
}

static __forceinline void havok_proxies_rotate_vector(phys_vec4 &result, const phys_quat &rotation, const phys_vec4 &vector)
{
    __vector4 w = __vspltw(rotation.m_quat_vec.m_simd_quad, 3);
    __vector4 cross = __vsubfp(__vmulfp(__vpermwi(rotation.m_quat_vec.m_simd_quad, 99), __vpermwi(vector.m_simd_quad, 135)),
        __vmulfp(__vpermwi(rotation.m_quat_vec.m_simd_quad, 135), __vpermwi(vector.m_simd_quad, 99)));
    __vector4 rotated = __vmaddfp(cross, w, __vmaddfp(__vspltw(__vmsum3fp(rotation.m_quat_vec.m_simd_quad, vector.m_simd_quad), 0),
        rotation.m_quat_vec.m_simd_quad,
        __vmulfp(vector.m_simd_quad, __vmaddfp(w, w, phys_simd_vec_minus_half))));
    result.m_simd_quad = __vaddfp(rotated, rotated);
}

static __forceinline __vector4 havok_proxies_inverse_length(const __vector4 &squared)
{
    __vector4 inverse;
    inverse.v[0] = 1.0f / sqrtf(squared.v[0]);
    return __vspltw(inverse, 0);
}
static __forceinline void havok_proxies_normalize_quaternion(phys_quat &rotation)
{
    rotation.m_quat_vec.m_simd_quad = __vmulfp(rotation.m_quat_vec.m_simd_quad,
        havok_proxies_inverse_length(__vmsum4fp(rotation.m_quat_vec.m_simd_quad, rotation.m_quat_vec.m_simd_quad)));
}

// @address 0x2422e0 ?havok_proxies_apply_hard_key_frame@@YAXABVphys_vec4@@ABVphys_quat@@M0PAVphys_rigid_body@@@Z
void havok_proxies_apply_hard_key_frame(const phys_vec4 &next_position, const phys_quat &next_orientation,
    float inverse_delta_time, const phys_vec4 &velocity_limits, phys_rigid_body *rigid_body)
{
    float maximum_linear_acceleration = 1.0f;
    float maximum_angular_acceleration = 0.75f * 3.1415927f;
    float maximum_linear_velocity = 3.0f;
    float maximum_angular_velocity = 1.0f * havok_proxies_maximum_angular_speed;
    const phys_vec4 &old_linear_velocity = rigid_body->m_motion.m_linear_velocity;
    float speed_squared = havok_proxies_dot3(velocity_limits, velocity_limits);
    float speed = 0.0f;
    phys_vec4 linear_delta, linear_velocity, center;
    havok_proxies_rotate_vector(center, next_orientation, rigid_body->m_motion.m_motion_state.m_swept_transform.m_center_of_mass_local);
    center.m_simd_quad = __vaddfp(center.m_simd_quad, next_position.m_simd_quad);
    linear_velocity.m_simd_quad = __vsubfp(center.m_simd_quad, rigid_body->m_motion.m_motion_state.m_swept_transform.m_center_of_mass1.m_simd_quad);
    linear_velocity.m_simd_quad = __vmulfp(linear_velocity.m_simd_quad, __vspltw(__lvlx(&inverse_delta_time, 0), 0));
    if (speed_squared > 9.999999e-09f) {
        speed = havok_proxies_dot3(velocity_limits, linear_velocity);
        speed = speed > 0.0f ? speed : 0.0f;
    }
    linear_delta.m_simd_quad = __vsubfp(linear_velocity.m_simd_quad, old_linear_velocity.m_simd_quad);
    havok_proxies_limit_velocity(linear_delta, maximum_linear_acceleration, maximum_linear_acceleration * maximum_linear_acceleration);
    linear_velocity.m_simd_quad = __vaddfp(old_linear_velocity.m_simd_quad, linear_delta.m_simd_quad);
    float maximum_linear_speed = maximum_linear_velocity + speed;
    havok_proxies_limit_velocity(linear_velocity, maximum_linear_speed, maximum_linear_speed * maximum_linear_speed);
    rigid_body->set_linear_velocity(linear_velocity);
    const phys_vec4 &old_angular_velocity = rigid_body->m_motion.m_ang_velocity;
    phys_vec4 angular_delta, angular_velocity;
    phys_quat rotation;
    rotation.set_mul_inverse(next_orientation, rigid_body->get_rotation());
    havok_proxies_normalize_quaternion(rotation);
    float angle = phys_math::acos(phys_math::fabs(rotation.m_quat_vec(3))) * 2.0f;
    if (angle < 0.0001f) {
#if defined(_XBOX) && !defined(__clang__)
        angular_velocity.m_simd_quad = __vxor(angular_velocity.m_simd_quad, angular_velocity.m_simd_quad);
#else
        angular_velocity.m_simd_quad = __vzero();
#endif
    } else {
        float rotation_angle = angle;
        rotation.get_rotation_axis(angular_velocity);
        float scale = rotation_angle * inverse_delta_time;
        angular_velocity.m_simd_quad = __vmulfp(angular_velocity.m_simd_quad, __vspltw(__lvlx(&scale, 0), 0));
    }
    angular_delta.m_simd_quad = __vsubfp(angular_velocity.m_simd_quad, old_angular_velocity.m_simd_quad);
    havok_proxies_limit_velocity(angular_delta, maximum_angular_acceleration, 5.551653f);
    angular_velocity.m_simd_quad = __vaddfp(old_angular_velocity.m_simd_quad, angular_delta.m_simd_quad);
    havok_proxies_limit_velocity(angular_velocity, maximum_angular_velocity, 9.869605f);
    rigid_body->set_angular_velocity(angular_velocity);
}

// @rdata 0x820221fc
static const float havok_proxies_normalize_one = 1.0f;
// @rdata 0x8202490c
static const float havok_proxies_normalize_epsilon = 0.0001f;

static __forceinline void havok_proxies_vector_from_real(phys_vec4 *vector, const real_vector3d *value)
{
    vector->set(value->i, value->j, value->k, 0.0f);
}

static __forceinline void havok_proxies_point_from_real(phys_vec4 *vector, const real_point3d *value)
{
    vector->set(value->x, value->y, value->z, 0.0f);
}

static __forceinline void havok_proxies_transform_from_matrix(phys_transform *transform, const real_matrix4x3 *matrix)
{
    havok_proxies_vector_from_real(&transform->m_rotation.get_column(0), &matrix->forward);
    havok_proxies_vector_from_real(&transform->m_rotation.get_column(1), &matrix->left);
    havok_proxies_vector_from_real(&transform->m_rotation.get_column(2), &matrix->up);
    havok_proxies_point_from_real(&transform->m_translation, &matrix->position);
}

static __forceinline phys_rigid_body *havok_proxies_get_rigid_body(c_havok_component *component, long index)
{
    return component->m_rigid_bodies[index].m_rigid_body;
}

static __forceinline void havok_proxies_real_from_vector(real_vector3d *vector, const phys_vec4 *value)
{
    vector->i = (*value)(0);
    vector->j = (*value)(1);
    vector->k = (*value)(2);
}

static __forceinline void havok_proxies_linear_velocity(c_havok_component *component, long index, real_vector3d *velocity)
{
    if (!havok_proxies_get_rigid_body(component, index)->body_is_fixed())
        havok_proxies_real_from_vector(velocity, &havok_proxies_get_rigid_body(component, index)->get_linear_velocity());
    else
        *velocity = *global_zero_vector3d;
}

static __forceinline void havok_proxies_angular_velocity(c_havok_component *component, long index, real_vector3d *velocity)
{
    if (!havok_proxies_get_rigid_body(component, index)->body_is_fixed())
        havok_proxies_real_from_vector(velocity, &havok_proxies_get_rigid_body(component, index)->get_angular_velocity());
    else
        *velocity = *global_zero_vector3d;
}

static __forceinline float havok_proxies_normalize3d(real_vector3d *vector)
{
    float length = sqrtf(vector->j * vector->j + (vector->k * vector->k + (vector->i * vector->i)));
    if (fabsf(length) < havok_proxies_normalize_epsilon)
        return 0.0f;
    float scale = havok_proxies_normalize_one / length;
    vector->i *= scale;
    vector->j *= scale;
    vector->k *= scale;
    return length;
}

// @address 0x102c08 ?havok_proxies_update@@YAXXZ
void havok_proxies_update()
{
    {
    c_object_iterator<object_datum> objects;
    objects.begin(0x1003, 0);
    if (objects.next()) {
    s_data_array *const &proxy_data = g_havok_proxy_data;
    s_data_array *const &object_headers = object_header_data;
    do {
        s_object_header *header = datum_get<s_object_header>(object_headers, objects.get_index());
        long free_proxies = 16 - ((s_data_array *)proxy_data)->actual_count;
        object_datum *object = objects.get_datum();
        bool active = (header->flags & 1) != 0;
        bool requires_proxy = (object->m_physics_flags.m_storage & (1 << 7)) != 0;
        bool damaged = (object->damage_flags & (1 << 2)) != 0;
        long object_index = objects.get_index();
        if (object->m_physics_flags.test(_object_physics_has_havok_proxy_bit)) {
            long proxy_index = object_get_havok_proxy_index(object_index);
            s_havok_proxy_datum *proxy = datum_get<s_havok_proxy_datum>(g_havok_proxy_data, proxy_index);
            if (!active || !requires_proxy || damaged)
                havok_proxy_release_internal(object_index, true);
            else if (free_proxies < 2 && game_time_get() > proxy->creation_time + tls_game_time_globals()->ticks_per_second &&
                !is_player_controlled_object(object_index))
                havok_proxy_release_internal(object_index, true);
        } else if (active && requires_proxy && !damaged) {
            if (free_proxies > 2 || (free_proxies != 0 && is_player_controlled_object(object_index))) {
                if (!((object_datum *)header->datum)->m_physics_flags.test(_object_physics_has_havok_proxy_bit))
                    havok_proxy_aquire_internal(object_index, true);
            }
        }
    } while (objects.next());
    }
    }
    float delta_time = tls_game_time_globals()->tick_length;
    float inverse_delta_time = 1.0f / delta_time;
    c_data_iterator<s_havok_proxy_datum> proxies;
    proxies.begin(g_havok_proxy_data);
    while (proxies.next()) {
        s_havok_proxy_datum *proxy = proxies.get_datum();
        object_datum *object = (object_datum *)object_get_and_verify_type(proxy->object_index, -1);
        if (object->havok_component_index != NONE && proxy->body &&
            (proxy->body->is_body_active() || !object->m_physics_flags.test(_object_physics_bit9))) {
            c_havok_component *component = havok_component_get(object->havok_component_index);
            long rigid_body_index = component->get_object_postion_controlling_rigid_body_index();
            phys_transform transform;
            phys_quat orientation;
            phys_vec4 velocity;
            real_matrix4x3 matrix;
            component->get_rigid_body_transform(rigid_body_index, &matrix);
            real_vector3d linear_velocity, angular_velocity;
            havok_proxies_linear_velocity(component, rigid_body_index, &linear_velocity);
            havok_proxies_angular_velocity(component, rigid_body_index, &angular_velocity);
            float angular_speed = havok_proxies_normalize3d(&angular_velocity);
            if (angular_speed != 0.0f) {
                real_matrix4x3 rotation;
                float cosine = (float)cos(angular_speed * delta_time);
                float sine = (float)sin(angular_speed * delta_time);
                matrix4x3_rotation_from_axis_and_angle(&rotation, &angular_velocity, sine, cosine);
                matrix3x3_multiply(&rotation.rotation, &matrix.rotation, &matrix.rotation);
            }
            point_from_line3d(&matrix.position, &linear_velocity, delta_time, &matrix.position);
            havok_proxies_transform_from_matrix(&transform, &matrix);
            orientation.set(transform.m_rotation);
            havok_proxies_vector_from_real(&velocity, &linear_velocity);
            havok_proxies_apply_hard_key_frame(transform.m_translation, orientation, inverse_delta_time, velocity, proxy->body);
        }
    }
}

