#ifndef OBJECTS_OBJECT_LIGHTS_H
#define OBJECTS_OBJECT_LIGHTS_H

// Functions defined in tu/objectslibxbox/object_lights.cpp that other units call.

union real_point3d;
union real_vector3d;
struct s_geometry_sample;

long lights_distant_lighting_at_point_new(long flags, long object_index, long ignore_object_index, bool unused,
	const real_point3d *point, s_geometry_sample *sample, real_vector3d *override_direction, bool vertical_only);

#endif // OBJECTS_OBJECT_LIGHTS_H
