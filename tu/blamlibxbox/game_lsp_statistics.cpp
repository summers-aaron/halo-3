// @address 0x3e204 ??_7c_overlapped_task@@6B@
// blamlibxbox/game_lsp_statistics.obj: from the records 0x24a6b0 0x24a860 0x24ab60 0x24ae80 0x24aef8 0x24b0b8 0x24b220 0x24b308 0x24b3e8 0x24b560 0x24b5d8 0x24b748 0x24b8b0
// @flags /TP /O2 /Oc
#include "game/game_lsp.h"
#include "game/game_lsp_statistics_externs.h"
#include "game/game_globals_storage.h"
#include "math/real_math_types.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include "cseries/cseries.h"
#include "game/player_types.h"
#include "game/game_results_storage_types.h"
// @address 0x24aef8 game_lsp_submit_campaign_result
// @address 0x24b3e8 game_lsp_statistics_copy_game_info
// @address 0x24b220 game_lsp_submit_matchmaking_quality
// @address 0x20e770 ?overlapped_task_block_until_finished@@YAXPAVc_overlapped_task@@@Z
// @address 0x3a7850 ?network_webstats_submit_buffer@@YA_NW4e_online_lsp_service_type@@PBXJPBDJ@Z
// @address 0x4470d0 ?rasterizer_main_get_video_mode@@YAPAUXVIDEO_MODE@@XZ
// @address 0x24b308 ??0s_blffile_webstats_matchmaking_quality@@QAA@XZ
// @address 0x3ad388 ??0s_blf_chunk_author@@QAA@XZ
// @address 0x24b5d8 game_lsp_statistics_get_controller_stats
#include <string.h>
#include <stdlib.h>
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_types.h"
#include "rasterizer/rasterizer_main.h"
#include "game/game_variant.h"
#include "interface/controller_types.h"
#include "cseries/pointer_utilities.h"
#include "interface/player_profile_types.h"
// @address 0x11401e8 g_controller_globals
#include "networking/session/network_session_interface_types.h"
// @address 0xf283c8 g_network_session_interface_local_users
#include "rasterizer/rasterizer_video_mode_types.h"
#include "networking/logic/storage/network_http_request_queue.h"
#include "networking/tools/blf_header_types.h"
#include "networking/tools/blf_chunk_author_types.h"
#include "networking/tools/network_webstats.h"
#include "networking/online/online_lsp_service_type.h"
// @address 0x24a860 ?game_lsp_submit_multiplayer_result@@YA_NPBUs_game_results@@@Z
#include "game/campaign_metagame_globals.h"
#include "game/campaign_metagame_player_types.h"
#include "game/player_configuration.h"

#include "saved_games/saved_film_recording_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include <new>
#include "networking/tools/blf_end_of_file_types.h"
#include "rasterizer/rasterizer.h"
#include "game/game_results_network_statistics.h"

struct s_blf_chunk_matchmaking_quality_data
{
    // The twelve-byte BLF header overlaps the quality record: its version
    // words land in the quality object's unused first four bytes, and the
    // quality payload (+8) begins four bytes after the header.
    union
    {
        s_blf_header header;
        struct
        {
            long chunk_type;
            long chunk_size;
            c_matchmaking_quality quality;
        };
    };
    unsigned char source_trailer[4];

    __forceinline s_blf_chunk_matchmaking_quality_data()
    {
        header.setup('mqdt', sizeof(*this), 1, 5);
    }
};
typedef char webstats_quality_payload_offset[(offsetof(s_blf_chunk_matchmaking_quality_data, quality)
    + offsetof(c_matchmaking_quality, m_aborted) == 0x10) ? 1 : -1];
typedef char webstats_quality_searches_offset[
    (sizeof(void *) != 4 || ((offsetof(s_blf_chunk_matchmaking_quality_data, quality)
    + offsetof(c_matchmaking_quality, m_session_searches) == 0x64d0))) ? 1 : -1];
typedef char webstats_quality_chunk_size[(sizeof(void *) != 4 || ((sizeof(s_blf_chunk_matchmaking_quality_data) == 0x9880))) ? 1 : -1];

#pragma pack(push, 1)
struct s_blffile_webstats_matchmaking_quality
{
    s_blffile_webstats_matchmaking_quality();

