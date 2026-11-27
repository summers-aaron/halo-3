// hkcollide/hkWeldingUtility.obj: from the records 0x705640
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x705640 ?init_edge_weld_table@phys_edge_weld_util@@SAXMM@Z
#include <math.h>
#include <ppcintrinsics.h>

#include "phys/phys_base.h"

#define PHYS_REAL_PI 3.14159265358979f

#include "phys/phys_edge_weld_util.h"

// @address 0x7ce7d8 ?m_sin_cos_table@phys_edge_weld_util@@2PAUtrig_table_entry@1@A
phys_edge_weld_util::trig_table_entry phys_edge_weld_util::m_sin_cos_table[phys_edge_weld_util::EDGE_WELD_ANGLE_COUNT + 1];

void phys_edge_weld_util::init_edge_weld_table(phys_real edge_normal_snap_angle, phys_real tri_normal_snap_angle)
{
    for (int i = 0; i < EDGE_WELD_ANGLE_COUNT + 1; i++)
    {
        phys_real bucket_edge_angle = -PHYS_REAL_PI + ((2 * PHYS_REAL_PI / phys_real(EDGE_WELD_ANGLE_COUNT)) * phys_real(i));

        if (bucket_edge_angle < 0)
        {
            phys_real flipped_edge_angle = PHYS_REAL_PI - phys_math::fabs(bucket_edge_angle);
            phys_real edge_snap_angle_limit = flipped_edge_angle - edge_normal_snap_angle;

            {
                m_sin_cos_table[i].m_cos_accept0 = 1.0f;
                m_sin_cos_table[i].m_sin_accept_0 = 0.0f;
            }

            if (edge_snap_angle_limit > tri_normal_snap_angle)
            {
                m_sin_cos_table[i].m_cos_snap0 = phys_math::cos(tri_normal_snap_angle);
            }
            else
            {
                phys_real clamped_snap_angle = phys_math::max_of_two(edge_snap_angle_limit, phys_real(0.0f));
                m_sin_cos_table[i].m_cos_snap0 = phys_math::cos(clamped_snap_angle);
            }

            {
                phys_real clamped_snap_angle = phys_math::max_of_two(edge_snap_angle_limit, phys_real(0.0f));
                m_sin_cos_table[i].m_cos_snap1 = phys_math::cos(clamped_snap_angle);
            }

            {
                m_sin_cos_table[i].m_cos_accept1 = phys_math::cos(flipped_edge_angle);
                m_sin_cos_table[i].m_sin_accept_1 = phys_math::sin(flipped_edge_angle);
            }
        }
        else
        {
            {
                m_sin_cos_table[i].m_cos_accept0 = phys_math::cos(bucket_edge_angle);
                m_sin_cos_table[i].m_sin_accept_0 = phys_math::sin(bucket_edge_angle);
            }

            {
                phys_real clamped_snap_angle = phys_math::min_of_two((bucket_edge_angle + edge_normal_snap_angle), PHYS_REAL_PI);
                m_sin_cos_table[i].m_cos_snap0 = phys_math::cos(clamped_snap_angle);
            }

            {
                phys_real edge_snap_angle_limit = bucket_edge_angle + edge_normal_snap_angle;
                phys_real tri_snap_angle_limit = PHYS_REAL_PI - tri_normal_snap_angle;
                if (edge_snap_angle_limit < tri_snap_angle_limit)
                {
                    m_sin_cos_table[i].m_cos_snap1 = phys_math::cos(tri_snap_angle_limit);
                }
                else
                {
                    phys_real clamped_snap_angle = phys_math::min_of_two(edge_snap_angle_limit, PHYS_REAL_PI);
                    m_sin_cos_table[i].m_cos_snap1 = phys_math::cos(clamped_snap_angle);
                }
            }

            {
                m_sin_cos_table[i].m_cos_accept1 = -1.0f;
                m_sin_cos_table[i].m_sin_accept_1 = 0.0f;
            }
        }
    }
}
