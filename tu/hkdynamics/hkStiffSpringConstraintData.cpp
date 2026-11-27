// hkdynamics/hkStiffSpringConstraintData.obj: from the records 0x64d538 0x64d550 0x64d560 0x64d590 0x64d5b8 0x64d5c0 0x64d5e0 0x64d5f0
// @flags /TP /O2 /Oc
// @address 0x64d538 ?destroy_loaded_stiff_spring_data@@YAXPAX@Z
// @address 0x64d550 ?get_constraint_info@phys_stiff_spring_data@@UBAXAAUconstraint_info@phys_constraint_data@@@Z
// @address 0x64d560 ?get_runtime_info@phys_stiff_spring_data@@UBAXVphys_bool@@AAUruntime_info@phys_constraint_data@@@Z
// @address 0x64d590 ?is_valid_aabb@phys_stiff_spring_data@@UBA?AVphys_bool@@XZ
// @address 0x64d5b8 ?get_type_id@phys_stiff_spring_data@@UBAHXZ
// @address 0x64d5c0 ?fixup_loaded_stiff_spring_data@@YAXPAX@Z
// @address 0x64d5e0 ?get_stiff_spring_data_vtable@@YAPBXXZ
// @address 0x64d5f0 ??0phys_stiff_spring_data@@QAA@XZ
#include "phys/phys_stiff_spring_data.h"
#include "phys/phys_base.h"

#include <vectorintrinsics.h>
#include "phys/phys_atoms.h"






void destroy_loaded_stiff_spring_data(void *object)
{
    static_cast<phys_stiff_spring_data *>(object)->~phys_stiff_spring_data();
}

void phys_stiff_spring_data::get_constraint_info(phys_constraint_data::constraint_info &info) const
{
    phys_constraint_data::gather_constraint_info(m_data_atoms.get_atoms(), m_data_atoms.get_size_of_all_atoms(), info);
}

void phys_stiff_spring_data::get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const
{
    if (want_runtime) {
        info.m_solver_result_count = 1;
        info.m_external_runtime_size = 8;
    } else {
        info.m_external_runtime_size = 0;
        info.m_solver_result_count = 0;
    }
}

phys_bool phys_stiff_spring_data::is_valid_aabb() const
{
    return m_data_atoms.m_spring_atom.m_length > 0.0f;
}

int phys_stiff_spring_data::get_type_id() const
{
    return 8;
}

void fixup_loaded_stiff_spring_data(void *memory)
{
    phys_fixup_tag flag;
    new (memory) phys_stiff_spring_data(flag);
}

const void *get_stiff_spring_data_vtable(void)
{
    __declspec(align(16)) char storage[sizeof(phys_stiff_spring_data)];
    phys_fixup_tag flag;
    phys_stiff_spring_data *object = new (storage) phys_stiff_spring_data(flag);
    return *reinterpret_cast<const void *const *>(object);
}

phys_stiff_spring_data::phys_stiff_spring_data()
{
    m_data_atoms.m_pivots_atom.m_translation_a.set_zero4();
    m_data_atoms.m_pivots_atom.m_translation_b.set_zero4();
    m_data_atoms.m_spring_atom.m_length = 0.0f;
}

