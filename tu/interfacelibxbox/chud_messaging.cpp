// interfacelibxbox/chud_messaging.obj: from the records 0x134408 0x134490 0x134f08 0x135258 0x39d7b8 0x47a720 0x47a790 0x47a860 0x47a948 0x47aa30 0x47ab18 0x47ab80 0x47acd0 0x47ada8 0x47aec8 0x47af80 0x47aff0 0x47b7d8 0x4a3f58
#include "game/players_global_data.h"
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "interface/chud/chud_manager.h"
#include "game/player_control_globals.h"
#include "text/font_id_types.h"
#include "text/character_data_index_types.h"
#include "game/game_time_types.h"
#include <string.h>
#include <math.h>
#include "game/player_control_types.h"
#include "objects/object_placement.h"
#include "memory/static_arrays.h"
#include "game/players.h"
#include "memory/data_types.h"
#include "objects/objects.h"
#include "game/game_engine_display.h"
#include "interface/chud/chud.h"
#include "interface/chud/chud_globals_definitions.h"
#include "interface/chud/chud_messaging.h"
#include "interface/user_interface_text.h"
#include "interface/user_interface_text_character_types.h"
#include "networking/network_utilities.h"
#include "game/campaign_metagame.h"
#include "items/weapons.h"
#include "game/player_types.h"
#include "interface/chud/chud_medal_message_types.h"
#include "interface/chud/chud_messaging_handle_types.h"
#include "interface/chud/utf32_types.h"
#include "game/player_interaction.h"
#include "interface/chud/chud_message_context_types.h"
#include "game/game_globals_storage.h"
#include "game/player_mapping_types.h"
#include "cseries/cseries.h"
class c_font_cache;
// @address 0x47ab80 ?post@c_chud_messaging_manager@@QAAXPB_WW4e_chud_message_context@@@Z
// @address 0x47acd0 ?post_medal@c_chud_messaging_manager@@QAAXJJ@Z
// @address 0x47b7d8 ?chud_weapon_state_is_depleted@@YA_NPBUweapon_interface_state@@@Z
// @address 0x134f08 ?update_medals@c_chud_messaging_manager@@AAAXJ@Z

#include "memory/tls_globals.h"

#include "text/string_list_hud.h"
#include "tag_files/string_ids.h"
// @address 0x4932c8 ?user_interface_parse_string@@YA_NJPA_WJ@Z

#include "interface/chud/chud_message_types.h"

#include "items/weapon_interface_state_types.h"

#include "interface/chud/chud_messaging_manager.h"
#include "interface/chud/chud_user_data_types.h"

#include "text/font_cache_thread_state_types.h"


#include "bitmaps/bitmap_group_definitions.h"
#include <wchar.h>
#include "text/font_cache_lock.h"
#include "text/font_cache_types.h"
struct s_font_character;
#include "interface/chud/chud_draw_string.h"
struct cache_file_tag_instance;
#include "tag_files/tag_groups.h"
#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"
#include "objects/object_datum.h"
#include "game/players_globals_types.h"
#include "units/unit_types.h"
#include "units/unit_seat.h"
#include "units/unit_definitions.h"
#include "units/vehicle_definitions.h"
#include "items/item_definitions.h"
#include "devices/device_definitions.h"
#include "game/game_time_access.h"
#include "objects/object_header_access.h"
#include "text/unicode.h"
union s_chud_action_scratch
{
    weapon_interface_state weapon;
    object_marker markers[1];
};

// @address 0x134408 ?chud_get_string@@YAXJJPAV?$c_static_wchar_string@$0BAA@@@@Z
void chud_get_string(long message_index, long string_index, c_static_wchar_string<256> *string)
{
    string->clear();
    if (message_index >= 0 && message_index < 4)
    {
        s_chud_globals_definition *table = chud_globals;
        if (table != 0)
        {
        long row_index = chud_get_user_skin_index(message_index);
        long string_list_index = table->rows[row_index].string_list.index;
            if (string_list_index != -1 && string_list_get_normal_string(string_list_index, string_index, string))
            {
                user_interface_parse_string(message_index, string->m_string, 256);
            }
        }
    }
}


