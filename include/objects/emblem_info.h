#ifndef OBJECTS_EMBLEM_INFO_H
#define OBJECTS_EMBLEM_INFO_H

// The emblem renderer's byte layout.
struct emblem_info {
    unsigned char foreground_emblem, background_emblem, flags;
    signed char primary_color;
    signed char secondary_color;
    signed char background_color;
    // March interface\c_player_profile.cpp:727 asserts emblem_info->pad == 0, reading this
    // field with a halfword load, so it is a 2-byte scalar, not a byte array.
    short pad;
};

typedef char emblem_info_size[(sizeof(emblem_info) == 8) ? 1 : -1];

// Preview bytes written by emblem_set_render_constants_for_emblem, defined in
// tu/objectslibxbox/emblems.cpp (March objectslibxbox emblems.obj defines this name).
extern "C" emblem_info g_emblem_render_info;

#endif // OBJECTS_EMBLEM_INFO_H
