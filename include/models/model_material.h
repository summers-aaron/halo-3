#ifndef MODELS_MODEL_MATERIAL_H
#define MODELS_MODEL_MATERIAL_H

#include <stddef.h>
#include "cseries/cseries_types.h"
#include "game/materials.h"

// One element of the hlmt materials block: the damage section and global material type of a model material.
// Field sequence corroborated by TagTool Model.Material (Size 0x14; the trailing 2 bytes are compiler padding).
struct s_damage_model_material {
    string_id material_name;
    short material_type;
    short damage_section;
    short runtime_collision_material_index;
    short runtime_damager_material_index;
    string_id global_material_name;
    c_global_material_type material;
};

typedef char damage_model_material_size[(sizeof(s_damage_model_material) == 0x14) ? 1 : -1];
typedef char damage_model_material_section_offset[(offsetof(s_damage_model_material, damage_section) == 6) ? 1 : -1];
typedef char damage_model_material_type_offset[(offsetof(s_damage_model_material, material) == 0x10) ? 1 : -1];

#endif // MODELS_MODEL_MATERIAL_H
