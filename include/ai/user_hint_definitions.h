#ifndef AI_USER_HINT_DEFINITIONS_H
#define AI_USER_HINT_DEFINITIONS_H
#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
struct s_flight_area_reference { short area; short point_index; unsigned short bsp; short pad6; };
struct s_flight_points { long count; TAG_POINTER(real_point3d) data; TAG_POINTER(void) def; };
// Element of scenario.ai_user_hint_data. Block offsets follow March ai_render_user_hints and its
// render_* helpers; nothing in March reads the blocks at 0x24 and 0x48.
struct s_flight_hints
{
    s_tag_block line_segment_geometry;   // user_hint_render_line_segment
    s_tag_block parallelogram_geometry;  // user_hint_render_parallelogram
    s_tag_block jump_hints;              // user_hint_render_jump
    s_tag_block pad24;
    s_tag_block well_hints;              // render_well_hints
    long count;                          // flight hints: render_flight_hints
    TAG_POINTER(s_flight_points) entries;
    TAG_POINTER(void) def;
    s_tag_block pad48;
    s_tag_block giant_hints;             // render_giant_sector_hints, render_giant_rail_hints
    s_tag_block sector_hints;            // render_sector_hints
};
typedef char user_hint_flight_offset[(offsetof(s_flight_hints, count) == 60) ? 1 : -1];
typedef char user_hint_size[(sizeof(void *) != 4 || sizeof(s_flight_hints) == 108) ? 1 : -1];
#endif
