// hkcollide/hkHeightFieldAgent.obj: from the records 0x630de0 0x630e70 0x630f50 0x630fa8 0x631280 0x631610 0x6318e8 0x631900 0x631918 0x631930 0x6319a0 0x631ab8 0x631ba8 0x631c20 0x631c88 0x632078 0x6320c8 0x632118 0x632200 0x6322b0 0x632398 0x6323e0
// @flags /TP /O2 /J /Oc
#include "phys/phys_agent_vtables.h"
#include "phys/phys_base_types.h"
#include "phys/phys_heightfield_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_heightfield_shape_base.h"
#include <vectorintrinsics.h>
#include <math.h>
#include <ppcintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_shape_type_enum.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_sphere.h"
#include "phys/phys_contact_point.h"
// @address 0x15f84 ??_7phys_ray_hit_collector@@6B@
// @address 0x15f90 ??_7phys_heightfield_ray_forwarder@@6B@
// @address 0x15f9c k_hk_height_field_agent_vtable
// @address 0x6319a0 ?register_agent@phys_heightfield_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x631ab8 ?add_ray_hit@phys_heightfield_ray_forwarder@@UAAXABVphys_collision_proxy@@ABUphys_ray_collector_output@@@Z
// @address 0x631ba8 ??1phys_heightfield_agent@@MAA@XZ
// @address 0x631c20 ??_Gphys_heightfield_agent@@MAAPAXI@Z

// Registers height-field collision agents in both shape orders.

#include "phys/phys_heightfield_ray_forwarder.h"

// @address 0x630e70 ??0phys_heightfield_agent@@IAA@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x630de0 ?cleanup@phys_heightfield_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x630f50 ?create_heightfield_b_agent@phys_heightfield_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x6318e8 ?sweep_cast@phys_heightfield_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x631900 ?get_penetrations@phys_heightfield_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x631918 ?get_closest_points@phys_heightfield_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x631930 ?create_heightfield_a_agent@phys_heightfield_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x632078 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_heightfield_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x632200 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_heightfield_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x632398 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_heightfield_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z

void phys_heightfield_agent::cleanup(phys_constraint_owner &owner)
{
    if (m_contact_manager) {
        for (int i = 0; i < m_contact_point_ids.getSize(); ++i) {
            if (m_contact_point_ids[i] != 0xffff) {
                m_contact_manager->contact_remove_point(m_contact_point_ids[i], owner);
            }
        }
    }
    delete this;
}

phys_heightfield_agent::phys_heightfield_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager) : phys_agent(pair_contact_manager)
{
    if (pair_contact_manager) {
        const phys_sphere_rep_shape *shape = (const phys_sphere_rep_shape *)agent_body_a.get_shape();
        int child_sphere_count = shape->get_collision_sphere_count();
        unsigned short invalid_id = 0xffff;
        m_contact_point_ids.set_array_size(child_sphere_count, invalid_id);
    }
}


phys_agent *phys_heightfield_agent::create_heightfield_b_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    return new phys_heightfield_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
}

void phys_heightfield_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    static_sweep_cast(agent_body_a, agent_body_b, input, collector, start_collector);
}

void phys_heightfield_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    static_penetrations(agent_body_a, agent_body_b, input, collector);
}

void phys_heightfield_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    static_closest_points(agent_body_a, agent_body_b, input, collector);
}

phys_agent *phys_heightfield_agent::create_heightfield_a_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    return new phys_symmetric_agent<phys_heightfield_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
}

void phys_heightfield_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_heightfield_agent::create_heightfield_a_agent;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, (phys_shape_type)21, (phys_shape_type)24);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_heightfield_agent::create_heightfield_b_agent;
  backward.m_penetrations_fn = phys_heightfield_agent::static_penetrations;
  backward.m_closest_points_fn = phys_heightfield_agent::static_closest_points;
  backward.m_linear_cast_func = phys_heightfield_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, (phys_shape_type)24, (phys_shape_type)21);
}

