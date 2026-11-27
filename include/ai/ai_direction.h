#ifndef AI_AI_DIRECTION_H
#define AI_AI_DIRECTION_H

#include "ai/ai_point3d.h"
#include "math/real_math_globals.h"

enum e_ai_direction_mode;

// 20-byte AI direction: a type tag plus an object, point, or vector payload.
class c_ai_direction {
public:
    union {
        long type;
        long m_type;
    };
    union {
        long object_index;
        long m_object_index;
        real_vector3d vector;
        real_point3d point;
        c_ai_point3d m_point;
    };

    // valid(): March 82ecbf70.
    __forceinline void clear() { type = -1; m_point.set((const real_point3d *)global_forward3d); } // Reset to invalid, facing forward.
    __forceinline bool valid() { return type != -1; }
    void flush_object(long object_index);
    bool get(long actor_index, e_ai_direction_mode mode, const real_point3d *point, real_vector3d *direction, bool *valid, real_point3d *result);

    __forceinline void to_movement() { type = 0; }
    __forceinline void to_prop(long index) { type = 1; object_index = index; }
    __forceinline void to_target() { type = 2; }
    __forceinline void to_point(const real_point3d *p) { type = 3; point = *p; }
    __forceinline void set_point(const real_point3d *p) { m_type = 3; m_point.point = *p; }
    __forceinline void to_vector(const real_vector3d *v) { type = 5; vector = *v; }
    __forceinline void to_danger() { type = 6; }
    __forceinline void to_object(long index) { type = 7; object_index = index; }
};

typedef char ai_direction_size[(sizeof(c_ai_direction) == 0x14) ? 1 : -1];

#endif // AI_AI_DIRECTION_H
