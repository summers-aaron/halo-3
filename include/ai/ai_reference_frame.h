#ifndef AI_AI_REFERENCE_FRAME_H
#define AI_AI_REFERENCE_FRAME_H

#include <math.h>

// Reference-frame storage: 150 eight-byte entries, allocated by actors.cpp.
struct s_attachment_state
{
    long object_index;
    union
    {
        long unknown;
        struct { short flags; short reserved; };
    };
};

extern "C" {
extern __declspec(thread) s_attachment_state *g_ai_reference_frame_data;
}

// Functions defined in tu/ailibxbox/ai_reference_frame.cpp that other units call.

class c_ai_point3d;
union real_point3d;
union real_vector3d;

bool object_refresh_reference_frames(long object_index);
void ai_refresh_reference_frames(void);
short ai_reference_frame_get_by_node_index(long object_index, short node_index);
bool ai_reference_frame_transform_point(short index, const real_point3d *point, real_point3d *result);
float ai_distance_squared3d(const c_ai_point3d *point, const real_point3d *other);
void ai_vector_from_points3d(const real_point3d *a, const c_ai_point3d *b, real_vector3d *result);
void ai_vector_from_points3d(const c_ai_point3d *a, const c_ai_point3d *b, real_vector3d *result);
real_point3d *ai_point_get_position(const c_ai_point3d *point, real_point3d *result);
float ai_distance_squared3d(const c_ai_point3d *a, const c_ai_point3d *b);

static __forceinline float ai_distance3d(const c_ai_point3d *point, const real_point3d *other)
{
    return sqrtf(ai_distance_squared3d(point, other));
}

static __forceinline float ai_distance3d(const c_ai_point3d *a, const c_ai_point3d *b)
{
    return sqrtf(ai_distance_squared3d(a, b));
}
bool ai_reference_frame_transform_vector(short index, const real_vector3d *vector, real_vector3d *result_vec);
bool ai_reference_frame_inverse_transform_vector(short index, const real_vector3d *vector, real_vector3d *result_vec);
bool ai_reference_frame_inverse_transform_point(short index, const real_point3d *point, real_point3d *result);
extern "C" bool object_can_have_reference_frames(long object_index);
bool ai_reference_frames_are_incompatible(short a, short b);
extern "C" bool ai_point_set_from_world_position(const real_point3d *point, long object_index, long node_index, c_ai_point3d *result);

#endif // AI_AI_REFERENCE_FRAME_H
