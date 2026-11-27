// blamlibxbox/game_engine_ctf_traits.obj: from the records 0x268758 0x268930 0x268a10 0x268c78 0x268da0 0x2690b8 0x269180 0x269320
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "game/game_types.h"
#include "game/player_traits.h"
#include "memory/bitstream.h"
#include "game/game_engine_ctf_variant.h"
// @address 0x268758 game_engine_oddball_variant_set_from_definition
// @address 0x268930 ?encode@c_game_engine_oddball_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x268a10 ?decode@c_game_engine_oddball_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x2690b8 ?encode@c_game_engine_ctf_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x269180 ?decode@c_game_engine_ctf_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x269320 ?custom_team_score_stats@c_game_engine_ctf_variant@@UBAXJJJ@Z
#include "game/game_engine_group.h"
#include "game/game_results_types.h"
#include "game/game_results.h"

#include "game/game_engine_base_variant.h"
#include "game/game_engine_player_traits.h"
#include "game/game_engine_oddball_variant.h"
#include "game/game_engine_oddball_definition.h"

#include "game/game_engine_traits.h"

#include "game/game_engine_infection_variant_copy.h"

extern "C" {
void game_engine_oddball_variant_set_from_definition(
    c_game_engine_oddball_variant *variant, const s_game_engine_oddball_variant_definition *settings)
{
    int team_scoring_setting;
    short signed_score_to_win;
    short carrying_points_value;
    short carrying_points;
    unsigned char kill_points_byte;
    signed char kill_points;
    unsigned char ball_kill_points_byte;
    signed char ball_kill_points;
    unsigned char carrier_kill_points_byte;
    signed char carrier_kill_points;
    unsigned char ball_count_byte;
    signed char ball_count;
    short signed_ball_spawn_delay;
    short signed_ball_inactive_respawn_delay;
    int flags_after_first_option;
    int flags_after_second_option;
    unsigned short score_to_win;
    unsigned short ball_spawn_delay;
    unsigned short ball_inactive_respawn_delay;
    game_engine_base_variant_set(variant,
        settings, _game_engine_oddball);
    team_scoring_setting = ((short)settings->team_scoring);
    if (!(((unsigned int)team_scoring_setting > 3))) {
        variant->m_team_scoring_method = team_scoring_setting;
    } else {
        variant->m_team_scoring_method = 0;
    }
    if (!(((unsigned int)(settings->flags & 1) == 0))) {
        flags_after_first_option = (variant->m_flags.m_storage | 1);
    } else {
        flags_after_first_option = (variant->m_flags.m_storage & -2);
    }
    // match: 0x268758: retain the intermediate flag store; removing volatile shortens the function by 4 bytes.
    static_cast<volatile unsigned long &>(variant->m_flags.m_storage) = flags_after_first_option;
    if (!((((unsigned char)(((unsigned int)settings->flags >> 1) & 1)) == 0))) {
        flags_after_second_option = (variant->m_flags.m_storage | 2);
    } else {
        flags_after_second_option = (variant->m_flags.m_storage & -3);
    }
    variant->m_flags.m_storage = flags_after_second_option;
    score_to_win = settings->score_to_win;
    signed_score_to_win = ((short)score_to_win);
    if (signed_score_to_win < -1 || signed_score_to_win > 0x3e8)
        score_to_win = 200;
    variant->m_score_to_win = score_to_win;
    carrying_points_value = ((signed char)settings->carrying_points);
    carrying_points = carrying_points_value;
    if (carrying_points >= -10 && carrying_points <= 10)
    {
        variant->m_carrying_points = carrying_points_value;
    }
    else
    {
        variant->m_carrying_points = 1;
    }
    kill_points_byte = settings->kill_points;
    kill_points = ((signed char)kill_points_byte);
    if (kill_points >= -10 && kill_points <= 10)
    {
        variant->m_kill_points = kill_points_byte;
    }
    else
    {
        variant->m_kill_points = 0;
    }
    ball_kill_points_byte = settings->ball_kill_points;
    ball_kill_points = ((signed char)ball_kill_points_byte);
    if (ball_kill_points >= -10 && ball_kill_points <= 10)
    {
        variant->m_ball_kill_points = ball_kill_points_byte;
    }
    else
    {
        variant->m_ball_kill_points = 0;
    }
    carrier_kill_points_byte = settings->carrier_kill_points;
    carrier_kill_points = ((signed char)carrier_kill_points_byte);
    if (carrier_kill_points >= -10 && carrier_kill_points <= 10)
    {
        variant->m_carrier_kill_points = carrier_kill_points_byte;
    }
    else
    {
        variant->m_carrier_kill_points = 0;
    }
    ball_count_byte = settings->ball_count;
    ball_count = ((signed char)ball_count_byte);
    if (ball_count >= 1 && ball_count <= 3)
    {
        variant->m_ball_count = ball_count_byte;
    }
    else
    {
        variant->m_ball_count = 1;
    }
    ball_spawn_delay = settings->ball_spawn_delay;
    signed_ball_spawn_delay = ((short)ball_spawn_delay);
    if (signed_ball_spawn_delay < 0 || signed_ball_spawn_delay > 120)
        ball_spawn_delay = 5;
    variant->m_ball_spawn_delay = ball_spawn_delay;
    ball_inactive_respawn_delay = settings->ball_inactive_respawn_delay;
    signed_ball_inactive_respawn_delay = ((short)ball_inactive_respawn_delay);
    if (signed_ball_inactive_respawn_delay < 0 || signed_ball_inactive_respawn_delay > 120)
        ball_inactive_respawn_delay = 30;
    variant->m_ball_inactive_respawn_delay = ball_inactive_respawn_delay;
    variant->m_carrier_traits.set(game_engine_settings_player_traits_get_by_name(settings->carrier_traits), 1);
    return;
}
}

