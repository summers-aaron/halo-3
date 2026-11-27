// blamlibxbox/main_render.obj: from the records 0x13fb68 0x140640 0x140778 0x140890 0x140970 0x140a90 0x140b98 0x140d68 0x140de8 0x141130 0x141190 0x1413c0 0x141438 0x142420 0x142468 0x142500 0x142558 0x150da0 0x150eb8 0x20d0e0 0x20d230 0x20d3d8 0x20d630 0x20d7c0 0x20dd28 0x20ddf8 0x20de30 0x20dec0 0x20ded8 0x20def0 0x20df08 0x20df20 0x20df38 0x20df50 0x20dfd0 0x20e000 0x20e018 0x21f100 0x21f290 0x21fdb0 0x2413a0 0x449500 0x450c40 0x450f48 0x451168 0x454470 0x4547b8
#include "main/main_render_storage.h"
#include "render/object_render_message_queue_tls.h"
// @address 0x10b0 k_font_cache_vtable
// @address 0x7e7cb0 g_water_composite_shading_constants
// @address 0x11df874 g_decal_message_queue
#define XVIDEO_MODE main_render_sdk_video_mode
#include "bink/bink_playback_types.h"
#undef XVIDEO_MODE
#include "multithreading/threads.h"
#include "main/main_render_types.h"
#include "cache/predicted_resource_visitor.h"
#include "main/main_render_prediction_types.h"
#include "structures/preplaced_decal_types.h"
#include "render/water_interaction_tls.h"
#include "math/real_math_constants.h"
#include "objects/object_name_list.h"
// @compiler xdk6130
#include "main/main_render_externs.h"
#include "text/font_id_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "cache/cache_file_progress_types.h"
#include "memory/tls_data_arrays.h"
// @address 0xe70 ??_7c_ui_view@@6B@
// @address 0x10f5d30 g_main_render_default_postprocess_settings
// @flags /TP /O2 /Oc
#include "cutscene/cinematic_new_globals_types.h"
#include "objects/object_reference_types.h"
#include <math.h>
#include "game/game_globals_storage.h"
#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"
#include "game/game_types.h"
#include "game/game_globals.h"
#include <ppcintrinsics.h>
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/simulation.h"
#include "camera/observer.h"
#include "effects/effects.h"
#include "interface/closed_caption.h"
#include "interface/interface.h"
#include "main/main.h"
#include "main/main_render.h"
#include "main/main_render_globals.h"
#include "game/game_time.h"
#include "rasterizer/xenon/rasterizer_xenon_memory.h"
#include "rasterizer/rasterizer_globals.h"
#include "main/main_time.h"
#include "scenario/scenario_pvs_types.h"
#include "multithreading/synchronized_value.h"
#include "rasterizer/rasterizer_main.h"
#include "render/render_water.h"
#include "render/render_water_globals.h"
#include "scenario/scenario_lightmap_definitions.h"
#include "text/draw_string.h"
#include "units/vehicle_type_component.h"
#include "bink/bink_playback.h"
#include "models/render_model_definitions.h"
#include "camera/director.h"
#include "cutscene/cinematics.h"
#include "scenario/scenario_tag.h"
#include "objects/cluster_reference.h"
#include "geometry/render_geometry_types.h"
#include "effects/decals.h"
#include "gpu_particle/particle_block.h"
#include "interface/user_interface.h"
#include "render/render.h"
#include "cache/predicted_resources.h"
#include "render/render_lens_flares.h"
#include "render/screen_postprocess_xenon.h"
#include "render/screen_postprocess.h"
#include "render/views/render_player_view.h"
#include "render/views/render_view.h"
#include "render/views/patchy_fog.h"
#include "scenario/scenario_geometry.h"
#include "tag_files/tag_resource_cache_paging.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header_access.h"
#include "scenario/scenario_pvs.h"
#include "memory/data_types.h"
#include "memory/lruv_cache_types.h"
#include "memory/lruv_block_types.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "render/render_text_style_types.h"
#include "cache/cache_files.h"
#include "input/input_abstraction.h"
#include "networking/logic/network_session_interface.h"
#include "game/player_mapping_types.h"
#include "input/input_abstraction_types.h"
#include "visibility/visibility_collection.h"
#include "camera/observer_globals_types.h"
#include "main/main_time_types.h"
#include "main/game_tick_time_samples.h"
#include "structures/structure_bsp_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "cache/tag_resource_cache_internal_handle.h"
#include "objects/object_datum.h"
#include "scenario/scenario_lightmap_types.h"
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "effects/effect_pending_pool.h"
#include "game/game_tuning_types.h"
#include "rasterizer/rasterizer.h"
#include "game/game_time_types.h"
#include "game/player_mapping_inline.h"
#include "camera/director_globals_types.h"
#include "rasterizer/rasterizer_global_definitions.h"
class c_tag_resource_collector;
#include "game/players_globals.h"
#include "cache/render_model_region_types.h"
#include "tag_files/tag_resource_cache_controller_demand_collector.h"
class c_simple_tag_resource_predictor : public c_visitor
{
public:
    enum { k_group_flags_size = 0 };
    inline c_simple_tag_resource_predictor(bool print_messages, long demand_type, c_tag_resource_demand_collector *collector)
    {
        m_active = false;
        m_demand_type = demand_type;
        m_atom_count = 0;
        m_print_messages = print_messages;
        m_unique_atom_count = 0;
        m_collector = collector;
        m_flags.clear();
        // There are no group flags, so the
        // clear is zero-sized. It emits no code but still counts in c2's symbol numbering.
        memset(&m_flags, 0, k_group_flags_size);
    }
    __forceinline ~c_simple_tag_resource_predictor() {}
    virtual long begin(long tag_index, long region_index, long permutation_index);
    virtual unsigned char has_handle(long handle);
    virtual void enter(long handle);
    virtual void leave();
    virtual void add_resource(const s_tag_resource *resource);
    virtual void visit(int index);
    __forceinline c_tag_resource_collector *structure_collector() { return (c_tag_resource_collector *)this; }

private:
    c_static_flags_no_init<65536> m_flags;
    bool m_print_messages;
    bool m_active;
    long m_demand_type;
    long m_atom_count;
    long m_unique_atom_count;
    c_tag_resource_demand_collector *m_collector;
};
// The seam query initializes the stack; do not clear it before the query.
#include "structures/structure_seam_cluster_stack.h"
struct s_main_render_cluster_result {
    s_cluster_stack stack;
    __forceinline s_main_render_cluster_result() : stack(s_cluster_stack::_uninitialized) {}
    long end_index;
};
#include "structures/structure_seams.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "bitmaps/bitmap_data.h"
#include "bitmaps/bitmap_hardware_resource.h"
extern "C" {
#include "multithreading/interlocked_slist.h"
#include "multithreading/interlocked_slist_push.h"
// @address 0x8c4038 g_effect_render_message_count
// @address 0xa31760 g_effect_pending_pool
}
#include "effects/decal_message_types.h"
#include "effects/effect_message.h"
#include "objects/object_render_thread_message.h"
#include "multithreading/message_queue.h"
#include "effects/decal_system_types.h"
#include "rasterizer/xenon/rasterizer_xenon_memory_types.h"
// Hands every collector callback straight to the demand collector.
class c_demand_collector_visitor : public c_visitor
{
public:
    __forceinline c_demand_collector_visitor(c_tag_resource_demand_collector *collector) : m_collector(collector) {}
    virtual long begin(long tag_index, long region_index, long permutation_index);
    virtual unsigned char has_handle(long handle);
    virtual void enter(long handle);
    virtual void leave();
    virtual void add_resource(const s_tag_resource *resource);
    virtual void visit(int index);

private:
    c_tag_resource_demand_collector *m_collector;
};
#include "camera/cinematic_scene_types.h"
#include "main/main_render_loading_types.h"
#include "main/levels_map_datum_types.h"
#include "main/levels_globals.h"
#include "render/ui_view.h"
#include "render/views/simple_view_types.h"
struct mr_synchronized_long {
    volatile long m_value;
    __forceinline long peek() const
    {
        long value = m_value;
        __lwsync();
        return value;
    }
};
template<long k_bit_count> class mr_synchronized_bitvector {
    mr_synchronized_long m_bits[(k_bit_count + 31) / 32];
public:
    bool test_bit(long index) const
    {
        long word = m_bits[index >> 5].peek();
        const long &bit = 1 << (index & 31);
        return (word & bit) != 0;
    }
};
template<class t_message, long k_count> struct mr_message_queue {
    mr_synchronized_long m_count;
    mr_synchronized_bitvector<k_count> m_valid;
    t_message m_messages[k_count];
};
struct s_font_header;
#include "text/font_cache_lock.h"
#include "text/font_cache_thread_state_types.h"
#include "text/font_cache_types.h"