// @address 0x134490 chud_messaging_set_text
extern "C" void chud_messaging_set_text(c_chud_messaging_manager *block, const wchar_t *text, long value, long unused, long index)
{
    s_chud_text_entry *entry = &block->m_state_messages[index];
    if (entry->active && chud_messaging_is_text_equal(entry->text, text))
    {
        ustrnzcpy_inline(entry->text, text, k_chud_message_length);
        return;
    }
    ustrnzcpy_inline(entry->text, text, k_chud_message_length);
    entry->precached = 0;
    entry->duration = value;
    entry->active = entry->text[0] != 0;
}

// The direct CHUD pairing for 0x39d7b8 is spurious. The network_utilities
// debug body at 0x82497a18 supplies the source order and actual signature.
// @address 0x39d7b8 ?network_get_unique_player_name@@YA_NPA_WJJPAPB_W@Z
// @address 0x58f528 wcscmp

// @address 0x189420 ??0c_chud_draw_string@@QAA@XZ
// @address 0x189960 ??0c_font_cache@@QAA@XZ
// @address 0x18a118 ?predict_character@c_chud_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@@Z
// @address 0x18a520 ?draw@c_draw_string@@UAA_NPAVc_font_cache@@PBUutf32@@@Z
// @address 0x18aea0 ?precache_character@c_chud_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@PAPBUs_font_character@@@Z
// @address 0x18b400 ?draw@c_draw_string@@UAA_NPAVc_font_cache@@PB_W@Z
// @address 0x1f1358 ?character_data_get_hardware_block_index@c_font_cache@@UAAJW4e_character_data_index@@@Z
// @address 0x290508 ?draw@c_draw_string@@UAA_NPAVc_font_cache@@PBD@Z


static __forceinline long chud_messaging_seconds_to_ticks_round(float seconds)
{
    return (long)((float)game_time_globals->ticks_per_second * seconds + 0.5f);
}

static __forceinline float chud_messaging_seconds_to_ticks_real(float seconds)
{
    return (float)game_time_globals->ticks_per_second * seconds;
}

// @address 0x98c90 pow
// @address 0xeded8 floor

// 0x134f08
void c_chud_messaging_manager::update_medals(long)
{
    s_chud_medal_message *medals = m_medals;
    long next = -1;
    long i;
    for (i = 0; i < 4; ++i)
    {
        if (medals[i].active && !medals[i].shown)
            next = i;
    }
    if (next != -1)
    {
        for (i = 0; i < 4; ++i)
        {
            if (medals[i].active && medals[i].shown)
                medals[i].target = medals[i].target + 1.0f;
        }
        medals[next].shown = 1;
        medals[next].display_time = game_time_get();
    }
    float maximum = (float)(long)(((game_globals->options.game_mode - 1) != 0) + 3);
    float limit = maximum + 1.0f;
    float step = 1.0f / ((float)game_time_globals->ticks_per_second * 0.25f);
    for (i = 0; i < 4; ++i)
    {
        s_chud_medal_message *medal = &medals[i];
        if (medal->active && medal->shown)
        {
            if (medal->value < medal->target)
            {
                float value = medal->value + step;
                if (value > medal->target)
                    value = medal->target;
                medal->value = value;
                if (value > limit)
                    value = limit;
                medal->value = value;
            }
            float current = medal->value;
            float whole = (float)floor((double)current);
            float fraction = medal->value - whole;
            medal->eased = (float)pow((double)fraction, 0.5) + whole;
            if (whole == maximum)
                medal->alpha = 1.0f - fraction;
            else if (whole > maximum)
                medal->alpha = 0.0f;
            else
                medal->alpha = 1.0f;
            long elapsed = (game_time_get() - chud_messaging_seconds_to_ticks_round(3.5f)) - medal->display_time;
            float progress = (float)elapsed / chud_messaging_seconds_to_ticks_real(1.5f);
            progress = MIN(MAX(progress, 0.0f), 1.0f);
            medal->alpha = (1.0f - progress) * medal->alpha;
            if (!(medal->alpha >= 0.0001f))
                medal->active = 0;
        }
    }
}

