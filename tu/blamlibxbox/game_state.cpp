// blamlibxbox/game_state.obj: from the records 0x848a8 0x19a838 0x19a840 0x19a8e0 0x19a8f0 0x19a948 0x19a960 0x19a978 0x19aa48 0x19afb8 0x19b110 0x19b2a0 0x19b310 0x19b440 0x19b518 0x19b760 0x19b970 0x19bbc0 0x19bcb0 0x19bda0 0x19be58 0x19c018 0x19c0d0 0x19c210 0x19c240 0x19c310 0x19c470 0x19c4f0 0x19c5d0 0x19c7c0 0x1d5ed8 0x1d5fd0 0x1d6028 0x1d6030 0x1d6038 0x1d60b8 0x1d6118 0x1d62a0 0x1d6468 0x1d65f0 0x1d6948 0x1e23c8 0x1ebee8 0x3c4898 0x7079c0 0x70b628
// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// @address 0x1142940 g_window_manager
// @flags /TP /O2 /Oc
// @address 0x8a1ed4 g_active_designer_zone_mask
// @address 0x8a1ed8 g_active_cinematic_zone_mask
// @address 0x96e760 g_game_state_procs
// @address 0x94e3b0 g_saved_film_timer_globals
// @address 0x74b80c g_runtime_state_allocation
// @address 0x948f78 main_globals
// @address 0x8a1ead g_utility_drive_mounted
#include "game/campaign_metagame_globals.h"
#include "memory/runtime_state_allocation.h"
#include "scenario/scenario_runtime_symbols.h"
#include "memory/normal_allocation.h"
#include "shell/utility_drive.h"
#include "saved_games/game_state_procs_externs.h"
#include "networking/logic/life_cycle/life_cycle_runtime_globals.h"
#include "memory/tls_globals.h"
#include "memory/tls_data_arrays.h"
#include "camera/camera_globals.h"
#include "memory/static_arrays.h"
#include "main/levels.h"
#include <stdio.h>
#include "cseries/cseries_wrapped.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "game/map_variant.h"
#include "game/game_options.h"
#include "game/game_results_externs.h"
#include <new>
#include <stdlib.h>
#include "memory/data.h"
#include "game/game_time_types.h"
#include "game/game_globals_storage.h"
#include "game/game_state_accessors.h"
#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory_regions_inline.h"
#include "game/game.h"
#include "game/players.h"
#include "scenario/scenario.h"
#include "simulation/simulation.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "cache/security_functions.h"
#include "main/main_game.h"
#include "main/main_time.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/transport/transport_security.h"
#include "saved_games/game_state.h"
#include "saved_games/game_state_procs.h"
#include "saved_games/game_state_xenon.h"
#include "saved_games/saved_film.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "zlib/blam_zlib_memory.h"
#include "game/game_time.h"
#include "memory/memory_pool.h"
#include "saved_games/saved_game_files.h"
#include "memory/crc.h"
#include "tag_files/file_reference.h"
#include "interface/c_gui_screen_widget.h"
#include "cache/restricted_memory.h"
#include "saved_games/game_state_allocation_callbacks.h"
#include "cache/cache_file_globals.h"
#include "cseries/async.h"
#include "text/text_group.h"
#include "interface/user_interface_networking.h"
#include "memory/zlib_compression_level_types.h"
#include "cseries/write_position_flags_types.h"
#include "cache/cache_files.h"
#include "interface/user_interface_session.h"
#include "memory/data_compress.h"
#include "zlib/blam_zlib_memory_types.h"
#include "main/main_halt.h"
#include "memory/physical_memory_map.h"
#include "main/main.h"
#include "sound/game_sound.h"
#include "bink/bink_playback.h"
#include "physics/havok.h"
#if !defined(_XBOX) || defined(__clang__)
#include "gpu_particle/contrail_layout_types.h"
#include "rasterizer/rasterizer_shader_gpr_types.h"
#endif

enum memory_stage;
// @address 0x1d6028 ?update@c_game_state_save_listener@@UAAXPAX@Z
// @address 0x1d6030 ?dispose@c_game_state_save_listener@@UAAXPAX@Z
// @address 0x1d6118 game_state_update_async_save
// @address 0x1d62a0 game_state_wait_for_async_save
// @address 0x1ce8d0 ??0c_map_variant@@QAA@XZ
// @address 0x286540 ?blam_zlib_malloc@@YAPAXPAXII@Z
// @address 0x286590 ?blam_zlib_free@@YAXPAX0@Z
// @address 0x183ef0 ?crc_checksum_buffer@@YAKKPBXJ@Z
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x1b8600 ?game_options_match@@YA_NPBUgame_options@@0@Z
// @address 0x1b7880 ?game_options_verify@@YA_NPBUgame_options@@PADJ@Z
// @address 0x3b4e98 ?network_life_cycle_session_get_map@@YA_NPAW4e_campaign_id@@PAW4e_map_id@@@Z
// @address 0x19a060 ?scenario_game_states_match@@YA_NPBUs_scenario_game_state@@0@Z
// @address 0x19f058 ?security_calculate_hash@@YA_NPBDK_NPAUs_network_http_request_hash@@@Z
// @address 0x3a1298 ?transport_secure_random@@YAXJPAE@Z
// @address 0x46d740 ?user_interface_game_settings_get_campaign_difficulty@@YA?AW4e_campaign_difficulty_level@@XZ
// @address 0x82370 _vsnprintf
// @address 0x397660 uncompress2
// @address 0x19a838 ?filter_base_offset@c_restricted_memory_callbacks@@UAAJJ@Z
// @address 0x19a840 ??0game_options@@QAA@XZ
// @address 0x19a8f0 ?handle_allocation@c_gamestate_nondeterministic_allocation_callbacks@@UAAXPBVc_restricted_memory@@PBD1JPAXI@Z
// @address 0x19a948 ?game_state_reset_mapping@@YAXJ@Z
// @address 0x19a960 ?game_state_set_header_address@@YAXPAX@Z
// @address 0x19a978 game_state_allocate_buffer
// @address 0x19b110 ?game_state_initialize_for_new_map@@YAXXZ
// @address 0x19b2a0 ?game_state_initialize_for_new_structure_bsp@@YAXK@Z
// @address 0x19b2a0 ?game_state_initialize_for_new_designer_zone_set@@YAXPBUs_game_non_bsp_zone_set@@@Z
// @address 0x19b440 ?game_state_prepare_for_revert@@YAXXZ
// @address 0x19bcb0 game_state_update_hash
// @address 0x19bda0 ?game_state_header_valid@@YA_NPAUs_game_state_header@@@Z
// @address 0x19c018 ?game_state_header_matches@@YA_NPBUs_game_state_header@@PAU1@@Z
// @address 0x19c210 ?game_state_set_revert_time@@YAXJ@Z
// @address 0x19c240 game_state_saved_game_matches_session
// @address 0x19c310 ?game_state_read_game_options_from_persistent_storage_blocking@@YA_NW4e_controller_index@@PAUgame_options@@@Z
// @address 0x1d5fd0 ?set_mode@c_game_state_save_listener@@UAAXJPAUs_game_state_buffer_description@@@Z
// @address 0x1d6038 ?set_mode@c_game_state_load_listener@@UAAXJPAUs_game_state_buffer_description@@@Z
// @address 0x1d60b8 game_state_compute_hash_async
// @address 0x3c4898 ?print@?$c_static_string@$0GAA@@@QAAPBDPBDZZ
#include <stdarg.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "cseries/crt_xdk.h"

#include "scenario/scenario_game_state.h"
#include "memory/game_state_globals.h"
typedef char game_state_cache_hash_offset[
    (sizeof(void *) != 4 || ((offsetof(s_cache_file_globals, header)
    + offsetof(s_cache_file_header, digest) + 10 * sizeof(unsigned long) == 872))) ? 1 : -1];