void c_game_engine_oddball_variant::encode(c_bitstream *packet) const
{
    c_game_engine_base_variant::encode(packet);
    packet->write_bool("oddball-auto-ball-pickup", (this->m_flags.m_storage & 1));
    packet->write_bool("oddball-ball-effect-enabled", ((unsigned char)(((unsigned int)this->m_flags.m_storage >> 1) & 1)));
    packet->write_integer("oddball-score-to-win", (this->m_score_to_win & 0x7ff), 11);
    packet->write_integer("oddball-carrying-points", (this->m_carrying_points & 31), 5);
    packet->write_integer("oddball-kill-points", (this->m_kill_points & 31), 5);
    packet->write_integer("oddball-ball-kill-points", (this->m_ball_kill_points & 31), 5);
    packet->write_integer("oddball-carrier-kill-points", (this->m_carrier_kill_points & 31), 5);
    packet->write_integer("oddball-ball-count", ((signed char)this->m_ball_count), 2);
    packet->write_integer("oddball-ball-spawn-delay", this->m_ball_spawn_delay, 7);
    packet->write_integer("oddball-ball-inactive-respawn-delay", this->m_ball_inactive_respawn_delay, 7);
    this->m_carrier_traits.encode(packet);
    return;
}

void c_game_engine_oddball_variant::decode(c_bitstream *packet)
{
    unsigned char first_flag_enabled;
    unsigned char second_flag_enabled;
    short decoded_carrying_points;
    short checked_carrying_points;
    signed char decoded_kill_points;
    signed char checked_kill_points;
    signed char decoded_ball_kill_points;
    signed char checked_ball_kill_points;
    signed char decoded_carrier_kill_points;
    signed char checked_carrier_kill_points;
    unsigned char decoded_ball_count;
    signed char checked_ball_count;
    short checked_ball_spawn_delay;
    short checked_ball_inactive_respawn_delay;
    int score_to_win_bits;
    int carrying_points_bits;
    int kill_points_bits;
    int ball_kill_points_bits;
    int carrier_kill_points_bits;
    int decoded_first_flags;
    int decoded_second_flags;
    short decoded_score_to_win;
    unsigned short decoded_ball_spawn_delay;
    unsigned short decoded_ball_inactive_respawn_delay;
    initialize();
    c_game_engine_base_variant::decode(packet);
    first_flag_enabled = ((unsigned char)packet->read_bool("oddball-auto-ball-pickup"));
    if (!((((unsigned char)first_flag_enabled) == 0))) {
        decoded_first_flags = (this->m_flags.m_storage | 1);
    } else {
        decoded_first_flags = (this->m_flags.m_storage & -2);
    }
    this->m_flags.m_storage = decoded_first_flags;
    second_flag_enabled = ((unsigned char)packet->read_bool("oddball-ball-effect-enabled"));
    if (!((((unsigned char)second_flag_enabled) == 0))) {
        decoded_second_flags = (this->m_flags.m_storage | 2);
    } else {
        decoded_second_flags = (this->m_flags.m_storage & -3);
    }
    this->m_flags.m_storage = decoded_second_flags;
    score_to_win_bits = ((int)packet->read_integer("oddball-score-to-win", 11));
    if (!(((unsigned int)(score_to_win_bits & 0x400) == 0))) {
        score_to_win_bits = (score_to_win_bits | -0x800);
    }
    decoded_score_to_win = ((short)score_to_win_bits);
    if (decoded_score_to_win < -1 || decoded_score_to_win > 0x3e8)
        decoded_score_to_win = 200;
    this->m_score_to_win = decoded_score_to_win;
    carrying_points_bits = ((int)packet->read_integer("oddball-carrying-points", 5));
    if (!(((unsigned int)(carrying_points_bits & 16) == 0))) {
        carrying_points_bits = (carrying_points_bits | -32);
    }
    decoded_carrying_points = ((short)carrying_points_bits);
    checked_carrying_points = decoded_carrying_points;
    if (checked_carrying_points >= -10 && checked_carrying_points <= 10)
    {
        this->m_carrying_points = decoded_carrying_points;
    }
    else
    {
        this->m_carrying_points = 1;
    }
    kill_points_bits = ((int)packet->read_integer("oddball-kill-points", 5));
    if (!(((unsigned int)(kill_points_bits & 16) == 0))) {
        kill_points_bits = (kill_points_bits | -32);
    }
    decoded_kill_points = ((signed char)kill_points_bits);
    checked_kill_points = decoded_kill_points;
    if (checked_kill_points >= -10 && checked_kill_points <= 10)
    {
        this->m_kill_points = decoded_kill_points;
    }
    else
    {
        this->m_kill_points = 0;
    }
    ball_kill_points_bits = ((int)packet->read_integer("oddball-ball-kill-points", 5));
    if (!(((unsigned int)(ball_kill_points_bits & 16) == 0))) {
        ball_kill_points_bits = (ball_kill_points_bits | -32);
    }
    decoded_ball_kill_points = ((signed char)ball_kill_points_bits);
    checked_ball_kill_points = decoded_ball_kill_points;
    if (checked_ball_kill_points >= -10 && checked_ball_kill_points <= 10)
    {
        this->m_ball_kill_points = decoded_ball_kill_points;
    }
    else
    {
        this->m_ball_kill_points = 0;
    }
    carrier_kill_points_bits = ((int)packet->read_integer("oddball-carrier-kill-points", 5));
    if (!(((unsigned int)(carrier_kill_points_bits & 16) == 0))) {
        carrier_kill_points_bits = (carrier_kill_points_bits | -32);
    }
    decoded_carrier_kill_points = ((signed char)carrier_kill_points_bits);
    checked_carrier_kill_points = decoded_carrier_kill_points;
    if (checked_carrier_kill_points >= -10 && checked_carrier_kill_points <= 10)
    {
        this->m_carrier_kill_points = decoded_carrier_kill_points;
    }
    else
    {
        this->m_carrier_kill_points = 0;
    }
    decoded_ball_count = ((unsigned char)((int)packet->read_integer("oddball-ball-count", 2)));
    checked_ball_count = ((signed char)decoded_ball_count);
    if (checked_ball_count >= 1 && checked_ball_count <= 3)
    {
        this->m_ball_count = decoded_ball_count;
    }
    else
    {
        this->m_ball_count = 1;
    }
    decoded_ball_spawn_delay = ((unsigned short)((int)packet->read_integer("oddball-ball-spawn-delay", 7)));
    checked_ball_spawn_delay = ((short)decoded_ball_spawn_delay);
    if (checked_ball_spawn_delay < 0 || checked_ball_spawn_delay > 120)
        decoded_ball_spawn_delay = 5;
    this->m_ball_spawn_delay = decoded_ball_spawn_delay;
    decoded_ball_inactive_respawn_delay = ((unsigned short)((int)packet->read_integer("oddball-ball-inactive-respawn-delay", 7)));
    checked_ball_inactive_respawn_delay = ((short)decoded_ball_inactive_respawn_delay);
    if (checked_ball_inactive_respawn_delay < 0 || checked_ball_inactive_respawn_delay > 120)
        decoded_ball_inactive_respawn_delay = 30;
    this->m_ball_inactive_respawn_delay = decoded_ball_inactive_respawn_delay;
    this->m_carrier_traits.decode(packet);
    return;
}