// 0x135258
// @address 0x135258 ?update_state_messages@c_chud_messaging_manager@@AAAXJ@Z
void c_chud_messaging_manager::update_state_messages(long user_index)
{
    for (long i = 0; i < 4; ++i)
    {
        s_chud_text_entry *message = &m_state_messages[i];
        if (!message->precached && message->text[0])
        {
            c_chud_draw_string draw;
            c_font_cache cache;
            if (draw.precache_wchar_string_for_font(&cache, message->text, _full_screen_hud_message_font))
            {
                message->precached = 1;
                message->start_time = game_time_get();
            }
        }
        if (message->precached)
        {
            if (i < 2)
            {
                long fade_ticks = chud_messaging_seconds_to_ticks_round(0.15f);
                long elapsed_ticks = game_time_get() - message->start_time;
                float progress = (float)elapsed_ticks / (float)fade_ticks;
                progress = MIN(progress, 1.0f);
                message->scale = 1.0f + (1.0f - progress);
                message->offset = 1.0f - progress;
            }
            else
            {
                message->scale = 1.0f;
                message->offset = 0.0f;
            }
            if (message->duration > 0)
            {
                long elapsed_ticks = game_time_get() - message->start_time;
                float progress = (float)(elapsed_ticks - message->duration) / chud_messaging_seconds_to_ticks_real(2.0f);
                progress = MIN(MAX(progress, 0.0f), 1.0f);
                message->alpha *= 1.0f - progress;
            }
            else
            {
                message->alpha = 1.0f;
            }
        }
    }
}

// 0x39d7b8
bool network_get_unique_player_name(wchar_t *name, long size, long name_count, const wchar_t **names)
{
    long attempts = 0;
    bool unique = false;
    memset(name, 0, sizeof(wchar_t) * 16);
    while (network_get_random_player_name(name, 16))
    {
        unique = true;
        for (long index = 0; index < 16; ++index)
        {
            if (names[index] && wcscmp(names[index], name) == 0)
            {
                unique = false;
                break;
            }
        }
        if (unique)
            break;
        ++attempts;
        if (attempts >= 100)
            break;
    }
    return unique;
}

static __forceinline c_chud_messaging_manager *chud_messaging_user_manager(long user_index)
{
    return &c_chud_manager::x_persistent_user_data[user_index].m_messaging_manager;
}

// @address 0x47a720 ?chud_messaging_post@@YAXJJW4e_chud_message_context@@@Z
void chud_messaging_post(long user_index, long string_index, e_chud_message_context context)
{
    if ((unsigned long)user_index <= 3)
    {
        c_static_wchar_string<256> string;
        chud_get_string(user_index, string_index, &string);
        chud_messaging_user_manager(user_index)->post(string.m_string, context);
    }
}

static __forceinline long chud_messaging_first_active_user()
{
    long user_index = -1;
    for (long index = 0; index < 4; ++index)
    {
        if (player_mapping_globals->player_indices[index] != -1)
        {
            user_index = index;
            break;
        }
    }
    return user_index;
}

static __forceinline long chud_messaging_user_is_outside(long user_index)
{
    if (user_index < 0)
        return 1;
    if (user_index >= 4)
        return 1;
    return 0;
}

// @address 0x47a860 ?chud_messaging_special_load@@YAX_N@Z
void chud_messaging_special_load(bool p0)
{
    if (game_globals->options.game_mode == 1)
    {
        long string_index = (p0 == 0) + _string_id_hud_loading_begin;
        for (long user_index = chud_messaging_first_active_user(); user_index != -1; user_index = ::players_next_active_user(user_index))
        {
            if (chud_messaging_user_is_outside(user_index))
                continue;
            (&c_chud_manager::x_persistent_user_data[user_index].m_messaging_manager)->clear_messages();
            c_static_wchar_string<256> string;
            chud_get_string(user_index, string_index, &string);
            (&c_chud_manager::x_persistent_user_data[user_index].m_messaging_manager)->post(string.m_string,
                _chud_message_context_neutral);
        }
    }
}

// @address 0x47aa30 chud_messaging_special_save
extern "C" void chud_messaging_special_save(bool p0)
{
    if (game_globals->options.game_mode == 1)
    {
        long string_index = (p0 == 0) + _string_id_hud_autosave_begin;
        for (long user_index = chud_messaging_first_active_user(); user_index != -1; user_index = ::players_next_active_user(user_index))
        {
            if (chud_messaging_user_is_outside(user_index))
                continue;
            (&c_chud_manager::x_persistent_user_data[user_index].m_messaging_manager)->clear_messages();
            c_static_wchar_string<256> string;
            chud_get_string(user_index, string_index, &string);
            (&c_chud_manager::x_persistent_user_data[user_index].m_messaging_manager)->post(string.m_string,
                _chud_message_context_neutral);
        }
    }
}

