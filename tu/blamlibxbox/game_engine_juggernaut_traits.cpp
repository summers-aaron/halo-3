// blamlibxbox/game_engine_juggernaut_traits.obj: from the records 0x26af40 0x26af98 0x26b240 0x26ba10 0x26bb58 0x26bbb0 0x26c060 0x26c190
// @flags /TP /O2 /Oc
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
#include "game/game_types.h"
#include "cseries/cseries_wrapped.h"
// @address 0x26af40 ?validate@c_game_engine_vip_variant@@UAAXXZ
// @address 0x26af98 ?set@c_game_engine_vip_variant@@QAAXPBV1@@Z
// @address 0x26b240 game_engine_territories_variant_set_from_definition
// @address 0x26bb58 ?validate@c_game_engine_juggernaut_variant@@UAAXXZ
// @address 0x26bbb0 ?set@c_game_engine_juggernaut_variant@@QAAXPBV1@@Z
// @address 0x26c060 ?encode@c_game_engine_juggernaut_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x1c2690 ?get_game_engine_name_string_id@c_game_engine_juggernaut_variant@@MBAJXZ
// @address 0x1c2698 ?get_game_engine_default_description_string_id@c_game_engine_juggernaut_variant@@MBAJXZ
// @address 0x26ba10 ?initialize@c_game_engine_juggernaut_variant@@UAAXXZ
// @address 0x26c190 ?decode@c_game_engine_juggernaut_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x1c26a0 ?can_be_cast_to@c_game_engine_juggernaut_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
// @address 0x1c24b0 ?get_score_to_win_round@c_game_engine_juggernaut_variant@@UBAJXZ
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x266b48 ?set@c_player_traits@@QAAXPBUs_game_engine_player_traits@@_N@Z
// @address 0x2bc148 ?game_engine_settings_player_traits_get_by_name@@YAPBUs_game_engine_player_traits@@J@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "game/game_engine_group.h"
#include "memory/bitstream.h"

// @address 0x3cb0c ??_7c_game_engine_vip_variant@@6B@
#include "game/game_engine_juggernaut_traits_internals.h"

#include "game/game_engine_base_variant.h"
#include "game/game_engine_player_traits.h"

#include "game/game_engine_vip_variant.h"

#include "game/game_engine_vip_variant.h"

#include "game/game_engine_juggernaut_variant_types.h"

#include "game/game_engine_juggernaut_variant.h"

static __forceinline void copy_offset(signed char *destination, const signed char *source)
{
    unsigned char value = *source;
    signed char offset = (signed char)value;
    if (offset < -10 || offset > 10)
        *destination = 0;
    else
        *destination = value;
}

static __forceinline int game_engine_juggernaut_traits_points_out_of_range(signed char x)
{
    if ((x < -10)) return 1;
    if ((x > 10)) return 1;
    return 0;
}
static __forceinline int game_engine_juggernaut_traits_influence_radius_out_of_range(int x)
{
    if ((x < 0)) return 1;
    if ((x > 50)) return 1;
    return 0;
}
static __forceinline void update_flag(short *p, int mask, unsigned char set)
{
    int flags = *(unsigned short *)p;
    if (set) flags = flags | mask;
    else flags = (short)flags & ~mask;
    *p = flags;
}

void c_game_engine_vip_variant::validate()
{
    c_game_engine_vip_variant validated_settings;
    validated_settings.set(this);
    set(&validated_settings);
    return;
}

