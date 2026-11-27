// blamlibxbox/game_engine_group.obj: from the records 0x2bc0e8 0x2bc148
// @flags /TP /O2
// @address 0x2bc0e8 ?game_engine_settings_try_and_get@@YAPBUs_game_engine_settings_definition@@XZ

#include "game/game_engine_group.h"
#include "game/game_engine_settings_definitions.h"
#include "game/game_globals.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario.h"

const s_game_engine_settings_definition *game_engine_settings_try_and_get()
{
    const s_game_engine_settings_definition *result = 0;
    const s_game_globals *globals = global_game_globals;
    if (globals && globals->multiplayer_globals.index != -1)
    {
        const s_multiplayer_globals_definition *definition =
            (const s_multiplayer_globals_definition *)tag_get('mulg', globals->multiplayer_globals.index);
        if (definition->universal.count == 1 &&
            ((const s_multiplayer_universal_globals_definition *)definition->universal.address)->game_engine_settings.index != -1)
            result = (const s_game_engine_settings_definition *)tag_get(
                'wezr', ((const s_multiplayer_universal_globals_definition *)definition->universal.address)->game_engine_settings.index);
    }
    return result;
}

// @address 0x2bc148 ?game_engine_settings_player_traits_get_by_name@@YAPBUs_game_engine_player_traits@@J@Z
const s_game_engine_player_traits *game_engine_settings_player_traits_get_by_name(long name)
{
    const s_game_engine_settings_definition *settings = game_engine_settings_try_and_get();
    const s_game_engine_player_traits *result = 0;
    if (settings)
    {
        for (long index = 0; index < settings->player_traits.count; ++index)
        {
            const s_game_engine_player_traits *traits = &((const s_game_engine_player_traits *)settings->player_traits.address)[index];
            if (traits->name == name)
            {
                result = traits;
                break;
            }
        }
    }
    return result;
}
