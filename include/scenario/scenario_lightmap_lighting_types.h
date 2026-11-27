#ifndef SCENARIO_SCENARIO_LIGHTMAP_LIGHTING_TYPES_H
#define SCENARIO_SCENARIO_LIGHTMAP_LIGHTING_TYPES_H
#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "objects/object_reference_types.h"

// Serialized lightmap records read by structure_renderer_setup_instance_lighting.
// Shorts after the last index pad each record to 4-byte alignment.
struct s_lighting_cluster { s_tag_block unused_vertex_buffer; union { int sample_index; long vertex_buffer_index; }; };
struct s_structure_lighting_cluster {
    short lightmap_index;
    union { short primary; short bucket_index; };
    union { short secondary; short lightprobe_index; };
    short padding_lightprobe_index; };
struct s_object_lighting_cluster {
    s_tag_block unused_vertex_buffer;
    union { short sample_index; short vertex_buffer_index; };
    short padding_vertex_buffer_index; };

typedef char lighting_cluster_size[(sizeof(s_lighting_cluster) == 16) ? 1 : -1];
typedef char structure_lighting_cluster_size[(sizeof(s_structure_lighting_cluster) == 8) ? 1 : -1];
typedef char object_lighting_cluster_size[(sizeof(s_object_lighting_cluster) == 16) ? 1 : -1];
typedef char lighting_cluster_samples_offset[(offsetof(s_lighting_cluster, sample_index) == 12) ? 1 : -1];
typedef char object_lighting_cluster_samples_offset[(offsetof(s_object_lighting_cluster, sample_index) == 12) ? 1 : -1];
// Serialized Halo 3 original-platform records. Layout corroborated by TagTool's
// format, where each airprobe stores
// key/fill colors at +0x14/+0x1c and coefficients at +0x24 in each airprobe.
struct s_lightmap_half_rgb_probe {
    unsigned short dominant_light_direction[3];
    unsigned short padding_direction;
    unsigned short dominant_light_intensity[3];
    unsigned short padding_intensity;
    union {
        unsigned short coefficients[27];
        unsigned short channel_coefficients[3][9];
    };
    unsigned short padding_coefficients;
};
struct s_lightmap_airprobe {
    real_point3d position;
    long name;
    union { unsigned long flags; unsigned short bsp_mask; };
    s_lightmap_half_rgb_probe lightprobe;
};
// Per-object static-lighting probes attached to scenery and machine instances.
struct s_lightmap_scenery_probe {
    c_object_identifier object_identifier;
    s_lightmap_half_rgb_probe probe;
};
struct s_lightmap_machine_sample {
    real_point3d position;
    s_lightmap_half_rgb_probe probe;
};
struct s_lightmap_machine_probe {
    c_object_identifier object_identifier;
    real_rectangle3d bounds;
    // probe_count, probes and probes_definition are the probes tag block (s_tag_block) spelled out.
    long probe_count;
    TAG_POINTER(s_lightmap_machine_sample) probes;
    TAG_POINTER(void) probes_definition;
};
typedef char lightmap_scenery_probe_size[(sizeof(s_lightmap_scenery_probe) == 0x50) ? 1 : -1];
typedef char lightmap_machine_probe_size[(sizeof(s_lightmap_machine_probe) == 0x2c) ? 1 : -1];
typedef char lightmap_machine_probe_count_offset[(offsetof(s_lightmap_machine_probe, probe_count) == 0x20) ? 1 : -1];
struct s_geometry_sampling_lightprobe_entity { short bitmap_index; };
struct s_geometry_sampling_cluster_entity : s_geometry_sampling_lightprobe_entity {
    short static_per_vertex_lighting_index;
};
typedef char sampling_airprobe_size[(sizeof(s_lightmap_airprobe) == 0x5c) ? 1 : -1];
typedef char sampling_airprobe_bsp_mask_offset[(offsetof(s_lightmap_airprobe, bsp_mask) == 0x10) ? 1 : -1];
typedef char sampling_instance_probe_size[(sizeof(s_lightmap_half_rgb_probe) == 0x48) ? 1 : -1];
#endif