void phys_heightfield_ray_forwarder::add_ray_hit(const phys_collision_proxy &hit_proxy, const phys_ray_collector_output &output)
{
    phys_heightfield_ray_forwarder *self = this;
    const phys_collision_proxy *body = &hit_proxy;
    const phys_ray_collector_output *hit = &output;
    phys_hit_point free_contact_point(*(const phys_collision_proxy *)self->agent_body_a, *body);
    phys_simd_scalar hit_frac(hit->m_hit_frac);
    free_contact_point.m_hit_contact.m_position.m_simd_quad = __vmaddfp(self->dir.m_simd_quad, hit_frac.m_simd_scalar, self->from.m_simd_quad);
    const phys_mat3 &rotation = body->get_transform().m_rotation;
    __vector4 local_normal = hit->m_normal.m_simd_quad;
    free_contact_point.m_hit_contact.m_separating_normal.m_simd_quad = __vmaddfp(rotation.m_column2.m_simd_quad, __vspltw(local_normal, 2),
        __vmaddfp(rotation.m_column1.m_simd_quad, __vspltw(local_normal, 1), __vmulfp(rotation.m_column0.m_simd_quad, __vspltw(local_normal, 0))));
    float hf_normal_scale = -self->neg_scale;
    phys_simd_scalar normal_offset(hf_normal_scale);
    free_contact_point.m_hit_contact.m_position.m_simd_quad =
        __vmaddfp(free_contact_point.m_hit_contact.m_separating_normal.m_simd_quad, normal_offset.m_simd_scalar,
        free_contact_point.m_hit_contact.m_position.m_simd_quad);
    free_contact_point.m_hit_contact.m_separating_normal(3) = hit->m_hit_frac;
    self->ray_collector->add_hit(free_contact_point);
    float early_out_frac = self->m_early_out_hit_frac, collector_early_out = self->ray_collector->get_early_out_distance();
    self->m_early_out_hit_frac = (collector_early_out - early_out_frac >= 0.0f) ? early_out_frac : collector_early_out;
}

phys_heightfield_agent::~phys_heightfield_agent()
{
}

template <>
void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_heightfield_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

// @address 0x6320c8 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_heightfield_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x632118 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_heightfield_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x6322b0 ?sweep_cast@?$phys_symmetric_agent@Vphys_heightfield_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x6323e0 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_heightfield_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z

template <>
void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_heightfield_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_heightfield_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_heightfield_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}

template <>
void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float hit_toi = output.m_collision_toi.m_time;
    phys_heightfield_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point) {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (hit_toi != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}

template <>
void phys_symmetric_agent<phys_heightfield_agent>::sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_heightfield_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_heightfield_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}

template <>
void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_heightfield_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_heightfield_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

// @address 0x5bec98 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_heightfield_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_heightfield_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}

template <typename T>
static __forceinline T *phys_heightfield_alloc_stack(int alloc_count)
{
    int nbytes = alloc_count * (int)sizeof(T);
    phys_thread_memory &memory = phys_thread_memory::get_singleton_instance();
    return static_cast<T *>(memory.allocate_stack(nbytes));
}
static __forceinline void phys_transform_spheres(const phys_transform &t, const phys_vec4 *vecs_in, int rotate_vec_count, phys_vec4 *vecs_out)
{
    phys_vec4 c0, c1, c2, tr;
    c0.m_simd_quad = t.m_rotation.m_column0.m_simd_quad;
    c1.m_simd_quad = t.m_rotation.m_column1.m_simd_quad;
    c2.m_simd_quad = t.m_rotation.m_column2.m_simd_quad;
    tr.m_simd_quad = t.m_translation.m_simd_quad;
    int i = rotate_vec_count - 1;
    do {
        phys_vec4 v;
        v.m_simd_quad = __vaddfp(__vmaddfp(c2.m_simd_quad, __vspltw(vecs_in[i].m_simd_quad, 2),
                                      __vmaddfp(c1.m_simd_quad, __vspltw(vecs_in[i].m_simd_quad, 1),
                                                __vmulfp(c0.m_simd_quad, __vspltw(vecs_in[i].m_simd_quad, 0)))),
                            tr.m_simd_quad);
        v.m_simd_quad = __vrlimi(v.m_simd_quad, vecs_in[i].m_simd_quad, 1, 0);
        vecs_out[i] = v;
    } while (--i >= 0);
}
static __forceinline void phys_set_transformed_pos(phys_vec4 &out, const phys_transform &t, const phys_vec4 &v)
{
    out.m_simd_quad = __vaddfp(__vmaddfp(t.m_rotation.m_column2.m_simd_quad, __vspltw(v.m_simd_quad, 2),
                                    __vmaddfp(t.m_rotation.m_column1.m_simd_quad, __vspltw(v.m_simd_quad, 1),
                                              __vmulfp(t.m_rotation.m_column0.m_simd_quad, __vspltw(v.m_simd_quad, 0)))),
                          t.m_translation.m_simd_quad);
}
static __forceinline void phys_set_rotated_dir(phys_vec4 &out, const phys_mat3 &r, const phys_vec4 &v)
{
    out.m_simd_quad = __vmaddfp(r.m_column2.m_simd_quad, __vspltw(v.m_simd_quad, 2),
                           __vmaddfp(r.m_column1.m_simd_quad, __vspltw(v.m_simd_quad, 1),
                                     __vmulfp(r.m_column0.m_simd_quad, __vspltw(v.m_simd_quad, 0))));
}