    s_blf_chunk_start_of_file start_of_file;
    s_blf_chunk_author author;
    s_blf_chunk_matchmaking_quality_data matchmaking_quality;
    s_blf_chunk_end_of_file end_of_file;
};

class c_overlapped_task;

struct display_statistics {
    s_blf_header header;
    long widescreen;
    long width;
    long height;
};
// 'clif': the display statistics of the submitting machine.
struct s_blf_chunk_local_info : display_statistics
{
    __forceinline s_blf_chunk_local_info()
    {
        header.setup('clif', sizeof(*this), 1, 1);
        widescreen = 0;
        width = 0;
        height = 0;
    }
};

#pragma pack(push, 8)
struct s_game_lsp_controller_statistics {
    unsigned char combined;
    unsigned char flag_bit2;
    unsigned char flag_bit1;
    unsigned char machine;
    unsigned char live;
    unsigned char silver_gold;
    unsigned char guest;
    // The player identifier must stay four-byte aligned after the flag bytes.
    __declspec(align(4)) s_player_identifier identifier;
    c_player_profile_interface profile;
};
typedef char lsp_controller_identifier_offset[(offsetof(s_game_lsp_controller_statistics, identifier) == 8) ? 1 : -1];
typedef char lsp_controller_statistics_offset[(offsetof(s_game_lsp_controller_statistics, profile) == 0x10) ? 1 : -1];
typedef char lsp_controller_statistics_size[(sizeof(void *) != 4 || ((sizeof(s_game_lsp_controller_statistics) == 0x998))) ? 1 : -1];

// 'glcp': one statistics record, with a copy of the signed-in profile, per controller.
struct s_lsp_local_game_chunk
{
    s_blf_header header;
    s_game_lsp_controller_statistics controllers[4];

    s_lsp_local_game_chunk();
    ~s_lsp_local_game_chunk();
};
#pragma pack(pop)
typedef char lsp_local_game_chunk_controllers_offset[(offsetof(s_lsp_local_game_chunk, controllers) == 0x10) ? 1 : -1];
typedef char lsp_local_game_chunk_size[(sizeof(void *) != 4 || ((sizeof(s_lsp_local_game_chunk) == 0x2670))) ? 1 : -1];

#pragma pack(push, 8)
struct s_lsp_campaign_chunk
{
    s_blf_header header;
    game_options state;

    __forceinline s_lsp_campaign_chunk()
    {
        header.major_version = 1;
        header.minor_version = 1;
        header.chunk_type = 'gmop';
        header.chunk_size = sizeof(*this);
    }
};
#pragma pack(pop)
typedef char lsp_campaign_chunk_size[(sizeof(s_lsp_campaign_chunk) == 0xf820) ? 1 : -1];
typedef char lsp_campaign_chunk_state_offset[(offsetof(s_lsp_campaign_chunk, state) == 0x10) ? 1 : -1];

// BLF payloads are four-byte aligned; the containing chunk supplies alignment.
#pragma pack(push, 4)
// 'cmrp': the campaign metagame results, when the metagame was running.
struct s_blf_chunk_campaign_metagame_globals
{
    s_blf_header header;
    unsigned char valid;
    s_campaing_metagame_globals globals;

    __forceinline s_blf_chunk_campaign_metagame_globals()
    {
        s_campaing_metagame_damage_event *event = &globals.events[0];
        long events_remaining = 255;
        do {
            event->flags = 0;
            event++;
        } while (--events_remaining >= 0);
        header.major_version = 1;
        header.minor_version = 1;
        header.chunk_type = 'cmrp';
        header.chunk_size = sizeof(*this);
    }
};
// 'cmrs': the campaign metagame player scores.
struct s_blf_chunk_campaign_metagame_player_globals
{
    s_blf_header header;
    unsigned char valid;
    s_campaign_metagame_player_globals globals;

    __forceinline s_blf_chunk_campaign_metagame_player_globals()
    {
        header.setup('cmrs', sizeof(*this), 1, 1);
    }
};
struct s_blffile_webstats_campaign_result
{
    s_blffile_webstats_campaign_result();

