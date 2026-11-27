#include "math/real_math_globals.h"
#include "effects/player_effect_tls.h"
// blamlibxbox/physics_constants.obj: from the records 0x2261b8 0x226208 0x226220 0x226228 0x2262c8 0x226300 0x2263a0 0x23e3d0 0x23f310
// @address 0x11df6e4 ?g_physics_constants@@3PAUs_physics_constants@@A
// @flags /TP /O2 /Oc
#include "physics/physics_constants_tls.h"
#include "math/real_math_types.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "game/game_globals_storage.h"
// @address 0x81428 cos
// @address 0x74b708 ?global_zero_vector3d@@3PBTreal_vector3d@@B
// @address 0x2261a8 ?__tls_set_g_physics_constants_allocator@@YAXPAX@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x226208 ?physics_constants_dispose@@YAXXZ
// @address 0x2262c8 ?global_slip_surface_maximum_k_get@@YAMXZ
// @address 0x23e3d0 ?__tls_set_g_player_effect_globals_allocator@@YAXPAX@Z
#include "cache/restricted_memory_regions.h"
#include "main/main.h"
#include "physics/physics_constants.h"


#include "physics/physics_constants_types.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_layout.h"
#include "physics/physics_constants_globals.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include "effects/player_effects.h"
#include <math.h>

static const float k_default_gravity = 4.1712594f;
static const float k_default_air_density = 0.0011f;


void physics_constants_dispose(void)
{
    g_physics_constants = 0;
}

float global_slip_surface_maximum_k_get(void)
{
    if (tls_game_globals()->options.game_mode == 2) {
        return 1.0f;
    }
    return g_physics_constants->slip_surface_maximum_k;
}

void __tls_set_g_player_effect_globals_allocator(void *allocator)
{
    g_player_effect_globals = (s_player_effect_globals *)allocator;
}

// @address 0x226300 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_physics_constants_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_physics_constants_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(
      &g_restricted_regions[3], name, type, size, 0, __tls_set_g_physics_constants_allocator,
      0, 0);
  unsigned int offset = restricted_region_get_member_offset(3, member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_physics_constants = (s_physics_constants *)address;
  return restricted_region_get_member_address(3, member_index);
}

// @address 0x2263a0 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_physics_constants_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_physics_constants_allocator>::allocate(
    unsigned int size, const char *name)
{
  return (void *)reserve_memory(name, "", size, 0);
}


// @address 0x23f310 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_effect_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effect_globals_allocator>::deallocate(
    void *memory)
{
  int value = m_member_index;
  g_player_effect_globals = 0;
  restricted_region_free_member(3, value);
}

// @address 0x226228 ?physics_constants_reset@@YAXXZ
void physics_constants_reset(void)
{
    g_physics_constants->gravity = k_default_gravity;
    g_physics_constants->water_density = 1.0f;
    g_physics_constants->air_density = k_default_air_density;
    g_physics_constants->character_ground_adhesion_forces_disable_time = 0;
    g_physics_constants->velocity_frame = *global_zero_vector3d;
    g_physics_constants->slip_surface_maximum_k = (float)cos((double)g_default_slip_surface_maximum_angle);
}

// @address 0x2261b8 ?physics_constants_initialize@@YAXXZ
void physics_constants_initialize(void)
{
    g_physics_constants = (s_physics_constants *)g_physics_constants_allocator.reserve_memory(
        "physics constants", 0, sizeof(s_physics_constants), 0);
    physics_constants_reset();
}

// @address 0x226220 ?physics_constants_initialize_for_new_map@@YAXXZ
void physics_constants_initialize_for_new_map(void)
{
    physics_constants_reset();
}

// @address 0x5bec98 ?physics_constants_dispose_from_old_map@@YAXXZ
void physics_constants_dispose_from_old_map(void)
{
}

float g_default_slip_surface_maximum_angle = 0.61086524f;
t_physics_constants_allocation_manager g_physics_constants_allocator;

__declspec(thread) s_physics_constants *g_physics_constants;
