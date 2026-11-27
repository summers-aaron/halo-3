// hkdynamics/hkPhantomBroadPhaseListener.obj: from the records 0x655780 0x655838
// @compiler xdk3424
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_dynamics.h"
// @address 0x655780 ?add_collision_pair@phys_volume_broadphase_listener@@UAAXAAVphys_typed_broadphase_pair@@@Z
// @address 0x655838 ?remove_collision_pair@phys_volume_broadphase_listener@@UAAXAAVphys_typed_broadphase_pair@@@Z

#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_volume_broadphase_listener.h"

void phys_volume_broadphase_listener::add_collision_pair(phys_typed_broadphase_pair &pair)
{
    if (pair.get_element_a()->get_type_id() == 2) {
        phys_collider *pair_collider_a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *pair_collider_b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        ((phys_volume *)pair_collider_a->get_collider_owner())->insert_overlapping_collider(pair_collider_b);
    }
    if (pair.get_element_b()->get_type_id() == 2) {
        phys_collider *pair_collider_a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *pair_collider_b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        ((phys_volume *)pair_collider_b->get_collider_owner())->insert_overlapping_collider(pair_collider_a);
    }
}

void phys_volume_broadphase_listener::remove_collision_pair(phys_typed_broadphase_pair &pair)
{
    if (pair.get_element_a()->get_type_id() == 2) {
        phys_collider *pair_collider_a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *pair_collider_b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        ((phys_volume *)pair_collider_a->get_collider_owner())->erase_overlapping_collider(pair_collider_b);
    }
    if (pair.get_element_b()->get_type_id() == 2) {
        phys_collider *pair_collider_a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *pair_collider_b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        ((phys_volume *)pair_collider_b->get_collider_owner())->erase_overlapping_collider(pair_collider_a);
    }
}
