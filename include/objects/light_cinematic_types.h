#ifndef OBJECTS_LIGHT_CINEMATIC_TYPES_H
#define OBJECTS_LIGHT_CINEMATIC_TYPES_H

#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "render/cinematic_light_entry_types.h"
#include "render/render_lighting_types.h"
#include "objects/light_sh_data_types.h"

struct s_lights_reference_entry {
    long light_index;
    long reserved4;
};
struct s_lights_reference_table {
    // Overlays the owners entry count plus first object index; see s_cinematic_light_owners.
    long light_entry_count;
    long first_object_index;
    s_lights_reference_entry entries[1];
};


struct s_cinematic_object_entry {
    long object_index;
    render_lighting lighting;
    float scale;
    float values0[9];
    float values1[9];
    float values2[9];
    // Overlays definition_index plus matrix; see s_light_sh_data.
    long definition_index;
    real_matrix4x3 matrix;
};

typedef char cinematic_entry_lighting_offset[(offsetof(s_cinematic_object_entry, lighting) == 4) ? 1 : -1];
typedef char cinematic_entry_scale_offset[(offsetof(s_cinematic_object_entry, scale) == 0x21c) ? 1 : -1];
typedef char cinematic_entry_size[(sizeof(s_cinematic_object_entry) == 0x2c4) ? 1 : -1];

struct s_cinematic_light_globals {
    long light_entry_count;
    long pad4;
    s_cinematic_light_entry light_entries[15];
    long pad128;
    long object_entry_count;
    union {
        s_cinematic_object_entry object_entries[64];
        s_light_sh_data object_sh_entries[64];
    };
};

typedef char cinematic_sh_entry_size[(sizeof(s_light_sh_data) == sizeof(s_cinematic_object_entry)) ? 1 : -1];
typedef char cinematic_object_count_offset[(offsetof(s_cinematic_light_globals, object_entry_count) == 0x84) ? 1 : -1];
typedef char cinematic_object_sh_offset[(offsetof(s_cinematic_light_globals, object_sh_entries) == 0x88) ? 1 : -1];
typedef char cinematic_light_globals_size[(sizeof(s_cinematic_light_globals) == 0xb188) ? 1 : -1];

struct s_cinematic_light_owner_entry {
    long object_index;
    long light_index;
};

struct s_cinematic_light_owners {
    long light_entry_count;
    s_cinematic_light_owner_entry light_entries[15];
};

struct s_light_sh_element {
    char spare_before_angles[4];
    real_euler_angles2d angles;
    float exponent;
    float coeff_a;
    float coeff_b;
    float coeff_c;
    float angle;
};






struct s_lights_attachment_definition {
    unsigned long flags;
    real_euler_angles2d angles;
    float offset;
    s_tag_reference light;
};

struct s_cinematic_lighting_definition {
    long sh_light_count;
    TAG_POINTER(s_light_sh_element) sh_lights;
    long sh_lights_definition;
    long attachment_count;
    TAG_POINTER(s_lights_attachment_definition) attachments;
    long attachments_definition;
    float scale;
};


extern "C" unsigned char g_light_multiple_markers_warning_issued;

#endif // OBJECTS_LIGHT_CINEMATIC_TYPES_H
