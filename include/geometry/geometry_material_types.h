#ifndef GEOMETRY_GEOMETRY_MATERIAL_TYPES_H
#define GEOMETRY_GEOMETRY_MATERIAL_TYPES_H
#include "tag_files/tag_types.h"
#include "cseries/cseries.h"
#include <stddef.h>
// One element of the render_model / structure_bsp materials block.
// Field sequence corroborated by Assembly Halo3 mode.xml/sbsp.xml Materials (elementSize 0x24).
struct geometry_material
{
    s_tag_reference render_method;
    s_tag_block properties;
    long imported_material_index;
    unsigned char breakable_surface_index;
};
typedef char geometry_material_types_layout_1[(sizeof(geometry_material) == 0x24) ? 1 : -1];
typedef char geometry_material_types_layout_2[(offsetof(geometry_material, breakable_surface_index) == 0x20) ? 1 : -1];
typedef char geometry_material_types_layout_3[(offsetof(geometry_material, properties) == 0x10) ? 1 : -1];
typedef char geometry_material_types_layout_4[(offsetof(geometry_material, imported_material_index) == 0x1c) ? 1 : -1];
#endif
