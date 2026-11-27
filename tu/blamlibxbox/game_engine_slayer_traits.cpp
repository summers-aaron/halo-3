// blamlibxbox/game_engine_slayer_traits.obj: from the records 0x267920 0x267a00 0x267a58 0x267f08 0x268030
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include <string.h>
#include "game/player_traits.h"
#include "memory/bitstream.h"
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x266df0 ?encode@c_player_traits@@QBAXPAVc_bitstream@@@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x264e28 ?encode@c_game_engine_base_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x265048 ?decode@c_game_engine_base_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x266ea8 ?decode@c_player_traits@@QAAXPAVc_bitstream@@@Z
// @address 0x1c1258 ?get_game_engine_name_string_id@c_game_engine_base_variant@@MBAJXZ
// @address 0x1c1258 ?get_game_engine_default_description_string_id@c_game_engine_base_variant@@MBAJXZ
// @address 0x264d40 ?initialize@c_game_engine_base_variant@@UAAXXZ
// @address 0x264dc8 ?validate@c_game_engine_base_variant@@UAAXXZ




#include "game/game_engine_base_variant.h"


#include "game/game_engine_slayer.h"

static __forceinline void game_engine_slayer_traits_initialize_player_traits(c_player_traits *traits)
{
    traits->m_shield_vitality_traits.m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
    traits->m_shield_vitality_traits.m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
    traits->m_shield_vitality_traits.m_vampirism = _vampirism_percentage_setting_unchanged;
    traits->m_shield_vitality_traits.m_headshot_immunity = _headshot_immunity_setting_unchanged;
    traits->m_shield_vitality_traits.m_shield_multiplier = _shield_multiplier_setting_unchanged;
    memset(traits->m_shield_vitality_traits.m_padding, 0, sizeof(traits->m_shield_vitality_traits.m_padding));
    traits->m_weapon_traits.m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
    traits->m_weapon_traits.m_initial_primary_weapon = -2;
    traits->m_weapon_traits.m_initial_secondary_weapon = -2;
    traits->m_weapon_traits.m_grenade_count = _grenade_count_setting_unchanged;
    traits->m_weapon_traits.m_recharging_grenades = _recharging_grenades_setting_unchanged;
    traits->m_weapon_traits.m_infinite_ammo = _infinite_ammo_setting_unchanged;
    traits->m_weapon_traits.m_weapon_pickup = _weapon_pickup_setting_unchanged;
    traits->m_movement_traits.m_speed = _player_speed_setting_unchanged;
    traits->m_movement_traits.m_gravity = _player_gravity_setting_unchanged;
    traits->m_movement_traits.m_vehicle_usage = _vehicle_usage_setting_unchanged;
    traits->m_movement_traits.m_padding = 0;
    traits->m_appearance_traits.m_active_camo = _active_camo_setting_unchanged;
    traits->m_appearance_traits.m_waypoint = _waypoint_setting_unchanged;
    traits->m_appearance_traits.m_aura = _aura_setting_unchanged;
    traits->m_appearance_traits.m_forced_change_color = _forced_change_color_setting_unchanged;
    traits->m_sensor_traits.m_motion_tracker = _motion_tracker_setting_unchanged;
    traits->m_sensor_traits.m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
}

// @address 0x267920 ?initialize@c_game_engine_slayer_variant@@UAAXXZ
void c_game_engine_slayer_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    m_score_to_win = 25;
    m_kill_points = 1;
    m_assist_points = 0;
    m_death_points = 0;
    m_suicide_points = -1;
    m_betrayal_points = -1;
    m_leader_killed_points = 0;
    m_elimination_points = 0;
    m_assassination_points = 0;
    m_headshot_points = 0;
    m_melee_points = 0;
    m_sticky_points = 0;
    m_splatter_points = 0;
    m_killing_spree_points = 0;
    game_engine_slayer_traits_initialize_player_traits(&m_leader_traits);
}

// @address 0x267a00 ?validate@c_game_engine_slayer_variant@@UAAXXZ
void c_game_engine_slayer_variant::validate()
{
    c_game_engine_slayer_variant local_variant;
    local_variant.set(this);
    set(&local_variant);
}

