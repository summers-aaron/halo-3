#ifndef RENDER_RENDER_OBJECT_GLOBALS_TYPES_H
#define RENDER_RENDER_OBJECT_GLOBALS_TYPES_H

#include <stddef.h>
struct render_lighting;
struct s_render_object_info;
struct s_shield_impact_definition;
struct s_shader_extern_info;

struct s_mesh_part_counter {
  unsigned long upper : 16;
  unsigned long count : 15;
  unsigned long flag : 1;
};

struct s_object_render_context {              // 0x10 bytes
  s_mesh_part_counter mesh_parts;
  union {
    long parameters;
    struct {
      unsigned char stencil_index;
      bool submit_instances;
    };
  };
  const render_lighting *lighting;
  const s_shader_extern_info *shader_extern_info;
};

struct s_context_mesh_part {
  long flags;
  s_render_object_info *object_info;
  unsigned long *data;
  unsigned short part_index;
  unsigned short mesh_index;
  unsigned short region_index;
  long render_method_index;
  const s_shield_impact_definition *shield_impact;
};

struct s_render_object_globals {
  s_object_render_context contexts[1024];
  union { long object_count; long current_a; };
  s_context_mesh_part mesh_parts[8192];
  union { long mesh_part_count; long current_b; };
  union { bool contexts_added; bool active; };
  union { long marker_depth; long marker; };
  union { long marker_object_counts[6]; long stack_a[6]; };
  union { long marker_mesh_part_counts[6]; long stack_b[6]; };
};

typedef char render_object_context_size[(sizeof(void *) != 4 || ((sizeof(s_object_render_context) == 0x10))) ? 1 : -1];
typedef char render_context_mesh_part_size[(sizeof(void *) != 4 || ((sizeof(s_context_mesh_part) == 0x1c))) ? 1 : -1];
typedef char render_context_mesh_part_method_offset[(sizeof(void *) != 4 || ((offsetof(s_context_mesh_part, render_method_index) == 0x14))) ? 1 : -1];
typedef char render_object_globals_size[(sizeof(void *) != 4 || ((sizeof(s_render_object_globals) == 0x3c040))) ? 1 : -1];
typedef char render_object_globals_parts_offset[(sizeof(void *) != 4 || ((offsetof(s_render_object_globals, mesh_parts) == 0x4004))) ? 1 : -1];
typedef char render_object_globals_marker_offset[(sizeof(void *) != 4 || ((offsetof(s_render_object_globals, marker_depth) == 0x3c00c))) ? 1 : -1];

#endif
