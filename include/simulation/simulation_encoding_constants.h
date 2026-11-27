#ifndef SIMULATION_SIMULATION_ENCODING_CONSTANTS_H
#define SIMULATION_SIMULATION_ENCODING_CONSTANTS_H

// Shared constant pools for simulation encoding.
extern const volatile float k_simulation_object_encoding_constant_pool[];
extern const float k_simulation_object_zero_constant_pool[];
extern "C" {
extern const float k_simulation_encoding_signed_unit_pool[];
extern const float k_simulation_encoding_negative_half_pi;
}

#endif
