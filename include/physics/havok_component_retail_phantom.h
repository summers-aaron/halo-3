#ifndef PHYSICS_HAVOK_COMPONENT_RETAIL_PHANTOM_H
#define PHYSICS_HAVOK_COMPONENT_RETAIL_PHANTOM_H

#include "phys/phys_dynamics.h"

// havok_component.cpp still spells the phantom dispatch through this namespace;
// it names the canonical class (getPenetrations is vtable slot 14 in both).
namespace havok_component_retail {
typedef ::phys_shape_volume phys_shape_volume;
}

#endif