void c_game_engine_vip_variant::set(const c_game_engine_vip_variant *source)
{
    c_game_engine_vip_variant *settings = this;
    short value;
    unsigned char enabled;
    ((c_game_engine_base_variant *)settings)->set((const c_game_engine_base_variant *)source);
    settings->m_miscellaneous_options.m_flags.m_storage |= 1;
    update_flag(&settings->m_variant_flags.m_storage, 1, source->m_variant_flags.m_storage & 1);
    update_flag(&settings->m_variant_flags.m_storage, 2, _rotl((unsigned short)source->m_variant_flags.m_storage, 31) & 1);
    update_flag(&settings->m_variant_flags.m_storage, 4, _rotl((unsigned short)source->m_variant_flags.m_storage, 30) & 1);
    value = (short)source->get_score_to_win_round();
    if (value < 0 || value > 500) {
        value = 15;
    }
    settings->m_score_to_win_round = value;
    copy_offset(&settings->m_kill_points, &source->m_kill_points);
    copy_offset(&settings->m_takedown_points, &source->m_takedown_points);
    copy_offset(&settings->m_kill_as_vip_points, &source->m_kill_as_vip_points);
    copy_offset(&settings->m_vip_death_points, &source->m_vip_death_points);
    copy_offset(&settings->m_destination_arrival_points, &source->m_destination_arrival_points);
    copy_offset(&settings->m_suicide_points, &source->m_suicide_points);
    copy_offset(&settings->m_vip_suicide_points, &source->m_vip_suicide_points);
    copy_offset(&settings->m_betrayal_points, &source->m_betrayal_points);
    if (source->m_vip_selection <= 3) settings->m_vip_selection = source->m_vip_selection; else settings->m_vip_selection = 0;
    if (source->m_zone_movement <= 10) settings->m_zone_movement = source->m_zone_movement; else settings->m_zone_movement = 0;
    enabled = source->m_zone_order.m_storage;
    if (enabled > 1) {
        enabled = 1;
    }
    settings->m_zone_order.m_storage = enabled;
    if ((short)source->m_influence_radius >= 0 && (short)source->m_influence_radius <= 50)
        settings->m_influence_radius = source->m_influence_radius;
    else
        settings->m_influence_radius = 0;
    settings->m_vip_team_traits.set(&source->m_vip_team_traits, true);
    settings->m_vip_influence_traits.set(&source->m_vip_influence_traits, true);
    settings->m_vip_traits.set(&source->m_vip_traits, true);
}

