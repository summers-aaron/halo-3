// blamlibxbox/game_engine_variant.obj: from the records 0x243820 0x2438b0 0x243980 0x243a08 0x243c38 0x243fb8
// @flags /TP /O2 /Oc
#include <stddef.h>
#include "tag_files/tag_types.h"
#include <string.h>
#include "memory/static_arrays.h"
#include "game/game_types.h"
#include "game/game_variant.h"
#include "game/game_engine_variant.h"
#include "game/game_engine_base_variant.h"
#include "game/game_engine_traits.h"
#include "game/game_engine.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "game/game_engine_group.h"
#include "saved_games/saved_game_file_types.h"
#include "saved_games/saved_game_files.h"
#include "game/game_engine_settings_definitions.h"
#include "game/game_engine_variant_definitions.h"
#include "game/game_engine_vip_variant.h"
#include "game/game_engine_territories_variant.h"
#include "game/game_engine_king_variant.h"
#include "game/game_options.h"
// @address 0x243820 ?game_engine_variant_is_valid@@YA_NPBVc_game_variant@@@Z
// @address 0x2438b0 ?game_engine_variant_describe_invalidity@@YAXPBVc_game_variant@@@Z
// @address 0x243980 ?game_engine_variant_validate@@YA_NPAVc_game_variant@@@Z
// @address 0x268f38 ?set@c_game_engine_infection_variant@@QAAXPBUs_game_engine_infection_variant_definition@@@Z
// @address 0x26a928 ?set@c_game_engine_territories_variant@@QAAXPBUs_game_engine_territories_variant_definition@@@Z

#include "game/game_engine_string_types.h"

struct s_tag_defined_variant_entry { long name_index; long description_index; };

#include "main/main_game.h"

#include "saved_games/saved_game_files_operations.h"
#include "game/game_engine_infection_variant.h"
#include "game/game_engine_definition_setters.h"
#include "game/game_engine_variant_display.h"

#ifdef HALO3_64BIT
#include "game/game_engine_juggernaut_variant.h"
#include "game/game_engine_sandbox_variant.h"
#include "game/game_engine_slayer.h"
#endif

bool game_engine_variant_is_valid(const c_game_variant *variant)
{
    if (main_game_globals.status == 2)
    {
        c_game_variant local;
        game_variant_set_from(&local, variant);
        return game_engine_variant_validate(&local);
    }
    return true;
}

void game_engine_variant_describe_invalidity(const c_game_variant *variant)
{
    if (!game_engine_variant_is_valid(variant))
    {
        c_game_variant local;
        local.copy_from_and_validate(variant);

        for (long offset = 0;
             offset < sizeof(local.variant_storage) && variant->variant_storage[offset] == local.variant_storage[offset];
             offset++)
        {
        }
    }
}

bool game_engine_variant_validate(c_game_variant *variant)
{
    bool result;
    c_game_variant local;
    local.copy_from_and_validate(variant);
    result = local.is_equal_to(variant);
    *variant = local;
    return result;
}

// @address 0x243fb8 ?game_engine_tag_defined_variant_get_strings@@YA_NW4e_game_engine_type@@JPAV?$c_static_wchar_string@$0DA@@@PAV?$c_static_wchar_string@$0BAA@@@@Z
bool game_engine_tag_defined_variant_get_strings(
    e_game_engine_type engine_type, long index, c_static_wchar_string<48> *name, c_static_wchar_string<256> *description)
{
    bool result = false;
    const s_game_engine_settings_definition *settings = game_engine_settings_try_and_get();
    if (settings) {
        c_static_wchar_string_1024 text;
        switch (engine_type - 1) {
        case 1:
            if (index >= 0 && index < settings->slayer.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->slayer.address + index * 80);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 2:
            if (index >= 0 && index < settings->oddball.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->oddball.address + index * 80);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 0:
            if (index >= 0 && index < settings->ctf.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->ctf.address + index * 80);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 8:
            if (index >= 0 && index < settings->king.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->king.address + index * 88);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 9:
            if (index >= 0 && index < settings->vip.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->vip.address + index * 92);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 3:
            if (index >= 0 && index < settings->juggernaut.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->juggernaut.address + index * 76);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 7:
            if (index >= 0 && index < settings->territories.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->territories.address + index * 76);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 5:
            if (index >= 0 && index < settings->infection.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->infection.address + index * 92);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 6:
            if (index >= 0 && index < settings->assault.count) {
                const s_tag_defined_variant_entry *entry = (const s_tag_defined_variant_entry *)((const char *)settings->assault.address + index * 84);
                text.string[0] = 0;
                game_engine_get_multiplayer_string(entry->name_index, &text);
                name->set(text.string);
                game_engine_get_multiplayer_string(entry->description_index, &text);
                description->set(text.string);
                result = true;
            }
            break;
        case 4:
        default:
            // match: 0x243fb8: unreachable hint omits dead branch check
            __assume(0);
            break;
        }
    }
    return result;
}