// @address 0x47a948 chud_messaging_special_progress
extern "C" void chud_messaging_special_progress(bool p0)
{
    if (game_globals->options.game_mode == 1)
    {
        long string_index = (p0 == 0) + _string_id_hud_saving_begin;
        for (long user_index = chud_messaging_first_active_user(); user_index != -1; user_index = ::players_next_active_user(user_index))
        {
            if (chud_messaging_user_is_outside(user_index))
                continue;
            (&c_chud_manager::x_persistent_user_data[user_index].m_messaging_manager)->clear_messages();
            c_static_wchar_string<256> string;
            chud_get_string(user_index, string_index, &string);
            (&c_chud_manager::x_persistent_user_data[user_index].m_messaging_manager)->post(string.m_string,
                _chud_message_context_neutral);
        }
    }
}

// @address 0x47ab18 chud_messaging_manager_get_new_medal_index
extern "C" long chud_messaging_manager_get_new_medal_index(c_chud_messaging_manager *manager)
{
    long index = -1;
    long oldest_time = game_time_globals->time + 1;
    long oldest_index = -1;
    for (long medal_index = 0; medal_index < k_chud_message_medal_count; medal_index++)
    {
        s_chud_medal_message *medal = &manager->m_medals[medal_index];
        if (!medal->active)
        {
            index = medal_index;
            break;
        }
        else if (medal->time < oldest_time)
        {
            oldest_time = medal->time;
            oldest_index = medal_index;
        }
    }
    if (index == -1)
    {
        index = oldest_index;
    }
    return index;
}

// 0x47ab80
void c_chud_messaging_manager::post(const wchar_t *text, e_chud_message_context context)
{
    if (text && text[0])
    {
        long message_index = get_new_message_index();
        s_chud_message *message = &m_messages[message_index];
        ustrnzcpy_inline(message->text, text, k_chud_message_length);
        message->visible = false;
        message->active = true;
        message->time = game_time_get();
        message->target_row = 0.0f;
        message->current_row = 0.0f;
        message->context = context;
    }
}

static __forceinline bool chud_messaging_medal_sequence_contains(const chud_table_row *row, long medal_value)
{
    bool found = false;
    if (row->medal_bitmap.index != -1)
    {
        for (long index = 0; index < row->medals.count; index++)
        {
            if (((long *)row->medals.address)[index] == medal_value)
            {
                found = true;
                break;
            }
        }
    }
    return found;
}

// 0x47acd0
void c_chud_messaging_manager::post_medal(long list_index, long medal_value)
{
    s_chud_globals_definition *table = chud_globals;
    const chud_table_row *row = &table->rows[list_index];
    if (chud_messaging_medal_sequence_contains(row, medal_value))
    {
        long medal_index = chud_messaging_manager_get_new_medal_index(this);
        s_chud_medal_message *medal = &m_medals[medal_index];
        medal->active = true;
        medal->medal_value = medal_value;
        medal->shown = false;
        medal->time = game_time_globals->time;
        medal->target = 0.0f;
        medal->value = 0.0f;
    }
}

static __forceinline void chud_messaging_canonicalize_glyph(wchar_t &character)
{
    switch (character)
    {
    case _unicode_private_font_icon_animating_left_thumbstick1:
    case _unicode_private_font_icon_animating_left_thumbstick2:
        character = _unicode_private_font_icon_animating_left_thumbstick0;
        break;
    case _unicode_private_font_icon_animating_right_thumbstick1:
    case _unicode_private_font_icon_animating_right_thumbstick2:
        character = _unicode_private_font_icon_animating_right_thumbstick0;
        break;
    }
}

// @address 0x47ada8 chud_messaging_is_text_equal
extern "C" bool chud_messaging_is_text_equal(const wchar_t *text, const wchar_t *other_text)
{
    bool result = false;
    if (text && other_text)
    {
        result = true;
        long index = 0;
        wchar_t raw_character;
        wchar_t raw_other_character;
        do
        {
            raw_character = text[index];
            raw_other_character = other_text[index];
            wchar_t other_character = raw_other_character;
            wchar_t character = raw_character;
            chud_messaging_canonicalize_glyph(character);
            chud_messaging_canonicalize_glyph(other_character);
            if (character != other_character)
            {
                result = false;
                break;
            }
            index++;
        } while (raw_character != 0 && raw_other_character != 0);
    }
    return result;
}


