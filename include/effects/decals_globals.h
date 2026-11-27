#include "effects/decal_message_queue_tls.h"
#include "rasterizer/rasterizer_depth_bias_globals.h"
#ifndef EFFECTS_DECALS_GLOBALS_H
#define EFFECTS_DECALS_GLOBALS_H

#include "rasterizer/rasterizer_floating_point_depth.h"

#include "render/render_objects_globals.h"
#include "effects/effect_array_globals.h"

#include "rasterizer/rasterizer.h"

#include "game/game_globals_storage.h"

class c_decal;
class c_decal_system;
struct s_data_array;
#include "math/real_math_types.h"
struct s_collision_test_flags;
struct s_game_cluster_bit_vectors;

struct s_decals_pass_counts
{
    unsigned long system_count;
    unsigned long child_count;
    long first_cache_total;
    long second_cache_total;
};
typedef char decals_pass_counts_size[(sizeof(s_decals_pass_counts) == 0x10) ? 1 : -1];

extern "C" {
#include "memory/shared_resource_allocator.h"

extern bool g_decal_system_pvs_locked;
extern bool g_decal_force_offset;
extern const c_decal_system *g_decal_last_created_system;
extern bool g_decal_system_allocation_available[2];
extern unsigned char g_decal_allocation_available[2];
extern __declspec(thread) s_decals_pass_counts *g_decal_counts;
extern __declspec(thread) s_data_array *g_decal_array;
extern s_game_cluster_bit_vectors g_decal_system_pvs;
extern real_plane3d g_decals_scratch_plane;
extern const c_decal *g_decal_preplaced_render_list[][0x700 / 4];
extern const c_decal *g_decal_runtime_render_list[][0x400 / 4];
}

extern s_collision_test_flags k_decal_vector_test_mask;

#endif // EFFECTS_DECALS_GLOBALS_H
