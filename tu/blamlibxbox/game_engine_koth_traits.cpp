// blamlibxbox/game_engine_koth_traits.obj: from the records 0x2696b8 0x2698a8 0x269e90 0x26a2d8 0x26a3a0 0x26a598
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "memory/bitstream.h"
#include "game/player_traits.h"
#include "game/game_engine_group.h"
// @address 0x2696b8 game_engine_king_variant_copy
// @address 0x2698a8 game_engine_king_variant_set_from_definition
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x266b48 ?set@c_player_traits@@QAAXPBUs_game_engine_player_traits@@_N@Z
// @address 0x2bc148 ?game_engine_settings_player_traits_get_by_name@@YAPBUs_game_engine_player_traits@@J@Z
// @address 0x26a2d8 ?encode@c_game_engine_king_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x26a3a0 ?decode@c_game_engine_king_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x265048 ?decode@c_game_engine_base_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x266ea8 ?decode@c_player_traits@@QAAXPAVc_bitstream@@@Z
// @address 0x26a598 king_get_hill_move_in_seconds
// @address 0x269e90 ?initialize@c_game_engine_king_variant@@UAAXXZ
// @address 0x264d40 ?initialize@c_game_engine_base_variant@@UAAXXZ
// @address 0x264e28 ?encode@c_game_engine_base_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x266df0 ?encode@c_player_traits@@QBAXPAVc_bitstream@@@Z

#include "game/game_engine_king_variant.h"
#include "game/game_engine_king_traits_internals.h"

#include "game/game_engine_king_settings_types.h"

extern "C" void game_engine_king_variant_copy(s_king_variant_copy_storage *target, const s_king_variant_copy_storage *source)
{
    ((c_game_engine_base_variant *)target)->set((const c_game_engine_base_variant *)source);
    target->base.m_miscellaneous_options.m_flags.set(c_game_engine_miscellaneous_options::_game_engine_miscellaneous_option_teams_enabled, false);
    if (source->hill_flags & 1)
        target->hill_flags |= 1;
    else
        target->hill_flags &= ~1;
    int team_scoring = source->team_scoring;
    if ((unsigned int)team_scoring <= 2)
        target->team_scoring = team_scoring;
    else
        target->team_scoring = 0;
    int moving_hill = source->moving_hill;
    if ((unsigned int)moving_hill <= 3)
        target->moving_hill = moving_hill;
    else
        target->moving_hill = 0;
    int moving_hill_order = source->moving_hill_order;
    if ((unsigned int)moving_hill_order > 16)
        moving_hill_order = 2;
    target->moving_hill_order = moving_hill_order;
    short score = source->score_to_win;
    if (score < 0 || score > 120)
        score = 30;
    target->score_to_win = score;
    signed char inside = source->points[0];
    if (inside < -10 || inside > 10)
        inside = 1;
    target->points[0] = inside;
    if (source->points[1] >= -10 && source->points[1] <= 10)
        target->points[1] = source->points[1];
    else
        target->points[1] = 0;
    if (source->points[2] >= -10 && source->points[2] <= 10)
        target->points[2] = source->points[2];
    else
        target->points[2] = 0;
    if (source->points[3] >= -10 && source->points[3] <= 10)
        target->points[3] = source->points[3];
    else
        target->points[3] = -1;
    if (source->points[4] >= -10 && source->points[4] <= 10)
        target->points[4] = source->points[4];
    else
        target->points[4] = -1;
    if (source->points[5] >= -10 && source->points[5] <= 10)
        target->points[5] = source->points[5];
    else
        target->points[5] = 0;
    target->traits[0].set(&source->traits[0], true);
    target->traits[1].set(&source->traits[1], true);
    target->traits[2].set(&source->traits[2], true);
    target->traits[3].set(&source->traits[3], true);
}

