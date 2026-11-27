#ifndef SCENARIO_SCENARIO_CUTSCENE_TITLE_TYPES_H
#define SCENARIO_SCENARIO_CUTSCENE_TITLE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_scenario_cutscene_title
{
    long name;
    rectangle2d bounds;
    short justification;
    short vertical_justification;
    short font;
    short pad12;
    unsigned long text_color;
    unsigned long shadow_color;
    float fade_in_time;
    float start_time;
    float duration;
};
typedef char scenario_cutscene_title_size[(sizeof(s_scenario_cutscene_title) == 0x28) ? 1 : -1];
typedef char scenario_cutscene_title_start_offset[(offsetof(s_scenario_cutscene_title, start_time) == 0x20) ? 1 : -1];
#endif