#include "text/draw_string_types.h"
#include "text/rasterizer_draw_string_types.h"
#include "rasterizer/d3d_device.h"
struct s_cinematics_scene;
#include "main/main_render_resources.h"
#include "interface/first_person_weapon_runtime_types.h"
#include "interface/first_person_weapon_tls.h"
struct s_main_render_first_person_render_info {
    long hands_model;
    long weapon_models[2];
};
#include "scenario/cluster_reference_helpers.h"
#include "rasterizer/rasterizer_timing_types.h"
#include "tag_files/tag_resource_control_datum.h"
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_cache_new.h"
typedef c_wrapped_flags s_main_render_deferred_flags;
struct s_main_render_address_interface {
    c_tag_resource_address_cache *cache;
    __forceinline void mark_demanded_resource(long resource_handle)
    {
        cache->m_demanded_resources.set(datum_index_to_absolute_index(resource_handle), true);
    }
};
#include "render/views/player_view_types.h"
#include "render/views/world_view_types.h"
#include "effects/light_volume_gpu.h"
#include "gpu_particle/particle_emitter_gpu.h"
#include "gpu_particle/contrail_gpu_types.h"
#include "render/water_renderer.h"
#include "render/render_player_view_globals.h"
class c_camera_fx_values;
// @address 0x72cd36 g_main_render_fence_enabled
bool g_main_render_fence_enabled = true;
// @address 0x72cd37 g_main_render_fence_active
bool g_main_render_fence_active = true;
// @address 0x72f783 g_main_render_predict_first_person
bool g_main_render_predict_first_person = true;
// @address 0x72f7e2 g_main_render_predict_structure
bool g_main_render_predict_structure = true;
// @address 0x72f7e3 g_main_render_predict_pvs
bool g_main_render_predict_pvs = true;
// @address 0x72fafa g_main_render_predict_objects
bool g_main_render_predict_objects = true;
// @address 0x72fafb g_main_render_predict_cinematic
bool g_main_render_predict_cinematic = true;
// @address 0x7c5d7c g_main_pregame_frame_scales
float g_main_pregame_frame_scales[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
// @address 0x832a84 g_main_render_gpu_fence
unsigned long g_main_render_gpu_fence;
// @address 0x833fc8 g_main_render_loading_globals
s_main_render_loading_globals g_main_render_loading_globals;
// @address 0x8c4711 g_main_render_gpu_fence_inserted
bool g_main_render_gpu_fence_inserted;
// @address 0x8c4712 g_main_render_rasterizer_reset
signed char g_main_render_rasterizer_reset;
// @address 0x8c4713 g_main_render_force_default_tuning
signed char g_main_render_force_default_tuning;
// @address 0x119a408 ?g_main_render_lag_counter@@3JA
long g_main_render_lag_counter;
// @address 0x35ae0 k_main_pregame_frame_colors
// @rdata 0x82035ae0..0x82035c24
const s_main_pregame_frame_colors k_main_pregame_frame_colors[] = {
    { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
    { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
    { { 0.0f, 0.2f, 0.0f }, { 0.55f, 0.45f, 0.65f }, { 0.08f, 0.05f, 0.012f } },
    { { 0.047058824f, 0.06666667f, 0.1254902f }, { 1.0f, 1.0f, 1.0f }, { 0.015686275f, 0.023529412f, 0.039215688f } },
    { { 0.0f, 0.0f, 0.0f }, { 0.55f, 0.45f, 0.65f }, { 0.08f, 0.05f, 0.012f } },
    { { 0.0f, 0.0f, 0.0f }, { 0.55f, 0.45f, 0.65f }, { 0.08f, 0.05f, 0.012f } },
    { { 0.29f, 0.06f, 0.36f }, { 0.55f, 0.45f, 0.65f }, { 0.08f, 0.05f, 0.012f } },
    { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
    { { 0.047058824f, 0.06666667f, 0.1254902f }, { 1.0f, 1.0f, 1.0f }, { 0.015686275f, 0.023529412f, 0.039215688f } },
};
// @rdata-end
namespace {
__forceinline bool main_render_game_paused()
{
    bool paused = false;
    if (cinematic_globals)
        paused = cinematic_globals->in_progress;
    return paused;
}
}
// @address 0x140778 main_render_get_view_layout
extern "C" s_main_render_view_layout *main_render_get_view_layout(s_main_render_view_layout *layout)
{
    layout->current_camera_index = 0;
    layout->current_user_index = -1;
    layout->current_observer_result = 0;
    if (main_render_game_paused() || tls_players_globals()->suppressed) {
        layout->view_count = 1;
    } else {
        layout->view_count = PIN(tls_player_mapping_globals()->local_player_count, 1, 4);
    }
    if (layout->view_count > 1) {
        if (rasterizer_main_get_video_mode()->fIsWideScreen)
            layout->layout = 2;
        else
            layout->layout = 1;
    } else {
        layout->layout = 0;
    }
    return layout;
}
// @address 0x140970 main_render_predict_geometry_resources
extern "C" void main_render_predict_geometry_resources(c_visitor *visitor, long tag)
{
    if (tag == -1)
        return;
    long handle = visitor->begin(tag, 0, 0);
    if (visitor->has_handle(handle))
        return;
    visitor->enter(handle);
    signed char slots[16];
    slots[0] = 0;
    memset(&slots[1], 0, sizeof(slots) - sizeof(slots[0]));
    render_model_definition *definition = (render_model_definition *)tag_get('mode', tag);
    for (long index = 0; index < definition->regions.count; index++) {
        signed char slot = slots[index];
        if (slot != -1)
            predicted_resources_add_resources_from_render_geometry_parts(visitor, &definition->render_geometry,
                &definition->materials,
                ((const s_render_model_permutation *)((const render_model_region *)definition->regions.address)[index].permutations.address)[slot].start_index);
    }
    visitor->leave();
}
// @address 0x140a90 ?begin@c_simple_tag_resource_predictor@@UAAJJJJ@Z
long c_simple_tag_resource_predictor::begin(long tag_index, long region_index, long permutation_index)
{
    return m_collector->get_slot_from_lookup(tag_index, region_index, permutation_index);
}
// @address 0x140b98 ?has_handle@c_simple_tag_resource_predictor@@UAAEJ@Z
unsigned char c_simple_tag_resource_predictor::has_handle(long handle)
{
    unsigned char result = true;

    if (handle != -1)
    {
        unsigned short i = (unsigned short)handle;
        result = m_flags.test(i);
        if (!result)
        {
            result = m_collector->test_active_bit(handle);
            if (result)
                m_flags.set(i, true);
        }
    }

    return result;
}
// @address 0x140d68 main_render_predict_model_element
extern "C" void main_render_predict_model_element(c_visitor *visitor, long tag, long index)
{
    if (tag != -1) {
        const s_game_globals *definition = (const s_game_globals *)tag_get('matg', tag);
        if (index >= 0 && index < definition->player_representations.count()) {
            const s_player_representation *element = &definition->player_representations[index];
            main_render_predict_geometry_resources(visitor, element->first_person_body.index);
            main_render_predict_geometry_resources(visitor, element->first_person_hands.index);
        }
    }
}
// @address 0x141130 main_render_get_cluster_seams
extern "C" s_main_render_cluster_result *main_render_get_cluster_seams(
    s_main_render_cluster_result *result, long scenario_tag, const s_cluster_reference *reference)
{
    result->end_index = -1;
    result->stack.m_count = 0;
    if (!structure_seams_connected_cluster_references_get(scenario_tag, *reference, true, &result->stack))
        result->stack.m_count = 0;
    return result;
}
// @address 0x1413c0 main_render_submit_model_sections
extern "C" void main_render_submit_model_sections(c_visitor *target, unsigned long tag_handle)
{
    if (tag_handle == 0xffffffff)
        return;
    const bitmap_group *definition = (const bitmap_group *)tag_get('bitm', tag_handle);
    if ((definition->flags & 0x10) == 0)
        return;
    for (long section_index = 0; section_index < definition->bitmaps.count; section_index++)
        main_render_submit_model_section(target, tag_handle, section_index);
}
// @address 0x141438 main_render_submit_model_section
extern "C" void main_render_submit_model_section(c_visitor *target, unsigned long tag_handle, long section_index)
{
    if (tag_handle == 0xffffffff)
        return;
    const bitmap_group *definition = (const bitmap_group *)tag_get('bitm', tag_handle);
    if (section_index < 0)
        return;
    if (section_index >= definition->bitmaps.count)
        return;
    if ((definition->flags & 0x10) == 0)
        return;
    const bitmap_data *element = &((const bitmap_data *)definition->bitmaps.address)[section_index];
    if ((element->runtime_flags & 0x40) != 0)
        target->add_resource((const s_tag_resource *)
            &((const s_bitmap_hardware_resource *)definition->interleaved_hardware_textures.address)[element->interleaved_resource_index]);
    else
        target->add_resource((const s_tag_resource *)&((const s_bitmap_hardware_resource *)definition->hardware_textures.address)[section_index]);
}
bool c_player_render_camera_iterator::next()
{
    bool result = m_current_camera_index < m_camera_count;
    if (result) {
        ++m_current_camera_index;
        ++m_current_user_index;
        for (; m_current_user_index < 4 && !players_user_is_active(m_current_user_index); ++m_current_user_index) {
        }
        if (m_current_user_index >= 4) {
            m_current_user_index = -1;
            result = false;
        }
        if (m_current_user_index != -1)
            m_current_observer_result = &observer_get_globals()->blocks[m_current_user_index].camera_result;
    }
    return result;
}
namespace {
__forceinline void reset_object_render_message_queue()
{
    g_object_message_queue->clear();
}
}
__forceinline void c_decal_system::reset_render_message_queue()
{
    g_decal_message_queue->clear();
}
void main_render_purge_pending_messages()
{
    reset_object_render_message_queue();
    main_render_purge_effect_messages(false);
    c_decal_system::reset_render_message_queue();
}
// @address 0x142468 ?main_render_purge_effect_messages@@YAX_N@Z
void main_render_purge_effect_messages(bool flush_pending)
{
    g_effect_render_message_count = 0;
    ((t_message_queue<s_effect_message, 512> *)g_effect_message_queue)->clear();
    if (flush_pending) {
        PSLIST_ENTRY entry = InterlockedFlushSList((PSLIST_HEADER)&g_effect_pending_pool.pending);
        __lwsync();
        while (entry) {
            PSLIST_ENTRY next = entry->Next;
            __lwsync();
            InterlockedPushEntrySList(&g_effect_pending_pool.free_list, entry);
            entry = next;
        }
    }
}
// @address 0x142500 ?main_render_update_last_milliseconds@@YAXXZ
void main_render_update_last_milliseconds()
{
    __int64 milliseconds = GetTickCount();
    milliseconds |= g_main_time_globals->milliseconds & 0xffffffff00000000ui64;
    if (milliseconds < g_main_time_globals->milliseconds)
        milliseconds += 1i64 << 32;
    g_main_time_globals->last_system_milliseconds = milliseconds;
}
// @address 0x10f5d70 g_rasterizer_xenon_memory

// @address 0x150da0 main_render_sync_rasterizer_memory
extern "C" void main_render_sync_rasterizer_memory(c_rasterizer_xenon_memory *memory)
{
    if (c_rasterizer::g_tiling_configuration != g_rasterizer_xenon_memory.m_tiling_configuration) {
        g_rasterizer_xenon_memory.m_tiling_configuration = c_rasterizer::g_tiling_configuration;
        g_rasterizer_xenon_memory.reset();
    }
    bool alternate_entry_point = false;
    if (!main_render_game_paused() && !tls_players_globals()->suppressed) {
        alternate_entry_point = ((main_render_timing_settings() >> 1) & 1) != 0;
    }
    if (alternate_entry_point != g_rasterizer_xenon_memory.alternate_entry_point) {
        g_rasterizer_xenon_memory.alternate_entry_point = alternate_entry_point;
        g_rasterizer_xenon_memory.reset();
    }
    if (g_main_render_rasterizer_reset != 0) {
        g_rasterizer_xenon_memory.reset();
        g_main_render_rasterizer_reset = 0;
    }
    g_rasterizer_xenon_memory.submit_count = 0;
    g_rasterizer_xenon_memory.collision_count = 0;
}
// @address 0x737548 g_game_tuning_players
// @address 0x150eb8 main_render_update_timing_config
static __forceinline void player_tuning_select_from_tag(long tuning_tag, long player_count, const s_player_tuning **tuning)
{
    const s_tag_block *player_tunings = (const s_tag_block *)tag_get('perf', tuning_tag);
    if (player_count <= player_tunings->count)
        *tuning = &((const s_player_tuning *)player_tunings->address)[player_count - 1];
}
#if !defined(_XBOX) || defined(__clang__)
extern "C" int __cdecl port_config_splitscreen_full_quality(void);
#endif
extern "C" void main_render_update_timing_config(void)
{
    long player_count;
    if (!main_render_game_paused() && !tls_players_globals()->suppressed) {
        player_count = tls_player_mapping_globals()->local_player_count;
        if ((short)player_count > 1)
            player_count = MAX(MIN(player_count, 4), 1);
        else
            player_count = 1;
#if !defined(_XBOX) || defined(__clang__)
        if (port_config_splitscreen_full_quality() != 0)
            player_count = 1;
#endif
    } else {
        player_count = 1;
    }
    const s_player_tuning *tuning = &g_game_tuning_players[player_count - 1];
    if (!g_main_render_force_default_tuning) {
        long tuning_tag = global_scenario->game_tuning.index;
        if (tuning_tag != -1) {
            player_tuning_select_from_tag(tuning_tag, player_count, &tuning);
        } else {
            long rasterizer_globals_tag = global_game_globals->rasterizer_globals_ref.index;
            const c_rasterizer_globals *rasterizer_globals = rasterizer_globals_tag != -1
                ? (const c_rasterizer_globals *)tag_get('rasg', rasterizer_globals_tag)
                : NULL;
            long default_tuning_tag = rasterizer_globals->m_default_performance_template.index;
            if (default_tuning_tag != -1)
                player_tuning_select_from_tag(default_tuning_tag, player_count, &tuning);
        }
    }
    memcpy(&main_render_timing_settings(), tuning, sizeof(*tuning));
}
inline bool c_pvs_cluster_iterator::valid_cluster_reference() const
{
    return m_structure_bsp_index < 16 && m_cluster_index < m_cluster_counts[m_structure_bsp_index];
}
inline bool c_pvs_cluster_iterator::visible_cluster_reference() const
{
    return valid_cluster_reference() && m_cluster_counts[m_structure_bsp_index] > 0 &&
        (m_cluster_bits[m_structure_bsp_index][m_cluster_index >> 5] & (1 << (m_cluster_index & 31))) != 0;
}

__forceinline bool c_pvs_cluster_iterator::go_to_next_cluster()
{
    m_cluster_index++;
    while (m_structure_bsp_index < 16 && !valid_cluster_reference()) {
        m_cluster_index = 0;
        m_structure_bsp_index++;
    }
    return m_structure_bsp_index < 16 && m_cluster_index < m_cluster_counts[m_structure_bsp_index];
}

bool c_pvs_cluster_iterator::next()
{
    while (go_to_next_cluster() && !visible_cluster_reference()) {
    }
    bool result = visible_cluster_reference();
    return result;
}
// @address 0x20dec0 ?begin@c_demand_collector_visitor@@UAAJJJJ@Z
long c_demand_collector_visitor::begin(long tag_index, long region_index, long permutation_index)
{
    return m_collector->get_slot_from_lookup(tag_index, region_index, permutation_index);
}
// @address 0x20ded8 ?has_handle@c_demand_collector_visitor@@UAAEJ@Z
unsigned char c_demand_collector_visitor::has_handle(long handle)
{
    return m_collector->test_active_bit(handle);
}
// @address 0x20def0 ?enter@c_demand_collector_visitor@@UAAXJ@Z
void c_demand_collector_visitor::enter(long handle)
{
    m_collector->set_active_slot(handle);
}
// @address 0x20df08 ?leave@c_demand_collector_visitor@@UAAXXZ
void c_demand_collector_visitor::leave()
{
    m_collector->update_active_slot();
}
// @address 0x20df20 ?add_resource@c_demand_collector_visitor@@UAAXPBUs_tag_resource@@@Z
void c_demand_collector_visitor::add_resource(const s_tag_resource *resource)
{
    m_collector->demand_resource(resource);
}
// @address 0x20df38 ?visit@c_demand_collector_visitor@@UAAXH@Z
void c_demand_collector_visitor::visit(int index)
{
    m_collector->set_key_bits(index);
}
// @address 0x20df50 ?enter@c_simple_tag_resource_predictor@@UAAXJ@Z
void c_simple_tag_resource_predictor::enter(long handle)
{
    if (handle != -1) {
        m_collector->set_active_slot(handle);
        long i = datum_index_to_absolute_index(handle);
        unsigned long *flags = m_flags.get_writeable_bits_direct();
        flags[i >> 5] |= 1 << (i & 31);
        m_active = true;
    }
}
// @address 0x20dfd0 ?leave@c_simple_tag_resource_predictor@@UAAXXZ
void c_simple_tag_resource_predictor::leave()
{
    if (m_active) {
        m_active = false;
        m_collector->update_active_slot();
    }
}
// @address 0x20e000 ?add_resource@c_simple_tag_resource_predictor@@UAAXPBUs_tag_resource@@@Z
void c_simple_tag_resource_predictor::add_resource(const s_tag_resource *resource)
{
    m_collector->demand_resource(resource);
}
// @address 0x20e018 ?visit@c_simple_tag_resource_predictor@@UAAXH@Z
void c_simple_tag_resource_predictor::visit(int index)
{
    m_collector->set_key_bits(index);
}

static __forceinline void main_render_set_instance_bit(unsigned long *bits, long index)
{
    bits[index >> 5] |= 1 << (index & 31);
}
// @address 0x21f100 main_render_predict_cluster_models
extern "C" void main_render_predict_cluster_models(c_visitor *collector, long scenario_index, s_cluster_reference cluster_reference)
{
    const struct scenario *scenario = (const struct scenario *)tag_get('scnr', scenario_index);
    unsigned long instance_mask[2];
    memset(instance_mask, 0, sizeof(instance_mask));
    const struct structure_bsp *bsp = (const struct structure_bsp *)tag_get('sbsp',
        ((const scenario_structure_bsp_reference *)scenario->structure_bsps.address)[cluster_reference.bsp_index].structure_bsp.index);
    const structure_cluster *cluster = &((const structure_cluster *)bsp->clusters.address)[cluster_reference.cluster_index];
    for (long instance_index = 0; instance_index < cluster->decorator_groups.count; instance_index++) {
        if (((const s_tag_reference *)bsp->decorator_sets.address)
            [((const s_decorator_group *)cluster->decorator_groups.address)[instance_index].decorator_set_index].index != -1)
            main_render_set_instance_bit(instance_mask,
                ((const s_decorator_group *)cluster->decorator_groups.address)[instance_index].decorator_set_index);
    }
    for (long definition_index = 0; definition_index < 48; definition_index++) {
        if (instance_mask[definition_index >> 5] & (1 << (definition_index & 31))) {
            const s_decorator_set *decorator_set = (const s_decorator_set *)tag_get('dctr',
                ((const s_tag_reference *)bsp->decorator_sets.address)[definition_index].index);
            main_render_submit_model_sections(collector, decorator_set->texture.index);
        }
    }
    const scenario_structure_bsp_reference *reference
        = &((const scenario_structure_bsp_reference *)scenario->structure_bsps.address)[cluster_reference.bsp_index];
    if (reference->decorator_model.index != -1) {
        for (long section_index = 0; section_index < cluster->cluster_cubemaps.count; section_index++)
            main_render_submit_model_section(collector, reference->decorator_model.index,
                ((const s_structure_cluster_cubemap_info *)cluster->cluster_cubemaps.address)[section_index].cubemap_bitmap_index);
    }
}

// @address 0x21fdb0 main_render_predict_tag_resources
extern "C" void main_render_predict_tag_resources(c_visitor *visitor, long tag)
{
    long handle = visitor->begin(tag, 17, 0);
    if (!visitor->has_handle(handle)) {
        visitor->enter(handle);
        visitor->visit(tag);
        visitor->leave();
    }
}
// @address 0x21f290 main_render_predict_scenario_tags
extern "C" void main_render_predict_scenario_tags(c_visitor *collector, long scenario_index, s_cluster_reference cluster_reference)
{
    const struct scenario *scenario = (const struct scenario *)tag_get('scnr', scenario_index);
    const scenario_structure_bsp_reference *reference
        = &((const scenario_structure_bsp_reference *)scenario->structure_bsps.address)[cluster_reference.bsp_index];
    unsigned long palette_flags[6];
    memset(palette_flags, 0, 4 * sizeof(palette_flags[0]));
    const struct structure_bsp *bsp = (const struct structure_bsp *)tag_get('sbsp', reference->structure_bsp.index);
    const structure_cluster *cluster = &((const structure_cluster *)bsp->clusters.address)[cluster_reference.cluster_index];
    unsigned long decal_count = (unsigned short)cluster->runtime_decal_count;
    if ((long)decal_count > 0) {
        const s_preplaced_decal *decal = (const s_preplaced_decal *)bsp->runtime_decals.address + cluster->runtime_first_decal_index;
        do {
            decal_count--;
            short palette_index = decal->palette_index;
            ++decal;
            palette_flags[palette_index >> 5] |= 1 << (palette_index & 31);
        } while (decal_count != 0);
    }
    for (long palette_index = 0; palette_index < scenario->decal_palette.count; palette_index++) {
        long decal_tag = ((const s_tag_reference *)scenario->decal_palette.address)[palette_index].index;
        if (decal_tag != -1)
            collector->visit(decal_tag);
    }
}
namespace {
__forceinline const struct scenario *main_render_prediction_scenario_get(long scenario_index)
{
    return (const struct scenario *)tag_get('scnr', scenario_index);
}
__forceinline struct structure_bsp *main_render_prediction_bsp_get(long scenario_index, long bsp_index)
{
    long structure_bsp = ((const scenario_structure_bsp_reference *)main_render_prediction_scenario_get(scenario_index)->structure_bsps.address)
        [bsp_index].structure_bsp.index;
    return (struct structure_bsp *)tag_get('sbsp', structure_bsp);
}
}
// @address 0x141190 ?predict_structure_cluster_resources@@YAXPAVc_tag_resource_collector@@JABUs_cluster_reference@@@Z
void predict_structure_cluster_resources(class c_tag_resource_collector * _sym_p0, long scenario_index, struct s_cluster_reference const & _sym_p2)
{
    const scenario_structure_bsp_reference *reference
        = (const scenario_structure_bsp_reference *)main_render_prediction_scenario_get(scenario_index)->structure_bsps.address +
        ((const s_cluster_reference *)&_sym_p2)->bsp_index;
    long group = ((c_visitor *)_sym_p0)->begin(reference->structure_bsp.index,
                         ((const s_cluster_reference *)&_sym_p2)->cluster_index, 0);
    if (!((c_visitor *)_sym_p0)->has_handle(group)) {
        ((c_visitor *)_sym_p0)->enter(group);
        {
            s_cluster_reference cluster = *((const s_cluster_reference *)&_sym_p2);
            struct structure_bsp *bsp = main_render_prediction_bsp_get(scenario_index, cluster.bsp_index);
            predicted_resources_add_resources_from_render_geometry_parts((c_visitor *)_sym_p0,
                ::scenario_structure_bsp_get_render_geometry(scenario_index, cluster.bsp_index), &bsp->materials,
                (int)((const structure_cluster *)bsp->clusters.address)[cluster.cluster_index].mesh_index);
        }
        {
            s_cluster_reference cluster = *((const s_cluster_reference *)&_sym_p2);
            struct structure_bsp *bsp = main_render_prediction_bsp_get(scenario_index, cluster.bsp_index);
            predicted_resources_add_resources_from_render_geometry_instances((c_visitor *)_sym_p0,
                ::scenario_structure_bsp_get_render_geometry(scenario_index, cluster.bsp_index), &bsp->materials,
                (int)((const structure_cluster *)bsp->clusters.address)[cluster.cluster_index].mesh_index);
        }
        main_render_predict_scenario_tags(((c_visitor *)_sym_p0), scenario_index, *((const s_cluster_reference *)&_sym_p2));
        main_render_predict_cluster_models(
            (c_visitor *)_sym_p0, scenario_index, *((const s_cluster_reference *)&_sym_p2));
        ((c_visitor *)_sym_p0)->leave();
    }
}
// @address 0x2413a0 main_render_predict_object_list
extern "C" void main_render_predict_object_list(s_cinematic_scene *list, long view_index, long unused, void *user_data)
{
    c_visitor *visitor = static_cast<c_visitor *>(user_data);
    for (long index = 0; index < list->objects.count; index++) {
        s_cinematic_scene_object *entry = &((s_cinematic_scene_object *)list->objects.address)[index];
        if ((entry->view_mask & (1 << view_index)) == 0)
            continue;
        if ((entry->flags & 2) == 0 && (entry->flags & 1) == 0) {
            predicted_resources_iterate_object_definition(visitor, entry->object.index);
            continue;
        }
        short name_index = scenario_object_name_index_from_string(global_scenario, entry->name);
        long object_index;
        if (name_index >= 0 && name_index < 0x800)
            object_index = ((long *)g_object_name_list)[name_index];
        else
            object_index = -1;
        predicted_resources_iterate_object_instance(visitor, object_index);
    }
}
static __forceinline float main_render_loading_average(float total)
{
    return total * 0.011111111f;
}
// @address 0x450c40 main_render_smooth_loading_progress
extern "C" float main_render_smooth_loading_progress(float value)
{
    long index = g_main_render_loading_globals.history_index;
    float sum = 0.0f;
    long next_index = (index + 1) % 90;
    for (long n = 0; n < 90; n++) {
        float weight = (float)((index + 90 - n) % 90) * 0.022222223f;
        sum += weight * g_main_render_loading_globals.history[n];
    }
    float average = main_render_loading_average(sum);
    g_main_render_loading_globals.history[index] = value;
    g_main_render_loading_globals.history_index = next_index;
    return average;
}

static __forceinline float main_render_approach_loading_rate(float value, float target, float change)
{
    float result = value;
    if (result < target)
    {
        result += change;
        if (result <= target)
            return result;
        result = target;
    }
    else
    {
        result -= change;
        if (result > target)
            return result;
        result = target;
    }
    return result;
}
static __forceinline void main_render_clamp_loading_rate(float *stored, float &rate, float minimum, float maximum)
{
    if (MAX(rate, minimum) < maximum)
    {
        if (rate > minimum)
            return;
        rate = minimum;
    }
    else
        rate = maximum;
    *stored = rate;
}
static __forceinline void main_render_stamp_progress(float *timestamp, float position, float previous, float current, float elapsed)
{
    if (position > 0.5f)
    {
        position = (1.0f - position) * 2.0f;
        if (position < previous)
            return;
    }
    else
    {
        position *= 2.0f;
        if (position < previous)
            return;
    }
    if (position < current)
        *timestamp = elapsed;
}
// @address 0x450f48 main_render_update_loading_progress
extern "C" void main_render_update_loading_progress(float value)
{
    unsigned long now = GetTickCount();
    float target_rate = 0.0f;
    float delta_elapsed = 0.0f;
    float elapsed = 0.0f;
    float previous_progress;
    float change = 0.0f;
    if (!g_main_render_loading_globals.started)
    {
        g_main_render_loading_globals.first_value = value;
        g_main_render_loading_globals.start_time = now;
        previous_progress = 0.0f;
        g_main_render_loading_globals.rate = 0.025f;
        g_main_render_loading_globals.started = true;
    }
    else
    {
        previous_progress = g_main_render_loading_globals.smoothed_progress;
        elapsed = (float)(now - g_main_render_loading_globals.start_time) * 0.001f;
        delta_elapsed = elapsed - g_main_render_loading_globals.last_elapsed;
    }
    float threshold = 0.25f;
    bool update_rate = false;
    float completed = g_main_render_loading_globals.first_value - value;
    g_main_render_loading_globals.last_value = value;
    if (elapsed >= threshold)
    {
        float estimated_duration = g_main_render_loading_globals.first_value / (completed < 0.1f ? 0.1f : completed) * elapsed;
        if (estimated_duration < 0.0001f)
            target_rate = 1.0f;
        else
            target_rate = (float)(1.0 - previous_progress) / (estimated_duration - elapsed);
        update_rate = true;
    }
    float rate = g_main_render_loading_globals.rate;
    if (update_rate)
    {
        change = rate * 0.05f;
        rate = main_render_approach_loading_rate(rate, target_rate, change);
        g_main_render_loading_globals.rate = rate;
    }
    main_render_clamp_loading_rate(&g_main_render_loading_globals.rate, rate, 1.0f / 180.0f, 1.0f / 6.0f);
    g_main_render_loading_globals.smoothed_progress = rate * delta_elapsed + previous_progress;
    g_main_render_loading_globals.progress = main_render_smooth_loading_progress(g_main_render_loading_globals.smoothed_progress);
    float current_progress = g_main_render_loading_globals.smoothed_progress;
    for (long index = 0; index < 2048; ++index)
    {
        float position = (float)index * (1.0f / 2048.0f);
        main_render_stamp_progress(&g_main_render_loading_globals.clear_values[index], position,
            previous_progress, current_progress, elapsed);
    }
    g_main_render_loading_globals.last_elapsed = elapsed;
    g_main_render_loading_globals.delta_elapsed = delta_elapsed;
    g_main_render_loading_globals.last_time = now;
}
// @address 0x454470 main_render_test_loading_buttons
extern "C" unsigned char main_render_test_loading_buttons(void)
{
    float first_button_amount;
    char buttons_held;
    buttons_held = 0;
    if (game_time_compute_holiday() == 9) {
        first_button_amount = (&input_abstraction_globals.input_states[0].buttons[8])->down_amount();
        if (first_button_amount > 0.0f && (&input_abstraction_globals.input_states[0].buttons[9])->down_amount() > 0.0f)
            buttons_held = 1;
    }
    return buttons_held;
}


static __forceinline float main_render_curve_average(const real_point2d *curve, float first, float second)
{
    float a = rasterizer_main_evaluate_curve(curve, first);
    float b = rasterizer_main_evaluate_curve(curve, second);
    return (a + b) * 0.5f;
}
// @address 0x4547b8 main_render_evaluate_loading_camera
extern "C" float main_render_evaluate_loading_camera(const s_main_render_camera_curves *curves, real_vector3d *position,
    real_vector3d *direction, float time, float projection_time, float *near_plane, float *far_plane)
{
    float before = 0.0f;
    time = PIN(time, 0.0f, 1000.0f);
    projection_time = PIN(projection_time, 0.0f, 1000.0f);
    float after = time + 0.1f;
    before = time - 0.1f;
    float position_angle = main_render_curve_average(curves->position_angle, after, before);
    float position_radius = main_render_curve_average(curves->position_radius, after, before);
    float position_height = main_render_curve_average(curves->position_height, after, before);
    if (main_render_test_loading_buttons())
        position_radius += 1.0f;
    float position_sine = (float)sin((double)position_angle);
    float position_cosine = (float)cos((double)position_angle);
    position->j = position_height;
    position->k = position_sine * position_radius;
    position->i = position_cosine * position_radius;
    float direction_angle = main_render_curve_average(curves->direction_angle, after, before);
    float direction_radius = main_render_curve_average(curves->direction_radius, after, before);
    float direction_before = rasterizer_main_evaluate_curve(curves->direction_height, before);
    float direction_after = rasterizer_main_evaluate_curve(curves->direction_height, after);
    float direction_sine = (float)sin((double)direction_angle);
    direction->i = (float)cos((double)direction_angle) * direction_radius;
    direction->k = direction_sine * direction_radius;
    direction->j = (direction_after + direction_before) * 0.5f;
    *near_plane = rasterizer_main_evaluate_curve(curves->near_plane, projection_time);
    *far_plane = rasterizer_main_evaluate_curve(curves->far_plane, projection_time);
    return rasterizer_main_evaluate_curve(curves->field_of_view, projection_time);
}

// @address 0x20d630 main_render_update_loading
extern "C" void main_render_update_loading()
{
    float progress;
    if (g_user_interface_networking_globals.loading) {
        e_campaign_id campaign;
        e_map_id map;
        levels_map_datum level;
        network_life_cycle_session_get_map(&campaign, &map);
        if (!levels_try_and_get_datum(g_level_globals.map_levels, map, &level))
            return;
        if (!g_main_render_loading_globals.initialized)
            render_initialize_loading_screen();
        long estimate = (long)cache_file_map_progress_helper(
            _scenario_type_campaign, level.name, _cache_file_progress_remaining);
        unsigned long remaining = ((unsigned long)estimate >> 2) * 1000;
        if (g_thread_owning_device.value != g_registered_thread_index || g_main_render_loading_globals.owner_thread != g_registered_thread_index)
            return;
        const float seconds_per_tick = k_real_one_thousandth_storage;
        progress = (float)(__int64)(long)remaining * seconds_per_tick;
    } else if (render_is_load_recently_complete()) {
        if (g_thread_owning_device.value != g_registered_thread_index || g_main_render_loading_globals.owner_thread != g_registered_thread_index)
            return;
        progress = 0.0f;
    } else {
        if (g_main_render_loading_globals.initialized) {
            g_main_render_loading_globals.initialized = false;
            g_main_render_loading_globals.owner_thread = -1;
        }
        return;
    }
    main_render_update_loading_progress(progress);
}



void c_fullscreen_view::render_setup()
{
    c_rasterizer::begin(get_rasterizer_camera()->fullscreen_pixel_bounds, get_rasterizer_camera()->fullscreen_pixel_bounds);
    render_setup_window(get_rasterizer_camera(), get_rasterizer_projection());
}

// @address 0x449500 ?render@c_fullscreen_view@@UAAXXZ
void c_fullscreen_view::render()
{
    rectangle2d display_bounds;
    rectangle2d title_safe_bounds;
    rectangle2d render_bounds;
    rasterizer_main_get_display_and_render_bounds(&display_bounds, &title_safe_bounds, &render_bounds, NULL);
    user_interface_render_windows(-1, -1, 4, &render_bounds, 1, false);
}



class c_view;
// @address 0x81dc04 g_view_stack



static __forceinline bool main_render_pregame_frame_has_text(e_main_pregame_frame frame)
{
    return frame == _main_pregame_frame_crash || frame == _main_pregame_frame_crash_upload_succeeded;
}

static __forceinline bool main_render_bink_active()
{
    return bink_globals.active && bink_globals.enabled;
}

// @address 0x20d230 ?main_render_pregame@@YAXW4e_main_pregame_frame@@PBD@Z
void main_render_pregame(e_main_pregame_frame frame, const char *text)
{
    unsigned long lock = _internal_halt_render_thread_and_lock_resources("..\\main\\main_render.cpp", 413);
    c_view::abort_current_view_stack();
    c_fullscreen_view view;
    view.setup_camera(NULL);
    c_view::begin(&view);
    c_fullscreen_view::render_blank_frame(&k_main_pregame_frame_colors[frame].background);
    s_render_text_style style;
    style.string = text;
    bool draw_text = true;
    style.color = &k_main_pregame_frame_colors[frame].text;
    style.shadow_color = &k_main_pregame_frame_colors[frame].shadow;
    style.scale = g_main_pregame_frame_scales[frame];
    if (frame != _main_pregame_frame_crash && frame != _main_pregame_frame_crash_upload_succeeded)
        draw_text = false;
    if (draw_text)
        render_draw_pregame_text(&style, main_render_pregame_frame_has_text(frame));
    if (frame == 1)
    {
        view.render();
        if (main_render_bink_active())
        {
            bink_playback_update();
            bink_playback_render_first_frame();
        }
    }
    c_view::end();
    unlock_resources_and_resume_render_thread(lock);
}

// @address 0x8c4034 g_effect_render_location_count
// @address 0x20d0e0 ?main_render_process_messages@@YAXXZ
void main_render_process_messages()
{
    if (g_thread_crashed[2].peek() != 1) {
        mr_message_queue<s_object_render_thread_message, 2048> *object_queue
            = (mr_message_queue<s_object_render_thread_message, 2048> *)g_object_message_queue;
        long object_message_count = object_queue->m_count.peek();
        for (long object_message_index = 0; object_message_index < object_message_count; object_message_index++) {
            if (object_queue->m_valid.test_bit(object_message_index))
                handle_object_render_message(&object_queue->m_messages[object_message_index], 0);
        }
        g_effect_render_location_count = 0;
        ((t_message_queue<s_effect_message, 512> *)g_effect_message_queue)->process_messages(handle_effect_render_message, 0);
        mr_message_queue<c_decal_message_union, 256> *decal_queue = (mr_message_queue<c_decal_message_union, 256> *)g_decal_message_queue;
        long decal_message_count = decal_queue->m_count.peek();
        for (long decal_message_index = 0; decal_message_index < decal_message_count; decal_message_index++) {
            if (decal_queue->m_valid.test_bit(decal_message_index)) {
                if (decal_queue->m_messages[decal_message_index].type >= 1)
                    decal_system_remove_cluster_from_pvs(&decal_queue->m_messages[decal_message_index].cluster_reference);
                else
                    c_decal_system::add_cluster_to_pvs(&decal_queue->m_messages[decal_message_index].cluster_reference);
            }
        }
    }
    g_object_message_queue->clear();
    main_render_purge_effect_messages(0);
    c_decal_system::reset_render_message_queue();
}


// @address 0x20d3d8 main_render_draw_message_screen
extern "C" void main_render_draw_message_screen(const wchar_t *text)
{
    c_fullscreen_view view;
    view.setup_camera(NULL);
    c_view::begin(&view);
    real_rgb_color background;
    background.red = 12.0f / 255.0f;
    background.green = 17.0f / 255.0f;
    background.blue = 32.0f / 255.0f;
    c_fullscreen_view::render_blank_frame(&background);
    {
        c_rasterizer_draw_string draw_string;
        c_font_cache font_cache;
        draw_string.m_font_id = _super_large_font;
        real_argb_color color = objects_get_default_argb_white()[0];
        real_argb_color shadow_color = objects_get_default_argb_white()[2];
        color.red = 1.0f;
        color.green = 1.0f;
        color.blue = 1.0f;
        shadow_color.red = 4.0f / 255.0f;
        shadow_color.green = 6.0f / 255.0f;
        shadow_color.blue = 10.0f / 255.0f;
        draw_string.m_font_header = font_get_header(_super_large_font);
        draw_string.m_justification = _text_justification_center;
        draw_string.set_color_deprecated(&color, true);
        draw_string.set_shadow_color(&shadow_color);
        rectangle2d bounds;
        if (rasterizer_main_get_video_mode()->fIsWideScreen != 0) {
            c_rasterizer::get_fullscreen_render_title_safe_pixel_bounds(&bounds);
        } else if (rasterizer_main_get_video_mode()->fIsHiDef != 0) {
            bounds.x0 = 32; bounds.y0 = 24; bounds.x1 = 608; bounds.y1 = 456;
        } else {
            bounds.x0 = 48; bounds.y0 = 36; bounds.x1 = 592; bounds.y1 = 444;
        }
        bounds.y0 += 200;
        draw_string.set_bounds(&bounds);
        draw_string.c_draw_string::draw(&font_cache, text);
    }
    c_view::end();
}

// @address 0x451168 main_render_draw_loading_screen
extern "C" void main_render_draw_loading_screen()
{
    if (g_thread_owning_device.value != g_registered_thread_index || g_main_render_loading_globals.owner_thread != g_registered_thread_index)
        return;
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)17);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    D3DDevice_Clear(c_rasterizer::get_device(), 0, 0, 1, 0, 1.0f, 0, 0);
    struct s_camera_clip { float near_plane; float far_plane; } clip;
    real_vector3d position;
    real_vector3d direction;
    float field_of_view = main_render_evaluate_loading_camera(&g_main_render_loading_globals.camera_curves, &position, &direction,
        g_main_render_loading_globals.progress, g_main_render_loading_globals.smoothed_progress, &clip.near_plane, &clip.far_plane);
    float far_plane = clip.far_plane;
    float near_plane = clip.near_plane;
    double fade_double;
    if (g_main_render_loading_globals.completion_time == -1)
        fade_double = 1.0;
    else
        fade_double = 1.0 - (double)(unsigned int)(GetTickCount() - (unsigned long)g_main_render_loading_globals.completion_time) * 0.00034482758638408037;
    float fade = PIN((float)fade_double, 0.0f, 1.0f);
    clip.near_plane = fade * near_plane;
    clip.far_plane = fade * far_plane;
    chud_loading_draw_screen(g_main_render_loading_globals.chud_loading_screen_header, fade * field_of_view, clip.near_plane, clip.far_plane,
        g_main_render_loading_globals.smoothed_progress, &position, &direction);
}
// @address 0x1104 ??_7c_simple_tag_resource_predictor@@6B@
// @address 0x3e1b4 ??_7c_demand_collector_visitor@@6B@
namespace {
__forceinline void main_render_first_person_get_render_info(long user_index, s_main_render_first_person_render_info *info)
{
    const fpw_first_person *weapons = &g_first_person_weapon_data[user_index];
    for (long weapon_index = 0; weapon_index < 2; weapon_index++) {
        const fpw_weapon *weapon = &weapons->weapons[weapon_index];
        info->weapon_models[weapon_index] = -1;
        if ((weapon->flags & 2) && weapon->object_indices[2] != -1)
            info->weapon_models[weapon_index] = weapon->object_indices[4];
    }
    info->hands_model = weapons->object_indices[1];
}
__forceinline void main_render_camera_cluster_reference(const c_player_render_camera_iterator *iterator, s_cluster_reference *cluster_reference)
{
    if (iterator->get_observer_result())
        *cluster_reference = iterator->get_observer_result()->location.cluster_reference;
    else
        *(short *)cluster_reference = -1;
}
__forceinline bool main_render_seam_iterator_next(s_main_render_cluster_result *iterator)
{
    iterator->end_index++;
    bool result = iterator->end_index < iterator->stack.m_count;
    return result;
}
__forceinline s_cluster_reference main_render_seam_iterator_get(const s_main_render_cluster_result *iterator)
{
    return iterator->stack.m_elements[iterator->end_index];
}
__forceinline void main_render_predict_structure(c_simple_tag_resource_predictor *predictor, long scenario_index, s_cluster_reference *cluster_reference)
{
    if (g_main_render_predict_structure) {
        const struct scenario *scenario = (const struct scenario *)global_tag_instances[(unsigned short)scenario_index].definition;
        s_main_render_cluster_result seams;
        main_render_get_cluster_seams(&seams, scenario_index, cluster_reference);
        while (main_render_seam_iterator_next(&seams)) {
            *cluster_reference = main_render_seam_iterator_get(&seams);
            predict_structure_cluster_resources(predictor->structure_collector(), scenario_index, *cluster_reference);
            s_scenario_lightmap_bsp_data *lightmap = scenario_lightmap_find_bsp(scenario->lightmap.index, cluster_reference->bsp_index);
            if (lightmap) {
                main_render_submit_model_sections(predictor, lightmap->coefficients_bitmap.index);
                main_render_submit_model_sections(predictor, lightmap->dominant_light_bitmap.index);
            }
        }
    }
}
__forceinline void main_render_pvs_iterator_initialize(
    c_pvs_cluster_iterator *iterator, long scenario_index, long zone_set_index, s_cluster_reference cluster_reference)
{
    iterator->reset();
    scenario_zone_set_pvs_get_row(scenario_index, (s_scenario_pvs_row *)iterator, zone_set_index, cluster_reference, false);
}
__forceinline void main_render_pvs_iterator_get(const c_pvs_cluster_iterator *iterator, s_cluster_reference *cluster_reference)
{
    cluster_reference->bsp_index = (signed char)iterator->get_structure_bsp_index();
    cluster_reference->cluster_index = (unsigned char)iterator->get_cluster_index();
}
__forceinline unsigned long main_render_pvs_iterator_bsp_mask(const c_pvs_cluster_iterator *iterator)
{
    unsigned long mask = 0;
    for (long structure_bsp_index = 0; structure_bsp_index < 16; structure_bsp_index++) {
        if (iterator->get_cluster_count(structure_bsp_index) > 0)
            mask |= 1 << structure_bsp_index;
    }
    return mask;
}
}
// @address 0x13fb68 ?demand_callback@c_main_render_demand_callback@@UAAXJPAVc_tag_resource_demand_collector@@@Z
void c_main_render_demand_callback::demand_callback(long demand_type, c_tag_resource_demand_collector *collector)
{
    if (!g_active_structure_bsp_mask)
        return;
    if (demand_type == 0) {
        if (main_render_game_paused() && g_main_render_predict_cinematic) {
            c_demand_collector_visitor visitor(collector);
            s_cinematic_new_globals_definition *objects = cinematic_new_globals;
            if (objects->current_scene != -1 && objects->scene_tag_index != -1) {
                s_cinematic_scene *list = (s_cinematic_scene *)tag_get('cisc', objects->scene_tag_index);
                if (list)
                    main_render_predict_object_list(list, objects->current_shot, 300, &visitor);
            }
        }
    } else if (demand_type == 6) {
        if (main_render_game_paused() && g_main_render_predict_cinematic) {
            c_demand_collector_visitor visitor(collector);
            s_cinematic_new_globals_definition *objects = cinematic_new_globals;
            if (objects->current_scene != -1 && objects->scene_tag_index != -1 && tag_get('cisc', objects->scene_tag_index))
                cinematic_iterate_shots(300, main_render_predict_object_list, &visitor);
        }
    } else {
        c_player_render_camera_iterator iterator;
        main_render_get_view_layout((s_main_render_view_layout *)&iterator);
        while (iterator.next()) {
            if (demand_type == 1) {
                if (!g_main_render_predict_first_person)
                    return;
                c_simple_tag_resource_predictor predictor(m_print_messages, demand_type, collector);
                long user_index = iterator.get_user_index();
                if (user_index != -1) {
                    long unit_index = tls_player_mapping_globals()->unit_indices[user_index];
                    if (unit_index != -1 && tls_director_globals()->slots[user_index].perspective == 0) {
                        s_main_render_first_person_render_info render_info;
                        main_render_first_person_get_render_info(user_index, &render_info);
                        for (long weapon_index = 0; weapon_index < 2; weapon_index++)
                            main_render_predict_geometry_resources(&predictor, render_info.weapon_models[weapon_index]);
                        main_render_predict_model_element(&predictor, m_first_person_hands_tag, render_info.hands_model);
                        c_simple_tag_resource_predictor object_predictor(m_print_messages, demand_type, collector);
                        for (object_datum *object = object_get(unit_index);
                            object && object->parent_object_index != -1;
                            object = object_get(object->parent_object_index))
                            predicted_resources_iterate_object_instance(&object_predictor, object->parent_object_index);
                    }
                }
            } else {
                long scenario_index = m_scenario_index;
                const struct scenario *scenario = (const struct scenario *)global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(scenario_index)].definition;
                s_cluster_reference cluster_reference;
                main_render_camera_cluster_reference(&iterator, &cluster_reference);
                if (scenario_cluster_reference_valid(&cluster_reference)) {
                    switch (demand_type) {
                    case 2: {
                        c_simple_tag_resource_predictor predictor(m_print_messages, demand_type, collector);
                        main_render_predict_structure(&predictor, scenario_index, &cluster_reference);
                        break;
                    }
                    case 3:
                        if (g_main_render_predict_objects) {
                            c_simple_tag_resource_predictor predictor(m_print_messages, demand_type, collector);
                            s_main_render_cluster_result seams;
                            main_render_get_cluster_seams(&seams, scenario_index, &cluster_reference);
                            while (main_render_seam_iterator_next(&seams)) {
                                cluster_reference = main_render_seam_iterator_get(&seams);
                                objects_hierarchy_list_add_cluster(&m_objects, &cluster_reference);
                            }
                            const s_objects_hierarchy_list *objects = &m_objects;
                            long object_count = objects->object_indices.count;
                            for (long index = 0; index < object_count; index++)
                                predicted_resources_iterate_object_instance(&predictor, objects->object_indices.indices[index]);
                        }
                        break;
                    case 4:
                        if (g_main_render_predict_structure) {
                            c_simple_tag_resource_predictor predictor(m_print_messages, demand_type, collector);
                            s_main_render_cluster_result seams;
                            main_render_get_cluster_seams(&seams, scenario_index, &cluster_reference);
                            while (main_render_seam_iterator_next(&seams))
                                predicted_resources_iterate_bsp_materials(&predictor, m_scenario_index, m_first_person_hands_tag,
                                    seams.stack.m_elements[seams.end_index]);
                        }
                        break;
                    case 5:
                        if (g_main_render_predict_objects) {
                            c_simple_tag_resource_predictor predictor(m_print_messages, demand_type, collector);
                            const s_objects_hierarchy_list *objects = &m_objects;
                            long definition_count = objects->definition_indices.count;
                            for (long index = 0; index < definition_count; index++)
                                main_render_predict_tag_resources(&predictor, objects->definition_indices.indices[index]);
                        }
                        break;
                    case 7:
                        if (g_main_render_predict_structure && g_main_render_predict_pvs) {
                            c_simple_tag_resource_predictor predictor(m_print_messages, demand_type, collector);
                            c_pvs_cluster_iterator pvs;
                            main_render_pvs_iterator_initialize(&pvs, scenario_index, m_zone_set_index, cluster_reference);
                            while (pvs.next()) {
                                s_cluster_reference pvs_cluster_reference;
                                main_render_pvs_iterator_get(&pvs, &pvs_cluster_reference);
                                predict_structure_cluster_resources(predictor.structure_collector(), m_scenario_index, pvs_cluster_reference);
                            }
                            unsigned long bsp_mask = main_render_pvs_iterator_bsp_mask(&pvs);
                            for (long structure_bsp_index = 0; structure_bsp_index < 16; structure_bsp_index++) {
                                if (bsp_mask & (1 << structure_bsp_index)) {
                                    s_scenario_lightmap_bsp_data *lightmap = scenario_lightmap_find_bsp(scenario->lightmap.index, structure_bsp_index);
                                    if (lightmap) {
                                        main_render_submit_model_sections(&predictor, lightmap->coefficients_bitmap.index);
                                        main_render_submit_model_sections(&predictor, lightmap->dominant_light_bitmap.index);
                                    }
                                }
                            }
                        }
                        break;
                    case 8:
                        if (g_main_render_predict_pvs && g_main_render_predict_objects) {
                            c_simple_tag_resource_predictor predictor(m_print_messages, demand_type, collector);
                            c_pvs_cluster_iterator pvs;
                            main_render_pvs_iterator_initialize(&pvs, scenario_index, m_zone_set_index, cluster_reference);
                            while (pvs.next()) {
                                s_cluster_reference pvs_cluster_reference;
                                main_render_pvs_iterator_get(&pvs, &pvs_cluster_reference);
                                objects_hierarchy_list_add_cluster(&m_objects, &pvs_cluster_reference);
                            }
                            const s_objects_hierarchy_list *objects = &m_objects;
                            long object_count = objects->object_indices.count;
                            for (long index = 0; index < object_count; index++)
                                predicted_resources_iterate_object_instance(&predictor, objects->object_indices.indices[index]);
                        }
                        break;
                    case 9:
                        break;
                    default:
                        // match: 0x13fb68: unreachable hint omits dead branch check
                        __assume(0);
                    }
                }
            }
        }
    }
}
static __forceinline void main_render_get_target_display_vblank_index(__int64 *target)
{
    memcpy(target, &g_main_time_globals->target_vblank_index, sizeof(*target));
}
// @address 0x142558 ?main_render@@YAXXZ
void main_render()
{
    int render_frame = 1;
    unsigned long wait = _internal_halt_render_thread_and_lock_resources("..\\main\\main_render.cpp", 281);
    s_rasterizer_timing_record *timing = &g_rasterizer_timing_globals.current_record;
    timing->input_collection_milliseconds = g_main_time_globals->input_collection_milliseconds;
    timing->input_adjustment_time = g_main_time_globals->input_collection_milliseconds;
    timing->synchronization_milliseconds = g_main_time_globals->synchronization_milliseconds;
    timing->last_system_milliseconds = g_main_time_globals->last_system_milliseconds;
    timing->render_start_milliseconds = GetTickCount();
    main_render_process_messages();
    if (g_main_render_lag_counter > 50) {
        render_frame = 0;
        g_main_render_lag_counter = 0;
    }
    if ((long)(int)timing->render_start_milliseconds - (long)(int)timing->last_system_milliseconds > 10)
        ++g_main_render_lag_counter;
    else
        g_main_render_lag_counter = 0;
    if ((unsigned char)render_frame) {
        int render_game = 1;
        if (c_rasterizer::begin_frame()) {
            if (!simulation_starting_up() || !tls_game_globals()->active_structure_bsp_mask)
                render_game = 0;
            else if (simulation_is_pending_start())
                render_game = 0;
            if ((unsigned char)render_game)
                render_game = 1;
            if ((unsigned char)render_game)
                main_render_game();
            else
                main_render_pregame(_main_pregame_frame_normal, 0);
            render_frame = 1;
        } else {
            render_frame = 0;
        }
        c_render_globals::set_frame_index(c_render_globals::get_frame_index() + 1);
    }
    g_rasterizer_timing_globals.current_record.render_end_milliseconds = GetTickCount();
    unlock_resources_and_resume_render_thread(wait);
    if ((unsigned char)render_frame) {
        __int64 target_vblank;
        main_render_get_target_display_vblank_index(&target_vblank);
        restricted_region_mirror_locked_for_current_thread(3);
        main_time_throttle(target_vblank);
        c_rasterizer::end_frame();
        s_game_tick_time_samples *samples = g_main_render_timing_data;
        float zero = k_real_zero_storage;
        samples->minimum = zero;
        samples->count = 0;
        samples->maximum = zero;
        samples->last_sample = 0;
        samples->total = zero;
    }
}
static __forceinline long main_render_external_handle(long handle) { return c_tag_resource_cache_internal_handle(handle).external; }
static __forceinline bool main_render_optional_handle(long handle) { return c_tag_resource_cache_internal_handle(handle).optional; }
class c_tag_resource_controller_page_table_io_listener;


// @address 0x140640 main_render_process_demanded_resources
extern "C" void main_render_process_demanded_resources(s_main_render_resource_controller *controller, const s_main_render_deferred_flags *requested,
    s_main_render_address_interface *address, c_tag_resource_controller_page_table_io_listener *listener, bool *load)
{
    s_main_render_deferred_flags requested_flags = *requested;
    s_main_render_deferred_flags loaded_flags = controller->m_available;
    unsigned long word_count = (unsigned long)(controller->m_resources->first_unallocated + 31) >> 5;
    long resource_index = 0;
    long word_index = -1;
    unsigned long word = 0;
    for (;;)
    {
        if (!word)
        {
            do
            {
                ++word_index;
                if (word_index >= (long)word_count)
                    break;
                word = loaded_flags.m_flags[word_index];
                word &= requested_flags.m_flags[word_index];
            } while (!word);
        }
        bool valid = false;
        if (word_index < (long)word_count)
        {
            unsigned long low_bit = -word & word;
            word = (word - 1) & word;
            unsigned long bit = -low_bit & low_bit;
            resource_index = (word_index << 5) - (long)_CountLeadingZeros(bit) + 31;
            valid = true;
        }
        if (!valid)
            return;
        s_data_array *resources = controller->m_resources;
        long handle = -1;
        if (resource_index != -1)
        {
            const s_datum_header *datum = (const s_datum_header *)(resources->data + resources->size * resource_index);
            handle = ((unsigned long)(unsigned short)datum->identifier << 16) | resource_index;
        }
        long external_handle = main_render_external_handle(handle);
        bool optional = main_render_optional_handle(handle);
        main_render_process_demanded_resource(controller, external_handle, optional, address, listener, load);
    }
}

static __forceinline s_tag_resource_control_datum_data *main_render_resource_get(s_data_array *resources, long handle)
{
    return datum_get<s_tag_resource_control_datum_data>(resources, handle);
}
static __forceinline bool main_render_page_is_idle(s_main_render_resource_controller *controller, long handle)
{
    s_tag_resource_page_datum *page = datum_get<s_tag_resource_page_datum>(
        controller->m_page_table->m_table->m_pages, handle);
    return page->state <= 1 && (page->flags & 2) == 0;
}
static __forceinline bool main_render_async_available(c_tag_resource_page_table *table)
{
    return table->m_async_requests->actual_count < table->m_async_requests->maximum_count;
}
static __forceinline bool main_render_datum_has_state(const c_tag_resource_control_datum &datum) { return datum.data->state != 0; }
static __forceinline bool main_render_control_active(s_tag_resource_control_datum_data *data)
{
    return (data->flags & 2) != 0 || (data->flags & 4) != 0;
}
static __forceinline bool main_render_page_has_io(s_tag_resource_page_datum *page)
{
    short io_references = page->io_references;
    return io_references == 1 || io_references == 3;
}

// @address 0x140de8 main_render_process_demanded_resource
extern "C" void main_render_process_demanded_resource(s_main_render_resource_controller *controller, long external_handle, long optional,
    s_main_render_address_interface *address, c_tag_resource_controller_page_table_io_listener *listener, bool *load)
{
    if (!controller->m_can_predict_optional)
        return;
    unsigned short salt = (unsigned short)~(external_handle >> 16);
    bool is_optional = optional == 1;
    if (is_optional)
        salt |= 0x8000;
    else
        salt &= 0x7fff;
    long index = datum_index_to_absolute_index(external_handle) << 1;
    if (!is_optional)
        index++;
    s_data_array *resources = controller->m_resources;
    long handle = (salt << 16) | index;
    if (!datum_try_and_get(resources, handle))
        return;
    s_tag_resource_control_datum_data *data = main_render_resource_get(resources, handle);
    c_tag_resource_control_datum datum(controller->m_can_predict_optional, handle, &controller->m_datum_info, data,
        (c_resource_buffer_handler *)controller->m_file_datum_handler);
    if ((data->flags & 0x20) == 0)
        return;
    if (main_render_control_active(data)
        && (!main_render_datum_has_state(datum) || main_render_page_is_idle(controller, data->page_handle))
        && *load
        && (!controller->m_require_async_page_capacity
            || main_render_async_available(controller->m_page_table->m_table)))
    {
        if (!data->state)
            data->state = 1;
        controller->m_page_table->m_table->demand_page(data->page_handle, listener);
        *load = !main_render_page_is_idle(controller, data->page_handle);
    }
    if (datum.deferred())
    {
        ((c_wrapped_flags *)&controller->m_predicted)->set(datum.get_absolute_internal_index(), true);
        address->mark_demanded_resource(datum.get_external_resource_handle());
    }
    c_tag_resource_page_table *table = controller->m_page_table->m_table;
    long page_handle = data->page_handle;
    if (page_handle != -1)
    {
        s_tag_resource_page_datum *page = &((s_tag_resource_page_datum *)table->m_pages->data)[(unsigned short)page_handle];
        if (main_render_page_has_io(page) && page->buffer.m_size > 0 && page->state >= 3)
        {
            s_lruv_cache *lru = table->m_lruv_cache.m_cache;
            ((s_lruv_block *)((s_data_array *)lru->header.block_data)->data)[(unsigned short)page_handle].last_used_frame = lru->frame;
        }
    }
}

// @address 0x72cdcb g_water_interaction_enabled
// @address 0x73a184 g_main_render_shader_state

static __forceinline bool main_render_water_interaction_recent(long last_water_interaction)
{
    return last_water_interaction != -1 && game_time_globals->time - last_water_interaction < 300;
}

// @address 0x20d7c0 ?main_render_game@@YAXXZ
void main_render_game()
{
    unsigned long lock = _internal_halt_render_thread_and_lock_resources("..\\main\\main_render.cpp", 835);
    if (!main_render_game_paused())
        observer_adopt_global_update_list();
    main_render_update_loading();
    long camera_count, layout;
    {
        c_player_render_camera_iterator iterator;
        main_render_get_view_layout((s_main_render_view_layout *)&iterator);
        camera_count = iterator.get_window_count();
        layout = iterator.get_window_arrangement();
        main_render_process_messages();
        lens_flares_update();
        g_main_render_fence_active = g_main_render_fence_enabled;
        if (g_main_render_fence_enabled)
        {
            g_main_render_gpu_fence = D3DDevice_InsertFence(c_rasterizer::get_device());
            g_main_render_gpu_fence_inserted = true;
        }
        for (long index = 0; index < camera_count; ++index)
        {
            long user_index;
            const s_observer_result *observer;
            if (iterator.next())
            {
                user_index = iterator.m_current_user_index;
                observer = iterator.m_current_observer_result;
            }
            else
            {
                user_index = 0;
                observer = &observer_get_globals()->blocks[0].camera_result;
            }
            c_player_view::x_global_player_views[index].setup_camera(index, camera_count, layout, user_index, observer, false);
        }
    }
    c_visible_items::clear_all();
    effects_frame_advance(g_main_render_timing_data->total);
    float elapsed = g_main_render_timing_data->total;
#if defined(_XBOX) && !defined(__clang__)
    long &gpr_allocation = g_main_render_shader_state.gpr_allocation;
    long &last_water_interaction = g_main_render_shader_state.last_water_interaction;
#else
    long &gpr_allocation = g_rasterizer_gpr_allocation;
    long &last_water_interaction = g_water_interaction_last_time;
#endif
    if (gpr_allocation != 4)
    {
        (*rasterizer_shader_gpr_get_globals())[0] = 112;
        (*rasterizer_shader_gpr_get_globals())[1] = 16;
        D3DDevice_SetShaderGPRAllocation(c_rasterizer::get_device(), 0, 112, 16);
        gpr_allocation = 4;
    }
    c_light_volume_gpu::frame_advance_all(elapsed);
    c_particle_emitter_gpu::spawn_all();
    c_particle_emitter_gpu::frame_advance_all(elapsed);
    c_contrail_gpu::frame_advance_all(elapsed);
    c_contrail_gpu::spawn_all();
    particle_block_frame_advance_all(elapsed);
    struct main_render_timing { unsigned long flags; float fade; };
    const main_render_timing *timing = (const main_render_timing *)&main_render_timing_settings();
    if (timing->fade > 0.66f)
    {
        render_water_set_enabled_flags(true, true);
    }
    else if (timing->fade <= 0.66f && timing->fade >= 0.33f)
    {
        render_water_set_enabled_flags(false, true);
    }
    else
    {
        render_water_set_enabled_flags(false, false);
    }
    if (g_water_interaction_enabled)
    {
        unsigned long event_count = 0;
        if (g_main_render_timing_data->total >= 0.00001f)
        {
            for (unsigned long index = 0; index < 128 && g_water_interaction_events[index].type != -1; ++index)
                event_count = index + 1;
            if (event_count)
                last_water_interaction = game_time_globals->time;
        }
        if (main_render_water_interaction_recent(last_water_interaction))
        {
            char name[32];
            sprintf_s(name, sizeof(name), "ripple_frame_adavance");
            water_renderer_handle_interaction_events(event_count);
            c_water_renderer::ripple_update();
        }
    }
    c_render_globals::set_depth_fade_active(true);
    c_patchy_fog::frame_advance_all(g_main_render_timing_data->total);
    if (!g_main_render_loading_globals.initialized)
    {
        main_render_sync_rasterizer_memory(&g_rasterizer_xenon_memory);
        main_render_update_timing_config();
        memcpy(&c_screen_postprocess::x_editable_settings, &g_main_render_default_postprocess_settings, sizeof(c_screen_postprocess::x_editable_settings));
        rasterizer_main_get_video_mode();
        for (long index = 0; index < camera_count; ++index)
        {
            char name[32];
            sprintf_s(name, sizeof(name), "player_view %d", index);
            g_water_composite_shading_constants.i = 0.0f;
            g_water_composite_shading_constants.j = 0.0f;
            g_water_composite_shading_constants.k = 1.0f;
            g_water_composite_shading_constants.l = 1.0f;
            if (index == camera_count - 1)
                c_player_view::x_global_player_views[index].last_view = true;
            else
                c_player_view::x_global_player_views[index].last_view = false;
            player_view_render_frame(&c_player_view::x_global_player_views[index], index);
        }
        c_ui_view ui_view;
        render_view_initialize_surface(&ui_view, 0, 1);
        c_view::begin(&ui_view);
        if (camera_count > 1)
            for (long index = 0; index < camera_count; ++index)
                screen_postprocess_blit_player_view(&c_player_view::x_global_player_views[index]);
        D3DDevice_SetRenderState_HighPrecisionBlendEnable(c_rasterizer::get_device(), 1);
        D3DDevice_SetRenderState_HighPrecisionBlendEnable1(c_rasterizer::get_device(), 1);
        D3DDevice_SetRenderState_HighPrecisionBlendEnable2(c_rasterizer::get_device(), 1);
        D3DDevice_SetRenderState_HighPrecisionBlendEnable3(c_rasterizer::get_device(), 1);
        if (main_render_bink_active())
            bink_playback_render_first_frame();
        interface_splitscreen_render();
        director_draw_fade_and_camera_debug();
        cinematic_render(true, true);
        ui_view.c_ui_view::render();
        closed_caption_render();
        c_view::end();
        rasterizer_dx9_dynamic_geometry_screenshot_parameters screenshot;
        rasterizer_render_screenshot(&screenshot, c_player_view::x_global_player_views,
            &g_player_view_camera_settings->values, camera_count);
        c_render_globals::set_depth_fade_active(false);
        c_render_globals::set_distortion_active(false);
    }
    if (g_main_render_loading_globals.initialized)
        main_render_draw_loading_screen();
    unlock_resources_and_resume_render_thread(lock);
}
