#ifndef PHYSICS_HAVOK_COMPONENT_GLOBALS_H
#define PHYSICS_HAVOK_COMPONENT_GLOBALS_H

#include "math/matrix_math_types.h"
#include "multithreading/registered_thread_index.h"
#include <vectorintrinsics.h>

// Listener and packing constants used by Havok components.
class c_entity_listener;

extern "C" {
extern c_entity_listener *g_global_entity_listener;
extern __vector4 k_havok_component_vector_sign_mask;
}

extern const float k_havok_pack_minimum;
extern const float k_havok_pack_maximum;
extern const float k_havok_phantom_byte_rounding_bias;

#endif // PHYSICS_HAVOK_COMPONENT_GLOBALS_H