    s_blf_chunk_start_of_file start_of_file;
    s_blf_chunk_author author;
    s_lsp_campaign_chunk campaign;
    s_lsp_local_game_chunk local_game_lsp;
    s_blf_chunk_campaign_metagame_globals metagame_globals;
    s_blf_chunk_campaign_metagame_player_globals metagame_player_globals;
    s_blf_chunk_local_info local_info;
    s_blf_chunk_end_of_file end_of_file;
};
#pragma pack(pop)
typedef char campaign_metagame_globals_chunk_size[(sizeof(s_blf_chunk_campaign_metagame_globals) == 0x2448) ? 1 : -1];
typedef char campaign_metagame_player_globals_chunk_size[(sizeof(s_blf_chunk_campaign_metagame_player_globals) == 120) ? 1 : -1];
typedef char campaign_extended_results_offset_check[
    (sizeof(void *) != 4 || ((offsetof(s_blffile_webstats_campaign_result, metagame_globals.globals) == 0x11f20))) ? 1 : -1];
typedef char campaign_lsp_offset_check[(offsetof(s_blffile_webstats_campaign_result, local_game_lsp) == 0xf8a0) ? 1 : -1];
typedef char campaign_file_size_check[(sizeof(void *) != 4 || ((sizeof(s_blffile_webstats_campaign_result) == 0x143fc))) ? 1 : -1];

#pragma pack(push,1)
struct s_lsp_machine_statistics
{
    s_entry machine;
    s_game_results_network_peer_statistics peer;
};
struct s_blf_chunk_multiplayer_game_description
{
#pragma pack(push, 8)
    union
    {
        unsigned __int64 alignment;
#pragma pack(push, 1)
        struct
        {
            s_blf_header header;
            unsigned char teams_enabled;
            unsigned __int64 unique_id;
            long map_id;
            char map_path[260];
            unsigned short map_name[32];
            unsigned __int64 game_variant_unique_id;
            unsigned __int64 map_variant_unique_id;
            unsigned char summary_valid;
            long end_reason;
            unsigned char finish_valid;
            long finish_value;
            unsigned char final_flags[3];
        };
#pragma pack(pop)
    };
#pragma pack(pop)
    // The wire payload is rounded to eight bytes without aligning its packed owner.

    __forceinline s_blf_chunk_multiplayer_game_description()
    {
        header.setup('mpgd', sizeof(*this), 2, 1);
    }
};
struct __declspec(align(4)) s_blf_chunk_multiplayer_matchmaking_options
{
    s_blf_header header;
    unsigned short hopper_identifier;
    unsigned short hopper_name[32];
    unsigned char hopper_ranked, hopper_team_based, hopper_unknown48;
    long hopper_mode, hopper_primary, hopper_secondary, hopper_score, hopper_map;
    // The wire chunk is rounded to a four-byte boundary.

    __forceinline s_blf_chunk_multiplayer_matchmaking_options()
    {
        header.setup('mpmo', sizeof(*this), 2, 1);
    }
};
struct s_blf_chunk_multiplayer_game_variant
{
    s_blf_header header;
    c_game_variant variant;

    __forceinline s_blf_chunk_multiplayer_game_variant()
    {
        header.setup('mpvr', sizeof(*this), 3, 1);
    }
};
#pragma pack(push, 2)
struct s_multiplayer_player
{
    unsigned char player_exists;
    unsigned char machine_index;
    unsigned char identifier[8];
    s_player_configuration_from_client client_configuration;
    s_player_configuration_from_host host_configuration;
    unsigned char standing;
    short final_score;
};
#pragma pack(pop)
typedef char lsp_multiplayer_player_size[(sizeof(s_multiplayer_player) == 0x11e) ? 1 : -1];
typedef char lsp_multiplayer_player_score_offset[(offsetof(s_multiplayer_player, final_score) == 0x11c) ? 1 : -1];
// BLF result arrays begin on an eight-byte boundary after the chunk header.
// The 64-bit view supplies that alignment without serialized padding members.
#pragma pack(push, 8)
template<class t_element, long k_count> union s_blf_result_array
{
    unsigned __int64 alignment;
    t_element elements[k_count];
};
#pragma pack(pop)
#pragma pack(push, 8)
struct s_blf_chunk_multiplayer_players
{
    // The serialized player array follows an eight-byte-aligned BLF header.
    union
    {
        s_blf_header header;
        unsigned __int64 header_alignment;
    };
    s_multiplayer_player players[16];