void c_game_engine_slayer_variant::set(const c_game_engine_slayer_variant *variant)
{
    c_game_engine_base_variant::set(variant);
    set_score_to_win(variant->get_score_to_win());
    set_kill_points(variant->get_kill_points());
    set_assist_points(variant->get_assist_points());
    set_death_points(variant->get_death_points());
    set_suicide_points(variant->get_suicide_points());
    set_betrayal_points(variant->get_betrayal_points());
    set_leader_killed_points(variant->get_leader_killed_points());
    set_elimination_points(variant->get_elimination_points());
    set_assassination_points(variant->get_assassination_points());
    set_headshot_points(variant->get_headshot_points());
    set_melee_points(variant->get_melee_points());
    set_sticky_points(variant->get_sticky_points());
    set_splatter_points(variant->get_splatter_points());
    set_killing_spree_points(variant->get_killing_spree_points());
    set_leader_traits(variant->get_leader_traits(), true);
}

// @address 0x267f08 ?encode@c_game_engine_slayer_variant@@UBAXPAVc_bitstream@@@Z
void c_game_engine_slayer_variant::encode(c_bitstream *packet) const
{
    this->c_game_engine_base_variant::encode(packet);
    packet->write_integer("slayer-score-to-win", m_score_to_win & 0x3ff, 10);
    packet->write_integer("slayer-kill-points", m_kill_points & 0x1f, 5);
    packet->write_integer("slayer-assist-points", m_assist_points & 0x1f, 5);
    packet->write_integer("slayer-death-points", m_death_points & 0x1f, 5);
    packet->write_integer("slayer-suicide-points", m_suicide_points & 0x1f, 5);
    packet->write_integer("slayer-betrayal-points", m_betrayal_points & 0x1f, 5);
    packet->write_integer("slayer-leader-killed-points", m_leader_killed_points & 0x1f, 5);
    packet->write_integer("slayer-elimination-points", m_elimination_points & 0x1f, 5);
    packet->write_integer("slayer-assassination-points", m_assassination_points & 0x1f, 5);
    packet->write_integer("slayer-headshot-points", m_headshot_points & 0x1f, 5);
    packet->write_integer("slayer-melee-points", m_melee_points & 0x1f, 5);
    packet->write_integer("slayer-sticky-points", m_sticky_points & 0x1f, 5);
    packet->write_integer("slayer-splatter-points", m_splatter_points & 0x1f, 5);
    packet->write_integer("slayer-killing-spree-points", m_killing_spree_points & 0x1f, 5);
    m_leader_traits.encode(packet);
}

// @address 0x268030 ?decode@c_game_engine_slayer_variant@@UAAXPAVc_bitstream@@@Z
void c_game_engine_slayer_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    { short s = (short)packet->read_signed_integer("slayer-score-to-win", 10); if (s >= -1 && s <= 250) m_score_to_win = s; else m_score_to_win = 25; }
    set_kill_points((short)packet->read_signed_integer("slayer-kill-points", 5));
    set_assist_points((signed char)packet->read_signed_integer("slayer-assist-points", 5));
    set_death_points((signed char)packet->read_signed_integer("slayer-death-points", 5));
    set_suicide_points((signed char)packet->read_signed_integer("slayer-suicide-points", 5));
    set_betrayal_points((signed char)packet->read_signed_integer("slayer-betrayal-points", 5));
    set_leader_killed_points((signed char)packet->read_signed_integer("slayer-leader-killed-points", 5));
    set_elimination_points((signed char)packet->read_signed_integer("slayer-elimination-points", 5));
    set_assassination_points((signed char)packet->read_signed_integer("slayer-assassination-points", 5));
    set_headshot_points((signed char)packet->read_signed_integer("slayer-headshot-points", 5));
    set_melee_points((signed char)packet->read_signed_integer("slayer-melee-points", 5));
    set_sticky_points((signed char)packet->read_signed_integer("slayer-sticky-points", 5));
    set_splatter_points((signed char)packet->read_signed_integer("slayer-splatter-points", 5));
    set_killing_spree_points((signed char)packet->read_signed_integer("slayer-killing-spree-points", 5));
    m_leader_traits.decode(packet);
}