// @address 0x268c78 ?initialize@c_game_engine_ctf_variant@@UAAXXZ
void c_game_engine_ctf_variant::initialize()
{
    this->c_game_engine_base_variant::initialize();
    m_miscellaneous_options.set_teams_enabled(true);
    m_flags.clear();
    m_flags.set(_ctf_variant_flags_0, false);
    m_option_a = (e_ctf_option_a)(1);
    m_option_b = (e_ctf_option_b)(0);
    m_option_c = (e_ctf_option_c)(0);
    m_option_f = 5;
    m_option_e = 0;
    m_option_g = 30;
    m_option_d = 5;
    m_carrier_traits.clear();
}

// @address 0x268da0 ?validate@c_game_engine_ctf_variant@@UAAXXZ
void c_game_engine_ctf_variant::validate()
{
    c_game_engine_ctf_variant validated;
    game_engine_infection_variant_copy(&validated, this);
    game_engine_infection_variant_copy(this, &validated);
}

void c_game_engine_ctf_variant::encode(c_bitstream *packet) const
{
    c_game_engine_base_variant::encode(packet);
    packet->write_bool("ctf-flag-at-home-to-score", m_flags.test(_ctf_variant_flags_0));
    packet->write_integer("ctf-home-flag-waypoint", m_option_a, 2);
    packet->write_integer("ctf-game-type", m_option_b, 2);
    packet->write_integer("ctf-respawn", m_option_c, 2);
    packet->write_integer("ctf-score-to-win", m_option_f, 6);
    packet->write_integer("ctf-sudden-death-time", m_option_e & 0x1ff, 9);
    packet->write_integer("ctf-flag-reset-time", m_option_g, 9);
    packet->write_integer("ctf-touch-return-time", m_option_d & 63, 6);
    m_carrier_traits.encode(packet);
}