    __forceinline s_blf_chunk_multiplayer_players()
    {
        header.setup('mppl', sizeof(*this), 2, 1);
        memset(players, 0, sizeof(players));
    }
};
#pragma pack(pop)
typedef char lsp_multiplayer_players_offset[(offsetof(s_blf_chunk_multiplayer_players, players) == 0x10) ? 1 : -1];
typedef char lsp_multiplayer_players_chunk_size[(sizeof(s_blf_chunk_multiplayer_players) == 0x11f0) ? 1 : -1];

#pragma pack(push, 8)
struct s_blf_chunk_multiplayer_teams
{
    s_blf_header header;
    s_blf_result_array<s_game_results_team_data, 16> data;

    __forceinline s_blf_chunk_multiplayer_teams()
    {
        header.setup('mptm', sizeof(*this), 1, 1);
        csmemset(data.elements, 0, sizeof(data));
    }
};
#pragma pack(pop)
struct s_blf_chunk_multiplayer_machines
{
    s_blf_header header;
    unsigned char squad, group;
    unsigned short reserved;
    s_lsp_machine_statistics data[16];

    __forceinline s_blf_chunk_multiplayer_machines()
    {
        header.setup('mpma', sizeof(*this), 2, 1);
        memset(data, 0, sizeof(data));
    }
};
#pragma pack(push, 8)
struct s_blf_chunk_multiplayer_statistics_values
{
    s_blf_header header;
    s_blf_result_array<s_game_results_player_statistics, 16> data;

    __forceinline s_blf_chunk_multiplayer_statistics_values()
    {
        header.setup('mps1', sizeof(*this), 2, 1);
        csmemset(data.elements, 0, sizeof(data));
    }
};
#pragma pack(pop)
#pragma pack(push, 8)
struct s_blf_chunk_multiplayer_team_statistics
{
    s_blf_header header;
    s_blf_result_array<s_game_results_statistic, 16 * 16 * 2> data;

    __forceinline s_blf_chunk_multiplayer_team_statistics()
    {
        header.setup('mps2', sizeof(*this), 2, 1);
        csmemset(data.elements, 0, sizeof(data));
    }
};
#pragma pack(pop)
#pragma pack(push, 8)
struct s_blf_chunk_multiplayer_objective_statistics
{
    s_blf_header header;
    s_blf_result_array<s_game_results_team_statistics, 16> data;

    __forceinline s_blf_chunk_multiplayer_objective_statistics()
    {
        header.setup('mps3', sizeof(*this), 2, 1);
        csmemset(data.elements, 0, sizeof(data));
    }
};
#pragma pack(pop)
struct s_blf_chunk_multiplayer_statistics
{
    s_blf_header header;
    long count;
    s_blf_chunk_multiplayer_statistics_values values;
    s_blf_chunk_multiplayer_team_statistics team_statistics;
    s_blf_chunk_multiplayer_objective_statistics objectives;

    __forceinline s_blf_chunk_multiplayer_statistics()
    {
        header.setup('_par', sizeof(*this), 1, 1);
        count = 3;
    }
};
#pragma pack(push, 8)
struct s_blf_chunk_multiplayer_events
{
    s_blf_header header;
    s_blf_result_array<s_game_results_event, 1000> data;

