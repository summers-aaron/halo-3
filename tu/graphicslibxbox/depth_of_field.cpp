// graphicslibxbox/depth_of_field.obj: from the records 0x112d48 0x44a490 0x44a4a0 0x44a560 0x44a578 0x44a788 0x44a900 0x44a9a0 0x44a9b8
// @flags /TP /O2
#include <math.h>
#include "render/depth_of_field.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions.h"

#include "render/depth_of_field_types.h"
#include "game/game_time_types.h"

struct s_depth_of_field_focus {
  int flags;
  float near_distance;
  float far_distance;
  float transition;
  float scale;
};

#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"




#include "cache/restricted_memory_registration.h"
#include "memory/tls_globals.h"
// @address 0x49fa4 k_depth_of_field_globals_name
// @rdata 0x82049fa4..0x82049fb0
extern "C" {
const char k_depth_of_field_globals_name[] = "DOF globals";
}
// @rdata-end
// @address 0x11df764 g_depth_of_field_game_globals
extern "C" {
__declspec(thread) s_depth_of_field *g_depth_of_field_game_globals;
}


// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z

// Allocator of the restricted-memory template family (region 1, layout 1, thread-local depth of field globals).


struct s_depth_of_field_fraction {
    float value;
};

void __tls_set_g_depth_of_field_game_globals_allocator(void *address)
{
    g_recycling_volumes = (s_depth_of_field *)address;
}

// @address 0x44a490 depth_of_field_set_globals
extern "C" void depth_of_field_set_globals(void *address)
{
    g_depth_of_field_game_globals = (s_depth_of_field *)address;
}


// Sets the defaults of a freshly reserved depth of field block (inlined into s_depth_of_field::initialize).
static __forceinline void depth_of_field_set_default_values(s_depth_of_field *dof)
{
  dof->enabled = false;
  dof->amount = 0.95f;
  dof->near_distance = 2.0f;
  dof->far_distance = 2.0f;
  dof->transition = 0.7f;
  dof->focus_a = 10.124f;
  dof->focus_b = 10.124f;
  dof->a = 0;
  dof->b = 0;
  dof->unknown = false;
  dof->c = 0;
  dof->d = 0;
  dof->e = 0.0f;
  dof->focus_distance = 0.0f;
  dof->g = -5.0f;
  dof->h = -5.0f;
  dof->i = 8.0f;
  dof->focus_distance = 0.0f;
  dof->g = -5.0f;
  dof->h = -5.0f;
  dof->i = 8.0f;
}


// @address 0x44a4a0 ?initialize@s_depth_of_field@@SAXXZ
void s_depth_of_field::initialize(void)
{
  g_depth_of_field_game_globals = (s_depth_of_field *)g_depth_of_field_game_globals_allocator.reserve_memory(
      k_depth_of_field_globals_name, 0, 64, 0);
  depth_of_field_set_default_values(g_depth_of_field_game_globals);
}

// @address 0x5bec98 ?dispose@s_depth_of_field@@SAXXZ
void s_depth_of_field::dispose(void)
{
}

// @address 0x44a560 ?initialize_for_new_map@s_depth_of_field@@SAXXZ
void s_depth_of_field::initialize_for_new_map(void)
{
    *(unsigned char *)g_depth_of_field_game_globals = 0;
}

// @address 0x5bec98 ?dispose_from_old_map@s_depth_of_field@@SAXXZ
void s_depth_of_field::dispose_from_old_map(void)
{
}

// @address 0x44a788 depth_of_field_compute_blur_sigma
extern "C" float depth_of_field_compute_blur_sigma(s_depth_of_field_focus *focus)
{
    float distance = g_depth_of_field_game_globals->focus_distance;
    if (focus != 0 && (focus->flags & 1) != 0) distance = focus->scale * 10.124f;
    float square = distance * distance;
    if (sqrtf(distance * distance - 2.5f) <= 0.004f) return 0.004f;
    return sqrtf(square - 2.5f);
}

// Registers a member of field layout 3 and publishes its per-thread address; the update callback is depth_of_field_set_globals.
// @address 0x44a900 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_depth_of_field_game_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_depth_of_field_game_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0, depth_of_field_set_globals, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(3, member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_depth_of_field_game_globals = (s_depth_of_field *)address;
  return restricted_region_get_base_address(3) + restricted_region_get_member_offset(3, member_index);
}

// @address 0x44a9a0 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_depth_of_field_game_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_depth_of_field_game_globals_allocator>::allocate(unsigned int size, const char *name)
{
  return reserve_memory(name, "", size, 0);
}

// Frees the member of region 3 that the allocation holds and clears the thread's slot.
// @address 0x44a9b8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_depth_of_field_game_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_depth_of_field_game_globals_allocator>::deallocate(void *memory)
{
  long value = m_member_index;
  g_depth_of_field_game_globals = 0;
  restricted_region_free_member(3, value);
}


// @address 0xc68 literal_3_0
// @address 0x1258 k_depth_of_field_smoothstep_two

#define DEPTH_OF_FIELD_MIN(a, b) ((a) < (b) ? (a) : (b))
#define DEPTH_OF_FIELD_MAX(a, b) ((a) > (b) ? (a) : (b))
#define DEPTH_OF_FIELD_PIN(x, lo, hi) DEPTH_OF_FIELD_MIN(DEPTH_OF_FIELD_MAX(x, lo), hi)

static __forceinline s_depth_of_field_fraction depth_of_field_smooth_fraction(long start, long end, long now)
{
    long span = end - start;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x44a578: decl-only values keep the 3.0/2.0 lis/lfd loads hoisted above subf; visible values sink them and cascade r10->r9, fr11->fr12
    double two = k_depth_of_field_smoothstep_two;
    double three = literal_3_0;
#else
    double two = 2.0;
    double three = 3.0;
#endif
    float t = (float)(now - start) / (float)(span > 1 ? span : 1);
    {
        s_depth_of_field_fraction result;
        result.value = ((float)((three - two * t) * t * t));
        return result;
    }
}

// @address 0x44a578 ?update_internal@s_depth_of_field@@AAAXXZ
void s_depth_of_field::update_internal(void)
{
    if (!enabled) return;

    long time = tls_game_time_globals()->time;
    if (time < a)
    {
        focus_distance = focus_b;
        b = time - 1;
        a = b;
    }

    float focus;
    if (time < b)
    {
        float fraction = depth_of_field_smooth_fraction(a, b, time).value;
        focus = focus_b * fraction + focus_a * (1.0f - fraction);
    }
    else
    {
        focus = focus_b;
    }

    float blend = 0.0f;
    focus_distance = focus;
    if (unknown)
    {
        if (time < c)
        {
            unknown = false;
            enabled = false;
            d = time - 1;
            c = d;
            e = 0.0f;
        }

        float alpha = 1.0f;
        if (time < d)
        {
            alpha = depth_of_field_smooth_fraction(c, d, time).value;
            if (alpha < 1.0f && e < 1.0f) blend = (1.0f - alpha) / (1.0f - e);
            e = alpha;
        }
        else
        {
            unknown = false;
        }
    }
    else
    {
        blend = DEPTH_OF_FIELD_PIN(amount, blend, 1.0f);
    }

    float inverse = 1.0f - blend;
    g = g * blend + near_distance * inverse;
    h = h * blend + far_distance * inverse;
    i = i * blend + transition * inverse;
}

depth_of_field_allocation_manager g_depth_of_field_game_globals_allocator;

