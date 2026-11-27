#ifndef GEOMETRY_GEOMETRY_SAMPLING_H
#define GEOMETRY_GEOMETRY_SAMPLING_H

#include <vectorintrinsics.h>

// XDK `XMLoadDHenN3` 16-byte unpack tables (integer words; loaded as vectors with `__lvx`).
extern "C" {
extern const unsigned int k_dhenn3_unpack_scale[4];
extern const unsigned int k_dhenn3_unpack_shift[4];
}

// Functions defined in tu/blamlibxbox/geometry_sampling.cpp that other units call.

union real_rgb_color;
union real_vector3d;

void calculate_dominant_light_from_lightprobe(const float *key_light, const float *fill_light, const float *ambient_light,
    real_vector3d *direction, real_rgb_color *color, float *intensity);

union real_point3d;
struct s_geometry_sampling_probe;

extern "C" long geometry_sampling_get_lightmap_probe(const real_point3d *point, long probe_index, s_geometry_sampling_probe *result);

struct s_geometry_sample;
enum e_geometry_sample_result;

extern "C" e_geometry_sample_result geometry_sampling_sample_point_lighting(const real_point3d &point, const real_vector3d &vector, bool test_objects,
    long ignore_object_index, bool sample_lightprobe, bool sample_diffuse, s_geometry_sample &sample);

extern "C" {
extern float g_geometry_sampling_diffuse_scale;
extern bool g_geometry_sampling_use_sky_lighting;
}

#include "math/real_math_types.h"
#include "game/materials.h"

struct s_geometry_sampling_real_rgb_lightprobe {
    float coefficients[27];
};
struct s_geometry_test_result {
    real_point3d collision_point;
    real_point3d position;
    real_point2d texcoord;
    real_point2d lightmap_texcoord;
    real_vector3d normal;
    long bsp_index;
    long cluster_index;
    long instance_index;
    long object_index;
    bool has_vertex_lightprobes;
    bool has_lightmap_texcoords;
    bool has_lightprobe;
    char pad71;
    s_geometry_sampling_real_rgb_lightprobe lightprobe;
    float linear_sh[3][4];
    union {
        real_vector3d dominant_light_direction;
        real_rgb_color dominant_light_color;
    };
    float u;
    float v;
    long triangle_index;
    real_point3d triangle_positions[3];
    real_point2d triangle_texcoords[3];
    real_point2d triangle_lightmap_texcoords[3];
    long material_index;
    c_global_material_type global_material;
};


enum e_geometry_test_vector_result {
    _geometry_test_vector_result_sampled = 0,
    _geometry_test_vector_result_object,
    _geometry_test_vector_result_structure,
    _geometry_test_vector_result_instance,
    _geometry_test_vector_result_none,
};

class c_geometry_sampler {
public:
    static e_geometry_test_vector_result geometry_test_vector(const real_point3d &point, const real_vector3d &vector, bool test_objects,
        long ignore_object_index, bool ignore_lighting, bool test_back_facing, bool find_material, s_geometry_test_result &result);
    static bool geometry_test_collision_result(long structure_bsp_index, long object_index, long instance_index, long surface_index,
        const real_point3d *point, bool ignore_lighting, bool find_material, bool &collision_valid, s_geometry_test_result &result);
    static s_geometry_sample m_default_lightprobe_sample;
private:
    static long find_render_method_index_from_collision(long bsp_index, long instance_index, long surface_index);

};

#endif // GEOMETRY_GEOMETRY_SAMPLING_H