void c_game_engine_ctf_variant::custom_team_score_stats(long team_index, long old_score, long new_score) const
{
    game_results_statistic_increment(-1, (e_game_team)team_index,
        _game_results_statistic_ctf_flag_scores, new_score - old_score);
}

void c_game_engine_ctf_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    if (packet->read_bool("ctf-flag-at-home-to-score"))
    {
        m_flags.set(_ctf_variant_flags_0, true);
    }
    else
    {
        m_flags.set(_ctf_variant_flags_0, false);
    }
    unsigned long option_a = packet->read_integer("ctf-home-flag-waypoint", 2);
    if (option_a <= 3)
    {
        m_option_a = (e_ctf_option_a)((signed char)option_a);
    }
    else
    {
        m_option_a = (e_ctf_option_a)(1);
    }
    unsigned long option_b = packet->read_integer("ctf-game-type", 2);
    if (option_b <= 2)
    {
        m_option_b = (e_ctf_option_b)((signed char)option_b);
    }
    else
    {
        m_option_b = (e_ctf_option_b)(0);
    }
    unsigned long option_c = packet->read_integer("ctf-respawn", 2);
    if (option_c <= 3)
    {
        m_option_c = (e_ctf_option_c)((signed char)option_c);
    }
    else
    {
        m_option_c = (e_ctf_option_c)(0);
    }
    short option_f = (short)packet->read_integer("ctf-score-to-win", 6);
    if (option_f < 0 || option_f > 50)
    {
        option_f = 5;
    }
    m_option_f = option_f;
    long option_e_bits = packet->read_integer("ctf-sudden-death-time", 9);
    if ((option_e_bits & 0x100u) != 0)
    {
        option_e_bits |= -0x200;
    }
    short option_e = (short)option_e_bits;
    if (option_e < -1 || option_e > 300)
    {
        m_option_e = 0;
    }
    else
    {
        m_option_e = option_e;
    }
    short option_g = (short)packet->read_integer("ctf-flag-reset-time", 9);
    if (option_g < 0 || option_g > 300)
    {
        option_g = 30;
    }
    m_option_g = option_g;
    long option_d = packet->read_integer("ctf-touch-return-time", 6);
    if ((option_d & 32u) != 0)
    {
        option_d |= -64;
    }
    m_option_d = (unsigned short)option_d;
    m_carrier_traits.decode(packet);
}
