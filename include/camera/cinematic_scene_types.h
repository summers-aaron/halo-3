#ifndef CAMERA_CINEMATIC_SCENE_TYPES_H
#define CAMERA_CINEMATIC_SCENE_TYPES_H
#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"

enum e_scene_object_type {
    _scene_object_type_unit = 0x756e6974,
};
struct s_cinematic_scene_object_attachment {
    long parent_marker_name;
    char object_name[32];
    long child_marker_name;
    s_tag_reference attachment_type;
};
struct s_cinematic_scene_object {
    char name[32];
    long identifier;
    long variant_name;
    s_tag_reference animation_graph;
    s_tag_reference object;
    long flags;
    long view_mask;
    unsigned long player_flags;
    s_tag_data import_override_creation_script;
    s_tag_block attachments;
};
typedef char cinematic_scene_object_size[(sizeof(s_cinematic_scene_object) == 0x74) ? 1 : -1];
typedef char cinematic_scene_object_flags_offset[(offsetof(s_cinematic_scene_object, flags) == 0x48) ? 1 : -1];
typedef char cinematic_scene_object_view_mask_offset[(offsetof(s_cinematic_scene_object, view_mask) == 0x4c) ? 1 : -1];
typedef char cinematic_scene_object_attachments_offset[(offsetof(s_cinematic_scene_object, attachments) == 0x68) ? 1 : -1];
typedef char cinematic_scene_object_animation_graph_offset[(offsetof(s_cinematic_scene_object, animation_graph) == 0x28) ? 1 : -1];

struct hs_cinematic_dialogue {
    s_tag_reference sound;
    long frame;
    float scale;
    long lipsync_actor;
    long playback_label;
    long subtitle;
};
struct hs_cinematic_effect {
    s_tag_reference effect;
    long frame;
    long marker_name;
    long marker_parent;
};
struct hs_cinematic_screen_effect {
    s_tag_reference effect;
    long frame;
};
struct s_cinematics_group_source {
    real_vector3d start;
    real_vector3d end;
    long frame0;
    long frame1;
    long count;
    TAG_POINTER(long) object_indices;
    TAG_POINTER(void) object_block_definition;
};
struct s_camera_frame;
struct s_cinematic_item_element { s_tag_reference lighting; long handle; long value; };
struct hs_cinematic_shot {
    s_tag_data import_script_header;
    unsigned long flags;
    float exposure;
    float forced_exposure;
    long lighting_count;
    TAG_POINTER(s_cinematic_item_element) lighting;
    TAG_POINTER(void) lighting_block_definition;
    long group_count;
    TAG_POINTER(s_cinematics_group_source) groups;
    TAG_POINTER(void) group_block_definition;
    long dialogue_count;
    TAG_POINTER(hs_cinematic_dialogue) dialogue;
    TAG_POINTER(void) dialogue_block_definition;
    s_tag_block music;
    long effect_count;
    TAG_POINTER(hs_cinematic_effect) effects;
    TAG_POINTER(void) effect_block_definition;
    s_tag_block functions;
    long screen_effect_count;
    TAG_POINTER(hs_cinematic_screen_effect) screen_effects;
    TAG_POINTER(void) screen_effect_block_definition;
    s_tag_block import_scripts;
    s_tag_data import_script_footer;
    long duration;
    long frame_count;
    TAG_POINTER(s_camera_frame) frames;
    TAG_POINTER(void) camera_block_definition;
};
struct s_cinematic_scene {
    long name_id;
    char name[32];
    short scene_type;
    short : 16;
    short type;
    short : 16;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    s_tag_block objects;
    s_tag_block shots;
    s_tag_block object_shots;
};
typedef char hs_cinematic_shot_exposure_offset[(offsetof(hs_cinematic_shot, exposure) == 0x18) ? 1 : -1];
typedef char hs_cinematic_shot_lighting_offset[(offsetof(hs_cinematic_shot, lighting) == 0x24) ? 1 : -1];
typedef char hs_cinematic_shot_frames_offset[(offsetof(hs_cinematic_shot, frames) == 0x9c) ? 1 : -1];
typedef char hs_cinematic_shot_music_offset[(offsetof(hs_cinematic_shot, music) == 0x44) ? 1 : -1];
typedef char hs_cinematic_shot_groups_offset[(offsetof(hs_cinematic_shot, groups) == 0x30) ? 1 : -1];
typedef char hs_cinematic_dialogue_frame_offset[(offsetof(hs_cinematic_dialogue, frame) == 0x10) ? 1 : -1];
typedef char hs_cinematic_dialogue_size[(sizeof(hs_cinematic_dialogue) == 0x24) ? 1 : -1];
typedef char hs_cinematic_effect_size[(sizeof(hs_cinematic_effect) == 0x1c) ? 1 : -1];
typedef char hs_cinematic_screen_effect_size[(sizeof(hs_cinematic_screen_effect) == 0x14) ? 1 : -1];
typedef char hs_cinematic_shot_size[(sizeof(hs_cinematic_shot) == 0xa4) ? 1 : -1];
typedef char hs_cinematic_shot_dialogue_offset[(offsetof(hs_cinematic_shot, dialogue) == 0x3c) ? 1 : -1];
typedef char hs_cinematic_shot_effects_offset[(offsetof(hs_cinematic_shot, effects) == 0x54) ? 1 : -1];
typedef char hs_cinematic_shot_screen_effects_offset[(offsetof(hs_cinematic_shot, screen_effects) == 0x6c) ? 1 : -1];
typedef char hs_cinematic_shot_frame_count_offset[(offsetof(hs_cinematic_shot, frame_count) == 0x98) ? 1 : -1];
typedef char cinematic_attachment_size[(sizeof(s_cinematic_scene_object_attachment) == 0x38) ? 1 : -1];
typedef char cinematic_attachment_type_offset[(offsetof(s_cinematic_scene_object_attachment, attachment_type) == 0x28) ? 1 : -1];
typedef char cinematic_object_reference_offset[(offsetof(s_cinematic_scene_object, object) == 0x38) ? 1 : -1];
typedef char cinematic_object_script_offset[(offsetof(s_cinematic_scene_object, import_override_creation_script) == 0x54) ? 1 : -1];
typedef char cinematic_group_source_size[(sizeof(s_cinematics_group_source) == 0x2c) ? 1 : -1];
typedef char cinematic_shot_forced_exposure_offset[(offsetof(hs_cinematic_shot, forced_exposure) == 0x1c) ? 1 : -1];
typedef char cinematic_shot_functions_offset[(offsetof(hs_cinematic_shot, functions) == 0x5c) ? 1 : -1];
typedef char cinematic_shot_footer_offset[(offsetof(hs_cinematic_shot, import_script_footer) == 0x80) ? 1 : -1];
typedef char s_cinematic_scene_objects_offset[(offsetof(s_cinematic_scene, objects) == 0x3c) ? 1 : -1];
typedef char s_cinematic_scene_shots_offset[(offsetof(s_cinematic_scene, shots) == 0x48) ? 1 : -1];
typedef char s_cinematic_scene_name_offset[(offsetof(s_cinematic_scene, name) == 4) ? 1 : -1];
typedef char s_cinematic_scene_object_shots_offset[(offsetof(s_cinematic_scene, object_shots) == 0x54) ? 1 : -1];
#endif