#include "multithreading/synchronized_value.h"
#include "saved_games/game_state_externs.h"
#include "saved_games/game_state_constants.h"


#include "saved_games/game_state_memory_mapping_types.h"
// @address 0x589d30 XPhysicalProtect
#include "cseries/xdk_memory.h"

#include "cache/restricted_memory.h"


#include "saved_games/game_state_header_types.h"
struct s_game_state_header : s_game_state_header_layout {};
// 0x19b970

extern "C" {
#include "multithreading/xenon_intrinsics.h"
}
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_globals.h"
#include "saved_games/game_state_pending_request.h"
#include "main/main_debug_upload.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "shell/shell_build.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_banhammer.h"
#include "saved_games/game_state_compression.h"
#include "memory/memory_pool_types.h"
#include "zlib/zlib.h"
#include "cseries/async_helpers.h"
#include "cseries/file_handle_types.h"
#include "multithreading/synchronized_value.h"
#include "cseries/async_write_position.h"
#include "cseries/async_completion.h"
#include "saved_games/game_state_procs_types.h"
#include "saved_games/game_state_lock.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "saved_games/game_state_procs_externs.h"
#include "camera/saved_film_director_internals.h"
#include "camera/saved_film_director_storage.h"
#include "game/player_mapping_types.h"
#include "memory/gameplay_tls_globals.h"
#include "saved_games/saved_film_globals.h"
#include "simulation/simulation_globals.h"
#include "simulation/simulation_watcher_types.h"
#include "main/main_flag_state.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/session/network_session.h"
#include "interface/c_window_manager.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_match_internals.h"
#include "simulation/simulation_abort.h"
struct s_game_state_file_handle { void * value; };
typedef char game_state_watcher_initial_player_offset[
    (sizeof(void *) != 4 || ((offsetof(c_simulation_watcher, m_initial_player_update) == 0x13f5))) ? 1 : -1];
struct game_state_header;
typedef void (*game_state_member_proc_t)(void *address);
typedef char s_game_state_header_size_check[(sizeof(s_game_state_header) == 0xf998) ? 1 : -1];
typedef char s_game_state_memory_mapping_size_check[(sizeof(void *) != 4 || ((sizeof(s_game_state_memory_mapping) == 0x50))) ? 1 : -1];

struct s_game_state_scratch_file
{
    s_file_reference reference;
    volatile unsigned char suppress_upload_debug;
    // Unreferenced tail of the stack scratch allocation; layout retained.
    char scratch_storage[0x198 - 0x111];
};
struct s_game_state_compression_buffer
{
    char header_copy[0x10000];
    c_game_state_compressor compressor;
};
class c_game_state_load_flags
{
public:
    __forceinline c_game_state_load_flags() : m_storage(0) {}
    __forceinline void set(long bit, bool value) { if (value) m_storage |= 1 << bit; else m_storage &= ~(1 << bit); }
    unsigned long m_storage;
};

long c_restricted_memory_callbacks::filter_base_offset(long offset)
{
    return offset;
}

unsigned int c_gamestate_nondeterministic_allocation_callbacks::filter_size_request(unsigned int size)
{
    unsigned int result = (size + 3) & ~3u;
    return result;
}

void c_gamestate_nondeterministic_allocation_callbacks::handle_allocation(
    const c_restricted_memory *memory, const char *name, const char *type, long member, void *address, unsigned int size)
{
    game_state_globals.allocation_checksum = crc_checksum_buffer(game_state_globals.allocation_checksum, &size, sizeof(size));
}

unsigned int c_gamestate_deterministic_allocation_callbacks::filter_size_request(unsigned int size)
{
    unsigned int result = (size + 3) & ~3u;
    return result;
}

void c_gamestate_deterministic_allocation_callbacks::handle_allocation(
    const c_restricted_memory *memory, const char *name, const char *type, long member, void *address, unsigned int size)
{
    game_state_globals.allocation_checksum = crc_checksum_buffer(game_state_globals.allocation_checksum, &size, sizeof(size));
}

// @address 0x74b804 g_gamestate_deterministic_allocation_callbacks
c_gamestate_deterministic_allocation_callbacks g_gamestate_deterministic_allocation_callbacks;
// @address 0x74b808 g_gamestate_nondeterministic_allocation_callbacks
c_gamestate_nondeterministic_allocation_callbacks g_gamestate_nondeterministic_allocation_callbacks;

static __forceinline void game_state_set_buffer_protection(void *address, long size, long offset)
{
    XPhysicalProtect(address, size + offset, 4);
}
void game_state_reset_mapping(long mapping)
{
    game_state_set_buffer_protection(g_game_state_async_file_state.address, g_game_state_async_file_state.size, 0);
}

void game_state_set_header_address(void *address)
{
    game_state_globals.current_state = (char *)address;
}


// 0x19a978
void game_state_allocate_buffer(void)
{
    game_state_globals.guard_page_size = 0;
    void *base = ::_physical_memory_malloc((memory_stage)1, (char const *)(0), 0xff0000, 0, (char const *)(0), -1);
    unsigned long random_value;
    transport_secure_random(sizeof(random_value), (unsigned char *)&random_value);
    char *aligned = (char *)base + ((random_value << 7) & 0xff80);
    long size = game_state_globals.guard_page_size;
    g_game_state_async_file_state.allocated = 1;
    g_game_state_async_file_state.address = base;
    g_game_state_async_file_state.size = 0xff0000;
    g_game_state_async_file_state.primary_buffer_capacity = 0xfe0000;
    g_game_state_async_file_state.save_buffer_size = k_game_state_file_size;
    g_game_state_async_file_state.primary_buffer_address = aligned;
    game_state_globals.game_state_buffer = aligned;
    memset(aligned, 0, size + 0xfe0000);
}

static __forceinline void game_state_initialize_scenario(s_game_state_scenario *scenario)
{
    scenario->state.flags = 1;
    scenario->state.active_designer_zone_mask = 0;
    scenario->state.touched_structure_bsp_mask = 0;
    scenario->state.active_cinematic_zone_mask = 0;
    scenario->state.touched_cinematic_zone_mask = 0;
    scenario->state.zone_set_index = -1;
}

static __forceinline s_game_state_header *game_state_current_header()
{
    return (s_game_state_header *)game_state_globals.current_state;
}

void game_state_initialize_for_new_map()
{
    game_state_reset_mapping(0);
    game_state_globals.initialized = true;
    game_state_globals.active = false;
    game_state_globals.pending = false;
    game_state_globals.index = 0;
    memset(game_state_globals.headers, 0, sizeof(game_state_globals.headers));
    game_state_globals.current_index = -1;
    memset(game_state_current_header(), 0, sizeof(s_game_state_header));
    game_state_initialize_scenario(&game_state_current_header()->scenario);
    game_state_current_header()->size = game_state_globals.header_size;
    csstrnzcpy(game_state_current_header()->scenario_name, g_cache_file_globals.header.scenario_path, 256);
    csstrnzcpy(game_state_current_header()->build, "11855.07.08.20.2317.halo3_ship", 32);
    game_state_current_header()->checksum = crc_checksum_buffer(-1, &g_cache_file_globals.header.digest.hashes[2], sizeof(s_network_http_request_hash));
    memcpy(&game_state_current_header()->game_options, &tls_game_globals()->options, sizeof(game_options));
    game_state_globals.loaded_from_persistent_storage = false;
}

void game_state_initialize_for_new_structure_bsp(unsigned long bsp_index)
{
    // match: 0x19b2a0: __restrict lets the zone-mask loads hoist above the state stores
    s_scenario_game_state * __restrict state = &game_state_current_header()->scenario.state;
    state->flags = 1;
    state->zone_set_index = global_zone_set_index;
    state->active_structure_bsp_mask = g_active_structure_bsp_mask;
    state->active_designer_zone_mask = g_active_designer_zone_mask;
    state->touched_structure_bsp_mask = g_touched_structure_bsp_mask;
    state->active_cinematic_zone_mask = g_active_cinematic_zone_mask;
    state->touched_cinematic_zone_mask = g_touched_cinematic_zone_mask;
}

