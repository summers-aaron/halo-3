// blamlibxbox/game_engine_sandbox_traits.obj: from the records 0x26cd68 0x26ce50 0x26cef8 0x26cf70 0x86d28
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "memory/bitstream.h"
#include "game/game_types.h"
#include "game/player_traits.h"
#include <string.h>
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x86d28 ?validate@c_game_engine_sandbox_variant@@UAAXXZ
// @address 0x1c1258 ?get_game_engine_default_description_string_id@c_game_engine_base_variant@@MBAJXZ
// @address 0x1c1258 ?get_game_engine_name_string_id@c_game_engine_base_variant@@MBAJXZ
// @address 0x1c26c8 ?get_game_engine_name_string_id@c_game_engine_sandbox_variant@@MBAJXZ
// @address 0x1c26d0 ?get_game_engine_default_description_string_id@c_game_engine_sandbox_variant@@MBAJXZ
// @address 0x264d40 ?initialize@c_game_engine_base_variant@@UAAXXZ
// @address 0x264dc8 ?validate@c_game_engine_base_variant@@UAAXXZ
// @address 0x264e28 ?encode@c_game_engine_base_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x265048 ?decode@c_game_engine_base_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x266ea8 ?decode@c_player_traits@@QAAXPAVc_bitstream@@@Z
// @address 0x266df0 ?encode@c_player_traits@@QBAXPAVc_bitstream@@@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x26cd68 ?initialize@c_game_engine_sandbox_variant@@UAAXXZ
// @address 0x26cef8 ?encode@c_game_engine_sandbox_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x26cf70 ?decode@c_game_engine_sandbox_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x42d298 ?can_add_to_recent_list@c_game_engine_sandbox_variant@@UBA_NXZ
// @address 0x42d298 ?get_score_to_win_round@c_game_engine_base_variant@@UBAJXZ
// @address 0x42d298 ?get_score_to_win_round@c_game_engine_sandbox_variant@@UBAJXZ
// @address 0x42d380 ?can_add_to_recent_list@c_game_engine_base_variant@@UBA_NXZ
// @address 0x5bec98 ?custom_team_score_stats@c_game_engine_base_variant@@UBAXJJJ@Z
// @address 0x2655e0 ?can_be_cast_to@c_game_engine_base_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
// @address 0x1c26d8 ?can_be_cast_to@c_game_engine_sandbox_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z


#include "game/game_engine_base_variant.h"
#include "game/game_engine_sandbox_variant.h"

static __forceinline long game_engine_sandbox_traits_get_edit_mode(const c_game_engine_sandbox_variant *variant)
{
    return variant->m_edit_mode;
}
static __forceinline void game_engine_sandbox_traits_set_edit_mode(c_game_engine_sandbox_variant *variant, unsigned long mode)
{
    if (mode <= 1)
        variant->m_edit_mode = (c_game_engine_sandbox_variant::e_sandbox_edit_mode_settings)mode;
    else
        variant->m_edit_mode = (c_game_engine_sandbox_variant::e_sandbox_edit_mode_settings)0;
}
static __forceinline short game_engine_sandbox_traits_get_respawn_time(const c_game_engine_sandbox_variant *variant)
{
    return variant->m_respawn_time;
}
static __forceinline void game_engine_sandbox_traits_set_respawn_time(c_game_engine_sandbox_variant *variant, short seconds)
{
    if (seconds < 0 || seconds > 60)
        seconds = 5;
    variant->m_respawn_time = seconds;
}
static __forceinline bool game_engine_sandbox_traits_get_open_channel_voice(const c_game_engine_sandbox_variant *variant)
{
    return variant->m_variant_flags.test(c_game_engine_sandbox_variant::_sandbox_variant_open_channel_voice);
}
static __forceinline void game_engine_sandbox_traits_set_open_channel_voice(c_game_engine_sandbox_variant *variant, bool enabled)
{
    variant->m_variant_flags.set(c_game_engine_sandbox_variant::_sandbox_variant_open_channel_voice, enabled);
}
static __forceinline const c_player_traits *game_engine_sandbox_traits_get_player_traits(const c_game_engine_sandbox_variant *variant)
{
    return &variant->m_player_traits;
}

// @address 0x26ce50 ?set@c_game_engine_sandbox_variant@@QAAXPBV1@@Z
void c_game_engine_sandbox_variant::set(const c_game_engine_sandbox_variant *source)
{
    c_game_engine_base_variant::set(source);
    game_engine_sandbox_traits_set_edit_mode(this, game_engine_sandbox_traits_get_edit_mode(source));
    game_engine_sandbox_traits_set_respawn_time(this, game_engine_sandbox_traits_get_respawn_time(source));
    game_engine_sandbox_traits_set_open_channel_voice(this, game_engine_sandbox_traits_get_open_channel_voice(source));
    m_player_traits.set(game_engine_sandbox_traits_get_player_traits(source), true);
}

void c_game_engine_sandbox_variant::validate()
{
    c_game_engine_sandbox_variant local;
    local.set(this);
    set(&local);
}

void c_game_engine_sandbox_variant::encode(c_bitstream *packet) const
{
    c_game_engine_base_variant::encode(packet);
    packet->write_bool("sandbox-open-channel-voice", game_engine_sandbox_traits_get_open_channel_voice(this));
    packet->write_integer("sandbox-edit-mode", game_engine_sandbox_traits_get_edit_mode(this), 2);
    packet->write_integer("sandbox-respawn-time", game_engine_sandbox_traits_get_respawn_time(this), 6);
    game_engine_sandbox_traits_get_player_traits(this)->encode(packet);
}

void c_game_engine_sandbox_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    game_engine_sandbox_traits_set_open_channel_voice(this, packet->read_bool("sandbox-open-channel-voice"));
    game_engine_sandbox_traits_set_edit_mode(this, packet->read_integer("sandbox-edit-mode", 2));
    game_engine_sandbox_traits_set_respawn_time(this, (unsigned short)packet->read_integer("sandbox-respawn-time", 6));
    m_player_traits.decode(packet);
}

void c_game_engine_sandbox_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    m_variant_flags.clear();
    game_engine_sandbox_traits_set_edit_mode(this, 0);
    game_engine_sandbox_traits_set_respawn_time(this, 5);
    game_engine_sandbox_traits_set_open_channel_voice(this, true);
    m_player_traits.clear();
    m_miscellaneous_options.m_round_limit = 1;
    m_miscellaneous_options.m_round_time_limit_minutes = 0;
}
