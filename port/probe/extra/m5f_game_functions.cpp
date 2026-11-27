// tmp/probe/extra (GAME-CHANGES F50): game functions the source calls (or a retail table points at) but does not
// define, each decompiled from the retail listing (recomp oracle sub_8XXXXXXX at the address given).
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <xtl.h>
#include "math/real_math_types.h"
#include <math.h>
#include "math/real_math_constant_storage.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "math/matrix_math_functions.h"
#include "math/real_constant_pool.h"
#include "physics/havok_runtime_storage.h"
#include "animations/animation_manager.h"
#include "animations/animation_manager_validity.h"
#include "physics/havok_component_class.h"
#include "game/dynamic_spawn_influencer_types.h"
#include "game/multiplayer_constants.h"
#include "game/game_engine_spawn_influencer.h"
#include "cache/restricted_memory_regions.h"
#include "physics/impacts_globals_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "tag_files/tag_resource_internals_types.h"

// (c_animation_manager::valid() now defined inline in include/animations/animation_manager.h)

// ---- 0x1ba838 havok_utilities_get_object_position_body_index: the position-controlling rigid body, if in range ----
short havok_utilities_get_object_position_body_index(c_havok_component *component)
{
    const long index = component->m_object_postion_controlling_rigid_body_index;
    return (short)((index >= 0 && index < component->m_rigid_bodies.m_size) ? index : -1);
}

// ---- 0x250b60 havok_component_delete_phantom_bodies: `delete m_bodies_in_phantom` (hkArray's scalar deleting
// destructor: release the element storage, then the array object through Havok's allocator). Every caller passes 1.
extern "C" void *havok_component_delete_phantom_bodies(phys_array<c_havok_component::s_body_in_phantom> *array, unsigned int flags)
{
    if (flags & 1)
        delete array;
    else
        array->~phys_array<c_havok_component::s_body_in_phantom>();
    return array;
}

// ---- 0x27c840 game_engine_build_netgame_goal_spawn_influencer: the same body as
// game_engine_build_player_spawn_influencer (ICF folded them; both influence blocks are inner / outer radius, upper /
// lower height, weight). A cylinder centered between the heights, axis-aligned, at the influencer's transform.
bool game_engine_build_netgame_goal_spawn_influencer(const s_netgame_goal_spawn_influence *influence, const real_matrix4x3 *matrix,
    s_dynamic_spawn_influencer *out)
{
    (void)matrix;
    const float height = fabsf(influence->lower_height) + influence->upper_height;
    const float offset = influence->upper_height - height * 0.5f;
    if (fabsf(offset) > 0.0001f) {
        real_point3d point;
        point.x = 0.0f;
        point.y = 0.0f;
        point.z = offset;
        matrix4x3_transform_point(&out->transform, &point, &out->transform.position);
    }
    out->transform.forward = k_real_math_identity_matrix_storage.forward;
    out->transform.left = k_real_math_identity_matrix_storage.left;
    out->transform.up = k_real_math_identity_matrix_storage.up;
    out->shape = 0;
    out->inner_radius = influence->inner_radius;
    out->outer_radius = influence->outer_radius;
    out->height = height;
    out->weight = influence->weight;
    return true;
}

// ---- 0x3a9300: one function under two source names. vsnprintf into a 33-byte buffer (32 characters), terminated;
// returns the buffer. (network_banhammer.cpp prints a XUID with it, online_files.cpp a CRC and a map version.)
static char *probe_format_value_33(char *buffer, const char *format, va_list arguments)
{
    vsnprintf(buffer, 32, format, arguments);
    buffer[32] = 0;
    return buffer;
}
extern "C" const char *netdebug_upload_format_value(char *buffer, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char *result = probe_format_value_33(buffer, format, arguments);
    va_end(arguments);
    return result;
}
extern "C" int network_banhammer_print_identifier(char *buffer, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char *result = probe_format_value_33(buffer, format, arguments);
    va_end(arguments);
    return (int)result;
}

// ---- g_impacts_globals_allocator's members (havok.cpp's specialization on __tls_set_g_impacts_globals_allocator) ----
// impacts.cpp defines allocate (0x24a670) and deallocate (0x1159e0) on the specialization spelled with
// __tls_set_g_impact_globals_allocator, a second source name for the same setter; the object's own specialization
// has neither. The same bodies:
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_impacts_globals_allocator> probe_impacts_globals_allocator;
template<> void *probe_impacts_globals_allocator::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
template<> void probe_impacts_globals_allocator::deallocate(void *address)
{
    (void)address;
    const long member_index = m_member_index;
    g_impacts_globals = 0;
    restricted_region_free_member(1, member_index);
}

// (c_http_stored_buffer_downloader<128>::operator= now defined in network_http_buffer_downloader.h)

// ---- 0x86ca0 tag_resource_interop_verifier_initialize (= ??0c_tag_resource_interop_verifier@@QAA@ZZ): the
// variadic constructor clears the three buffers (it reads none of its arguments).
extern "C" void tag_resource_interop_verifier_initialize(c_tag_resource_interop_verifier *verifier,
    c_basic_buffer<void>, c_basic_buffer<void>, c_basic_buffer<void>)
{
    memset(verifier, 0, sizeof(*verifier));
}
