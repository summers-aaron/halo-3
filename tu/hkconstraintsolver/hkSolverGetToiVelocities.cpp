// hkconstraintsolver/hkSolverGetToiVelocities.obj: from the records 0x7070b0
// @compiler xdk4314
// @flags /TP /O2 /Oc /QVMXReserve
// @address 0x7070b0 phys_solve_toi_violating_vel

#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_schema.h"
#include "phys/phys_base.h"

typedef float phys_real;
typedef unsigned char phys_u8;
typedef unsigned short phys_u16;
typedef signed char phys_i8;
#include "phys/phys_vel_accum.h"
#include "phys/phys_jacobian_element.h"

#include "phys/phys_header_schema.h"
#include "phys/phys_solver_velocity.h"

namespace phys_solver
{
    static __forceinline phys_real compute_toi_vel(const phys_vel_accum &a, const phys_vel_accum &b, const phys_1lin2ang_jacobian *lin_ang_jac)
    {
        phys_vec4 ang_sq0; ang_sq0.set_mul4(a.m_ang_vel, lin_ang_jac->m_ang_jacobian[0]);
        phys_vec4 lin_part; lin_part.set_sub4(a.m_lin_vel, b.m_lin_vel);
        phys_vec4 ang_sq1; ang_sq1.set_mul4(b.m_ang_vel, lin_ang_jac->m_ang_jacobian[1]);
        lin_part.mul4(lin_ang_jac->m_lin_part_0);
        ang_sq0.add4(ang_sq1);
        ang_sq0.add4(lin_part);
        return ang_sq0.horizontal_add3();
    }
}


// @address 0x7070b0
extern "C" int phys_solve_toi_violating_vel(void *info, const phys_schema *schema_list,
    const phys_vel_accum *vel_accums, int max_num_velocities, phys_real *velocities_out)
{
    const phys_vel_accum *agent_body_a = 0;
    const phys_vel_accum *agent_body_b = 0;
    int velocity_count = 0;
    const phys_schema *schema_cursor = schema_list;

    {
        const phys_header_schema *h = static_cast<const phys_header_schema *>(schema_cursor);
        agent_body_a = h->get_body_a(vel_accums);
        agent_body_b = h->get_body_b(vel_accums);
        phys_solver::load_vel_accums(*agent_body_a, *agent_body_b);
        schema_cursor = reinterpret_cast<const phys_schema *>(h + 1);
    }

    while (1)
    {
        phys_schema::schema_kind type = phys_schema::get_type_id(schema_cursor);
        switch (type)
        {
        case phys_schema::SCHEMA_KIND_SHIFT_RESULTS_PTR:
        case 7:
        case 8:
        case 9:
        case 11:
        case 12:
        case 13:
        case 14:
        case 21:
        case 22:
            do
            {
                schema_cursor = phys_add_byte_offset_const(schema_cursor, phys_schema::getSize(schema_cursor));
            } while (phys_schema::get_type_id(schema_cursor) == type);
            break;

        case phys_schema::SCHEMA_KIND_GOTO:
            schema_cursor = phys_add_byte_offset_const(schema_cursor, static_cast<const phys_goto_schema *>(schema_cursor)->m_offset);
            break;

        case phys_schema::SCHEMA_KIND_END:
        case phys_schema::SCHEMA_KIND_HEADER:
            return velocity_count;

        case 19:
        case 20:
            schema_cursor = phys_add_byte_offset_const(schema_cursor, phys_schema::getSize(schema_cursor));
            return velocity_count;

        case 15:
        case 16:
            {
                if (velocity_count + 1 > max_num_velocities)
                {
                    return velocity_count;
                }
                velocity_count += 1;
                const phys_1lin2ang_jacobian *lin_ang_jac = reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor);
                velocities_out[0] = phys_math::min_of_two(0.0f, phys_solver::compute_toi_vel(*agent_body_a, *agent_body_b, lin_ang_jac));
                velocities_out++;
                schema_cursor = phys_add_byte_offset_const(schema_cursor, 48);
                break;
            }

        case 17:
        case 18:
            {
                if (velocity_count + 2 > max_num_velocities)
                {
                    return velocity_count;
                }
                velocity_count += 2;
                const phys_1lin2ang_jacobian *lin_ang_jac = reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor);
                velocities_out[0] = phys_math::min_of_two(0.0f, phys_solver::compute_toi_vel(*agent_body_a, *agent_body_b, lin_ang_jac));
                velocities_out++;
                velocities_out[0] = phys_math::min_of_two(0.0f, phys_solver::compute_toi_vel(*agent_body_a, *agent_body_b, lin_ang_jac + 1));
                velocities_out++;
                schema_cursor = phys_add_byte_offset_const(schema_cursor, 112);
                break;
            }

        case phys_schema::SCHEMA_KIND_1D_BILATERAL:
            {
                if (velocity_count + 1 > max_num_velocities)
                {
                    return velocity_count;
                }
                velocity_count += 1;
                const phys_1lin2ang_jacobian *lin_ang_jac = reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor);
                velocities_out[0] = phys_solver::compute_toi_vel(*agent_body_a, *agent_body_b, lin_ang_jac);
                velocities_out++;
                schema_cursor = phys_add_byte_offset_const(schema_cursor, 48);
                break;
            }

        case phys_schema::SCHEMA_KIND_1D_BILATERAL_IMPULSE_CAP:
            {
                if (velocity_count + 1 > max_num_velocities)
                {
                    return velocity_count;
                }
                velocity_count += 1;
                const phys_1lin2ang_jacobian *lin_ang_jac = reinterpret_cast<const phys_1lin2ang_jacobian *>(schema_cursor);
                velocities_out[0] = phys_solver::compute_toi_vel(*agent_body_a, *agent_body_b, lin_ang_jac);
                velocities_out++;
                schema_cursor = phys_add_byte_offset_const(schema_cursor, 64);
                break;
            }

        default:
            break;
        }
    }
    return -1;
}