void game_state_initialize_for_new_designer_zone_set(const s_game_non_bsp_zone_set *new_zone_set)
{
    // match: 0x19b2a0: __restrict lets the zone-mask loads hoist above the state stores
    s_scenario_game_state * __restrict state = &game_state_current_header()->scenario.state;
    state->flags = 1;
    state->zone_set_index = global_zone_set_index;
    state->active_structure_bsp_mask = g_active_structure_bsp_mask;
    state->active_designer_zone_mask = g_active_designer_zone_mask;
    state->touched_structure_bsp_mask = g_touched_structure_bsp_mask;
    state->active_cinematic_zone_mask = g_active_cinematic_zone_mask;
    state->touched_cinematic_zone_mask = g_touched_cinematic_zone_mask;
}

// @address 0x5bec98 ?game_state_initialize@@YAXXZ
void game_state_initialize()
{
}

// @address 0x5bec98 ?game_state_dispose@@YAXXZ
void game_state_dispose()
{
}

// @address 0x5bec98 ?game_state_dispose_from_old_map@@YAXXZ
void game_state_dispose_from_old_map()
{
}

// @address 0x5bec98 ?game_state_dispose_from_old_structure_bsp@@YAXK@Z
void game_state_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
}

// @address 0x5bec98 ?game_state_dispose_from_old_designer_zone_set@@YAXPBUs_game_non_bsp_zone_set@@@Z
void game_state_dispose_from_old_designer_zone_set(const s_game_non_bsp_zone_set *old_zone_set)
{
}

// @address 0x5bec98 ?game_state_restore_internal@@YAXJ@Z
void game_state_restore_internal(long game_state_index)
{
}


// @address 0x19b310 ?game_state_save@@YAXXZ
void game_state_save()
{
    main_set_idle_recursion(1);
    game_state_globals.count = 0;
    game_state_globals.last_time = game_time_globals->time;
    game_state_globals.pending = game_state_globals.active;
    game_state_globals.index = (game_state_globals.index + 1) % game_state_globals.size;
    game_state_lock_and_halt_render_thread(0x200);
    if (g_game_state_async_file_state.storage_mode != 1)
    {
        game_state_save_async(game_state_globals.index);
    }
    else
    {
        g_game_state_async_file_state.memory_save_valid = false;
        memcpy(g_game_state_async_file_state.memory_save_address,
            g_game_state_async_file_state.primary_buffer_address, g_game_state_async_file_state.save_buffer_size);
        g_game_state_async_file_state.memory_save_size = g_game_state_async_file_state.save_buffer_size;
        game_state_update_hash(0, g_game_state_async_file_state.memory_save_address);
        g_game_state_async_file_state.memory_save_valid = true;
    }
    game_state_unlock_and_resume_render_thread(0x200);
    game_state_globals.active = true;
    memcpy(&game_state_globals.headers[game_state_globals.index], game_state_globals.current_state, sizeof(game_state_globals.headers[0]));
    main_time_reset();
    players_update_for_checkpoint();
    main_set_idle_recursion(0);
}

void game_state_prepare_for_revert()
{
    if (game_state_globals.active && game_state_globals.pending &&
        (float)(unsigned long)(game_time_globals->time - game_state_globals.last_time) * game_time_globals->tick_length < 8.0f)
    {
        game_state_globals.count = game_state_globals.count + 1;
        if (game_state_globals.count < 5)
            return;
        game_state_globals.active = true;
        game_state_globals.pending = false;
        game_state_globals.index = (game_state_globals.index + 1) % game_state_globals.size;
        return;
    }
    game_state_globals.count = 0;
}

// The image's 140 bytes at 0x96df48 are all zero.
// @address 0x96df48 g_game_state_pending_request
extern "C" {
s_game_state_pending_request g_game_state_pending_request;
}



// @address 0x19b970 game_state_write_scratch_file
extern "C" void game_state_write_scratch_file(const char *name)
{
    long mode = 0;
    long authoritative = true;
    game_globals_storage *globals = tls_game_globals();
    signed char simulation = globals->options.simulation_type;
    if (simulation == 2 || simulation == 4)
        authoritative = false;
    unsigned char simulation_allowed = (unsigned char)authoritative;
    if (!simulation_allowed)
    {
        if ((unsigned long)(simulation - 4) <= 1)
            simulation_allowed = true;
        if (!simulation_allowed)
            // match: 0x19b970: goto keeps mode=200 ahead of the else block; the || form swaps the block order
            goto no_simulation;
    }
    if (game_is_cooperative())
    {
no_simulation:
        mode = 200;
    }
    else
    {
        if (game_in_progress())
            simulation_globals_get()->world->m_gamestate_flushed_outside_game_tick = true;
    }
    game_state_lock_and_halt_render_thread(mode);
    bool success = false;
    char *buffer = game_state_globals.game_state_buffer;
    s_game_state_scratch_file file;
    s_file_reference *reference = &file.reference;
    unsigned long error;
    file_create_parent_directories_if_not_present(reference);
    if (!file_exists(reference))
        file_create(reference);
    if (file_open(reference, 2, &error))
    {
        success = file_write(reference, k_game_state_file_size, buffer);
        file_close(reference);
    }
    game_state_unlock_and_resume_render_thread(mode);
    if (g_game_state_pending_request.armed != 0 && strncmp(g_game_state_pending_request.name, name, 128) == 0)
    {
        if (success != 0 && g_suppress_upload_debug == 0)
        {
            file.suppress_upload_debug = g_suppress_upload_debug;
            main_set_idle_recursion(1);
            main_set_idle_recursion(0);
        }
        g_game_state_pending_request.armed = 0;
    }
}

// 0x19bbc0


// @address 0x19bbc0 game_state_saved_game_header_valid
bool game_state_saved_game_header_valid(const game_state_header *header)
{
    bool valid = true;
    valid &= header->game_options.game_mode == 1;
    valid &= header->game_options.language >= 0 && header->game_options.language < 12;
    valid &= levels_get_campaign_map_index(header->game_options.campaign_id, header->game_options.map_id) != -1;
    valid &= header->game_options.campaign_difficulty >= 0 && header->game_options.campaign_difficulty < 4;
    valid &= header->game_options.campaign_insertion_point >= 0 && header->game_options.campaign_insertion_point < 4;
    valid &= header->game_options.campaign_metagame_scoring >= 0 && header->game_options.campaign_metagame_scoring < 3;
    return valid;
}


// 0x19bcb0

void game_state_update_hash(unsigned char clear_header, void *address)
{
    char *buffer;
    s_game_state_header *header;
    if (address == 0)
    {
        buffer = game_state_globals.game_state_buffer;
        header = game_state_current_header();
    }
    else
    {
        buffer = (char *)address;
        header = (s_game_state_header *)address;
    }
    if (clear_header != 0)
    {
        long *marks = (long *)&header->scenario.digest;
        marks[0] = -0x44444445;
        marks[1] = -0x44444445;
        marks[2] = -0x44444445;
        marks[3] = -0x44444445;
        marks[4] = -0x44444445;
    }
    else
    {
        transport_secure_random(20, header->scenario.hash_salt);
        game_state_digest_payload *payload = &header->scenario.digest_payload;
        payload->parts[0] = 0;
        payload->parts[1] = 0;
        payload->tail = 0;
        s_network_http_request_hash *hash = (s_network_http_request_hash *)payload;
        s_network_http_request_hash computed;
        if (security_calculate_hash(buffer, k_game_state_file_size, true, &computed))
        {
            *hash = computed;
        }
    }
}

// 0x19bda0

