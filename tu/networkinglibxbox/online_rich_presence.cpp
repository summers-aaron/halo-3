// networkinglibxbox/online_rich_presence.obj: from the records 0x405aa8 0x405e80
// @flags /TP /O2 /Oc
#include "main/levels.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "interface/user_interface_matchmaking.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_interface_types.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "game/game_types.h"
#include "networking/online/online_rich_presence.h"
#include "interface/hopper_information_types.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_membership_types.h"
#include "interface/gui_game_mode_types.h"
#include "memory/data_types.h"
#include "main/levels_globals.h"
#include "interface/controller_types.h"
#include "game/game_variant.h"
#include "networking/online/online_presence_accessors.h"
#include "interface/controller_inline.h"
extern "C" {
#include "networking/online/online_rich_presence_constants.h"
#include "cseries/crt_xdk.h"
#include "xbox/online_services.h"
// @address 0x73a7ac k_rich_presence_context_ids
unsigned long k_rich_presence_context_ids[6] = { 0x17, 0x1b, 0x1a, 0x1c, 0x1d, 0x8001 };
}

c_online_rich_presence_manager g_rich_presence_manager;


static __forceinline bool online_rich_presence_get_multiplayer_level(long map_id, s_datum *level)
{
    return levels_try_and_get_datum(online_presence_multiplayer_levels(), map_id, level);
}
static __forceinline bool online_rich_presence_set_difficulty(s_presence_contexts *context)
{
    bool valid = false;
    switch (network_squad_session_get_campaign_difficulty()) {
    case _campaign_difficulty_level_easy: context->difficulty = 0; valid = true; break;
    case _campaign_difficulty_level_normal: context->difficulty = 1; valid = true; break;
    case _campaign_difficulty_level_heroic: context->difficulty = 2; valid = true; break;
    case _campaign_difficulty_level_legendary: context->difficulty = 3; valid = true; break;
    }
    return valid;
}

static __forceinline bool online_rich_presence_user_is_guest(const short &user_index)
{
    bool guest = false;
    if (user_index >= 0)
    if (user_index < 4)
        guest = online_presence_user_slot(user_index).state == 2;
    return guest;
}
static __forceinline bool online_rich_presence_in_squad_session(c_network_session **session)
{
    bool in_session = false;
    if (online_presence_life_cycle_globals().initialized) {
        if (!online_presence_life_cycle_globals().manager.m_active_squad_session->disconnected()) {
            *session = online_presence_life_cycle_globals().manager.m_active_squad_session;
            in_session = true;
        }
    }
    return in_session;
}
// @address 0x405aa8 ?update@c_online_rich_presence_manager@@QAAXXZ
void c_online_rich_presence_manager::update()
{
    const bool initialized = online_presence_life_cycle_globals().initialized;
    bool in_session = false;
    c_network_session *session = 0;
    if (online_rich_presence_in_squad_session(&session))
        in_session = session != 0 && !session->leaving_session();
    if (in_session) {
        long state = 0;
        if (initialized)
            state = online_presence_life_cycle_globals().manager.m_current_state;
        switch (state) {
        case _life_cycle_state_pre_game:
        {
            long game_mode = ((long)::network_life_cycle_squad_session_get_ui_game_mode());
            for (long user = 0; user < k_number_of_controllers; ++user) {
                switch (game_mode) {
                case _gui_game_mode_campaign: m_contexts[user].game_mode = 0; break;
                case _gui_game_mode_matchmaking: m_contexts[user].game_mode = 1; break;
                case _gui_game_mode_multiplayer: m_contexts[user].game_mode = 2; break;
                case _gui_game_mode_mapeditor: m_contexts[user].game_mode = 3; break;
                case _gui_game_mode_theater: m_contexts[user].game_mode = 4; break;
                }
                m_contexts[user].state = 1;
            }
            break;
        }
        case _life_cycle_state_start_game:
        case _life_cycle_state_in_game:
        case _life_cycle_state_end_game_write_stats:
        case _life_cycle_state_in_match:
        case _life_cycle_state_end_match_write_stats:
            set_presence_state_to_game();
            break;
        case _life_cycle_state_matchmaking_start:
        case _life_cycle_state_matchmaking_find_match_client:
        case _life_cycle_state_matchmaking_find_match:
        case _life_cycle_state_matchmaking_find_and_assemble_match:
        case _life_cycle_state_matchmaking_assemble_match:
        case _life_cycle_state_matchmaking_arbitration:
        case _life_cycle_state_matchmaking_select_host:
        case _life_cycle_state_matchmaking_prepare_map:
        case _life_cycle_state_post_match: {
            unsigned short hopper = network_squad_session_get_hopper();
            for (long user = 0; user < k_number_of_controllers; ++user) {
                s_hopper_information information;
                if (hopper != 0xffff &&
                    ::user_interface_matchmaking_get_hopper_information(hopper, &information) &&
                    information.kind != 0)
                {
                    m_contexts[user].hopper = information.kind;
                    m_contexts[user].state = 2;
                } else {
                    m_contexts[user].state = 10;
                }
            }
            break;
        }
        }
    } else {
        for (long user = 0; user < k_number_of_controllers; ++user)
            m_contexts[user].state = 0;
    }
    for (long user = 0; user < k_number_of_controllers; ++user) {
        if (online_rich_presence_user_is_guest(controller_get(static_cast<e_controller_index>(user))->user_index))
            m_contexts[user].state = 11;
    }
    if (GetTickCount() > m_last_update_time + 1000) {
        bool changed = false;
        long user = 0;
        s_presence_contexts *current = m_contexts;
        for (; user < k_number_of_controllers; ++user, ++current)
        {
            c_controller_interface *controller = controller_get(static_cast<e_controller_index>(user));
            long *values = &current->game_mode;
            if (controller->is_signed_in_to_live() ||
                (online_local_xuid_is_online_enabled() && online_rich_presence_user_is_guest(controller->user_index))) {
                unsigned long *type = k_rich_presence_context_ids;
                for (long index = 0; index < 6; ++type, ++index) {
                    if (values[index] != values[index + 24] && values[index] != -1) {
                        XUserSetContextEx(user, *type, values[index], 0);
                        changed = true;
                        values[index + 24] = values[index];
                    }
                }
            } else {
                for (long index = 0; index < 6; ++index)
                    values[index + 24] = -1;
            }
        }
        if (changed)
            m_last_update_time = GetTickCount();
    }
}