// @address 0x47aec8 ?clear_messages@c_chud_messaging_manager@@QAAXXZ
void c_chud_messaging_manager::clear_messages()
{
    for (long message_index = 0; message_index < k_chud_message_count; message_index++)
    {
        s_chud_message *message = &m_messages[message_index];
        message->active = false;
    }
    chud_messaging_set_text(this, (const wchar_t *)L"", -1, -1, 1);
    chud_messaging_set_text(this, (const wchar_t *)L"", -1, -1, 0);
    chud_messaging_set_text(this, (const wchar_t *)L"", -1, -1, 2);
    chud_messaging_set_text(this, (const wchar_t *)L"", -1, -1, 3);
}

// @address 0x47af80 chud_messaging_set_text_from_string
extern "C" void chud_messaging_set_text_from_string(long user_index, long string_index, long unused, long index)
{
    c_static_wchar_string<256> string;
    chud_get_string(user_index, string_index, &string);
    chud_messaging_set_text(chud_messaging_user_manager(user_index), string.m_string, -1, -1, index);
}

// 0x47b7d8
bool chud_weapon_state_is_depleted(const weapon_interface_state *weapon_state)
{
    bool depleted = false;
    if (weapon_state->magazine_count > 0 && weapon_state->magazines[0].rounds_loaded_maximum != 0
        && weapon_state->magazines[0].rounds_loaded == 0 && weapon_state->magazines[0].rounds_inventory == 0)
    {
        depleted = true;
    }
    if (weapon_state->age == 0.0f)
    {
        depleted = true;
    }
    return depleted;
}


// @address 0x4a3f58 chud_messaging_clamp_index
extern "C" bool chud_messaging_clamp_index(s_chud_messaging_handle handle, long index, long *result)
{
    const s_chud_messaging_handle &handle_reference = handle;
    (void)handle_reference;
    long tag_index = chud_globals->medal_bitmap.index;
    if (tag_index != -1)
    {
        *result = MIN(MAX(index, 0), TAG_GET('bitm', bitmap_group, tag_index)->sequences.count);
    }
    return true;
}


// @address 0x47a790 chud_messaging_post_medal_for_user
extern "C" void chud_messaging_post_medal_for_user(long user_index, long medal_value)
{
    if (user_index < 4 && campaign_metagame_is_enabled() && medal_value != 2)
    {
        if (medal_value > 2)
            medal_value--;
        c_chud_messaging_manager *manager = chud_messaging_user_manager(user_index);
        long result;
        s_chud_messaging_handle handle = { chud_globals };
        if (chud_messaging_clamp_index(handle, medal_value, &result))
        {
            s_chud_medal_message *medal = &manager->m_medals[chud_messaging_manager_get_new_medal_index(manager)];
            medal->active = true;
            medal->shown = false;
            medal->medal_value = medal_value;
            medal->time = game_time_globals->time;
            medal->target = 0.0f;
            medal->value = 0.0f;
        }
    }
}


extern "C" {
// @address 0x8c48da g_chud_preserve_state_messages
unsigned char g_chud_preserve_state_messages;
}
static __forceinline unit_datum *chud_post_object(const long &object_index)
{
    return (unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)].datum;
}
template<typename t_definition>
static __forceinline t_definition *chud_post_definition(long object_index)
{
    long definition_index = chud_post_object(object_index)->object.definition_index;
    return TAG_GET(0, t_definition, definition_index);
}
static __forceinline long chud_unit_inventory_get_weapon(long unit_index, short inventory_index)
{
    unit_datum *unit = chud_post_object(unit_index);
    long weapon_index = -1;
    if (inventory_index != -1)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}