void game_engine_king_variant_set_from_definition(s_king_variant_copy_storage *target, const s_king_variant_copy_definition *source)
{
    game_engine_base_variant_set(&target->base, &source->base, _game_engine_infection);
    target->base.m_miscellaneous_options.m_flags.set(c_game_engine_miscellaneous_options::_game_engine_miscellaneous_option_teams_enabled, false);
    if (source->flags & 1)
        target->hill_flags |= 1;
    else
        target->hill_flags &= ~1;
    int mode = source->mode;
    if (mode >= 0 && mode <= 2)
        target->team_scoring = mode;
    else
        target->team_scoring = 0;
    int type = source->type;
    if (type >= 0 && type <= 3)
        target->moving_hill = type;
    else
        target->moving_hill = 0;
    int count = source->count;
    if (count >= 0 && count <= 16)
        target->moving_hill_order = count;
    else
        target->moving_hill_order = 2;
    short time = source->time;
    if (time < 0 || time > 120)
        time = 30;
    target->score_to_win = time;
    signed char value = source->values[0];
    if (value < -10 || value > 10)
        value = 1;
    target->points[0] = value;
    if (source->values[1] >= -10 && source->values[1] <= 10)
        target->points[1] = source->values[1];
    else
        target->points[1] = 0;
    if (source->values[2] >= -10 && source->values[2] <= 10)
        target->points[2] = source->values[2];
    else
        target->points[2] = 0;
    if (source->values[3] >= -10 && source->values[3] <= 10)
        target->points[3] = source->values[3];
    else
        target->points[3] = -1;
    if (source->values[4] >= -10 && source->values[4] <= 10)
        target->points[4] = source->values[4];
    else
        target->points[4] = -1;
    if (source->values[5] >= -10 && source->values[5] <= 10)
        target->points[5] = source->values[5];
    else
        target->points[5] = 0;
    target->traits[0].set(game_engine_settings_player_traits_get_by_name(source->references[0]), true);
    target->traits[1].set(game_engine_settings_player_traits_get_by_name(source->references[1]), true);
    target->traits[2].set(game_engine_settings_player_traits_get_by_name(source->references[2]), true);
    target->traits[3].set(game_engine_settings_player_traits_get_by_name(source->references[3]), true);
}

void c_game_engine_king_variant::encode(c_bitstream *packet) const
{
    c_game_engine_base_variant::encode(packet);
    packet->write_bool("king-opaque-hill", m_flags.test(_king_opaque_hill));
    packet->write_integer("king-score-to-win", m_score_to_win, 10);
    packet->write_integer("king-moving-hill", m_moving_hill, 4);
    packet->write_integer("king-moving-hill-order", m_moving_hill_order, 2);
    packet->write_integer("king_inside_hill_points", (unsigned char)m_inside_hill_points & 31, 5);
    packet->write_integer("king_outside_hill_points", (unsigned char)m_outside_hill_points & 31, 5);
    packet->write_integer("king_uncontested_hill_bonus", (unsigned char)m_uncontested_hill_bonus & 31, 5);
    packet->write_integer("king_kill_points", (unsigned char)m_kill_points & 31, 5);
    m_inside_hill_traits.encode(packet);
}


extern "C" int king_get_hill_move_in_seconds(void)
{
    short seconds;
    switch (g_king_variant->m_moving_hill) {
    case 1: seconds = 10; break;
    case 2: seconds = 15; break;
    case 3: seconds = 30; break;
    case 4: seconds = 60; break;
    case 5: seconds = 120; break;
    case 6: seconds = 180; break;
    case 7: seconds = 240; break;
    case 8: seconds = 300; break;
    default: seconds = 0; break;
    }
    return seconds;
}

void c_game_engine_king_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    m_flags.set(_king_opaque_hill, packet->read_bool("king-opaque-hill"));
    unsigned short score = (unsigned short)packet->read_integer("king-score-to-win", 10);
    if ((short)score < 0 || (short)score > 1000)
        score = 100;
    m_score_to_win = score;
    unsigned long moving_hill = packet->read_integer("king-moving-hill", 4);
    if (moving_hill <= 8)
        m_moving_hill = (signed char)moving_hill;
    else
        m_moving_hill = 3;
    unsigned long moving_hill_order = packet->read_integer("king-moving-hill-order", 2);
    if (moving_hill_order <= 1)
        m_moving_hill_order = (signed char)moving_hill_order;
    else
        m_moving_hill_order = 1;
    unsigned long value = packet->read_integer("king_inside_hill_points", 5);
    if (value & 16)
        value |= -32;
    short points2 = (signed char)value;
    if (points2 >= -10 && points2 <= 10)
        m_inside_hill_points = points2;
    else
        m_inside_hill_points = 1;
    value = packet->read_integer("king_outside_hill_points", 5);
    if (value & 16)
        value |= -32;
    short points3 = (signed char)value;
    if (points3 >= -10 && points3 <= 10)
        m_outside_hill_points = points3;
    else
        m_outside_hill_points = 0;
    value = packet->read_integer("king_uncontested_hill_bonus", 5);
    if (value & 16)
        value |= -32;
    short points4 = (signed char)value;
    if (points4 >= -10 && points4 <= 10)
        m_uncontested_hill_bonus = points4;
    else
        m_uncontested_hill_bonus = 0;
    value = packet->read_integer("king_kill_points", 5);
    if (value & 16)
        value |= -32;
    short points5 = (signed char)value;
    if (points5 >= -10 && points5 <= 10)
        m_kill_points = points5;
    else
        m_kill_points = 0;
    m_inside_hill_traits.decode(packet);
}

void c_game_engine_king_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    m_flags.clear();
    m_flags.set(_king_opaque_hill, false);
    m_score_to_win = 100;
    m_moving_hill = 3;
    m_moving_hill_order = 1;
    m_inside_hill_points = 1;
    m_outside_hill_points = 0;
    m_uncontested_hill_bonus = 0;
    m_kill_points = 0;
    m_inside_hill_traits.clear();
}