extern "C" {
void game_engine_territories_variant_set_from_definition(
    c_game_engine_vip_variant *settings, const s_game_engine_vip_settings_definition *definition)
{
    short signed_score_limit;
    unsigned char primary_points_byte;
    signed char primary_points;
    unsigned char secondary_points_byte;
    signed char secondary_points;
    unsigned char tertiary_points_byte;
    signed char tertiary_points;
    unsigned char quaternary_points_byte;
    signed char quaternary_points;
    unsigned char arrival_points_byte;
    signed char arrival_points;
    unsigned char suicide_points_byte;
    signed char suicide_points;
    unsigned char betrayal_points_byte;
    signed char betrayal_points;
    unsigned char special_suicide_points_byte;
    signed char special_suicide_points;
    int selection_mode;
    int movement_mode;
    __int64 influence_radius_wide;
    int influence_radius;
    unsigned short selection_storage;
    unsigned short movement_storage;
    unsigned short order_storage;
    unsigned short score_limit;
    int zone_order;
    game_engine_base_variant_set(settings, &definition->base, _game_engine_vip);
    settings->m_miscellaneous_options.m_flags.m_storage = (settings->m_miscellaneous_options.m_flags.m_storage | 1);
    update_flag(&settings->m_variant_flags.m_storage, 1, (unsigned char)((unsigned int)definition->flags >> 0 & 1));
    update_flag(&settings->m_variant_flags.m_storage, 2, (unsigned char)((unsigned int)definition->flags >> 1 & 1));
    update_flag(&settings->m_variant_flags.m_storage, 4, (unsigned char)((unsigned int)definition->flags >> 2 & 1));
    score_limit = (unsigned short)definition->score_to_win;
    signed_score_limit = ((short)score_limit);
    if (signed_score_limit < 0 || signed_score_limit > 500)
        score_limit = 15;
    settings->m_score_to_win_round = score_limit;
    primary_points_byte = (unsigned char)definition->kill_points;
    primary_points = ((signed char)primary_points_byte);
    if (!(game_engine_juggernaut_traits_points_out_of_range(primary_points))) {
        settings->m_kill_points = primary_points_byte;
    } else {
        settings->m_kill_points = 0;
    }
    secondary_points_byte = (unsigned char)definition->takedown_points;
    secondary_points = ((signed char)secondary_points_byte);
    if (!(game_engine_juggernaut_traits_points_out_of_range(secondary_points))) {
        settings->m_takedown_points = secondary_points_byte;
    } else {
        settings->m_takedown_points = 0;
    }
    tertiary_points_byte = (unsigned char)definition->kill_as_vip_points;
    tertiary_points = ((signed char)tertiary_points_byte);
    if (!(game_engine_juggernaut_traits_points_out_of_range(tertiary_points))) {
        settings->m_kill_as_vip_points = tertiary_points_byte;
    } else {
        settings->m_kill_as_vip_points = 0;
    }
    quaternary_points_byte = (unsigned char)definition->vip_death_points;
    quaternary_points = ((signed char)quaternary_points_byte);
    if (!(game_engine_juggernaut_traits_points_out_of_range(quaternary_points))) {
        settings->m_vip_death_points = quaternary_points_byte;
    } else {
        settings->m_vip_death_points = 0;
    }
    arrival_points_byte = (unsigned char)definition->destination_arrival_points;
    arrival_points = ((signed char)arrival_points_byte);
    if (!(game_engine_juggernaut_traits_points_out_of_range(arrival_points))) {
        settings->m_destination_arrival_points = arrival_points_byte;
    } else {
        settings->m_destination_arrival_points = 0;
    }
    suicide_points_byte = (unsigned char)definition->suicide_points;
    suicide_points = ((signed char)suicide_points_byte);
    if (!(game_engine_juggernaut_traits_points_out_of_range(suicide_points))) {
        settings->m_suicide_points = suicide_points_byte;
    } else {
        settings->m_suicide_points = 0;
    }
    betrayal_points_byte = (unsigned char)definition->vip_suicide_points;
    betrayal_points = ((signed char)betrayal_points_byte);
    if (!(game_engine_juggernaut_traits_points_out_of_range(betrayal_points))) {
        settings->m_vip_suicide_points = betrayal_points_byte;
    } else {
        settings->m_vip_suicide_points = 0;
    }
    special_suicide_points_byte = (unsigned char)definition->betrayal_points;
    special_suicide_points = ((signed char)special_suicide_points_byte);
    if (!(game_engine_juggernaut_traits_points_out_of_range(special_suicide_points))) {
        settings->m_betrayal_points = special_suicide_points_byte;
    } else {
        settings->m_betrayal_points = 0;
    }
    selection_mode = (short)definition->selection;
    if (!(((unsigned int)selection_mode > 3))) {
        settings->m_vip_selection = selection_mode;
    } else {
        settings->m_vip_selection = 0;
    }
    movement_mode = (short)definition->movement;
    if (!(((unsigned int)movement_mode > 10))) {
        settings->m_zone_movement = movement_mode;
    } else {
        settings->m_zone_movement = 0;
    }
    zone_order = (short)definition->order;
    if (!(((unsigned int)zone_order <= 1))) {
        zone_order = 1;
    }
    settings->m_zone_order.m_storage = zone_order;
    influence_radius_wide = (signed char)definition->influence_radius;
    influence_radius = (int)influence_radius_wide;
    if (!(game_engine_juggernaut_traits_influence_radius_out_of_range(influence_radius))) {
        settings->m_influence_radius = influence_radius_wide;
    } else {
        settings->m_influence_radius = 0;
    }
    settings->m_vip_team_traits.set(game_engine_settings_player_traits_get_by_name((int)definition->team_traits), true);
    settings->m_vip_influence_traits.set(game_engine_settings_player_traits_get_by_name((int)definition->influence_traits), true);
    settings->m_vip_traits.set(game_engine_settings_player_traits_get_by_name((int)definition->vip_traits), true);
    return;
}
}

