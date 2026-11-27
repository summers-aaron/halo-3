#ifndef INCLUDE_OBJECTS_OBJECT_HEADER_H
#define INCLUDE_OBJECTS_OBJECT_HEADER_H

#include "memory/tls_globals.h"
#include "math/matrix_math.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "objects/object_iterator.h"

#include "objects/object_header_access.h"

static __forceinline void *object_header_block_get(const void *object, const s_object_block_reference *reference) {
    return (char *)object + reference->offset;
}

// The block of the object an index names (March objects.obj object_header_block_get(long, const object_header_block_reference *)).
static __forceinline void *object_header_block_get(datum_index object_index, const object_header_block_reference *reference) {
    void *object = object_get_and_verify_type(object_index, NONE);
    return (char *)object + reference->offset;
}

#define OBJECT_GET(object_index, type, mask) ((type *)object_get_and_verify_type((object_index), (mask)))
#define OBJECT_TRY_AND_GET(object_index, type, mask) ((type *)object_try_and_get_and_verify_type((object_index), (mask)))

struct object_region_information {
    signed char variant_index;
    signed char state;
    unsigned char properties;
    char padding;
    long effect;
};

// Out-of-line objects.obj functions
void *object_try_and_get_and_verify_type(datum_index object_index, unsigned long type_mask);
void *object_try_and_get_unsafe_and_verify_type(datum_index object_index, unsigned long type_mask);
long object_get_ultimate_parent(datum_index object_index);

// Local inline body for inlined call sites, named apart from the out-of-line function above:
// this build emits bl for cross-unit calls, so inlined sites need this copy (was duplicated in
// objects.cpp and actors.cpp).
static __forceinline long inl_object_get_ultimate_parent(long object_index)
{
    long result = -1;
    while (object_index != -1) {
        result = object_index;
        object_index = object_get(object_index)->parent_object_index;
    }
    return result;
}
real_point3d *object_get_origin(datum_index object_index, real_point3d *origin);
void object_get_orientation(datum_index object_index, real_vector3d *forward, real_vector3d *up);
real_matrix4x3 *object_get_world_matrix(datum_index object_index, real_matrix4x3 *matrix);
void object_get_velocities(datum_index object_index, real_vector3d *linear_velocity, real_vector3d *angular_velocity);
void object_set_velocities(datum_index object_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity);
real_point3d *object_get_center_of_mass(datum_index object_index, real_point3d *center);
void object_get_region_information(datum_index object_index, long *region_count, char **region_permutations,
    char **visible_permutations, object_region_information **region_information);
bool object_get_change_color(datum_index object_index, long change_color_index, real_rgb_color *color);
void object_delete(datum_index object_index);
void object_detach(datum_index object_index);
void object_reset(datum_index object_index);
void object_reconnect_to_physics(datum_index object_index);
void object_disconnect_from_physics(datum_index object_index);
void objects_dispose_from_old_structure_bsp(unsigned long old_structure_bsp_mask);
void attachments_delete(datum_index object_index);

#endif // INCLUDE_OBJECTS_OBJECT_HEADER_H
