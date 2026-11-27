// blamlibxbox/game_engine_territories_traits.obj: from the records 0x269fa8 0x26a000 0x26a158 0x26a658 0x26a7c8 0x26a820 0x26aae8 0x26ab88
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "memory/bitstream.h"
#include "game/game_types.h"
#include "game/player_traits.h"
// @address 0x269fa8 ?validate@c_game_engine_king_variant@@UAAXXZ
// @address 0x26a000 ?set@c_game_engine_king_variant@@QAAXPBV1@@Z
// @address 0x26a158 ?set@c_game_engine_king_variant@@QAAXPBUs_game_engine_king_variant_definition@@@Z
// @address 0x26a7c8 ?validate@c_game_engine_territories_variant@@UAAXXZ
// @address 0x26aae8 ?encode@c_game_engine_territories_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x1c1258 ?get_game_engine_default_description_string_id@c_game_engine_base_variant@@MBAJXZ
// @address 0x1c1258 ?get_game_engine_name_string_id@c_game_engine_base_variant@@MBAJXZ
// @address 0x1c25a0 ?get_game_engine_name_string_id@c_game_engine_territories_variant@@MBAJXZ
// @address 0x1c25a8 ?get_game_engine_default_description_string_id@c_game_engine_territories_variant@@MBAJXZ
// @address 0x1c25b0 ?can_be_cast_to@c_game_engine_territories_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
// @address 0x264d40 ?initialize@c_game_engine_base_variant@@UAAXXZ
// @address 0x264dc8 ?validate@c_game_engine_base_variant@@UAAXXZ
// @address 0x264e28 ?encode@c_game_engine_base_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x265048 ?decode@c_game_engine_base_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x2655e0 ?can_be_cast_to@c_game_engine_base_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
// @address 0x26a658 ?initialize@c_game_engine_territories_variant@@UAAXXZ
// @address 0x26ab88 ?decode@c_game_engine_territories_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x42d298 ?get_score_to_win_round@c_game_engine_base_variant@@UBAJXZ
// @address 0x42d380 ?can_add_to_recent_list@c_game_engine_base_variant@@UBA_NXZ
// @address 0x5bec98 ?custom_team_score_stats@c_game_engine_base_variant@@UBAXJJJ@Z
// @address 0x26a820 ?set@c_game_engine_territories_variant@@QAAXPBV1@@Z
// @address 0x3ca1c ??_7c_game_engine_king_variant@@6B@
#include <string.h>
#include "game/game_engine_group.h"
#include "game/game_engine_traits.h"
#include "game/game_engine_king_variant.h"
#include "game/game_engine_territories_variant.h"

void c_game_engine_king_variant::validate()
{
    c_game_engine_king_variant scratch_variant;
    scratch_variant.set(this);
    set(&scratch_variant);
}

void c_game_engine_king_variant::set(const c_game_engine_king_variant *source)
{
    c_game_engine_base_variant::set(source);
    m_flags.set(_king_opaque_hill, source->m_flags.test(_king_opaque_hill));
    short score_to_win = source->m_score_to_win;
    if (score_to_win < 0 || score_to_win > 1000)
        score_to_win = 100;
    m_score_to_win = score_to_win;
    unsigned long moving_hill = source->m_moving_hill;
    if (moving_hill > 8)
        moving_hill = 3;
    m_moving_hill = (signed char)moving_hill;
    unsigned long moving_hill_order = source->m_moving_hill_order;
    if (moving_hill_order <= 1)
        m_moving_hill_order = (signed char)moving_hill_order;
    else
        m_moving_hill_order = 1;
    signed char points = source->m_inside_hill_points;
    if (points < -10 || points > 10)
        m_inside_hill_points = 1;
    else
        m_inside_hill_points = points;
    points = source->m_outside_hill_points;
    if (points < -10 || points > 10)
        m_outside_hill_points = 0;
    else
        m_outside_hill_points = points;
    points = source->m_uncontested_hill_bonus;
    if (points < -10 || points > 10)
        m_uncontested_hill_bonus = 0;
    else
        m_uncontested_hill_bonus = points;
    points = source->m_kill_points;
    if (points < -10 || points > 10)
        m_kill_points = 0;
    else
        m_kill_points = points;
    m_inside_hill_traits.set(&source->m_inside_hill_traits, true);
}