static __forceinline void game_engine_juggernaut_traits_flag_clear(unsigned char *f, int mask) { *f = *f & ~mask; }

// 0x26bb58
void c_game_engine_juggernaut_variant::validate()
{
    c_game_engine_juggernaut_variant validated;
    validated.set(this);
    set(&validated);
}

// 0x26bbb0
void c_game_engine_juggernaut_variant::set(const c_game_engine_juggernaut_variant *variant)
{
    c_game_engine_base_variant::set(variant);
    game_engine_juggernaut_traits_flag_clear(&m_miscellaneous_options.m_flags.m_storage, 1);
    m_pad1b2 = 0;
    set_allied_against_juggernaut(variant->get_allied_against_juggernaut());
    set_respawn_on_lone_juggernaut(variant->get_respawn_on_lone_juggernaut());
    set_destination_zones_enabled(variant->get_destination_zones_enabled());
    set_score_to_win_round((short)variant->get_score_to_win_round());
    set_initial_juggernaut(variant->get_initial_juggernaut());
    set_next_juggernaut(variant->get_next_juggernaut());
    set_zone_movement(variant->get_zone_movement());
    set_zone_order(variant->get_zone_order());
    set_kill_points(variant->get_kill_points());
    set_juggernaut_kill_points(variant->get_juggernaut_kill_points());
    set_kill_as_juggernaut_points(variant->get_kill_as_juggernaut_points());
    set_destination_arrival_points(variant->get_destination_arrival_points());
    set_suicide_points(variant->get_suicide_points());
    set_betrayal_points(variant->get_betrayal_points());
    set_juggernaut_delay(variant->get_juggernaut_delay());
    set_juggernaut_traits(variant->get_juggernaut_traits(), true);
}

// 0x26c060
void c_game_engine_juggernaut_variant::encode(c_bitstream *packet) const
{
    const c_game_engine_juggernaut_variant *object = this;

    c_game_engine_base_variant::encode(packet);
    packet->write_bool("juggernaut-allied-against-juggernaut", object->m_flags.m_storage & 1);
    packet->write_bool("juggernaut-respawn-on-lone-juggernaut", object->m_flags.m_storage >> 1 & 1);
    packet->write_bool("juggernaut-destination-zones-enabled", object->m_flags.m_storage >> 2 & 1);
    long score_to_win_round = object->get_score_to_win_round();
    packet->write_integer("juggernaut-score-to-win-round", score_to_win_round, 9);
    packet->write_integer("juggernaut-initial-juggernaut", object->m_initial_juggernaut.m_storage, 2);
    packet->write_integer("juggernaut-next-juggernaut", object->m_next_juggernaut.m_storage, 2);
    packet->write_integer("juggernaut-zone-movement", object->m_zone_movement.m_storage, 4);
    packet->write_integer("juggernaut-zone-order", object->m_zone_order.m_storage, 1);
    packet->write_integer("juggernaut-kill-points", (unsigned char)object->m_kill_points & 0x1f, 5);
    packet->write_integer("juggernaut-juggernaut-kill-points", (unsigned char)object->m_juggernaut_kill_points & 0x1f, 5);
    packet->write_integer("juggernaut-kill-as-juggernaut-points", (unsigned char)object->m_kill_as_juggernaut_points & 0x1f, 5);
    packet->write_integer("juggernaut-destination-arrival-points", (unsigned char)object->m_destination_arrival_points & 0x1f, 5);
    packet->write_integer("juggernaut-suicide-points", (unsigned char)object->m_suicide_points & 0x1f, 5);
    packet->write_integer("juggernaut-betrayal-points", (unsigned char)object->m_betrayal_points & 0x1f, 5);
    packet->write_integer("juggernaut-juggernaut-delay", object->m_juggernaut_delay, 4);
    object->m_juggernaut_traits.encode(packet);
}

