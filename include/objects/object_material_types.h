#ifndef OBJECTS_OBJECT_MATERIAL_TYPES_H
#define OBJECTS_OBJECT_MATERIAL_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "game/materials.h"

struct s_object_material_definition
{
    char spare_before_model[0x34];
    s_tag_reference model;
    char spare_before_default_material_type[0x3a8 - 0x44];
    c_global_material_type default_material_type;
    c_global_material_type dead_material_type;
};

typedef char object_material_model_offset[(sizeof(void *) != 4 || offsetof(s_object_material_definition, model) == 0x34) ? 1 : -1];
typedef char object_material_default_offset[(sizeof(void *) != 4 || offsetof(s_object_material_definition, default_material_type) == 0x3a8) ? 1 : -1];

#endif // OBJECTS_OBJECT_MATERIAL_TYPES_H
