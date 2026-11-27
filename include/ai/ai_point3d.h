#ifndef AI_AI_POINT3D_H
#define AI_AI_POINT3D_H

#include "math/real_math_types.h"

class c_ai_point3d {
public:
    union {
        real_point3d point;
        real_point3d position;
    };
    union {
        short location[2];                        // [0] reference frame / sector (-1 = absolute), [1] bsp index
        short sector_ref[2];
        struct {
            short attachment_index;
            short pad;
        };
    };

    __forceinline void set(const real_point3d *p, short reference_frame, short structure_bsp_index) {
        point = *p;
        location[0] = reference_frame;
        location[1] = structure_bsp_index;
    }

    __forceinline void set(const real_point3d *p, short reference_frame) {
        point = *p;
        location[0] = reference_frame;
        location[1] = -1;
    }

    __forceinline void set(const real_point3d *p) {
        point = *p;
        location[0] = -1;
        location[1] = -1;
    }

    __forceinline const real_point3d *get_point() const { return &point; }
    __forceinline short reference_frame() const { return location[0]; }
    __forceinline short bsp_index() const { return location[1]; }
};

typedef char ai_point3d_size[(sizeof(c_ai_point3d) == 0x10) ? 1 : -1];

#endif // AI_AI_POINT3D_H