// @address 0x631c88 ?process_collision@phys_heightfield_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_heightfield_agent::process_collision(
    const phys_collision_proxy &heightfield_proxy, const phys_collision_proxy &field_proxy, const phys_process_collision_input &input,
        phys_process_collision_output &process_output)
{
    const phys_sphere_rep_shape *sphere_rep_shape = (const phys_sphere_rep_shape *)heightfield_proxy.get_shape();
    const phys_heightfield_shape_base *field_shape = (const phys_heightfield_shape_base *)field_proxy.get_shape();

    phys_transform b_from_a;
    b_from_a.set_mul_inverse_mul(field_proxy.get_transform(), heightfield_proxy.get_transform());

    const int child_sphere_count = m_contact_point_ids.getSize();

    phys_sphere *sphere_buffer = phys_alloc_stack<phys_sphere>(child_sphere_count);

    unsigned short *heightfield_contact_id = m_contact_point_ids.begin();

    {
        const phys_sphere *spheres = sphere_rep_shape->get_collision_spheres(&sphere_buffer[0]);
        phys_transform_spheres(b_from_a, &spheres[0].get_contact_position(), child_sphere_count, (phys_vec4 *)&sphere_buffer[0].get_contact_position());
    }

    phys_vec4 *second_out = phys_heightfield_alloc_stack<phys_vec4>(child_sphere_count);
    {
        phys_heightfield_shape_base::collide_spheres_input sphere_input;
        sphere_input.m_sphere_array = &sphere_buffer[0];
        sphere_input.m_sphere_count = child_sphere_count;
        sphere_input.m_box_tolerance = input.get_tolerance();
        field_shape->collide_spheres(sphere_input, &second_out[0]);
    }

    {
        phys_vec4 *out_cursor = &second_out[0];
        phys_sphere *sphere_cursor = &sphere_buffer[0];
        float tolerance = input.get_tolerance();
        for (int i = child_sphere_count - 1; i >= 0; i--) {
            if (out_cursor[0](3) > tolerance) {
                if (heightfield_contact_id[0] != 0xffff) {
                    m_contact_manager->contact_remove_point(heightfield_contact_id[0], (*process_output.m_constraint_owner));
                    heightfield_contact_id[0] = 0xffff;
                }
            } else {
                phys_process_hit_point &point = *process_output.reserve_contact_points(1);
                phys_simd_scalar neg_radius(-sphere_cursor->get_convex_radius());
                phys_vec4 position;
                position.m_simd_quad = __vmaddfp(out_cursor[0].m_simd_quad, neg_radius.m_simd_scalar, sphere_cursor->get_contact_position().m_simd_quad);
                phys_set_transformed_pos(point.m_hit_contact.m_position, field_proxy.get_transform(), position);
                phys_set_rotated_dir(point.m_hit_contact.m_separating_normal, field_proxy.get_transform().m_rotation, out_cursor[0]);
                point.m_hit_contact.m_separating_normal(3) = out_cursor[0](3);
                if (heightfield_contact_id[0] == 0xffff) {
                    heightfield_contact_id[0] = m_contact_manager->add_manifold_contact_point(heightfield_proxy, field_proxy, input,
                        process_output, 0, point.m_hit_contact);
                    if (heightfield_contact_id[0] == 0xffff) {
                        process_output.abort_contact_points(1);
                    } else {
                        process_output.commit_contact_points(1);
                    }
                } else {
                    process_output.commit_contact_points(1);
                }
                point.m_process_contact_id = heightfield_contact_id[0];
            }
            sphere_cursor++;
            out_cursor++;
            heightfield_contact_id++;
        }
    }
    phys_free_stack(second_out);
    phys_free_stack(sphere_buffer);
}

