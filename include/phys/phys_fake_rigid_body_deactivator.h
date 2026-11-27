#ifndef INCLUDE_PHYS_FAKE_RIGID_BODY_DEACTIVATOR_H
#define INCLUDE_PHYS_FAKE_RIGID_BODY_DEACTIVATOR_H

// hkFakeRigidBodyDeactivator, defined in tu/hkdynamics/hkFakeRigidBodyDeactivator.cpp.

#include "phys/phys_dynamics.h"

class phys_fake_rigid_body_deactivator : public phys_rigid_body_deactivator {
public:
    static __forceinline void *operator new(size_t, void *p) { return p; }
    static __forceinline phys_fake_rigid_body_deactivator *get_fake_body_deactivator() { return &m_fake_body_deactivator; }
    phys_fake_rigid_body_deactivator() {}
    phys_fake_rigid_body_deactivator(phys_fixup_tag flag) : phys_rigid_body_deactivator(flag) {}
    virtual phys_bool should_sleep_high_freq(const phys_entity *entity);
    virtual phys_bool should_sleep_low_freq(const phys_entity *entity);
    virtual sleep_detector_kind get_body_deactivator_type() const;
protected:
    static phys_fake_rigid_body_deactivator m_fake_body_deactivator;
};

#endif // HAVOK_HK_FAKE_RIGID_BODY_DEACTIVATOR_H
