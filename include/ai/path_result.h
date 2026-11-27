#ifndef AI_PATH_RESULT_H
#define AI_PATH_RESULT_H
#include <stddef.h>
#include "ai/path_point_types.h"
#include "ai/path_types.h"
#include "math/real_math.h"

// March 82f49660: point +0, vector +16, sector +28, distance +32.
class c_path_destination
{
public:
    c_ai_point3d point;
    real_vector3d vector;
    c_sector_ref sector;
    float distance;
    void set(const c_ai_point3d *new_point, const c_sector_ref *new_sector, float distance)
    {
        point = *new_point;
        sector = *new_sector;
        vector = *global_zero_vector3d;
        this->distance = distance;
    }
};
struct path_result
{
    bool success;
    bool final_point;
    bool x2;
    char pad3;
    c_ai_point3d start_point;
    c_path_destination destination;
    long ignorable_object_index;
    short x3c;
    char count;
    char x3f;
    path_point points[4];
    union { path_error_info error; struct { short xB0_stage; short xB2_count; long xB4_data[6]; }; };
};
typedef char path_destination_size[(sizeof(c_path_destination) == 0x24) ? 1 : -1];
typedef char path_result_size[(sizeof(path_result) == 0xcc) ? 1 : -1];
typedef char path_result_points_offset[(offsetof(path_result, points) == 0x40) ? 1 : -1];
#endif