void c_game_engine_king_variant::set(const s_game_engine_king_variant_definition *definition)
{
    game_engine_base_variant_set(this, &definition->base, _game_engine_king);
    long team_scoring = definition->team_scoring;
    if ((unsigned long)team_scoring <= 3)
        m_team_scoring_method = (short)team_scoring;
    else
        m_team_scoring_method = 0;
    m_flags.set(_king_opaque_hill, (definition->flags & 1) != 0);
    short score_to_win = definition->score_to_win;
    if (score_to_win < 0 || score_to_win > 1000)
        score_to_win = 100;
    m_score_to_win = score_to_win;
    long moving_hill = definition->moving_hill;
    if ((unsigned long)moving_hill > 8)
        moving_hill = 3;
    m_moving_hill = (signed char)moving_hill;
    long moving_hill_order = definition->moving_hill_order;
    if ((unsigned long)moving_hill_order <= 1)
        m_moving_hill_order = (signed char)moving_hill_order;
    else
        m_moving_hill_order = 1;
    signed char points = definition->inside_hill_points;
    if (points < -10 || points > 10)
        m_inside_hill_points = 1;
    else
        m_inside_hill_points = points;
    points = definition->outside_hill_points;
    if (points < -10 || points > 10)
        m_outside_hill_points = 0;
    else
        m_outside_hill_points = points;
    points = definition->uncontested_hill_bonus;
    if (points < -10 || points > 10)
        m_uncontested_hill_bonus = 0;
    else
        m_uncontested_hill_bonus = points;
    points = definition->kill_points;
    if (points < -10 || points > 10)
        m_kill_points = 0;
    else
        m_kill_points = points;
    m_inside_hill_traits.set(game_engine_settings_player_traits_get_by_name(definition->inside_hill_traits), true);
}

void c_game_engine_territories_variant::set(const c_game_engine_territories_variant *source)
{
    c_game_engine_base_variant::set(source);
    m_miscellaneous_options.set_teams_enabled(true);
    set_one_sided(source->get_one_sided());
    set_lock_after_first_capture(source->get_lock_after_first_capture());
    long respawn_on_capture = source->m_respawn_on_capture;
    if ((unsigned long)respawn_on_capture > _territories_respawn_on_capture_all)
        respawn_on_capture = _territories_respawn_on_capture_disabled;
    m_respawn_on_capture = (e_territories_respawn_on_capture_settings)respawn_on_capture;
    short capture_time = source->m_capture_time;
    if (capture_time < 0 || capture_time > 120)
        capture_time = 30;
    m_capture_time = capture_time;
    short sudden_death = source->sudden_death_time;
    if (sudden_death < -1 || sudden_death > 300)
        sudden_death = -1;
    sudden_death_time = sudden_death;
    m_defender_traits.set(&source->m_defender_traits, true);
    m_attacker_traits.set(&source->m_attacker_traits, true);
}

void c_game_engine_territories_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    m_miscellaneous_options.set_teams_enabled(true);
    m_flags.clear();
    set_lock_after_first_capture(true);
    set_one_sided(true);
    m_respawn_on_capture = _territories_respawn_on_capture_disabled;
    m_capture_time = 30;
    sudden_death_time = -1;
    m_defender_traits.clear();
    m_attacker_traits.clear();
}

void c_game_engine_territories_variant::validate()
{
    c_game_engine_territories_variant validated;
    validated.set(this);
    set(&validated);
}

void c_game_engine_territories_variant::encode(c_bitstream *packet) const
{
    c_game_engine_base_variant::encode(packet);
    packet->write_bool("territories-one-sided", get_one_sided());
    packet->write_bool("territories-lock-after-first-capture", get_lock_after_first_capture());
    packet->write_integer("territories-respawn-on-capture", m_respawn_on_capture, 2);
    packet->write_integer("territories-capture-time", m_capture_time, 7);
    packet->write_integer("territories-sudden-death-time", (unsigned short)sudden_death_time & 0x3ff, 10);
    m_defender_traits.encode(packet);
    m_attacker_traits.encode(packet);
}

void c_game_engine_territories_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    set_one_sided(packet->read_bool("territories-one-sided"));
    set_lock_after_first_capture(packet->read_bool("territories-lock-after-first-capture"));
    unsigned long respawn_on_capture = packet->read_integer("territories-respawn-on-capture", 2);
    if (respawn_on_capture <= _territories_respawn_on_capture_all)
        m_respawn_on_capture = (e_territories_respawn_on_capture_settings)respawn_on_capture;
    else
        m_respawn_on_capture = _territories_respawn_on_capture_disabled;
    short capture_time = (short)packet->read_integer("territories-capture-time", 7);
    if (capture_time < 0 || capture_time > 120)
        capture_time = 30;
    m_capture_time = capture_time;
    short sudden_death = (short)packet->read_signed_integer("territories-sudden-death-time", 10);
    if (sudden_death < -1 || sudden_death > 300)
        sudden_death = -1;
    sudden_death_time = sudden_death;
    m_defender_traits.decode(packet);
    m_attacker_traits.decode(packet);
}