// 0x26c190
void c_game_engine_juggernaut_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    set_allied_against_juggernaut(packet->read_bool("juggernaut-allied-against-juggernaut"));
    set_respawn_on_lone_juggernaut(packet->read_bool("juggernaut-respawn-on-lone-juggernaut"));
    set_destination_zones_enabled(packet->read_bool("juggernaut-destination-zones-enabled"));
    set_score_to_win_round((unsigned short)packet->read_integer("juggernaut-score-to-win-round", 9));
    set_initial_juggernaut((e_juggernaut_initial_juggernaut_settings)packet->read_integer("juggernaut-initial-juggernaut", 2));
    set_next_juggernaut((e_juggernaut_next_juggernaut_settings)packet->read_integer("juggernaut-next-juggernaut", 2));
    set_zone_movement((e_juggernaut_zone_movement_settings)packet->read_integer("juggernaut-zone-movement", 4));
    set_zone_order((e_juggernaut_zone_order_settings)packet->read_integer("juggernaut-zone-order", 1));
    set_kill_points((signed char)packet->read_signed_integer("juggernaut-kill-points", 5));
    set_juggernaut_kill_points((signed char)packet->read_signed_integer("juggernaut-juggernaut-kill-points", 5));
    set_kill_as_juggernaut_points((signed char)packet->read_signed_integer("juggernaut-kill-as-juggernaut-points", 5));
    set_destination_arrival_points((signed char)packet->read_signed_integer("juggernaut-destination-arrival-points", 5));
    set_suicide_points((signed char)packet->read_signed_integer("juggernaut-suicide-points", 5));
    set_betrayal_points((signed char)packet->read_signed_integer("juggernaut-betrayal-points", 5));
    set_juggernaut_delay((unsigned char)packet->read_integer("juggernaut-juggernaut-delay", 4));
    m_juggernaut_traits.decode(packet);
}

static __forceinline void game_engine_juggernaut_traits_initialize_player_traits(c_player_traits *traits)
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

static __forceinline void game_engine_juggernaut_traits_set_teams_enabled(unsigned char *flags, bool enable)
{
    if (enable) *flags |= 1;
    else *flags &= ~1;
}

// match: 0x26ba10: copying the default setting's storage byte from a separate constant keeps the
// c_enum zero temporaries from coalescing (8 bytes).
static const unsigned char k_game_engine_juggernaut_traits_default_next = _juggernaut_next_juggernaut_killer;

void c_game_engine_juggernaut_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    game_engine_juggernaut_traits_set_teams_enabled(&m_miscellaneous_options.m_flags.m_storage, false);
    m_flags.clear();
    m_flags.set(_juggernaut_flags_allied_against_juggernaut, true);
    m_flags.set(_juggernaut_flags_respawn_on_lone_juggernaut, false);
    m_flags.set(_juggernaut_flags_destination_zones_enabled, false);
    m_score_to_win = (unsigned short)15;
    m_pad1b2 = 0;
    m_initial_juggernaut = _juggernaut_initial_juggernaut_random;
    memcpy(&m_next_juggernaut, &k_game_engine_juggernaut_traits_default_next, sizeof(m_next_juggernaut));
    m_zone_movement = _juggernaut_zone_movement_none;
    m_zone_order = _juggernaut_zone_order_sequence;
    m_kill_points = 0;
    m_juggernaut_kill_points = 1;
    m_kill_as_juggernaut_points = 1;
    m_destination_arrival_points = 1;
    m_suicide_points = 1;
    m_betrayal_points = 1;
    m_juggernaut_delay = 0;
    game_engine_juggernaut_traits_initialize_player_traits(&m_juggernaut_traits);
}