bool game_state_header_valid(s_game_state_header *header)
{
    bool valid = false;
    if (shell_build_string_is_compatible(header->build) && (unsigned long)header->size == (unsigned long)game_state_globals.header_size &&
        strlen(header->scenario_name) != 0)
    {
        char error[512];
        if (game_options_verify(&header->game_options, error, sizeof(error)))
        {
            valid = true;
        }
    }
    return valid;
}


// 0x19c018
bool game_state_header_matches(const s_game_state_header *current_header, s_game_state_header *file_header)
{
    bool matches = false;
    if (game_state_header_valid(file_header)
        && strcmp(current_header->scenario_name, file_header->scenario_name) == 0
        && current_header->checksum == file_header->checksum
        && game_options_match(&current_header->game_options, &file_header->game_options)
        && scenario_game_states_match(&current_header->scenario.state, &file_header->scenario.state))
    {
        matches = true;
    }
    return matches;
}


// @address 0x19c0d0 game_state_verify_hash
extern "C" bool game_state_verify_hash(s_game_state_header *external_header, bool allow_sentinel)
{
    GetTickCount();
    bool valid = false;
    char *buffer;
    s_game_state_header *header;
    s_network_http_request_hash saved;
    s_network_http_request_hash actual;
    if (external_header == 0)
    {
        buffer = game_state_globals.game_state_buffer;
        header = (s_game_state_header *)game_state_globals.current_state;
    }
    else
    {
        buffer = (char *)external_header;
        header = external_header;
    }
    if (allow_sentinel)
    {
        memset(&saved, 0xbb, sizeof(saved));
        if (memcmp(&header->scenario.digest, &saved, sizeof(saved)) == 0)
            valid = true;
    }
    if (!valid)
    {
        game_state_digest_payload *digest = &header->scenario.digest_payload;
        memcpy(&saved, digest, sizeof(saved));
        memset(digest, 0, sizeof(saved));
        valid = text_group_verify_digest(buffer, k_game_state_file_size, true, &saved, &actual);
        memcpy(digest, &saved, sizeof(saved));
    }
    if (valid)
        GetTickCount();
    else if (external_header == 0)
        main_halt_and_display_errors();
    return valid;
}

// 0x19c210
void game_state_set_revert_time(long)
{
    game_state_globals.revert_time = game_time_globals->time;
    game_time_set_pause_reason(0, 2);
}

// 0x19c240


bool game_state_saved_game_matches_session(const game_state_header *header)
{
    bool result = false;
    e_campaign_id campaign_id;
    e_map_id map_id;
    if (network_life_cycle_session_get_map(&campaign_id, &map_id))
    {
        e_campaign_difficulty_level difficulty = user_interface_game_settings_get_campaign_difficulty();
        // match: 0x19c240: preserve the caller's full-register return before its signed-short comparison.
        int cheater_result = ((int (*)(void))network_banhammer_find_cheaters_in_current_session)();
        int field_result = network_life_cycle_session_get_primary_skulls();
        int precache_result = network_life_cycle_session_get_global_map_precache_status();
        if (campaign_id == header->game_options.campaign_id && map_id == header->game_options.map_id)
        {
            if (difficulty == header->game_options.campaign_difficulty)
            {
                if ((short)cheater_result == header->game_options.campaign_insertion_point)
                {
                    if (field_result == header->game_options.primary_skulls)
                    {
                        if (precache_result == header->game_options.secondary_skulls)
                        {
                            result = true;
                        }
                    }
                }
            }
        }
    }
    return result;
}


// 0x19c310
bool game_state_read_game_options_from_persistent_storage_blocking(e_controller_index controller_index, game_options *options)
{
    bool result = false;
    s_game_state_header header;
    if (saved_game_files_get_game_state_options(controller_index, &header, sizeof(header)))
    {
        memcpy(options, &header.game_options, sizeof(*options));
        result = true;
    }
    return result;
}

// Compressed saves retain a 64 KiB header before the compressor stream.



// @address 0x19c470 game_state_compress_current_state
extern "C" bool game_state_compress_current_state(
    s_game_state_compressed_buffer *context, long unused, unsigned long flags, void **buffer_out, unsigned long *size_out)
{
    char *buffer = game_state_globals.game_state_buffer;
    game_state_lock_and_halt_render_thread(0);
    bool result = game_state_compress_buffer(context, flags, buffer, k_game_state_file_size, buffer_out, size_out);
    game_state_unlock_and_resume_render_thread(0);
    if (result)
        context->last = 3;
    return result;
}


// @address 0x19c4f0 game_state_compress_buffer
extern "C" bool game_state_compress_buffer(s_game_state_compressed_buffer *context, unsigned long flags, const void *data,
    // match: 0x19c4f0: pointer __restrict qualifier prevents alias reload
    unsigned long data_size, void **__restrict buffer_out, unsigned long *size_out)
{
    bool result = false;
    e_zlib_compression_level compression_level;
    if (flags >= 1)
        compression_level = _zlib_compression_level_best;
    else
    {
        // match: 0x19c4f0: zero-bit rotate preserves 32-bit GPR value
        compression_level = (e_zlib_compression_level)_rotl(1, 0);
    }
    context->last = result;
    memset(context->compressed_buffer, 0, context->compressed_size);
    memcpy(context->compressed_buffer->header_copy, data, sizeof(context->compressed_buffer->header_copy));
    unsigned long compressed_size;
    if (!runtime_data_compress(data, data_size, &context->compressed_buffer->compressor, &compressed_size,
            context->compressed_size - (long)sizeof(context->compressed_buffer->header_copy),
            compression_level, context->buffer_size, context->buffer))
    {
        context->extra = 0;
        context->last = 4;
    } else {
        result = 1;
        context->extra = compressed_size + sizeof(context->compressed_buffer->header_copy);
    }
    if (result) {
        *buffer_out = context->compressed_buffer;
        *size_out = context->extra;
    }
    return result;
}


// @address 0x19c5d0 game_state_load_from_compressed_buffer
extern "C" unsigned char game_state_load_from_compressed_buffer(s_game_state_compressed_buffer *state, int p1, int p2, int p3)
{
    bool loaded = false;
    s_game_state_header *header = (s_game_state_header *)state->compressed_buffer->header_copy;
    state->last = 2;
    if (state->extra <= (long)sizeof(state->compressed_buffer->header_copy))
        state->last = 9;
    else if (!game_state_header_valid(header))
        state->last = 6;
    else
    {
        bool valid = false;
        game_options_validate_variants(&header->game_options);
        if (!game_state_prepare_game_from_header(header, true, false, &valid))
            state->last = 7;
        else if (game_state_header_matches((const s_game_state_header *)game_state_globals.current_state, header))
          {
        unsigned long decompressed_size = 0;
        char *destination = game_state_globals.game_state_buffer;
        long saved_state = tls_game_globals()->options.game_playback;
        game_state_prepare_for_load(0);
        bool decompressed = state->compressed_buffer->compressor.game_state_decompress_buffer_and_load(
            (unsigned long)state->extra - sizeof(state->compressed_buffer->header_copy), destination, &decompressed_size, k_game_state_file_size,
            state->buffer_size, state->buffer);
        if (!decompressed)
            state->last = 5;
        else
            loaded = true;
        loaded &= game_state_verify_hash(0, false);
        game_globals_storage *game = tls_game_globals();
        bool in_progress = false;
        if (game && game->in_progress && !game->initializing && game->map_active)
            in_progress = true;
        if (in_progress && game_is_cooperative() && game->options.game_mode == 2)
            saved_film_update_state_checkpoint();
        tls_game_globals()->options.game_playback = (short)saved_state;
        tls_game_globals()->options.record_saved_film = false;
        game_state_call_before_load_procs();
        game_state_handle_after_load(0);
        game_state_handle_coop_load();
          }
          else
            state->last = 8;
    }
    if (loaded)
        state->last = 0;
    return loaded;
}