static __forceinline short chud_unit_get_current_grenade_type(long unit_index)
{
    return chud_post_object(unit_index)->unit.current_grenade_index;
}
static __forceinline short chud_unit_get_grenade_count(long unit_index, short grenade_type)
{
    if (grenade_type != -1)
        return chud_post_object(unit_index)->unit.grenade_counts[grenade_type];
    return 0;
}
static __forceinline long chud_device_get_action_string(long device_index)
{
    switch (object_header_get(device_index)->object_type)
    {
    case _object_type_control:
        return chud_post_definition<s_control_definition>(device_index)->action_string;
    case _object_type_terminal:
        return chud_post_definition<s_machine_definition>(device_index)->action_string;
    }
    return 0;
}
static __forceinline bool chud_seat_test(const unit_seat *seat, long bit)
{
    return ((seat->flags >> bit) & 1) != 0;
}
// @address 0x47aff0 ?chud_post_action_response@@YAXJ@Z
void chud_post_action_response(long user_index)
{
    long player_index = -1;
    if (user_index != -1)
        player_index = player_mapping_globals->player_indices[user_index];
    s_player_interaction *interaction = &player_control_globals->states[user_index].action.action_context.interaction;
    player_datum *player = &((player_datum *)player_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)];
    bool secondary_posted = false;
    short state_message = players_globals->respawn_failure;
    bool primary_posted = false;
    s_chud_action_scratch scratch;
    wchar_t text_buffer[256];

    if (interaction->type == 0 && player->unit_index != -1)
    {
        long primary_weapon = chud_unit_inventory_get_weapon(player->unit_index, chud_post_object(player->unit_index)->unit.current_weapon_index);
        long secondary_weapon = chud_unit_inventory_get_weapon(player->unit_index, chud_post_object(player->unit_index)->unit.desired_weapon_index);
        unit_datum *unit = chud_post_object(player->unit_index);
        bool can_respond = true;
        long seat_string = 0;
        long marker_string = 0;
        long enter_string = 0;
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
        {
            unit_definition *parent_definition = chud_post_definition<unit_definition>(unit->object.parent_object_index);
            unit_seat *seat = &parent_definition->seats[unit->unit.parent_seat_index];
            can_respond = !chud_seat_test(seat, 3) && !chud_seat_test(seat, 2);
            if (chud_seat_test(seat, 11))
            {
                if (chud_unit_get_grenade_count(player->unit_index, chud_unit_get_current_grenade_type(player->unit_index)) > 0
                    && seat->boarding_grenade_marker != 0)
                {
                    if (internal_object_get_markers_by_string_id(unit->object.parent_object_index, seat->boarding_grenade_marker,
                        scratch.markers, 1, false) == 1)
                        marker_string = seat->boarding_grenade_string;
                }
                seat_string = seat->boarding_melee_string;
            }
            enter_string = seat->detach_weapon_string;
        }
        if (marker_string != 0)
        {
            chud_messaging_set_text_from_string(user_index, marker_string, -1, 0);
            primary_posted = true;
        }
        else if (seat_string != 0)
        {
            chud_messaging_set_text_from_string(user_index, seat_string, -1, 0);
            primary_posted = true;
        }
        else if (enter_string != 0)
        {
            chud_messaging_set_text_from_string(user_index, enter_string, -1, 0);
            primary_posted = true;
        }
        else if (primary_weapon != -1 && can_respond)
        {
            weapon_build_weapon_interface_state(primary_weapon, &scratch.weapon);
            bool depleted = chud_weapon_state_is_depleted(&scratch.weapon);
            if (secondary_weapon != -1)
            {
                weapon_build_weapon_interface_state(secondary_weapon, &scratch.weapon);
                depleted = depleted && chud_weapon_state_is_depleted(&scratch.weapon);
            }
            if (depleted)
            {
                long backpack_weapon = chud_compute_unit_backpack_weapon_index(player->unit_index);
                if (backpack_weapon != -1)
                {
                    weapon_build_weapon_interface_state(backpack_weapon, &scratch.weapon);
                    if (chud_weapon_state_is_depleted(&scratch.weapon))
                        backpack_weapon = -1;
                }
                if (backpack_weapon != -1)
                {
                    chud_messaging_set_text_from_string(user_index, chud_post_definition<s_item_definition>(backpack_weapon)->switch_to_message, -1, 0);
                    primary_posted = true;
                }
                else
                {
                    chud_messaging_set_text_from_string(user_index, chud_post_definition<s_item_definition>(primary_weapon)->all_weapons_empty_message, -1, 0);
                    primary_posted = true;
                }
            }
        }
    }
    else
    {
        switch (interaction->type)
        {
        case 2:
            if (player_interaction_exists(player_index, _object_mask_equipment, interaction))
            {
                long string = chud_post_definition<s_item_definition>(interaction->object_index)->swap_message;
                if (string != 0)
                {
                    chud_messaging_set_text_from_string(user_index, string, -1, 0);
                    primary_posted = true;
                }
            }
            break;
        case 1:
        case 9:
            if (player_interaction_exists(player_index, _object_mask_weapon, interaction))
            {
                s_item_definition *definition = chud_post_definition<s_item_definition>(interaction->object_index);
                long primary_string = 0;
                long secondary_string = 0;
                long flags = interaction->weapon_flags;
                if ((flags & 1) != 0)
                    primary_string = definition->pickup_message;
                else if ((flags & 2) != 0)
                    primary_string = definition->swap_message;
                if ((flags & 4) != 0)
                    secondary_string = definition->pickup_or_dual_wield_message;
                else if ((flags & 8) != 0)
                    secondary_string = definition->swap_or_dual_wield_message;
                if (secondary_string != 0)
                {
                    chud_messaging_set_text_from_string(user_index, secondary_string, -1, 1);
                    secondary_posted = true;
                }
                if (primary_string != 0)
                {
                    chud_messaging_set_text_from_string(user_index, primary_string, -1, 0);
                    primary_posted = true;
                }
            }
            break;
        case 3:
            if (player_interaction_exists(player_index, _object_mask_terminal | _object_mask_control, interaction))
            {
                long string = chud_device_get_action_string(interaction->object_index);
                if (string != 0)
                {
                    chud_messaging_set_text_from_string(user_index, string, -1, 0);
                    primary_posted = true;
                }
            }
            break;
        case 4:
        case 5:
        case 8:
            if (player_interaction_exists(player_index, _object_mask_unit, interaction))
            {
                unit_definition *definition = chud_post_definition<unit_definition>(interaction->object_index);
                long seat_index = interaction->seat_index;
                long string = 0;
                if (seat_index >= 0 && seat_index < definition->seats.count())
                    string = definition->seats[seat_index].enter_seat_string;
                chud_messaging_set_text_from_string(user_index, string, -1, 0);
                primary_posted = true;
            }
            break;
        case 10:
            if (player_interaction_exists(player_index, _object_mask_vehicle, interaction))
            {
                chud_messaging_set_text_from_string(user_index, chud_post_definition<vehicle_definition>(interaction->object_index)->flip_message, -1, 0);
                primary_posted = true;
            }
            break;
        case 7:
            if (object_try_and_get_and_verify_type(interaction->object_index, _object_mask_unit) != 0)
            {
                unit_datum *unit = chud_post_object(interaction->object_index);
                long weapon_index = chud_unit_inventory_get_weapon(interaction->object_index, unit->unit.current_weapon_index);
                if (weapon_index != -1)
                {
                    long string = chud_post_definition<s_item_definition>(weapon_index)->switch_to_from_ai_message;
                    if (string != 0 && string != -1)
                        chud_messaging_set_text_from_string(user_index, string, -1, 0);
                    primary_posted = true;
                }
            }
            break;
        }
    }
    if (!secondary_posted && state_message != _player_respawn_failure_none)
    {
        long state_string = 0;
        switch (state_message)
        {
        case _player_respawn_failure_friend_in_combat:
            state_string = _string_id_hud_respawn_faliure_combat;
            break;
        case _player_respawn_failure_friend_airborne_or_moving:
            state_string = _string_id_hud_respawn_faliure_moving;
            break;
        case _player_respawn_failure_friend_in_vehicle:
            state_string = _string_id_hud_respawn_faliure_vehicle;
            break;
        case _player_respawn_failure_scripting_says_it_isnt_safe:
        case _player_respawn_failure_nearby_enemies_pursuing_friend:
        case _player_respawn_failure_waiting_for_checkpoint:
        case _player_respawn_failure_can_not_find_valid_location:
            state_string = _string_id_hud_respawn_faliure_unsafe;
            break;
        }
        if (state_string != 0)
        {
            chud_messaging_set_text_from_string(user_index, state_string, -1, 1);
            secondary_posted = true;
        }
    }
    if (g_chud_preserve_state_messages == 0)
    {
        if (!secondary_posted)
            chud_messaging_set_text(chud_messaging_user_manager(user_index), (const wchar_t *)L"", -1, -1, 1);
        if (!primary_posted)
            chud_messaging_set_text(chud_messaging_user_manager(user_index), (const wchar_t *)L"", -1, -1, 0);
    }
    if (game_engine_hud_get_state_message(user_index, text_buffer, 256, true))
        chud_messaging_set_text(chud_messaging_user_manager(user_index), text_buffer, -1, -1, 2);
    else
        chud_messaging_set_text(chud_messaging_user_manager(user_index), (const wchar_t *)L"", -1, -1, 2);
}