// @address 0x405e80 ?set_presence_state_to_game@c_online_rich_presence_manager@@AAAXXZ
void c_online_rich_presence_manager::set_presence_state_to_game()
{
    long game_mode = ((long)::network_life_cycle_squad_session_get_ui_game_mode());
    for (long user = 0; user < k_number_of_controllers; ++user) {
        s_presence_contexts *context = &m_contexts[user];
        switch (game_mode) {
        case _gui_game_mode_campaign:
            if (online_rich_presence_set_difficulty(context)) {
                long player_count = 0;
                if (network_squad_session_get_membership(0, 0, 0, 0, 0, 0, 0, &player_count, 0, 0) && player_count > 1)
                    context->state = 4;
                else
                    context->state = 3;
            }
            break;
        case _gui_game_mode_matchmaking: {
            unsigned short hopper = network_squad_session_get_hopper();
            s_hopper_information information;
            if (hopper != 0xffff && ::user_interface_matchmaking_get_hopper_information(hopper, &information) &&
                information.kind != 0)
            {
                context->hopper = information.kind;
                context->state = 5;
            } else {
                context->state = 9;
            }
            break;
        }
        case _gui_game_mode_multiplayer: {
            long engine = -1;
            const c_game_variant *variant = user_interface_game_settings_get_game_variant();
            if (variant) {
                switch (variant->game_engine_index) {
                case _game_engine_type_none: engine = -1; break;
                case _game_engine_type_ctf: engine = 1; break;
                case _game_engine_type_slayer: engine = 2; break;
                case _game_engine_type_oddball: engine = 3; break;
                case _game_engine_type_koth: engine = 4; break;
                case _game_engine_type_sandbox: engine = 5; break;
                case _game_engine_type_vip: engine = 6; break;
                case _game_engine_type_jugg: engine = 7; break;
                case _game_engine_type_terries: engine = 8; break;
                case _game_engine_type_assault: engine = 9; break;
                case _game_engine_type_zombiez: engine = 10; break;
                }
                context->game_engine = engine;
                context->state = 6;
            }
            break;
        }
        case _gui_game_mode_mapeditor: {
            long campaign = -1, map = -1;
            char name[128];
            s_datum level;
            ::network_squad_session_get_map((e_campaign_id *)&campaign, (e_map_id *)&map, (char *)(name));
            if (campaign == -1 && global_scenario &&
                online_rich_presence_get_multiplayer_level(global_scenario->map_id, &level)) {
                context->map = level.presence_context_id;
                context->state = 7;
            }
            break;
        }
        case _gui_game_mode_theater: context->state = 8; break;
        }
    }
}