// @address 0x19c7c0 ?set_mode@c_game_state_compressor_listener@@UAAXJPAUs_game_state_buffer_description@@@Z
void c_game_state_compressor_listener::set_mode(long mode, s_game_state_buffer_description *description)
{
    if (mode == 1)
    {
        description->size = 0;
        description->type = 0;
        return;
    }
    description->type = 2;
    description->size = 0x400000;
}


static __forceinline s_game_state_memory_mapping *game_state_memory_mapping()
{
    return &g_game_state_async_file_state;
}

static __forceinline bool game_state_save_type_uses_mapping(long save_type)
{
    return save_type == 4 || save_type == 5 || save_type == 6;
}

static __forceinline bool game_state_save_type_loads_from_mapping(long save_type)
{
    return save_type == 3 || save_type == 5;
}

void c_game_state_save_listener::set_mode(long save_type, s_game_state_buffer_description *description)
{
    if (game_state_save_type_uses_mapping(save_type))
    {
        description->type = 2;
        description->size = game_state_memory_mapping()->save_buffer_size;
    }
    else
    {
        description->type = 0;
        description->size = 0;
    }
}

void c_game_state_load_listener::set_mode(long save_type, s_game_state_buffer_description *description)
{
    if (game_state_save_type_uses_mapping(save_type) || game_state_save_type_loads_from_mapping(save_type))
    {
        description->type = 2;
        description->size = game_state_memory_mapping()->save_buffer_size;
    }
    else
    {
        description->type = 0;
        description->size = 0;
    }
}


int game_state_compute_hash_async(void)
{
    game_state_update_hash(0, g_game_state_async_file_state.async_save_address);
    __lwsync();
    _InterlockedExchange(&g_game_state_async_file_state.hash_done.value, 1);
    return 1;
}

template<long k_maximum_count>
const char *c_static_string<k_maximum_count>::print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnprintf(m_string, k_maximum_count - 1, format, args);
    m_string[k_maximum_count - 1] = 0;
    return m_string;
}
template const char *c_static_string<1536>::print(const char *format, ...);

static __forceinline unsigned long data_decompressed_size(const void *buffer,unsigned long size) {
    unsigned long result=0;
    if(size>=4) result=*(const unsigned long *)buffer;
    return result;
}
// @address 0x1ebee8 ?game_state_decompress_buffer_and_load@c_game_state_compressor@@QAA_NKPAXPAKJJ0@Z
bool c_game_state_compressor::game_state_decompress_buffer_and_load(
    unsigned long buffer_size, void *destination, unsigned long *destination_size, long capacity, long scratch_size, void *scratch_buffer)
{
    bool success=false;
    s_memory_pool *pool=fixed_memory_pool_new("zlib pool",scratch_buffer,scratch_size,_memory_pool_callback_list_none);
    if(pool) {
        s_blam_zlib_memory memory;
        blam_zlib_initialize(pool,&memory);
        *destination_size=data_decompressed_size(((const void *)this),buffer_size);
        if (uncompress2((unsigned char *)destination, destination_size, (const unsigned char *)((const void *)this) + 4, buffer_size,
                blam_zlib_malloc, blam_zlib_free, &memory) == 0)
            success = true;
        blam_zlib_dispose(&memory);
    }
    return success;
}


static __forceinline s_game_state_file_handle game_state4_file_handle(void *value) { s_game_state_file_handle result = {value}; return result; }
static __forceinline s_file_handle game_state_make_file_handle(void *value) { s_file_handle result = {value}; return result; }
// 0x1d6118
void game_state_update_async_save(void *)
{
    bool finished = false;
    s_async_task task;
    switch (g_game_state_async_file_state.async_save_state)
    {
    case 2:
    {
        memset(&task, 0, sizeof(task));
        g_game_state_async_file_state.async_save_state = 3;
        __lwsync();
        _InterlockedExchange(&g_game_state_async_file_state.hash_done.value, 0);
        long result = ::async_task_add_ex(_async_priority_blocking_generic, &task, _async_category_saved_games,
            (e_async_completion (*)(s_async_task *))game_state_compute_hash_async,
            &g_game_state_async_file_state.hash_done, true);
        g_game_state_async_file_state.async_task_id = result;
        if (result == -1)
            finished = 1;
        break;
    }
    case 3:
    {
        long sync_value = g_game_state_async_file_state.hash_done.value;
        __lwsync();
        if (sync_value == 0)
            break;
        long slot = g_game_state_async_file_state.save_slot;

        g_game_state_async_file_state.async_task_id = -1;
        c_write_position_flags flags;
        ((long (*)(s_game_state_file_handle, void *, long, unsigned long, c_write_position_flags, long, long, bool,
            c_synchronized_long *, c_synchronized_long *))async_write_position_ex)(
            game_state4_file_handle(g_game_state_async_file_state.file_handles[slot]),
                g_game_state_async_file_state.async_save_address,
            g_game_state_async_file_state.save_buffer_size, 0, flags, 8, 5, true,
            &g_game_state_async_file_state.bytes_written,
            &g_game_state_async_file_state.write_done);
        g_game_state_async_file_state.async_save_state = 4;
    }
    // fall through: the write just started may already be done
    case 4:
    {
        long pending = g_game_state_async_file_state.write_done.value;
        __lwsync();
        if (pending != 0)
        {
            long written = g_game_state_async_file_state.bytes_written.value;
            __lwsync();
            g_game_state_async_file_state.slot_valid[g_game_state_async_file_state.save_slot] =
                (g_game_state_async_file_state.save_buffer_size - written == 0);
            finished = 1;
        }
        break;
    }
    default:
        finished = 1;
        break;
    }
    if (finished != 0)
    {
        ::game_state_call_after_load_procs(3, g_game_state_async_file_state.async_save_address);
        g_game_state_async_file_state.async_save_address = 0;
        g_game_state_async_file_state.async_save_state = 0;
    }
}


void c_game_state_save_listener::update(void *buffer)
{
    game_state_update_async_save(buffer);
}

void c_game_state_save_listener::dispose(void *buffer)
{
    game_state_wait_for_async_save(buffer);
}

void game_state_wait_for_async_save(void *buffer)
{
    while (g_game_state_async_file_state.async_save_address != 0)
    {
        game_state_update_async_save(buffer);
        if (g_game_state_async_file_state.async_save_address == 0)
            break;
        SwitchToThread();
    }
}

// @address 0x75dd60 ?g_game_state_save_listener@@3Vc_game_state_save_listener@@A
// PC build only: on Xbox the image holds the object's vtable word (0x3cb88) and nothing in
// this unit constructs it.
#if !defined(_XBOX) || defined(__clang__)
c_game_state_save_listener g_game_state_save_listener;
#endif

// @address 0x1d6468 game_state_save_async
extern "C" void game_state_save_async(int p0)
{
    if (g_game_state_async_file_state.async_save_address != 0)
        game_state_wait_for_async_save(g_game_state_async_file_state.async_save_address);
    g_game_state_async_file_state.save_slot = p0;
    g_game_state_async_file_state.slot_valid[p0] = 0;
    g_game_state_async_file_state.async_save_address = game_state_procs_try_and_get_slot(
        3, 2, g_game_state_async_file_state.save_buffer_size, &g_game_state_save_listener);
    if (g_game_state_async_file_state.async_save_address != 0)
    {
        g_game_state_async_file_state.async_save_state = 1;
        g_game_state_async_file_state.async_task_id = -1;
        __lwsync();
        _InterlockedExchange(&g_game_state_async_file_state.hash_done.value, 0);
        __lwsync();
        _InterlockedExchange(&g_game_state_async_file_state.write_done.value, 0);
        __lwsync();
        _InterlockedExchange(&g_game_state_async_file_state.bytes_written.value, 0);
        memcpy(g_game_state_async_file_state.async_save_address,
            g_game_state_async_file_state.primary_buffer_address, g_game_state_async_file_state.save_buffer_size);
        g_game_state_async_file_state.async_save_state = 2;
        game_state_update_async_save(g_game_state_async_file_state.async_save_address);
        return;
    }
    game_state_update_hash(0, 0);
    c_synchronized_long done(0);
    c_synchronized_long bytes_written(0);
    c_write_position_flags flags;
    s_game_state_file_handle file = game_state4_file_handle(g_game_state_async_file_state.file_handles[p0]);
    async_write_position_ex(file.value, g_game_state_async_file_state.primary_buffer_address,
        g_game_state_async_file_state.save_buffer_size, 0,
        flags, 8, 6, true, &bytes_written, &done);
    async_block_until_done(&done, simple_yield_function, 1, 0, 0, 0);
    g_game_state_async_file_state.slot_valid[p0] = (bytes_written.peek() == g_game_state_async_file_state.save_buffer_size);
}

