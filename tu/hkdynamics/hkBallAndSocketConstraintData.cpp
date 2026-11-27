// hkdynamics/hkBallAndSocketConstraintData.obj: from the records 0x64d6b0 0x64d6c8 0x64d708 0x64d728 0x64d738 0x64d768 0x64d778 0x64d780 0x64d7a0 0x64d7b0
// @flags /TP /O2
// @address 0x64d6b0 ?destroy_loaded_ball_socket_data@@YAXPAX@Z
// @address 0x64d6c8 ?set_in_world_space@phys_ball_socket_data@@QAAXABVphys_transform@@0ABVphys_vec4@@@Z
// @address 0x64d708 ?set_in_body_space@phys_ball_socket_data@@QAAXABVphys_vec4@@0@Z
// @address 0x64d728 ?get_constraint_info@phys_ball_socket_data@@UBAXAAUconstraint_info@phys_constraint_data@@@Z
// @address 0x64d738 ?get_runtime_info@phys_ball_socket_data@@UBAXVphys_bool@@AAUruntime_info@phys_constraint_data@@@Z
// @address 0x64d768 ?is_valid_aabb@phys_ball_socket_data@@UBA?AVphys_bool@@XZ
// @address 0x64d778 ?get_type_id@phys_ball_socket_data@@UBAHXZ
// @address 0x64d780 ?fixup_loaded_ball_socket_data@@YAXPAX@Z
// @address 0x64d7a0 ?get_ball_and_socket_data_vtable@@YAPBXXZ
// @address 0x64d7b0 ??0phys_ball_socket_data@@QAA@XZ
#include "phys/phys_base.h"
#include <vectorintrinsics.h>
#include "phys/phys_atoms.h"

#include "phys/phys_ball_socket_data.h"
#include "phys/phys_math.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_thread_memory.h"

void destroy_loaded_ball_socket_data(void *storage)
{
    static_cast<phys_ball_socket_data *>(storage)->~phys_ball_socket_data();
}

void phys_ball_socket_data::set_in_world_space(const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot)
{
    m_data_atoms.m_pivots_atom.m_translation_a.vec_set_transformed_inverse_pos(body_a_transform, pivot);
    m_data_atoms.m_pivots_atom.m_translation_b.vec_set_transformed_inverse_pos(body_b_transform, pivot);
}

void phys_ball_socket_data::set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b)
{
    m_data_atoms.m_pivots_atom.m_translation_a = joint_pivot_a;
    m_data_atoms.m_pivots_atom.m_translation_b = joint_pivot_b;
}

void phys_ball_socket_data::get_constraint_info(phys_constraint_data::constraint_info &info) const
{
    phys_constraint_data::gather_constraint_info(m_data_atoms.get_atoms(), m_data_atoms.get_size_of_all_atoms(), info);
}

void phys_ball_socket_data::get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const
{
    if (want_runtime) {
        info.m_solver_result_count = 3;
        info.m_external_runtime_size = 24;
    } else {
        info.m_solver_result_count = 0;
        info.m_external_runtime_size = 0;
    }
}

phys_bool phys_ball_socket_data::is_valid_aabb() const
{
    return true;
}

int phys_ball_socket_data::get_type_id() const
{
    return 0;
}

void fixup_loaded_ball_socket_data(void *storage)
{
    phys_fixup_tag flag;
    new (storage) phys_ball_socket_data(flag);
}

const void *get_ball_and_socket_data_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_ball_socket_data)];
    phys_fixup_tag flag;
    phys_ball_socket_data *object = new (storage) phys_ball_socket_data(flag);
    return *reinterpret_cast<const void *const *>(object);
}

phys_ball_socket_data::phys_ball_socket_data()
{
    m_data_atoms.m_pivots_atom.m_translation_a.set_zero4();
    m_data_atoms.m_pivots_atom.m_translation_b.set_zero4();
}

