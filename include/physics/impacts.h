#ifndef PHYSICS_IMPACTS_H
#define PHYSICS_IMPACTS_H

#include "math/real_math_types.h"
#include "sound/game_sound_types.h"
#include "sound/game_sound_impulse_parameters.h"
#include "objects/object_reference_types.h"

// Impact-domain shared types (resolved effects, sound submission, spatial sound fixup).

union impacts_material_effects {
    long indices[7];
    struct {
        long sounds[3];
        long effects[3];
        long additional_effect;
    };
};

struct __declspec(align(16)) impacts_sound_submission_parameters : s_game_sound_impulse_internal_parameters {};

struct impacts_sound_context {
    impacts_sound_submission_parameters sound_parameters;
    real_plane3d contact_plane;
};

struct impacts_spatial_location { s_sound_location sound; s_location location; };

// Functions defined in tu/blamlibxbox/impacts.cpp.

class c_havok_contact_point;

void __tls_set_g_impact_globals_allocator(void *allocator);
void __tls_set_g_impact_data_allocator(void *object);
void impacts_notify_object_deleted(long object_index);
void impacts_disconnect_from_physics();
void impacts_reconnect_to_physics();
void impacts_quicksort(void);
void impact_delete(long impact_index);
void impacts_create_orphaned_impacts(void);
void impacts_create_orphaned_impact(long object_index, c_havok_contact_point *contact_point, bool create);
void impacts_merge();
bool impacts_score_compare(long first, long second, void *);
extern "C" void impacts_create_constraint_impacts(long object_index);

union real_point3d;
union real_vector3d;

extern "C" void impacts_find_material_effects(long p0, long p1, const real_vector3d *vector, long p3, bool p4,
    const real_point3d *point, unsigned short p6, unsigned short p7, long p8, long *p9);

extern "C" void __tls_set_g_impacts_globals_allocator(void *allocation);

#endif // PHYSICS_IMPACTS_H