// @address 0x75dd64 ?g_game_state_load_listener@@3Vc_game_state_load_listener@@A
// PC build only: on Xbox both slots are the shared empty body at 0x5bec98 and the image
// holds the object's vtable word (0x3cb94).
#if !defined(_XBOX) || defined(__clang__)
void c_game_state_load_listener::update(void *buffer)
{
}

void c_game_state_load_listener::dispose(void *buffer)
{
}

c_game_state_load_listener g_game_state_load_listener;
#endif


// @address 0x1d65f0 game_state_load_from_slot
extern "C" bool game_state_load_from_slot(long index, unsigned long flags)
{
    bool success = false;
    void *pending = g_game_state_async_file_state.async_save_address;
    if (pending != 0)
        game_state_wait_for_async_save(pending);
    if (g_game_state_async_file_state.slot_valid[index] != 0)
    {
        void *buffer = game_state_procs_try_and_get_slot(
            4, 2, g_game_state_async_file_state.save_buffer_size, &g_game_state_load_listener);
        g_game_state_async_file_state.load_buffer_address = buffer;
        if (buffer == 0)
        {
            game_state_prepare_for_load(flags);
            main_set_idle_recursion(1);
        }
        c_synchronized_long done(0);
        c_synchronized_long bytes_read(0);
        async_read_position(g_game_state_async_file_state.file_handles[index],
            buffer != 0 ? buffer : g_game_state_async_file_state.primary_buffer_address,
                g_game_state_async_file_state.save_buffer_size, 0, 8, 9,
            &bytes_read, &done);
        async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
        success = bytes_read.peek() == g_game_state_async_file_state.save_buffer_size;
        if (buffer != 0)
        {
            if (success)
            {
                success = game_state_verify_hash((s_game_state_header *)buffer, false) & success;
                if (success)
                {
                    game_state_prepare_for_load(flags);
                    memcpy(g_game_state_async_file_state.primary_buffer_address, buffer,
                        g_game_state_async_file_state.save_buffer_size);
                    game_state_call_before_load_procs();
                    game_state_handle_after_load(flags);
                }
            }
            game_state_call_after_load_procs(4, g_game_state_async_file_state.load_buffer_address);
            g_game_state_async_file_state.load_buffer_address = 0;
        }
        else
        {
            bool valid = game_state_verify_hash(0, false);
            success = valid & success;
            game_state_call_before_load_procs();
            game_state_handle_after_load(flags);
            main_set_idle_recursion(0);
        }
    }
    return success;
}


// @address 0x1d5ed8 game_state_close_save_files
extern "C" void game_state_close_save_files(void)
{
    g_game_state_procs.entries[3].handler = 0;
    g_game_state_procs.entries[4].handler = 0;
    if (g_game_state_async_file_state.storage_mode != 1)
    {
        c_synchronized_long done(0);
        for (short i = 0; (unsigned short)i < 2; i++)
        {
            async_close_file(game_state_make_file_handle(g_game_state_async_file_state.file_handles[i]),
                _async_category_saved_games, _async_priority_blocking_below_sound, &done);
            async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
        }
        if (g_utility_drive_mounted != 0)
            XFlushUtilityDrive();
        g_game_state_async_file_state.slot_valid[2] = 0;
    }
    else if (g_game_state_async_file_state.memory_save_address != 0)
    {
        g_game_state_async_file_state.memory_save_address = 0;
    }
}


// 0x1d6948

// Shipped-image .rdata word at 0x340a4: the 32767 bound game_results_clamp_high loads.
// Hosted in this TU (which never reads it): a same-TU visible initializer folds the clamp's
// .rdata load into `li r11, 32767` plus a stack spill; the load must survive.
// @rdata 0x820340a4..0x820340a8
// @address 0x340a4 k_game_results_maximum_statistic
const long k_game_results_maximum_statistic = 32767;
// @rdata-end

// The render fence id game_state_prepare_for_load blocks on; the image holds -1 (idle).
// @address 0x737664 g_game_state_render_fence
extern "C" {
volatile int g_game_state_render_fence = -1;
}

#if !defined(_XBOX) || defined(__clang__)
// The Xbox address aliases reach the same fence and shader allocation.
extern "C" {
s_contrail_layout g_contrail_export_layout = { g_game_state_render_fence, g_rasterizer_gpr_allocation };
s_contrail_layout g_light_volume_gpu_layout = { g_game_state_render_fence, g_rasterizer_gpr_allocation };
s_contrail_layout g_rasterizer_particle_submission_state = { g_game_state_render_fence, g_rasterizer_gpr_allocation };
}
#endif

// The image's five after-load handlers in slot order (0x74b13c..0x74b150).
// @address 0x74b13c g_game_state_load_callbacks
extern "C" {
void (*g_game_state_load_callbacks[5])(long) =
{
    havok_save_to_game_state,
    game_state_reset_mapping,
    game_sound_clear,
    (void (*)(long))bink_playback_abort_movie,
    simulation_prepare_to_load_saved_game
};
}

// @address 0x1d6948 game_state_prepare_for_load
extern "C" void game_state_prepare_for_load(int arg)
{
    int fence;
    long token;
    int *marks;
    g_game_state_shared_lock.render_thread_lock = ::_internal_halt_render_thread_and_lock_resources("..\\saved_games\\game_state_procs.cpp", 0x134);
    fence = g_game_state_render_fence;
    D3DDevice_IsFencePending(fence);
    D3DDevice_BlockOnFence(fence);
    g_game_state_shared_lock.lock.set(1);
    token = ::tag_resources_lock_game();
    for (short i = 0; (unsigned short)i < 5; ++i)
        g_game_state_load_callbacks[i](arg);
    ::tag_resources_unlock_game(token);
    _InterlockedExchange(&g_game_state_lock.value, 1);
    marks = (int *)&game_state_current_header()->scenario.digest;
    memset(marks, 0xdd, sizeof(game_state_current_header()->scenario.digest));
}


void game_state_shell_dispose(void)
{
    restricted_region_unlock_primary(0);
    restricted_region_unlock_primary(1);
    restricted_region_unlock_primary(2);
    restricted_region_unlock_primary(3);
    restricted_region_destroy(0);
    restricted_region_destroy(1);
    restricted_region_destroy(2);
    restricted_region_destroy(3);
    g_game_state_async_file_state.address = 0;
    g_game_state_async_file_state.size = 0;
    g_game_state_async_file_state.primary_buffer_address = 0;
    g_game_state_async_file_state.primary_buffer_capacity = 0;
    g_game_state_async_file_state.allocated = 0;
    ((c_allocation_base *)&g_runtime_state_allocation)->deallocate(game_state_globals.compressed.buffer);
    if (game_state_globals.compressed.compressed_buffer)
        ::game_state_call_after_load_procs(5, game_state_globals.compressed.compressed_buffer);
    g_game_state_procs.entries[5].handler = 0;
    game_state_globals.compressed.compressed_buffer = 0;
    game_state_globals.compressed.compressed_size = 0;
    game_state_globals.compressed.buffer = 0;
    game_state_globals.compressed.buffer_size = 0;
    game_state_globals.compressed.extra = 0;
    game_state_globals.compressed.active = false;
    game_state_globals.compressed.acquire_requested = false;
    game_state_globals.compressed.last = 0;
    game_state_close_save_files();
    memset(&game_state_globals, 0, sizeof(game_state_globals));
}

