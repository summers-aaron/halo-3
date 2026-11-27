#ifndef CAMERA_CINEMATIC_SCENE_MUSIC_TYPES_H
#define CAMERA_CINEMATIC_SCENE_MUSIC_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

struct s_cinematic_scene_music
{
    unsigned long flags;
    s_tag_reference sound;
    long frame;
};
typedef char cinematic_scene_music_size[(sizeof(s_cinematic_scene_music) == 0x18) ? 1 : -1];
typedef char cinematic_scene_music_index_offset[(offsetof(s_cinematic_scene_music, sound) + offsetof(s_tag_reference, index) == 0x10) ? 1 : -1];

#endif
