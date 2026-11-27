// hkdynamics/hkFakeRigidBodyDeactivator.obj: from the records 0x65dab0 0x65dac8 0x65dad0 0x65dae0 0x65daf0 0x65db10 0x70b458
// @flags /TP /O2
#include "phys/phys_base.h"
#include <string.h>
// @address 0x65dab0 ?destroy_loaded_fake_body_deactivator@@YAXPAX@Z
// @address 0x65dac8 ?get_body_deactivator_type@phys_fake_rigid_body_deactivator@@UBA?AW4sleep_detector_kind@phys_rigid_body_deactivator@@XZ
// @address 0x65dad0 ?should_sleep_high_freq@phys_fake_rigid_body_deactivator@@UAA?AVphys_bool@@PBVphys_entity@@@Z
// @address 0x65dae0 ?should_sleep_low_freq@phys_fake_rigid_body_deactivator@@UAA?AVphys_bool@@PBVphys_entity@@@Z
// @address 0x65daf0 ?fixup_loaded_fake_deactivator@@YAXPAX@Z
// @address 0x65db10 ?get_fake_deactivator_vtable@@YAPBXXZ

#include "phys/phys_fake_rigid_body_deactivator.h"

phys_fake_rigid_body_deactivator phys_fake_rigid_body_deactivator::m_fake_body_deactivator;

void destroy_loaded_fake_body_deactivator(void *storage)
{
    static_cast<phys_fake_rigid_body_deactivator *>(storage)->~phys_fake_rigid_body_deactivator();
}

phys_rigid_body_deactivator::sleep_detector_kind phys_fake_rigid_body_deactivator::get_body_deactivator_type() const
{
    return sleep_policy_never;
}

phys_bool phys_fake_rigid_body_deactivator::should_sleep_high_freq(const phys_entity *entity)
{
    return phys_bool(false);
}

phys_bool phys_fake_rigid_body_deactivator::should_sleep_low_freq(const phys_entity *entity)
{
    return phys_bool(false);
}

void fixup_loaded_fake_deactivator(void *storage)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    if (storage) new (storage) phys_fake_rigid_body_deactivator(flag);
}

const void *get_fake_deactivator_vtable()
{
    __declspec(align(4)) char storage[sizeof(phys_fake_rigid_body_deactivator)];
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 0;
    new (storage) phys_fake_rigid_body_deactivator(flag);
    const void *result;
    memcpy(&result, storage, sizeof(result));
    return result;
}