// @address 0x11df70c g_director_globals

static __forceinline bool game_state_user_is_active(long user)
{
    bool result = false;
    if (user != -1)
        result = player_mapping_globals->player_indices[user] + 1 != 0;
    return result;
}
// @address 0x1e23c8 game_state_handle_coop_load
extern "C" void game_state_handle_coop_load(void)
{
    game_globals_storage *game_globals = tls_game_globals();
    if (game_in_progress() && game_is_cooperative()) {
        if (game_globals->options.game_mode == 1)
            game_state_save();
        long lock = ::tag_resources_lock_game();
        if (game_is_cooperative())
            players_remap_cooperative_users(1);
        ::tag_resources_unlock_game(lock);
        memset(&g_saved_film_timer_globals.authored_camera_users, 0, sizeof(g_saved_film_timer_globals.authored_camera_users));
        for (long user = 0; user < 4; user++) {
            if (!game_state_user_is_active(user))
                continue;
            // match: 0x1e23c8: a plain pointer local swaps the directors[user] address add to add r3,r11,r30.
            s_controllers *const &controllers = tls_director_globals();
            c_director *user_director = controllers->directors[user].get_director();
            if (user_director->get_type() == 1) {
                s_controllers *g = tls_director_globals();
                c_saved_film_director *director = static_cast<c_saved_film_director *>(g->directors[user].get_director());
                director->m_adjusting_playback_speed = false;
                float f0 = -g_camera_globals.saved_film_revert_transition_time;
                g->transition = f0;
                g->transition_remaining = f0;
            }
        }
    }
}

static __forceinline unsigned char game_state_saved_file_valid(long index)
{
    unsigned char valid;
    if (g_game_state_async_file_state.storage_mode != 1) {
        if (g_game_state_async_file_state.async_save_address != 0)
            game_state_wait_for_async_save(g_game_state_async_file_state.async_save_address);
        valid = g_game_state_async_file_state.slot_valid[index];
    } else {
        valid = g_game_state_async_file_state.memory_save_valid;
    }
    return valid;
}
// @address 0x19b518 ?game_state_revert@@YAXK@Z
void game_state_revert(unsigned long flags)
{
    bool reverted = false;
    if (game_state_globals.active && game_state_saved_file_valid(game_state_globals.index)) {
        s_game_state_header header;
        memcpy(&header, &game_state_globals.headers[game_state_globals.index], sizeof(header));
        if (game_state_header_valid(&header)
            && game_state_prepare_game_from_header(&header, true, false, 0)
            && game_state_header_matches(game_state_current_header(), &header)) {
            unsigned long base = (flags & 1) << 4;
            unsigned long load_flags = base | 32;
            bool second = ((flags >> 1) & 1) != 0;
            if (!second)
                load_flags = base & ~32u;
            bool loaded;
            if (g_game_state_async_file_state.storage_mode != 1) {
                loaded = game_state_load_from_slot(game_state_globals.index, load_flags);
            } else {
                bool success = false;
                if (g_game_state_async_file_state.memory_save_valid
                    && game_state_verify_hash((s_game_state_header *)g_game_state_async_file_state.memory_save_address, false)) {
                    game_state_prepare_for_load(load_flags);
                    memcpy(g_game_state_async_file_state.primary_buffer_address,
                        g_game_state_async_file_state.memory_save_address, g_game_state_async_file_state.save_buffer_size);
                    game_state_call_before_load_procs();
                    game_state_handle_after_load(load_flags);
                    success = true;
                }
                loaded = success;
            }
            if (loaded && game_state_header_matches(game_state_current_header(), &header)) {
                players_update_for_checkpoint();
                if (game_in_progress())
                    simulation_globals_get()->watcher->m_initial_player_update = true;
                reverted = true;
            }
        }
    }
    if (!reverted) {
        main_globals.reset_map = true;
        main_globals.reset_map_parameter = false;
    }
}

// @address 0x19be58 game_state_prepare_game_from_header
extern "C" unsigned char game_state_prepare_game_from_header(s_game_state_header *header, bool keep_session, bool change_game,
    bool *game_changed)
{
    unsigned char result = true;
    if (!keep_session) {
        if ((*network_life_cycle_globals_get()).manager.get_current_state() == 3)
            (*network_life_cycle_globals_get()).in_game.get_flags().bits |= 2;
        network_life_cycle_end();
    }
    header->game_options.game_simulation = 1;
    header->game_options.game_playback = 0;
    game_state_current_header()->game_options.game_simulation = 1;
    game_globals_storage *game = tls_game_globals();
    if (!(game_in_progress() &&
          game_options_match(&game->options, &header->game_options))) {
        if (change_game) {
            if (game_changed)
                *game_changed = true;
            result = main_game_change_immediate(&header->game_options);
        } else {
            result = false;
        }
    }
    bool loaded = result && scenario_set_game_state(&header->scenario.state);
    if (loaded) {
        long zone_set = global_zone_set_index;
        unsigned long active_bsps = g_active_structure_bsp_mask;
        long non_bsp = g_active_designer_zone_mask;
        long touched = g_touched_structure_bsp_mask;
        long designer = g_active_cinematic_zone_mask;
        long cinematic = g_touched_cinematic_zone_mask;
        s_game_state_scenario *scenario = &game_state_current_header()->scenario;
        scenario->state.flags = 1;
        scenario->state.zone_set_index = zone_set;
        scenario->state.active_structure_bsp_mask = active_bsps;
        scenario->state.active_designer_zone_mask = non_bsp;
        scenario->state.touched_structure_bsp_mask = touched;
        scenario->state.active_cinematic_zone_mask = designer;
        scenario->state.touched_cinematic_zone_mask = cinematic;
    }
    return loaded;
}


// @address 0x19b760 ?game_state_try_and_load_from_persistent_storage@@YAXW4e_controller_index@@@Z
void game_state_try_and_load_from_persistent_storage(e_controller_index controller_index)
{
    bool loaded = false;
    bool game_changed = false;
    s_game_state_header header;
    if (saved_game_files_get_game_state_options(controller_index, &header, sizeof(header))
        && game_state_header_valid(&header)
        && game_state_saved_game_matches_session((const game_state_header *)&header)
        && game_state_prepare_game_from_header(&header, true, true, &game_changed)
        && game_state_header_matches(game_state_current_header(), &header))
    {
        void *buffer = game_state_procs_try_and_get_slot(
            4, 2, g_game_state_async_file_state.save_buffer_size, &g_game_state_load_listener);
        g_game_state_async_file_state.load_buffer_address = buffer;
        if (buffer != 0)
        {
            if (saved_game_files_get_saved_game_state_header(controller_index, (game_state_header *)buffer, k_game_state_file_size)
                && game_state_verify_hash((s_game_state_header *)buffer, false))
            {
                game_state_prepare_for_load(0x404);
                memcpy(game_state_globals.game_state_buffer, buffer, k_game_state_file_size);
                game_state_call_before_load_procs();
                game_state_handle_after_load(0x404);
                loaded = true;
            }
            game_state_call_after_load_procs(4, g_game_state_async_file_state.load_buffer_address);
            g_game_state_async_file_state.load_buffer_address = 0;
        }
        else
        {
            game_state_prepare_for_load(0x404);
            bool read_ok = saved_game_files_get_saved_game_state_header(controller_index,
                (game_state_header *)game_state_globals.game_state_buffer, k_game_state_file_size);
            bool valid = game_state_verify_hash(0, false);
            loaded = valid & read_ok;
            game_state_call_before_load_procs();
            game_state_handle_after_load(0x404);
        }
        if (loaded)
        {
            if (g_saved_film_timer_globals.film.get_mode() == 1)
                saved_film_notify_game_state_load();
            game_state_save();
            game_state_globals.loaded_from_persistent_storage = true;
        }
    }
    if (!loaded && game_changed)
        main_menu_launch();
}

