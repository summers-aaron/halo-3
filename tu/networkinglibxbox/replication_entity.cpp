// networkinglibxbox/replication_entity.obj: from the records 0x3adff0
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
// @address 0x3adff0 simulation_read_quantized_node_space_position

#include "math/real_math.h"
#include "networking/replication/replication_entity.h"
#include "simulation/simulation_encoding.h"

// @address 0x270d0 k_simulation_node_space_bounds
// @rdata 0x820270d0..0x82027100
const real_bounds k_simulation_node_space_bounds[2][3] =
{
    // _simulation_node_space_encoding_type_world
    { { -3.0f, 3.0f }, { -3.0f, 3.0f }, { -3.0f, 3.0f } },
    // _simulation_node_space_encoding_type_object
    { { -0.7f, 0.7f }, { -0.7f, 0.7f }, { -0.7f, 0.7f } },
};
// @rdata-end

extern "C" void simulation_read_quantized_node_space_position(c_bitstream *packet, real_point3d *position, long bounds_index, long bits_per_component)
{
    long_point3d quantized_point;
    for (long component = 0; component < 3; ++component)
        quantized_point.n[component] = packet->read_integer("node-point-quantization", bits_per_component);
    dequantize_real_point3d(&quantized_point, (const real_rectangle3d *)k_simulation_node_space_bounds[bounds_index], bits_per_component, position);
}
