#ifndef SCENARIO_SCENARIO_LIGHTMAP_TYPES_H
#define SCENARIO_SCENARIO_LIGHTMAP_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "geometry/render_geometry_types.h"

struct s_lightmap_luminance_scale
{
    union
    {
        // geometry_sampling.cpp decodes each packed coefficient using two RGB scales.
        real_rgb_color packed_coefficient_scale[2];
        // 8-bit coefficient bitmaps use only the first channel as a single scale.
        float scale;
    };
};

// One lightmap entry per BSP, holding its identifier, lighting data, and render geometry.
// March scenario_lightmap_definitions.obj also uses a 436-byte element stride.
struct s_scenario_lightmap_bsp_data
{
    unsigned short flags;
    short identifier;
    long structure_checksum;
    s_lightmap_luminance_scale coefficients_map_scale[9];
    s_tag_reference coefficients_bitmap;
    s_tag_reference dominant_light_bitmap;
    s_tag_block static_per_vertex_lighting;
    s_tag_block cluster_static_per_vertex_lighting;
    s_tag_block instanced_geometry_lighting;
    s_tag_block instanced_geometry_light_probes;
    s_render_geometry render_geometry;
};

struct s_scenario_lightmap_definition
{
    unsigned long job_guid;
    s_tag_block lightmaps;
    s_tag_block per_pixel_lightmap_data_references;
    s_tag_block per_vertex_lightmap_data_references;
    s_tag_block airprobes;
    s_tag_block scenery_light_probes;
    s_tag_block machine_light_probes;
    s_tag_block deprecated_lightmap_bsp_data;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char lightmap_packed_coefficient_scale_offset[(offsetof(s_lightmap_luminance_scale, packed_coefficient_scale) == 0) ? 1 : -1];

typedef char lightmap_luminance_scale_size[(sizeof(s_lightmap_luminance_scale) == 0x18) ? 1 : -1];
typedef char scenario_lightmap_bsp_size[(sizeof(s_scenario_lightmap_bsp_data) == 0x1b4) ? 1 : -1];
typedef char scenario_lightmap_bsp_identifier_offset[(offsetof(s_scenario_lightmap_bsp_data, identifier) == 2) ? 1 : -1];
typedef char scenario_lightmap_geometry_offset[(offsetof(s_scenario_lightmap_bsp_data, render_geometry) == 0x130) ? 1 : -1];
typedef char scenario_lightmap_definition_size[(sizeof(s_scenario_lightmap_definition) == 0x58) ? 1 : -1];
#endif

#endif // SCENARIO_SCENARIO_LIGHTMAP_TYPES_H
