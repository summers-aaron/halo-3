#ifndef AI_PATH_POINT_TYPES_H
#define AI_PATH_POINT_TYPES_H

#include "ai/ai_point3d.h"
#include "ai/hint_ref.h"
#include "ai/sector_ref.h"

struct path_point
{
    short flags;
    short type;
    c_sector_ref sector;
    c_hint_ref hint;
    c_ai_point3d point;
};

typedef char path_point_size[(sizeof(path_point) == 0x1c) ? 1 : -1];

#endif // AI_PATH_POINT_TYPES_H
