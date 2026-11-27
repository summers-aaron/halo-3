#ifndef CAMERA_CINEMATIC_TYPES_H
#define CAMERA_CINEMATIC_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "camera/cinematic_scene_reference_types.h"
struct s_cinematic {
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    long tag_field;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    s_tag_block scenes;
};
typedef char cinematic_tag_field_offset[(offsetof(s_cinematic, tag_field) == 0x64) ? 1 : -1];
typedef char cinematic_scenes_offset[(offsetof(s_cinematic, scenes) == 0x7c) ? 1 : -1];

#endif // CAMERA_CINEMATIC_TYPES_H
