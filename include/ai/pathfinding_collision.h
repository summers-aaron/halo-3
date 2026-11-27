#ifndef AI_PATHFINDING_COLLISION_H
#define AI_PATHFINDING_COLLISION_H

#include "math/real_math_types.h"
#include "ai/sector_ref.h"

// Result of a pathfinding collision test.
struct path_collision_result
{
    bool collision;
    real_point3d point;
    c_sector_ref sector;
    long link_index;
    float t;
    // Latched by pathfinding_test_vector_with_flags on first entering a sector
    // whose normal is not flat-up (0x10 clear): whether that happened, and the
    // ray distance at entry (FLT_MAX when the whole walk stays on flat floor).
    bool entered_non_floor_sector;
    float non_floor_sector_t;
};

typedef char path_collision_result_size[(sizeof(path_collision_result) == 0x24) ? 1 : -1];

struct path_error_info;
extern "C" bool pathfinding_test_vector(const real_point3d *, c_sector_ref, c_sector_ref,
    const real_vector3d *, float, const path_error_info *, path_collision_result *);
bool pathfinding_test_line(const real_point3d *point, c_sector_ref sector_ref, const real_point3d *point2, c_sector_ref sector_ref2,
    const path_error_info *error_info, path_collision_result *result);

struct obstacles;
struct pathfinding_test_pill2d_result;
// The callers hold values in registers across this call; removal renumbers colors and repacks frames:
// match: 0xce208, 0x5743c8, 0xc6b00, 0x576740, 0x576e08: 0xce208 fr27-30 + savefpr_27/28; 0x576740/0x576e08 savegprlr_22/21; 0xc6b00 0x210/0x240.
__declspec(noalias) bool __cdecl pathfinding_test_pill2d(bool, const obstacles *, short, short, bool, const real_point3d *, c_sector_ref,
    c_sector_ref, const real_vector3d *, float, float, bool, bool, bool, const path_error_info *,
    pathfinding_test_pill2d_result *);

#endif // AI_PATHFINDING_COLLISION_H
