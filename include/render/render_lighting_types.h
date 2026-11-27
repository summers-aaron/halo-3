#ifndef RENDER_RENDER_LIGHTING_TYPES_H
#define RENDER_RENDER_LIGHTING_TYPES_H
#include <stddef.h>
#include "geometry/geometry_sampling_types.h"
#include "cubemaps/cubemap_sample.h"

// Shared layout used by cached lighting, interpolation and shader externs.
struct render_lighting
{
    real_vector3d down;
    s_geometry_sample sample;
    c_dynamic_cubemap_sample cubemap;
    long unknown;
};
typedef char render_lighting_size[(sizeof(render_lighting) == 0x218) ? 1 : -1];
typedef char render_lighting_sample_offset[(offsetof(render_lighting, sample) == 0xc) ? 1 : -1];
typedef char render_lighting_cubemap_offset[(offsetof(render_lighting, cubemap) == 0x204) ? 1 : -1];
#endif