    __forceinline s_blf_chunk_multiplayer_events()
    {
        header.setup('mpev', sizeof(*this), 2, 1);
        csmemset(data.elements, 0, sizeof(data));
    }
};
#pragma pack(pop)
#pragma pack(push, 4)
struct s_blffile_webstats_multiplayer_game_result
{
    s_blffile_webstats_multiplayer_game_result();
    s_blf_chunk_start_of_file start_of_file;
    s_blf_chunk_author author;
    s_blf_chunk_multiplayer_game_description game_description;
    s_blf_chunk_multiplayer_matchmaking_options matchmaking_options;
    s_blf_chunk_multiplayer_game_variant game_variant;
    s_blf_chunk_multiplayer_players players;
    s_blf_chunk_multiplayer_teams teams;
    s_blf_chunk_multiplayer_machines machines;
    s_blf_chunk_multiplayer_statistics statistics;
    s_blf_chunk_multiplayer_events events;
    s_lsp_local_game_chunk local_game_lsp;
    s_blf_chunk_local_info local_info;
    s_blf_chunk_end_of_file end_of_file;
};
#pragma pack(pop)
#pragma pack(pop)
#pragma pack(pop)
typedef char multiplayer_result_file_size_check[(sizeof(void *) != 4 || ((sizeof(s_blffile_webstats_multiplayer_game_result) == 72724))) ? 1 : -1];
typedef char multiplayer_lsp_offset_check[(offsetof(s_blffile_webstats_multiplayer_game_result, local_game_lsp) == 62840) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_teams_data_offset_check[(offsetof(s_blf_chunk_multiplayer_teams, data) == 0x10) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_teams_size_check[(sizeof(s_blf_chunk_multiplayer_teams) == 0x50) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_statistics_values_data_offset_check[(offsetof(s_blf_chunk_multiplayer_statistics_values, data) == 0x10) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_statistics_values_size_check[(sizeof(s_blf_chunk_multiplayer_statistics_values) == 0x4190) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_team_statistics_data_offset_check[(offsetof(s_blf_chunk_multiplayer_team_statistics, data) == 0x10) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_team_statistics_size_check[(sizeof(s_blf_chunk_multiplayer_team_statistics) == 0x410) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_objective_statistics_data_offset_check[(offsetof(s_blf_chunk_multiplayer_objective_statistics, data) == 0x10) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_objective_statistics_size_check[(sizeof(s_blf_chunk_multiplayer_objective_statistics) == 0x670) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_events_data_offset_check[(offsetof(s_blf_chunk_multiplayer_events, data) == 0x10) ? 1 : -1];
typedef char s_blf_chunk_multiplayer_events_size_check[(sizeof(s_blf_chunk_multiplayer_events) == 0x8cb0) ? 1 : -1];
typedef char lsp_results_players_offset[(offsetof(c_game_results, players) == 0x430) ? 1 : -1];
typedef char lsp_results_summary_offset[(offsetof(c_game_results, summary_valid) == 0x41c) ? 1 : -1];
typedef char lsp_results_unique_id_offset[(offsetof(c_game_results, unique_id) == 0x68) ? 1 : -1];

// @address 0x24b8b0 ??4c_string_verify_task@@QAAAAV0@ABV0@@Z
// @address 0x24b748 ??4c_player_profile_interface@@QAAAAV0@ABV0@@Z

static __forceinline void game_lsp_statistics_copy_wide(unsigned short *destination, const unsigned short *source)
{
    for (int i = 0; i <= 32; i++) {
        if ((destination[i] = *source) == 0) break;
        source++;
    }
}

#define TEST_BIT16(flags, bit) (_rotl((flags), 32 - (bit)) & 1)
static __forceinline unsigned __int64 game_lsp_statistics_get_local_user_xuid(long user_index)
{
    return g_network_session_interface_local_users[user_index].xuid;
}
static __forceinline bool game_lsp_statistics_xuid_is_guest_account(unsigned __int64 xuid)
{
    unsigned int account_flags = (unsigned int)(xuid >> 48);
    bool result = (account_flags & 15) == 9 && (account_flags & 192) != 0;
    return result;
}
static __forceinline bool game_lsp_statistics_controller_is_guest(const c_controller_interface *controller)
{
    bool result = false;
    short user_index = controller->user_index;
    if (user_index >= 0 && user_index < 4)
        result = game_lsp_statistics_xuid_is_guest_account(game_lsp_statistics_get_local_user_xuid(user_index));
    return result;
}

// @address 0x24a6b0 ??0s_lsp_local_game_chunk@@QAA@XZ
s_lsp_local_game_chunk::s_lsp_local_game_chunk()
{
    header.setup('glcp', sizeof(*this), 1, 1);
}

// @address 0x24ae80 ??1s_lsp_local_game_chunk@@QAA@XZ
s_lsp_local_game_chunk::~s_lsp_local_game_chunk()
{
}

extern "C" void game_lsp_statistics_get_controller_stats(s_lsp_local_game_chunk *stats)
{
    for (long controller_index = 0; controller_index < k_number_of_controllers; controller_index++) {
        const c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
        bool active = controller->is_signed_in_to_machine() || TEST_BIT16(controller->flags, 2);
        if (active) {
            stats->controllers[controller_index].combined = controller->is_signed_in_to_machine()
                || TEST_BIT16(controller->flags, 2);
            stats->controllers[controller_index].flag_bit2 = TEST_BIT16(controller->flags, 2);
            stats->controllers[controller_index].flag_bit1 = TEST_BIT16(controller->flags, 1);
            stats->controllers[controller_index].machine = controller->is_signed_in_to_machine();
            stats->controllers[controller_index].live = controller->is_signed_in_to_live();
            stats->controllers[controller_index].silver_gold = controller->is_silver_or_gold_live();
            // match: 0x24b5d8: the profile copy reads its source from &user_index + 6 (r29 = controller + 2);
            // addressing it from controller keeps controller + 8 live in r29 instead.
            const short *user_index = &controller->user_index;
            stats->controllers[controller_index].guest = game_lsp_statistics_controller_is_guest(controller);
            controller->get_player_identifier(&stats->controllers[controller_index].identifier);
            stats->controllers[controller_index].profile = *static_cast<const c_player_profile_interface *>(offset_pointer(
                const_cast<short *>(user_index), offsetof(c_controller_interface, player_profile) - offsetof(c_controller_interface, user_index)));
        } else {
            memset(&stats->controllers[controller_index], 0, sizeof(stats->controllers[controller_index]));
        }
    }
}

// The BLF writers copy wire fields at byte alignment, as the packed chunk layouts above.
#pragma pack(push, 1)

extern "C" bool game_lsp_submit_matchmaking_quality(const void *data) {
 bool success=false;
 s_blffile_webstats_matchmaking_quality file;
 csstrnzcpy(file.start_of_file.name,"halo3 match qual",32);
 csstrnzcpy(file.author.build_name,"",16);
 file.author.build_identifier=0x100002e4fui64;
 csstrnzcpy(file.author.build_string,"11855.07.08.20.2317.halo3_ship", 28);
 csstrnzcpy(file.author.author_name,"",16);
 memcpy(&file.matchmaking_quality.quality.m_aborted,data,c_matchmaking_quality::k_serialized_data_size);
 file.end_of_file.total_file_size=offsetof(s_blffile_webstats_matchmaking_quality, end_of_file);
 success=network_webstats_submit_buffer(online_lsp_service_webstats,&file,sizeof(file),"application/x-halo3-qos",-1);
 return success;
}

s_blffile_webstats_matchmaking_quality::s_blffile_webstats_matchmaking_quality()
{
}

// @address 0x24b560 game_lsp_statistics_get_display_info
void game_lsp_statistics_get_display_info(display_statistics *out)
{
    rectangle2d bounds;
    c_rasterizer::get_display_pixel_bounds(&bounds);
    int wide = rasterizer_main_get_video_mode()->fIsWideScreen != 0;
    int height = bounds.y1 - bounds.y0;
    out->width = bounds.x1 - bounds.x0;
    out->height = height;
    out->widescreen = wide;
}

// @address 0x24b0b8 ??0s_blffile_webstats_campaign_result@@QAA@XZ
s_blffile_webstats_campaign_result::s_blffile_webstats_campaign_result()
{
}

// @address 0x24ab60 ??0s_blffile_webstats_multiplayer_game_result@@QAA@XZ
s_blffile_webstats_multiplayer_game_result::s_blffile_webstats_multiplayer_game_result()
{
}

bool game_lsp_submit_campaign_result(game_options *options, s_campaing_metagame_globals *metagame_globals,
    s_campaign_metagame_player_globals *player_globals)
{
    bool submitted;
    s_blffile_webstats_campaign_result campaign_file;
    csstrnzcpy(campaign_file.start_of_file.name, "halo3 campaign", 32);
    csstrnzcpy(campaign_file.author.build_name, "", 16);
    campaign_file.author.build_identifier = 0x100002e4fui64;
    csstrnzcpy(campaign_file.author.build_string, "11855.07.08.20.2317.halo3_ship", 28);
    csstrnzcpy(campaign_file.author.author_name, "", 16);
    campaign_file.campaign.state = *options;
    game_lsp_statistics_get_controller_stats(&campaign_file.local_game_lsp);
    s_campaing_metagame_globals *metagame_destination = &campaign_file.metagame_globals.globals;
    if (metagame_globals) {
        campaign_file.metagame_globals.valid = 1;
        *metagame_destination = *metagame_globals;
    } else {
        campaign_file.metagame_globals.valid = 0;
        memset(metagame_destination, 0, sizeof(*metagame_destination));
    }
    if (player_globals) {
        campaign_file.metagame_player_globals.valid = 1;
        memcpy(&campaign_file.metagame_player_globals.globals, player_globals, sizeof(*player_globals));
    } else {
        campaign_file.metagame_player_globals.valid = 0;
        memset(&campaign_file.metagame_player_globals.globals, 0, sizeof(campaign_file.metagame_player_globals.globals));
    }
    game_lsp_statistics_get_display_info(&campaign_file.local_info);
    campaign_file.end_of_file.total_file_size = offsetof(s_blffile_webstats_campaign_result, end_of_file);
    submitted = network_webstats_submit_buffer(online_lsp_service_webstats, &campaign_file, sizeof(campaign_file),
        "application/x-halo3-campaign", -1);
    return submitted;
}

extern "C" void game_lsp_statistics_copy_game_info(s_blffile_webstats_multiplayer_game_result *webstats,
    const c_game_results *game_results)
{
    game_globals_storage *globals = tls_game_globals();
    bool active;
    if (globals && (globals->initializing || globals->map_active))
        active = true;
    else
        active = false;
    game_options *options = &globals->options;
    if (!active)
        options = 0;
    char *text = webstats->game_description.map_path;
    webstats->game_description.teams_enabled = game_results->teams_enabled;
    webstats->game_description.unique_id = game_results->unique_id;
    webstats->game_description.map_id = game_results->map_id;
    strncpy(text, game_results->map_path, sizeof(webstats->game_description.map_path));
    text[259] = 0;
    game_lsp_statistics_copy_wide(webstats->game_description.map_name, game_results->map_name);
    __int64 game_variant_unique_id;
    if (options)
        game_variant_unique_id = game_variant_compute_unique_id(&options->game_variant);
    else
        game_variant_unique_id = 0;
    webstats->game_description.game_variant_unique_id = game_variant_unique_id;
    __int64 map_variant_unique_id;
    if (options)
        map_variant_unique_id = map_variant_compute_unique_id(&options->map_variant);
    else
        map_variant_unique_id = 0;
    webstats->game_description.map_variant_unique_id = map_variant_unique_id;
    webstats->game_description.summary_valid = game_results->summary_valid;
    webstats->game_description.end_reason = game_results->end_reason;
    webstats->game_description.finish_valid = game_results->finish_valid;
    webstats->game_description.finish_value = game_results->finish_value;
    webstats->game_description.final_flags[0] = game_results->final_flags[0];
    webstats->game_description.final_flags[1] = game_results->final_flags[1];
    webstats->game_description.final_flags[2] = game_results->final_flags[2];
}

static __forceinline unsigned short *game_lsp_statistics_ustrncpy(unsigned short *dest, const unsigned short *source, long count)
{
    for (long i = 0; i <= count; i++)
    {
        if (!(dest[i] = *source))
            break;
        ++source;
    }
    return dest;
}
static __forceinline long game_lsp_statistics_float_bits(const float *f) {float value=*f;return *(long *)&value;}
static __forceinline void game_lsp_statistics_fill_description(s_blffile_webstats_multiplayer_game_result *out,const c_game_results *in) {
 out->matchmaking_options.hopper_identifier=in->hopper_identifier;game_lsp_statistics_ustrncpy(out->matchmaking_options.hopper_name,in->hopper_name,32);
 out->matchmaking_options.hopper_ranked=in->hopper_ranked;
     out->matchmaking_options.hopper_team_based=in->hopper_team_based;
     out->matchmaking_options.hopper_unknown48=in->hopper_unknown48;
     out->matchmaking_options.hopper_mode=in->hopper_mode;
 out->matchmaking_options.hopper_primary=game_lsp_statistics_float_bits(&in->hopper_primary);
     out->matchmaking_options.hopper_secondary=game_lsp_statistics_float_bits(&in->hopper_secondary);
     out->matchmaking_options.hopper_score=in->hopper_score;
     out->matchmaking_options.hopper_map=in->hopper_map;
}
static __forceinline void game_lsp_statistics_fill_players(s_blffile_webstats_multiplayer_game_result *out, const c_game_results *in)
{
    for (long i = 0; i < 16; i++)
    {
        out->players.players[i].player_exists = in->players[i].player_exists;
        out->players.players[i].machine_index = in->players[i].machine_index;
        memcpy(out->players.players[i].identifier, in->players[i].identifier, sizeof(out->players.players[i].identifier));
        memcpy(&out->players.players[i].client_configuration, &in->players[i].configuration, sizeof(in->players[i].configuration));
        out->players.players[i].standing = in->players[i].standing;
        out->players.players[i].final_score = in->players[i].final_score;
    }
}
static __forceinline void game_lsp_statistics_fill_teams(s_blffile_webstats_multiplayer_game_result *out, const c_game_results *in)
{
    for (long i = 0; i < 16; i++)
    {
        out->teams.data.elements[i].team_exists = in->teams.team[i].team_exists;
        out->teams.data.elements[i].final_score = in->teams.team[i].final_score;
        out->teams.data.elements[i].standing = in->teams.team[i].standing;
    }
}
static __forceinline void game_lsp_statistics_fill_machines(s_blffile_webstats_multiplayer_game_result *out, const c_game_results *in)
{
    out->machines.squad = g_game_results_network_statistics.squad;
    out->machines.group = g_game_results_network_statistics.group;
    for (long i = 0; i < 16; i++)
    {
        memcpy(&out->machines.data[i].machine, &in->entries[i], sizeof(out->machines.data[i].machine));
        memcpy(&out->machines.data[i].peer, &g_game_results_network_statistics.peers[i], sizeof(out->machines.data[i].peer));
    }
}
bool game_lsp_submit_multiplayer_result(const s_game_results *source) {
 const c_game_results *results = (const c_game_results *)source;
 s_blffile_webstats_multiplayer_game_result file;
 csstrnzcpy(file.start_of_file.name,"halo3 multiplayer",32);
 csstrnzcpy(file.author.build_name,"",16);
 file.author.build_identifier=0x100002e4fui64;
 csstrnzcpy(file.author.build_string,"11855.07.08.20.2317.halo3_ship", 28);
 csstrnzcpy(file.author.author_name,"",16);
 game_lsp_statistics_copy_game_info(&file,results);
 game_lsp_statistics_fill_description(&file,results);
 memcpy(&file.game_variant.variant,&results->game_variant,sizeof(results->game_variant));
 game_lsp_statistics_fill_players(&file,results);
 game_lsp_statistics_fill_teams(&file,results);
 game_lsp_statistics_fill_machines(&file,results);
 memcpy(file.statistics.values.data.elements,results->statistics.player,sizeof(file.statistics.values.data.elements));
 memcpy(file.statistics.team_statistics.data.elements,results->statistics.pvp_statistics,sizeof(file.statistics.team_statistics.data.elements));
 memcpy(file.statistics.objectives.data.elements,results->statistics.team,sizeof(file.statistics.objectives.data.elements));
 memcpy(file.events.data.elements,results->events,sizeof(file.events.data.elements));
 game_lsp_statistics_get_controller_stats(&file.local_game_lsp);
 game_lsp_statistics_get_display_info(&file.local_info);
 file.end_of_file.total_file_size=offsetof(s_blffile_webstats_multiplayer_game_result, end_of_file);
 bool success=network_webstats_submit_buffer(online_lsp_service_webstats,&file,sizeof(file),"application/x-halo3-multi",-1);
 return success;
}
#pragma pack(pop)

// @address 0x7ea8e8 g_game_results_network_statistics
s_game_results_network_statistics g_game_results_network_statistics;