// @address 0x631610 ?static_sweep_cast@phys_heightfield_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_heightfield_agent::static_sweep_cast(const phys_collision_proxy &heightfield_proxy, const phys_collision_proxy &field_proxy,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    if (start_collector)
    {
        static_closest_points(heightfield_proxy, field_proxy, input, *start_collector);
    }

    const phys_sphere_rep_shape *sphere_rep_shape = (const phys_sphere_rep_shape *)heightfield_proxy.get_shape();
    const phys_heightfield_shape_base *field_shape = (const phys_heightfield_shape_base *)field_proxy.get_shape();

    phys_transform b_from_a;
    b_from_a.set_mul_inverse_mul(field_proxy.get_transform(), heightfield_proxy.get_transform());

    const int child_sphere_count = sphere_rep_shape->get_collision_sphere_count();
    phys_sphere *sphere_buffer = phys_alloc_stack<phys_sphere>(child_sphere_count);

    const phys_vec4 &path = input.m_path;
    phys_vec4 hf_local_path;
    const phys_mat3 &field_rot = field_proxy.get_transform().m_rotation;
    hf_local_path.m_simd_quad = __vmrghw(__vmrghw(__vmsum3fp(field_rot.m_column0.m_simd_quad, path.m_simd_quad),
        __vmsum3fp(field_rot.m_column2.m_simd_quad, path.m_simd_quad)),
                                __vmrghw(__vmsum3fp(field_rot.m_column1.m_simd_quad, path.m_simd_quad), phys_simd_vec_0000));

    const phys_sphere *spheres = sphere_rep_shape->get_collision_spheres(&sphere_buffer[0]);

    {
        phys_heightfield_shape_base::phys_sphere_cast_input ray;
        ray.m_max_extra_penetration = input.m_max_extra_penetration;

        phys_heightfield_ray_forwarder ray_forwarder(heightfield_proxy, path, collector);

        for (int i = 0; i < child_sphere_count; i++)
        {
            phys_set_transformed_pos(ray.m_ray_from, b_from_a, spheres->get_contact_position());
            ray.m_radius = spheres->get_convex_radius();
            ray.m_ray_to.set_add4(ray.m_ray_from, hf_local_path);

            phys_set_transformed_pos(ray_forwarder.from, heightfield_proxy.get_transform(), spheres->get_contact_position());
            ray_forwarder.neg_scale = spheres->get_convex_radius();

            field_shape->trace_sphere(ray, field_proxy, ray_forwarder);
            spheres++;
        }
    }

    phys_free_stack(sphere_buffer);
}

