// xgraphics/xenoncompress.obj: from the records 0x5f8198
// @compiler xdk6130
// @flags /TP /O1 /Oc
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>

struct s_xenoncompress_point { unsigned short x; unsigned short y; };


// @address 0x5f8198 xenoncompress_find_closest_point
extern "C" int xenoncompress_find_closest_point(const s_xenoncompress_point *points, float x, float y)
{
    float dx0;
    float dy0;
    float dx;
    float dy;
    float dist2;
    float best_dist2;
    int best;
    int i;
    best = 0;
    i = 1;
    dx0 = (x - ((float)points[0].x * 1.5259022e-05f));
    dy0 = (y - ((float)points[0].y * 1.5259022e-05f));
    best_dist2 = ((dy0 * dy0) + (dx0 * dx0));
    do {
        dx = (x - ((float)points[i].x * 1.5259022e-05f));
        dy = (y - ((float)points[i].y * 1.5259022e-05f));
        dist2 = ((dy * dy) + (dx * dx));
        if (dist2 < best_dist2) {
            best_dist2 = dist2;
            best = i;
        }
        i = (i + 1);
    } while (i < 4);
    return best;
}