// @address 0x243a08 game_engine_variant_initialize_default
extern "C" c_game_variant *game_engine_variant_initialize_default(c_game_variant *output_variant, int game_engine_index)
{
    unsigned short storage_flags;
    c_game_variant default_variant;
    int item_metadata[64];
    int variant_name[512];
    default_variant.set_game_engine_index((e_game_engine_type)game_engine_index);
    c_game_engine_base_variant *base_variant = (c_game_engine_base_variant *)default_variant.m_variant_storage;
    *(unsigned short *)variant_name = 0;
    base_variant->get_game_engine_name((c_static_wchar_string<1024> *)variant_name);
    saved_game_files_initialize_metadata((s_saved_game_item_metadata_record *)item_metadata, -1,
        game_engine_index_to_saved_game_file_type((e_game_engine_type)game_engine_index), (const wchar_t *)variant_name,
        L"", 0, 0x3bc);
    storage_flags = base_variant->m_flags.m_storage;
    item_metadata[57] = game_engine_index;
    base_variant->m_flags.m_storage = (storage_flags & -2);
    game_engine_variant_set_metadata((s_game_engine_base_variant_metadata_view *)base_variant,
        (const s_saved_game_item_metadata_packed *)item_metadata);
    game_variant_set_from(output_variant, &default_variant);
    return output_variant;
}

static __forceinline char *game_engine_variant_begin_built_in(c_game_variant *game_variant, long type)
{
    game_engine_variant_initialize_default(game_variant, type);
    char *storage = game_variant->variant_storage;
    ((c_game_engine_base_variant *)storage)->m_flags.m_storage &= ~1;
    return storage;
}

#ifdef HALO3_64BIT
struct s_game_engine_variant_slayer_preset
{
    s_game_engine_base_variant_definition base;
    short team_scoring;
    short score_to_win;
    signed char kill;
    signed char assist;
    signed char death;
    signed char suicide;
    signed char betrayal;
    signed char leader_killed;
    signed char elimination;
    signed char assassination;
    signed char headshot;
    signed char melee;
    signed char sticky;
    signed char splatter;
    signed char killing_spree;
    long leader_traits;
};
static_assert(sizeof(s_game_engine_variant_slayer_preset) == 80, "Slayer tag preset size");

static void game_engine_variant_set_slayer_preset(c_game_engine_slayer_variant *variant,
    const s_game_engine_variant_slayer_preset *definition)
{
    // The legacy setter's storage overlay fixes the scoring fields at Xbox offsets.
    game_engine_base_variant_set(variant, &definition->base, _game_engine_slayer);
    const short team_scoring = definition->team_scoring;
    variant->m_team_scoring_method = team_scoring >= 0 && team_scoring <= 3 ? team_scoring : 0;
    variant->set_score_to_win(definition->score_to_win);
    variant->set_kill_points(definition->kill);
    variant->set_assist_points(definition->assist);
    variant->set_death_points(definition->death);
    variant->set_suicide_points(definition->suicide);
    variant->set_betrayal_points(definition->betrayal);
    variant->set_leader_killed_points(definition->leader_killed);
    variant->set_elimination_points(definition->elimination);
    variant->set_assassination_points(definition->assassination);
    variant->set_headshot_points(definition->headshot);
    variant->set_melee_points(definition->melee);
    variant->set_sticky_points(definition->sticky);
    variant->set_splatter_points(definition->splatter);
    variant->set_killing_spree_points(definition->killing_spree);
    variant->m_leader_traits.set(game_engine_settings_player_traits_get_by_name(definition->leader_traits), true);
}

template<typename t_variant>
static void game_engine_variant_move_preset_fields(char *storage)
{
    // The Sandbox and Juggernaut setters use overlays with a fixed 0x1b0 base
    // span. On 64-bit, the derived fields start after the four padding bytes.
    const size_t legacy_offset = 0x1b0;
    const size_t fields_offset = offsetof(t_variant, m_base_padding) + sizeof(t_variant::m_base_padding);
    static_assert(fields_offset == legacy_offset + 4, "64-bit preset field offset");
    memmove(storage + fields_offset, storage + legacy_offset, sizeof(t_variant) - fields_offset);
    memset(storage + legacy_offset, 0, fields_offset - legacy_offset);
}
#endif

