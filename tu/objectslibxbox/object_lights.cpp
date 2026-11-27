// objectslibxbox/object_lights.obj: from the records 0x15cee8
// @flags /TP /O2 /Oc
#include "objects/object_header.h"
#include "math/real_math.h"
#include <string.h>
#include "objects/object_lights.h"
#include "geometry/geometry_sampling_types.h"



#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"

#include "geometry/geometry_sampling.h"

// @rdata 0x820215b8..0x82021624
static const real_vector3d k_distant_lighting_vectors[9] = {
    { -10.0f, 0.0f, -5.0f }, { 10.0f, 0.0f, -5.0f }, { 0.0f, -10.0f, -5.0f }, { 0.0f, 10.0f, -5.0f }, { 0.0f, 0.0f, -40.0f },
    { -10.0f, 0.0f, 5.0f }, { 10.0f, 0.0f, 5.0f }, { 0.0f, -10.0f, 5.0f }, { 0.0f, 10.0f, 5.0f },
};
// @rdata-end

// @address 0x15cee8 ?lights_distant_lighting_at_point_new@@YAJJJJ_NPBTreal_point3d@@PAUs_geometry_sample@@PATreal_vector3d@@0@Z
long lights_distant_lighting_at_point_new(long flags, long object_index, long ignore_object_index, bool unused,
    const real_point3d *point, s_geometry_sample *sample, real_vector3d *override_direction, bool vertical_only)
{
    override_direction = NULL;
    long result = _geometry_sample_result_no_geometry;
    long vector_count;
    long offset_count;
    const real_vector3d *vectors;
    const real_vector3d *offsets = NULL;
    real_vector3d vector_array[16];
    real_vector3d offset_array[16];

    if (TEST_BIT(flags, 0))
    {
        vectors = k_distant_lighting_vectors;
        vector_count = 9;
        offset_count = 0;
    }
    else if (TEST_BIT(flags, 2))
    {
        vectors = override_direction;
        vector_count = 1;
        offset_count = 0;
    }
    else
    {
        const s_object_header *header = object_header_get_const(object_index);
        long object_type = header->object_type;
        bool unit_like = object_type == _object_type_scenery || object_type == _object_type_biped;
        bool dead_biped = false;
        if (object_type == _object_type_biped && (bool)((header->datum->damage_flags >> 2) & 1))
            dead_biped = true;

        real_vector3d forward;
        real_vector3d up;
        object_get_orientation(object_index, &forward, &up);
        float radius = MAX(header->datum->bounding_radius, 0.05f);
        float extent = MAX(radius, 0.4f);

        vectors = vector_array;
        offsets = offset_array;
        if (vertical_only)
        {
            scale_vector3d(global_down3d, extent * 2.0f, &vector_array[0]);
            offset_array[0].i = 0.0f;
            offset_array[0].j = 0.0f;
            offset_array[0].k = 0.0f;
            scale_vector3d(global_up3d, extent * 2.0f, &vector_array[1]);
            offset_array[1].i = 0.0f;
            offset_array[1].j = 0.0f;
            offset_array[1].k = 0.0f;
            vector_count = 2;
            offset_count = 2;
        }
        else if (dead_biped)
        {
            scale_vector3d(global_down3d, extent * 10.0f, &vector_array[0]);
            scale_vector3d(global_up3d, radius, &offset_array[0]);
            scale_vector3d(&up, -extent * 3.0f, &vector_array[1]);
            scale_vector3d(vectors, -0.01f, &offset_array[1]);
            scale_vector3d(&up, -extent * 4.0f, &vector_array[2]);
            scale_vector3d(&up, radius, &offset_array[2]);
            vector_count = 3;
            offset_count = 3;
        }
        else if (unit_like)
        {
            scale_vector3d(&up, -extent * 3.0f, &vector_array[0]);
            scale_vector3d(vectors, -0.01f, &offset_array[0]);
            scale_vector3d(&up, -extent * 4.0f, &vector_array[1]);
            scale_vector3d(&up, radius, &offset_array[1]);
            scale_vector3d(global_down3d, extent * 10.0f, &vector_array[2]);
            scale_vector3d(global_up3d, radius, &offset_array[2]);
            vector_count = 3;
            offset_count = 3;
        }
        else
        {
            scale_vector3d(global_down3d, extent * 10.0f, &vector_array[0]);
            if (object_type == _object_type_vehicle && radius > 2.0f && TEST_BIT(global_scenario->flags, 9))
            {
                radius *= 0.1f;
                scale_vector3d(global_up3d, radius, &offset_array[0]);
            }
            else
                scale_vector3d(global_up3d, radius, &offset_array[0]);
            vector_count = 1;
            offset_count = 1;
        }
    }

    bool hit_geometry = false;
    for (long vector_index = 0; vector_index < vector_count && result == _geometry_sample_result_no_geometry; vector_index++)
    {
        real_point3d sample_point;
        if (offset_count == 0)
            point_from_line3d(point, &vectors[vector_index], -0.001f, &sample_point);
        else
            point_from_line3d(point, &offsets[vector_index], 1.0f, &sample_point);

        s_geometry_sample geometry_sample;
        e_geometry_sample_result sample_result = geometry_sampling_sample_point_lighting(
            sample_point, vectors[vector_index], false, ignore_object_index, true, true, geometry_sample);
        if (sample_result == _geometry_sample_result_sampled)
        {
            result = _geometry_sample_result_sampled;
            memcpy(sample, &geometry_sample, sizeof(s_geometry_sample));
            hit_geometry = true;
        }
        else if (sample_result == _geometry_sample_result_unlit_geometry)
        {
            hit_geometry = true;
        }
    }
    if (result != _geometry_sample_result_sampled && hit_geometry)
        result = _geometry_sample_result_unlit_geometry;
    return result;
}
