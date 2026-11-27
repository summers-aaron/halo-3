#ifndef PHYSICS_SOFT_CEILING_DESIGN_TYPES_H
#define PHYSICS_SOFT_CEILING_DESIGN_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

class phys_bvh_bytecode;
struct s_soft_ceiling_group;

// Shared design prefix used by soft-ceiling queries and breakable surfaces.
struct s_soft_ceiling_design {
  long plane_count;
  long mopp_size;
  TAG_POINTER(phys_bvh_bytecode) code;
  long unusedc;
  long group_count;
  TAG_POINTER(s_soft_ceiling_group) groups;
};

typedef char soft_ceiling_design_code_offset[(offsetof(s_soft_ceiling_design, code) == 8) ? 1 : -1];
typedef char soft_ceiling_design_groups_offset[(offsetof(s_soft_ceiling_design, groups) == 0x14) ? 1 : -1];

#endif
