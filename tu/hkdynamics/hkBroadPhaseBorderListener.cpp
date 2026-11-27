// hkdynamics/hkBroadPhaseBorderListener.obj: from the records 0x654058 0x654130
// @flags /TP /O2 /Oc
// @compiler xdk3424
// @address 0x654058 ?add_collision_pair@phys_broadphase_border_listener@@UAAXAAVphys_typed_broadphase_pair@@@Z
// @address 0x654130 ?remove_collision_pair@phys_broadphase_border_listener@@UAAXAAVphys_typed_broadphase_pair@@@Z
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_dynamics.h"

#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_broadphase_border_listener.h"

void phys_broadphase_border_listener::add_collision_pair(phys_typed_broadphase_pair &pair)
{
    signed char shape_type_a = pair.get_element_a()->get_type_id();
    if (shape_type_a == 3 && pair.get_element_b()->get_type_id() == 3)
        return;
    if (shape_type_a == 3) {
        phys_collider *pair_collider_a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *pair_collider_b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        ((phys_volume *)pair_collider_a->get_collider_owner())->insert_overlapping_collider(pair_collider_b);
    }
    if (pair.get_element_b()->get_type_id() == 3) {
        phys_collider *pair_collider_a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *pair_collider_b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        ((phys_volume *)pair_collider_b->get_collider_owner())->insert_overlapping_collider(pair_collider_a);
    }
}

void phys_broadphase_border_listener::remove_collision_pair(phys_typed_broadphase_pair &pair)
{
    signed char shape_type_a = pair.get_element_a()->get_type_id();
    if (shape_type_a == 3 && pair.get_element_b()->get_type_id() == 3)
        return;
    if (shape_type_a == 3) {
        phys_collider *pair_collider_a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *pair_collider_b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        ((phys_volume *)pair_collider_a->get_collider_owner())->erase_overlapping_collider(pair_collider_b);
    }
    if (pair.get_element_b()->get_type_id() == 3) {
        phys_collider *pair_collider_a = (phys_collider *)pair.get_element_a()->get_collider_owner();
        phys_collider *pair_collider_b = (phys_collider *)pair.get_element_b()->get_collider_owner();
        ((phys_volume *)pair_collider_b->get_collider_owner())->erase_overlapping_collider(pair_collider_a);
    }
}