// @address 0x630fa8 ?static_penetrations@phys_heightfield_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_heightfield_agent::static_penetrations(const phys_collision_proxy &heightfield_proxy,
    const phys_collision_proxy &field_proxy, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_sphere_rep_shape *sphere_rep_shape = (const phys_sphere_rep_shape *)heightfield_proxy.get_shape();
    const phys_heightfield_shape_base *field_shape = (const phys_heightfield_shape_base *)field_proxy.get_shape();

    phys_transform b_from_a;
    b_from_a.set_mul_inverse_mul(field_proxy.get_transform(), heightfield_proxy.get_transform());

    const int child_sphere_count = sphere_rep_shape->get_collision_sphere_count();
    phys_sphere *sphere_buffer = phys_alloc_stack<phys_sphere>(child_sphere_count);

    {
        const phys_sphere *spheres = sphere_rep_shape->get_collision_spheres(&sphere_buffer[0]);
        phys_transform_spheres(b_from_a, &spheres[0].get_contact_position(), child_sphere_count, (phys_vec4 *)&sphere_buffer[0]);
    }

    phys_vec4 *second_out = phys_heightfield_alloc_stack<phys_vec4>(child_sphere_count);
    {
        phys_heightfield_shape_base::collide_spheres_input sphere_input;
        sphere_input.m_sphere_array = &sphere_buffer[0];
        sphere_input.m_sphere_count = child_sphere_count;
        sphere_input.m_box_tolerance = input.m_box_tolerance;
        field_shape->collide_spheres(sphere_input, &second_out[0]);
    }

    {
        phys_vec4 *out_cursor = &second_out[0];
        for (int i = child_sphere_count - 1; i >= 0; i--) {
            if (out_cursor[0](3) < 0.0f) {
                collector.add_proxy_pair(heightfield_proxy, field_proxy);
                break;
            }
            out_cursor++;
        }
    }
    phys_free_stack(second_out);
    phys_free_stack(sphere_buffer);
}

// @address 0x631280 ?static_closest_points@phys_heightfield_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_heightfield_agent::static_closest_points(const phys_collision_proxy &heightfield_proxy,
    const phys_collision_proxy &field_proxy, const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_sphere_rep_shape *sphere_rep_shape = (const phys_sphere_rep_shape *)heightfield_proxy.get_shape();
    const phys_heightfield_shape_base *field_shape = (const phys_heightfield_shape_base *)field_proxy.get_shape();

    phys_transform b_from_a;
    b_from_a.set_mul_inverse_mul(field_proxy.get_transform(), heightfield_proxy.get_transform());

    const int child_sphere_count = sphere_rep_shape->get_collision_sphere_count();
    phys_sphere *sphere_buffer = phys_alloc_stack<phys_sphere>(child_sphere_count);

    {
        const phys_sphere *spheres = sphere_rep_shape->get_collision_spheres(&sphere_buffer[0]);
        phys_transform_spheres(b_from_a, &spheres[0].get_contact_position(), child_sphere_count, (phys_vec4 *)&sphere_buffer[0]);
    }

    phys_vec4 *second_out = phys_heightfield_alloc_stack<phys_vec4>(child_sphere_count);
    {
        phys_heightfield_shape_base::collide_spheres_input sphere_input;
        sphere_input.m_sphere_array = &sphere_buffer[0];
        sphere_input.m_sphere_count = child_sphere_count;
        sphere_input.m_box_tolerance = input.m_box_tolerance;
        field_shape->collide_spheres(sphere_input, &second_out[0]);
    }

    {
        phys_vec4 *out_cursor = &second_out[0];
        phys_sphere *sphere_cursor = &sphere_buffer[0];
        float tolerance = input.m_box_tolerance;
        for (int i = child_sphere_count - 1; i >= 0; i--) {
            if (out_cursor[0](3) <= tolerance) {
                phys_hit_point point(heightfield_proxy, field_proxy);
                phys_simd_scalar scale(-sphere_cursor->get_convex_radius() - out_cursor[0](3));
                phys_vec4 position;
                position.m_simd_quad = __vmaddfp(out_cursor[0].m_simd_quad, scale.m_simd_scalar, sphere_cursor->get_contact_position().m_simd_quad);
                phys_set_transformed_pos(point.m_hit_contact.m_position, field_proxy.get_transform(), position);
                phys_set_rotated_dir(point.m_hit_contact.m_separating_normal, field_proxy.get_transform().m_rotation, out_cursor[0]);
                point.m_hit_contact.m_separating_normal(3) = out_cursor[0](3);
                collector.add_hit(point);
            }
            sphere_cursor++;
            out_cursor++;
        }
    }
    phys_free_stack(second_out);
    phys_free_stack(sphere_buffer);
}
