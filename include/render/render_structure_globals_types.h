#ifndef RENDER_RENDER_STRUCTURE_GLOBALS_TYPES_H
#define RENDER_RENDER_STRUCTURE_GLOBALS_TYPES_H
#include "objects/object_reference_types.h"
#include <stddef.h>
struct s_scenario_lightmap_bsp_data;
struct s_render_geometry;
struct s_render_cluster_part {
  long pass;
  s_cluster_reference cluster_reference;
  unsigned short mesh_index;
  unsigned short part_index;
};
struct s_render_instance_part {
  long pass;
  short structure_bsp_index;
  unsigned short instance_index;
  unsigned short mesh_index;
  unsigned short part_index;
  unsigned char lod;
};
struct render_structure_globals {
  bool lightmap_geometry_valid;
  s_scenario_lightmap_bsp_data *lightmap_bsp_data[16];
  const s_render_geometry *render_geometry[16];
  s_render_cluster_part cluster_parts[512];
  long current_a;
  s_render_instance_part parts[1536];
  long current_b;
  long depth;
  union {
    struct {
      long pad7890;
      long stack_a[6];
      long stack_b[6];
    };
    struct {
      long cluster_markers[6];
      long instance_markers[6];
    };
  };
  long cached_definition_index;
  union { long cached_instance_marker; unsigned long cached_instance_index; };
};

typedef char render_structure_globals_types_layout_1[(sizeof(void *) != 4 || ((sizeof(render_structure_globals) == 0x78cc))) ? 1 : -1];
typedef char render_structure_globals_types_layout_2[(sizeof(void *) != 4 || ((offsetof(render_structure_globals, cluster_parts) == 0x84))) ? 1 : -1];
typedef char render_structure_globals_types_layout_3[(sizeof(void *) != 4 || ((offsetof(render_structure_globals, parts) == 0x1888))) ? 1 : -1];
typedef char render_structure_globals_types_layout_4[(sizeof(void *) != 4 || ((offsetof(render_structure_globals, stack_a) == 0x7894))) ? 1 : -1];
typedef char render_structure_globals_cluster_markers_offset[
    (sizeof(void *) != 4 || ((offsetof(render_structure_globals, cluster_markers) == 0x7890))) ? 1 : -1];
typedef char render_structure_globals_instance_markers_offset[
    (sizeof(void *) != 4 || ((offsetof(render_structure_globals, instance_markers) == 0x78a8))) ? 1 : -1];
#endif
