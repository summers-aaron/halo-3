#ifndef INCLUDE_PHYS_AXIS_SWEEP_CONSTANTS_H
#define INCLUDE_PHYS_AXIS_SWEEP_CONSTANTS_H

#include <vectorintrinsics.h>

extern "C" {
extern __vector4 k_phys_axis_sweep_negate_mask;
extern const __vector4 k_phys_axis_sweep_conversion_bias;
extern __declspec(align(16)) const unsigned int k_phys_axis_sweep_select_masks[3][4];
extern __declspec(align(16)) const unsigned int k_phys_axis_sweep_endpoint_mask[4];
extern __declspec(align(16)) const unsigned int k_phys_axis_sweep_component_indices[4];
}

#endif
