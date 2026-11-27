#ifndef STRUCTURES_STRUCTURE_DESIGN_TYPES_H
#define STRUCTURES_STRUCTURE_DESIGN_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

struct s_structure_design
{
    long version;
    s_tag_block soft_ceiling_mopp_codes;
    s_tag_block soft_ceilings;
    s_tag_block water_mopp_codes;
    s_tag_block water_groups;
    s_tag_block water_instances;
};
typedef char structure_design_size[(sizeof(s_structure_design) == 0x40) ? 1 : -1];
typedef char structure_design_soft_ceiling_mopp_offset[(offsetof(s_structure_design, soft_ceiling_mopp_codes) == 4) ? 1 : -1];
typedef char structure_design_water_mopp_offset[(offsetof(s_structure_design, water_mopp_codes) == 0x1c) ? 1 : -1];

#endif
