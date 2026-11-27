#ifndef MATH_REAL_MATH_TYPES_H
#define MATH_REAL_MATH_TYPES_H

#include <stddef.h>

// Shared math layouts; callable helpers and constants live in real_math.h.
// Union types:
union point2d {
    struct { short x, y; };
    short n[2];
};

union long_point3d {
    struct { long x, y, z; };
    long n[3];
};

union rectangle2d {
    struct { short y0, x0, y1, x1; };
    short n[4];
};

union real_point2d {
    struct { float x, y; };
    struct { float u, v; };
    float n[2];
};

union real_vector2d {
    struct { float i, j; };
    float n[2];
};

union real_point3d {
    struct { float axial; real_vector2d yz; };
    struct { float x, y, z; };
    struct { float i, j, k; };
    struct { float u, v, w; };
    real_point2d xy;
    float n[3];
    float coordinates[3];
};


union real_vector3d {
    struct { float i, j, k; };
    real_vector2d xy;
    float n[3];
    long words[3];
};

union real_vector4d {
    struct { float i, j, k, l; };
    struct { float x, y, z, w; };
    real_vector3d xyz;
    float n[4];
};

union real_euler_angles2d {
    struct { float yaw, pitch; };
    float n[2];
};

union real_euler_angles3d {
    struct { float yaw, pitch, roll; };
    float n[3];
};

struct real_bounds {
    float lower, upper;
};

union real_rectangle2d {
    struct { float x0, x1, y0, y1; };
    real_bounds bounds[2];
    float n[4];
    float m[2][2];
};

union real_rectangle3d {
    struct { float x0, x1, y0, y1, z0, z1; };
    real_bounds n[3];
    float m[3][2];
};

union real_rgb_color {
    struct { float red, green, blue; };
    float n[3];
};

union real_argb_color {
    struct {
        float alpha;
        union {
            real_rgb_color rgb;
            struct { float red, green, blue; };
        };
    };
    float n[4];
};

// Verified struct (`U`) types:
struct real_quaternion {
    real_vector3d v;
    float w;
};

struct real_plane2d {
    union {
        struct { real_vector2d n; float d; };
        struct { real_vector2d normal; float distance; };
        struct { float i, j; };
    };
};

struct real_plane3d {
    union {
        struct { real_vector3d n; float d; };
        struct { real_vector3d normal; float distance; };
        struct { float i, j, k; };
    };
};

struct real_orientation {
    real_quaternion rotation;
    real_point3d translation;
    float scale;
};


typedef char real_math_size_point2d[(sizeof(point2d) == 4) ? 1 : -1];
typedef char real_math_size_long_point3d[(sizeof(long_point3d) == 12) ? 1 : -1];
typedef char real_math_size_rectangle2d[(sizeof(rectangle2d) == 8) ? 1 : -1];
typedef char real_math_size_real_point2d[(sizeof(real_point2d) == 8) ? 1 : -1];
typedef char real_math_size_real_point3d[(sizeof(real_point3d) == 12) ? 1 : -1];
typedef char real_math_size_real_vector2d[(sizeof(real_vector2d) == 8) ? 1 : -1];
typedef char real_math_size_real_vector3d[(sizeof(real_vector3d) == 12) ? 1 : -1];
typedef char real_math_size_real_vector4d[(sizeof(real_vector4d) == 16) ? 1 : -1];
typedef char real_math_size_real_euler_angles2d[(sizeof(real_euler_angles2d) == 8) ? 1 : -1];
typedef char real_math_size_real_euler_angles3d[(sizeof(real_euler_angles3d) == 12) ? 1 : -1];
typedef char real_math_size_real_bounds[(sizeof(real_bounds) == 8) ? 1 : -1];
typedef char real_math_size_real_rectangle2d[(sizeof(real_rectangle2d) == 16) ? 1 : -1];
typedef char real_math_size_real_rectangle3d[(sizeof(real_rectangle3d) == 24) ? 1 : -1];
typedef char real_math_size_real_rgb_color[(sizeof(real_rgb_color) == 12) ? 1 : -1];
typedef char real_math_size_real_argb_color[(sizeof(real_argb_color) == 16) ? 1 : -1];
typedef char real_math_size_real_quaternion[(sizeof(real_quaternion) == 16) ? 1 : -1];
typedef char real_math_size_real_plane2d[(sizeof(real_plane2d) == 12) ? 1 : -1];
typedef char real_math_size_real_plane3d[(sizeof(real_plane3d) == 16) ? 1 : -1];
typedef char real_math_size_real_orientation[(sizeof(real_orientation) == 32) ? 1 : -1];
typedef char real_math_point3d_xy_offset[(offsetof(real_point3d, xy) == 0) ? 1 : -1];
typedef char real_math_point3d_coordinates_offset[(offsetof(real_point3d, coordinates) == 0) ? 1 : -1];
typedef char real_math_vector3d_words_offset[(offsetof(real_vector3d, words) == 0) ? 1 : -1];
typedef char real_math_plane2d_j_offset[(offsetof(real_plane2d, j) == 4) ? 1 : -1];
typedef char real_math_plane3d_k_offset[(offsetof(real_plane3d, k) == 8) ? 1 : -1];

#endif // MATH_REAL_MATH_TYPES_H
