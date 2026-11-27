// blamlibxbox/game_achievements.obj: from the records 0x2a6618 0x2a6760
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "shell/shell.h"
// @address 0x2a6618 ?game_grant_achievement_to_local_player@@YAXW4e_controller_index@@W4e_achievement@@_N@Z
// @address 0x2a6760 ?game_conditions_allow_achievement@@YA_NJW4e_achievement@@@Z
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0x3a5458 ?gamer_grant_achievement@@YAXW4e_controller_index@@W4e_achievement@@@Z
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x8a698 ?is_guest@c_controller_interface@@QBA_NXZ
#include <string.h>
#include <stdlib.h>
#define TEST_BIT16(flags, bit) (_rotl((flags), 32 - (bit)) & 1)
#include "game/game.h"
#include "interface/user_interface_matchmaking.h"
#include "game/game_globals.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "interface/hopper_information_types.h"
#include "game/game_achievements.h"
#include "game/game_achievements_types.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "game/game_globals_storage.h"

static __forceinline long game_achievements_local_player_index_get(e_controller_index controller_index)
{
    const game_options *options = &tls_game_globals()->options;
    long result = -1;
    for (long player_index = 0; player_index < 16u; player_index++) {
        const game_player_options *player = &options->players[player_index];
        if (player->valid && player->controller_index == controller_index
            && !memcmp(&g_transport_security_globals.local_identifier, &player->machine_identifier, sizeof(g_transport_security_globals.local_identifier))) {
            result = player_index;
            break;
        }
    }
    return result;
}

void game_grant_achievement_to_local_player(e_controller_index controller_index, e_achievement achievement, bool force)
{
    if (!game_is_cooperative()) {
        s_controller *controller = controller_get(controller_index);
        if (((c_controller_interface *)controller)->in_use() && !TEST_BIT16(controller->flags, 2)
            && !((c_controller_interface *)controller)->is_guest()) {
            bool allowed = force;
            if (!allowed) {
                long player_index = game_achievements_local_player_index_get(controller_index);
                if (player_index >= 0 && player_index < 16 && game_conditions_allow_achievement(player_index, achievement))
                    allowed = true;
            }
            if (allowed)
                gamer_grant_achievement(controller_index, achievement);
        }
    }
}

bool game_conditions_allow_achievement(long player_index, e_achievement achievement)
{
    bool result = false;
    if (!game_is_cooperative()) {
        const game_options *options = &tls_game_globals()->options;
        if (player_index >= 0 && (unsigned long)player_index < 16) {
            bool campaign = options->game_mode == 1;
            bool multiplayer = options->game_mode == 2;
            s_hopper_information hopper = {};
            bool matchmaking = multiplayer && options->matchmade_game;
            unsigned short hopper_identifier;
            if (matchmaking)
                hopper_identifier = options->matchmaking_options.hopper_identifier;
            else
                hopper_identifier = -1;
            bool ranked;
            do {
                if (matchmaking) {
                    bool hopper_valid = user_interface_matchmaking_get_hopper_information(hopper_identifier, &hopper);
                    ranked = hopper.ranked;
                    if (hopper_valid)
                        break;
                }
                ranked = false;
            } while (false);
            unsigned char teams;
            if (multiplayer)
                teams = options->game_variant.flags & 1;
            else
                teams = 0;
            long difficulty = options->campaign_difficulty;
            if (difficulty < 0 || difficulty >= k_number_of_campaign_difficulty_levels)
                difficulty = -1;
            switch (achievement) {
            case _achievement_landfall:
            case _achievement_holdout:
            case _achievement_the_road:
            case _achievement_assault:
            case _achievement_cleansing:
            case _achievement_refuge:
            case _achievement_last_stand:
            case _achievement_the_key:
            case _achievement_return:
            case _achievement_campaign_complete_normal:
            case _achievement_campaign_complete_heroic:
            case _achievement_campaign_complete_legendary:
            case _achievement_marathon_man:
            case _achievement_guerilla:
            case _achievement_demon:
            case _achievement_cavalier:
            case _achievement_askar:
            case _achievement_exterminator:
            case _achievement_ranger:
            case _achievement_vanguard:
            case _achievement_orpheus:
            case _achievement_reclaimer:
                if (campaign)
                    result = true;
                break;
            case _achievement_iron:
            case _achievement_black_eye:
            case _achievement_tough_luck:
            case _achievement_catch:
            case _achievement_fog:
            case _achievement_famine:
            case _achievement_thunderstorm:
            case _achievement_tilt:
            case _achievement_mythic:
                if (campaign && difficulty >= _campaign_difficulty_level_normal)
                    result = true;
                break;
            case _achievement_two_for_one:
            case _achievement_too_close_to_the_sun:
            case _achievement_triple_kill:
            case _achievement_overkill:
            case _achievement_lee_r_wilson_memorial:
            case _achievement_killing_frenzy:
            case _achievement_steppin_razor:
            case _achievement_mongoose_mowdown:
                if (ranked && !teams)
                    result = true;
                break;
            case _achievement_mvp:
                if (ranked && teams)
                    result = true;
                break;
            case _achievement_maybe_next_time_buddy:
                if (matchmaking && !teams)
                    result = true;
                break;
            case _achievement_up_close_and_personal:
            case _achievement_headshot_honcho:
                if ((ranked && !teams) || campaign)
                    result = true;
                break;
            case _achievement_fear_the_pink_mist:
                if (ranked || campaign)
                    result = true;
                break;
            case _achievement_used_car_salesman:
                if (ranked || campaign)
                    result = true;
                break;
            case _achievement_we_re_in_for_some_chop:
                if (ranked || campaign)
                    result = true;
                break;
            default:
                result = true;
                break;
            }
        }
    }
    return result;
}