// @address 0x243c38 ?game_engine_tag_defined_variant_get_built_in_variant@@YA_NW4e_game_engine_type@@JPAVc_game_variant@@@Z
bool game_engine_tag_defined_variant_get_built_in_variant(e_game_engine_type engine_type, long index, c_game_variant *game_variant)
{
    bool result = false;
    const s_game_engine_settings_definition *settings = game_engine_settings_try_and_get();
    if (settings) {
        switch (engine_type - 1) {
        case 1:
            if (index >= 0 && index < settings->slayer.count) {
                const void *definition = (const char *)settings->slayer.address + index * 80;
                char *storage = game_engine_variant_begin_built_in(game_variant, 2);
#ifdef HALO3_64BIT
                game_engine_variant_set_slayer_preset(reinterpret_cast<c_game_engine_slayer_variant *>(storage),
                    static_cast<const s_game_engine_variant_slayer_preset *>(definition));
#else
                game_engine_slayer_variant_set_from_definition((oddball_unit_slayer_variant *)storage, (const oddball_unit_slayer_definition *)definition);
#endif
                result = true;
            }
            break;
        case 2:
            if (index >= 0 && index < settings->oddball.count) {
                const void *definition = (const char *)settings->oddball.address + index * 80;
                char *storage = game_engine_variant_begin_built_in(game_variant, 3);
                game_engine_oddball_variant_set_from_definition(
                    (c_game_engine_oddball_variant *)storage, (const s_game_engine_oddball_variant_definition *)definition);
                result = true;
            }
            break;
        case 0:
            if (index >= 0 && index < settings->ctf.count) {
                const void *definition = (const char *)settings->ctf.address + index * 80;
                char *storage = game_engine_variant_begin_built_in(game_variant, 1);
                ((c_game_engine_infection_variant *)storage)->set((const s_game_engine_infection_variant_definition *)definition);
                result = true;
            }
            break;
        case 8:
            if (index >= 0 && index < settings->king.count) {
                const void *definition = (const char *)settings->king.address + index * 88;
                char *storage = game_engine_variant_begin_built_in(game_variant, 9);
                game_engine_assault_variant_set_from_definition(
                    (c_game_engine_assault_variant *)storage, (const s_game_engine_assault_variant_definition *)definition);
                result = true;
            }
            break;
        case 9:
            if (index >= 0 && index < settings->vip.count) {
                const void *definition = (const char *)settings->vip.address + index * 92;
                char *storage = game_engine_variant_begin_built_in(game_variant, 10);
                game_engine_king_variant_set_from_definition(storage, definition);
                result = true;
            }
            break;
        case 3:
            if (index >= 0 && index < settings->juggernaut.count) {
                const void *definition = (const char *)settings->juggernaut.address + index * 76;
                char *storage = game_engine_variant_begin_built_in(game_variant, 4);
                ((c_game_engine_king_variant *)storage)->set((const s_game_engine_king_variant_definition *)definition);
                result = true;
            }
            break;
        case 4:
            if (index >= 0 && index < settings->sandbox.count) {
                const void *definition = (const char *)settings->sandbox.address + index * 68;
                char *storage = game_engine_variant_begin_built_in(game_variant, 5);
                game_engine_zone_variant_set((const s_game_engine_zone_variant_definition *)definition, (s_game_engine_zone_variant *)storage);
#ifdef HALO3_64BIT
                game_engine_variant_move_preset_fields<c_game_engine_sandbox_variant>(storage);
#endif
                result = true;
            }
            break;
        case 7:
            if (index >= 0 && index < settings->territories.count) {
                const void *definition = (const char *)settings->territories.address + index * 76;
                char *storage = game_engine_variant_begin_built_in(game_variant, 8);
                ((c_game_engine_territories_variant *)storage)->set((const s_game_engine_territories_variant_definition *)definition);
                result = true;
            }
            break;
        case 5:
            if (index >= 0 && index < settings->infection.count) {
                const void *definition = (const char *)settings->infection.address + index * 92;
                char *storage = game_engine_variant_begin_built_in(game_variant, 6);
                game_engine_territories_variant_set_from_definition(
                    (c_game_engine_vip_variant *)storage, (const s_game_engine_vip_settings_definition *)definition);
                result = true;
            }
            break;
        case 6:
            if (index >= 0 && index < settings->assault.count) {
                const void *definition = (const char *)settings->assault.address + index * 84;
                char *storage = game_engine_variant_begin_built_in(game_variant, 7);
                game_engine_juggernaut_variant_set_from_definition(
                    (s_assault_unit_variant *)storage, (const s_assault_unit_definition *)definition);
#ifdef HALO3_64BIT
                game_engine_variant_move_preset_fields<c_game_engine_juggernaut_variant>(storage);
#endif
                result = true;
            }
            break;
        }
    }
    return result;
}