// @address 0x95e608 main_game_globals

// @address 0x848a8 game_state_process_pending_changes
extern "C" void game_state_process_pending_changes()
{
    if (main_game_globals.game_won) {
        bool metagame_blocks_change = false;
        if (g_campaing_metagame_globals != 0) {
            long flags = g_campaing_metagame_globals->flags;
            if ((flags & 2) != 0)
                metagame_blocks_change = ((unsigned long)flags >> 3) & 1;
        }
        if (metagame_blocks_change == 0) {
            if (game_is_cooperative()) {
                c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, 0x100b9);
                if (screen != 0)
                    screen->transition_out_with_transition_type(0, _screen_transition_normal);
            } else {
                bool advanced_to_next_level = false;
                if (simulation_starting_up() && tls_game_globals()->options.game_mode == 1)
                    advanced_to_next_level = main_game_goto_next_level();
                if (advanced_to_next_level == false) {
                    bool session_is_established = false;
                    c_network_session *session = 0;
                    bool local_peer_is_leader = false;
                    bool has_session = false;
                    bool in_range;
                    if ((*network_life_cycle_globals_get()).initialized) {
                        long session_state = (*network_life_cycle_globals_get()).manager.m_active_squad_session->m_local_state;
                        // match: 0x848a8: value range assumption guides branch lowering
                        __assume(session_state >= 0 && session_state < 16);
                        if (session_state != 0) {
                            session = (*network_life_cycle_globals_get()).manager.m_active_squad_session;
                            has_session = true;
                        }
                    }
                    if (has_session) {
                        in_range = (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
                        if (in_range) {
                            session_is_established = true;
                            bool membership_has_local_leader = false;
                            if (session->m_session_membership.m_current_membership.update_number != -1)
                                membership_has_local_leader = session->m_session_membership.local_peer_index()
                                    == session->m_session_membership.leader_peer_index();
                            local_peer_is_leader = membership_has_local_leader;
                        }
                    }
                    if (!session_is_established || local_peer_is_leader) {
                        if (!simulation_starting_up() || tls_game_globals()->options.game_mode != 3)
                            simulation_abort_immediate(_simulation_abort_reason_2);
                        if (user_interface_networking_set_pre_game_state() == false)
                            main_menu_launch();
                    }
                }
            }
            main_game_globals.game_won = false;
        }
    }
    if (main_game_globals.game_change_in_progress) {
        if (main_game_globals.game_unload_pending) {
            if (main_game_globals.status != 6)
                main_game_change_immediate(0);
        } else {
            main_game_change_immediate(&main_game_globals.options);
        }
        main_game_globals.game_change_in_progress = false;
        main_game_globals.game_unload_pending = false;
        main_game_globals.game_change_timer = 0;
    }
}

// The game state memory is carved into restricted sections, which back the four restricted regions.


// @address 0x74b800 g_game_state_compressor_listener
// PC build only: on Xbox update() is the shared empty body at 0x5bec98 and the image holds
// the object's vtable word (0x3ae74).
#if !defined(_XBOX) || defined(__clang__)
void c_game_state_compressor_listener::update(void *buffer)
{
}

c_game_state_compressor_listener g_game_state_compressor_listener;
#endif

static __forceinline unsigned long game_state_crc_new(void)
{
    return 0xffffffff;
}

static __forceinline unsigned char *game_state_initialize_section(
    long section_index, unsigned char *starting_address, unsigned int size, long critical_section_id)
{
    g_restricted_section[section_index].initialize(starting_address, size, critical_section_id);
    return starting_address + size;
}

static __forceinline void game_state_restricted_region_create(
    long index, c_restricted_section *primary_section, long critical_section_index, c_restricted_memory_callbacks *callbacks)
{
    g_restricted_regions[index].initialize(index, primary_section, critical_section_index, callbacks);
}

static __forceinline void game_state_restricted_region_add_mirror(long index, c_restricted_section *mirror_section)
{
    g_restricted_regions[index].add_mirror(mirror_section);
}



// @address 0x19aa48 ?game_state_shell_initialize@@YAXXZ
void game_state_shell_initialize(void)
{
    g_game_state_shared_lock.lock.set(0);
    game_state_globals.allocation_checksum = game_state_crc_new();

    c_allocation_base *allocator = (c_allocation_base *)&g_runtime_state_allocation;

    game_state_globals.compressed.buffer = allocator->allocate(0x4b000, "game-state-compression-scratch");
    game_state_globals.compressed.buffer_size = 0x4b000;
    g_game_state_procs.entries[5].handler = &g_game_state_compressor_listener;
    game_state_globals.compressed.compressed_buffer = 0;
    game_state_globals.compressed.compressed_size = 0;
    game_state_globals.compressed.active = false;
    game_state_globals.compressed.acquire_requested = false;
    game_state_globals.compressed.last = 0;
    game_state_globals.compressed.extra = 0;

    if (g_utility_drive_mounted != 0)
    {
        g_game_state_async_file_state.storage_mode = 2;
        game_state_create_or_open_file();
    }
    else
    {
        g_game_state_async_file_state.storage_mode = 1;
        g_game_state_async_file_state.memory_save_address = _physical_memory_malloc((memory_stage)1, 0, k_game_state_file_size, 0, 0, -1);
    }
    g_game_state_procs.entries[3].handler = &g_game_state_save_listener;
    g_game_state_procs.entries[4].handler = &g_game_state_load_listener;

    unsigned char *address = game_state_initialize_section(0, (unsigned char *)game_state_globals.game_state_buffer, 0x10000, 8);
    address = game_state_initialize_section(1, address, 0x200000, 10);
    address = game_state_initialize_section(2, address, 0x1d0000, 12);
    address = game_state_initialize_section(3, address, 0x400000, 14);
    address = game_state_initialize_section(4, address, 0x400000, 15);
    game_state_initialize_section(5, address, 0x400000, 16);
    long buffer_count;
    if (g_utility_drive_mounted != 0)
        buffer_count = 2;
    else
        buffer_count = 1;
    game_state_globals.size = buffer_count;

    game_state_restricted_region_create(0, &g_restricted_section[0], 7, &g_gamestate_deterministic_allocation_callbacks);
    game_state_restricted_region_create(1, &g_restricted_section[1], 9, &g_gamestate_deterministic_allocation_callbacks);
    game_state_restricted_region_create(2, &g_restricted_section[2], 11, &g_gamestate_nondeterministic_allocation_callbacks);
    game_state_restricted_region_create(3, &g_restricted_section[3], 13, &g_gamestate_deterministic_allocation_callbacks);
    game_state_restricted_region_add_mirror(3, &g_restricted_section[4]);
    game_state_restricted_region_add_mirror(3, &g_restricted_section[5]);

    restricted_region_lock_primary(0);
    restricted_region_lock_primary(1);
    restricted_region_lock_primary(2);
    restricted_region_lock_primary(3);

    long header_member = g_restricted_regions[0].add_member(
        "header", "game_state_header", sizeof(s_game_state_header), 0,
            game_state_set_header_address, 0, 0);
    game_state_globals.current_state = (char *)restricted_region_get_member_address(0, header_member);
    game_state_globals.shell_initialized = true;
}


c_synchronized_long g_game_state_unused_lock;
c_restricted_section g_restricted_section[7];
c_synchronized_long g_game_state_lock;
s_game_state_globals_storage game_state_globals;
extern "C" {
s_game_state_memory_mapping g_game_state_async_file_state;
}
