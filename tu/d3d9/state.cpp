// d3d9/state.obj: from the records 0x16feb8
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "rasterizer/rasterizer.h"
#include "math/real_math_types.h"
#include "geometry/compression_info_types.h"
#include "rasterizer/xenon/rasterizer_shader_bounds.h"

typedef struct s_d3d9_state_constants { real_vector4d scale; real_vector4d offset; real_vector4d rect; } s_d3d9_state_constants;
// @address 0x16feb8 d3d9_set_vertex_shader_bounds
extern "C" void d3d9_set_vertex_shader_bounds(const s_compression_info *bounds)
{
    s_d3d9_state_constants constants;
    constants.offset.w = 0.0f;
    constants.offset.x = bounds->x.lower * 2.0f;
    constants.offset.y = bounds->y.lower * 2.0f;
    constants.offset.z = bounds->z.lower * 2.0f;
    constants.rect.x = bounds->u.upper - bounds->u.lower;
    constants.rect.y = bounds->v.upper - bounds->v.lower;
    constants.scale.x = (bounds->x.upper - bounds->x.lower) * 2.0f;
    constants.rect.z = bounds->u.lower;
    constants.rect.w = bounds->v.lower;
    constants.scale.y = (bounds->y.upper - bounds->y.lower) * 2.0f;
    constants.scale.z = (bounds->z.upper - bounds->z.lower) * 2.0f;
    constants.scale.w = 1.0f;
    c_rasterizer::get_device()->SetVertexShaderConstantF(12, (const float *)&constants, 3);
}
