// blamlibxbox/game_engine_infection_traits.obj: from the records 0x268df8 0x268f38 0x269330 0x269660 0x269ac8 0x269be0
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include <string.h>
#include "memory/bitstream.h"
#include "game/player_traits.h"
#include "game/game_engine_group.h"
#include "game/game_engine_base_variant.h"
#include "game/game_engine_player_traits.h"
#include "game/game_engine_infection_variant.h"
#include "game/game_engine_infection_services.h"
#include "game/game_engine_infection_constants.h"
#include "game/game_engine_infection_variant_copy.h"
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x264e28 ?encode@c_game_engine_base_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x266df0 ?encode@c_player_traits@@QBAXPAVc_bitstream@@@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x266ea8 ?decode@c_player_traits@@QAAXPAVc_bitstream@@@Z
// @address 0x265048 ?decode@c_game_engine_base_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x268f38 ?set@c_game_engine_infection_variant@@QAAXPBUs_game_engine_infection_variant_definition@@@Z
// @address 0x266b48 ?set@c_player_traits@@QAAXPBUs_game_engine_player_traits@@_N@Z
// @address 0x2bc148 ?game_engine_settings_player_traits_get_by_name@@YAPBUs_game_engine_player_traits@@J@Z
// @address 0x265748 game_engine_base_variant_set

// @address 0x264d40 ?initialize@c_game_engine_base_variant@@UAAXXZ

// @address 0x268df8 game_engine_infection_variant_copy
extern "C" void game_engine_infection_variant_copy(void *destination, const void *source)
{
    c_game_engine_infection_variant *self = (c_game_engine_infection_variant *)destination;
    const c_game_engine_infection_variant *variant = (const c_game_engine_infection_variant *)source;
    self->c_game_engine_base_variant::set(variant);
    self->m_miscellaneous_options.m_flags.set(c_game_engine_miscellaneous_options::_game_engine_miscellaneous_option_teams_enabled, true);

    int safe_havens = (signed char)variant->m_safe_havens;
    if ((unsigned int)safe_havens > 3) {
        safe_havens = 1;
    }
    self->m_safe_havens = (signed char)safe_havens;

    int next_zombie = (signed char)variant->m_next_zombie;
    if ((unsigned int)next_zombie <= 2) {
        self->m_next_zombie = (signed char)next_zombie;
    } else {
        self->m_next_zombie = 0;
    }

    int initial_zombies = (signed char)variant->m_initial_zombie_count;
    if ((unsigned int)initial_zombies <= 3) {
        self->m_initial_zombie_count = (signed char)initial_zombies;
    } else {
        self->m_initial_zombie_count = 0;
    }

    unsigned short limit_val = variant->m_limit;
    short limit = (short)limit_val;
    if (limit < 0 || limit > 50) {
        limit_val = 5;
    }
    self->m_limit = limit_val;

    unsigned short points_val = variant->m_points;
    short points = (short)points_val;
    if (points >= -1 && points <= 300) {
        self->m_points = points_val;
    } else {
        self->m_points = 0;
    }

    unsigned short time_val = variant->m_time;
    short time = (short)time_val;
    if (time < 0 || time > 300) {
        time_val = 30;
    }
    self->m_time = time_val;

    self->m_safe_haven_movement_time = variant->m_safe_haven_movement_time;

    unsigned char flag = variant->m_flags & 1;
    if (flag)
        self->m_flags |= 1;
    else
        self->m_flags &= ~1;

    self->m_traits.set(&variant->m_traits, true);
}

static __forceinline void game_engine_infection_traits_set_enum(signed char &field, long value, long maximum, signed char fallback)
{
    if ((unsigned long)value <= (unsigned long)maximum)
        field = (signed char)value;
    else
        field = fallback;
}
static __forceinline signed char game_engine_infection_traits_normalize_points(signed char value, signed char fallback)
{
    signed char result;
    if (value >= -10 && value <= 10)
        result = value;
    else
        result = fallback;
    return result;
}
static __forceinline short game_engine_infection_traits_normalize_time(short value)
{
    short result;
    if (value >= 0 && value <= 120)
        result = value;
    else
        result = 30;
    return result;
}
// @address 0x269330 ?initialize@c_game_engine_infection_variant@@UAAXXZ
void c_game_engine_infection_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    m_miscellaneous_options.m_flags.set(c_game_engine_miscellaneous_options::_game_engine_miscellaneous_option_teams_enabled, false);
    m_infection_flags.clear();
    m_infection_flags.set(_infection_variant_flags_respawn_on_haven_move, false);
    game_engine_infection_traits_set_enum(m_safe_havens, 0, 2, 0);
    game_engine_infection_traits_set_enum(m_next_zombie, 0, 3, 0);
    game_engine_infection_traits_set_enum(m_initial_zombie_count, 2, 16, 2);
    m_safe_haven_movement_time = game_engine_infection_traits_normalize_time(30);
    m_points_signed_bytes[0] = game_engine_infection_traits_normalize_points(1, 1);
    m_points_signed_bytes[1] = game_engine_infection_traits_normalize_points(0, 0);
    m_limit_signed_bytes[0] = game_engine_infection_traits_normalize_points(0, 0);
    m_limit_signed_bytes[1] = game_engine_infection_traits_normalize_points(-1, -1);
    m_time_signed_bytes[0] = game_engine_infection_traits_normalize_points(-1, -1);
    m_time_signed_bytes[1] = game_engine_infection_traits_normalize_points(0, 0);
    m_traits.clear();
    m_first_zombie_traits.clear();
    m_safe_haven_defender_traits.clear();
    m_last_human_traits.clear();
}

// @address 0x269660 ?validate@c_game_engine_infection_variant@@UAAXXZ
void c_game_engine_infection_variant::validate()
{
    c_game_engine_infection_variant validated;
    game_engine_king_variant_copy((s_king_variant *)&validated, (s_king_variant *)this);
    game_engine_king_variant_copy((s_king_variant *)this, (s_king_variant *)&validated);
}

// @address 0x269ac8 ?encode@c_game_engine_infection_variant@@UBAXPAVc_bitstream@@@Z
void c_game_engine_infection_variant::encode(c_bitstream *packet) const
{
    c_game_engine_base_variant::encode(packet);
    packet->write_bool("infection-respawn-on-haven-move", m_flags & 1);
    packet->write_integer("infection-safe-havens", m_safe_havens, 2);
    packet->write_integer("infection-next-zombie", m_next_zombie, 2);
    packet->write_integer("infection-initial-zombie-count", m_initial_zombie_count, 5);
    packet->write_integer("infection-safe-haven-movement-time", (short)m_safe_haven_movement_time, 7);
    packet->write_integer("infection-zombie-kill-points", m_points_bytes[0] & 31, 5);
    packet->write_integer("infection-infection-points", m_points_bytes[1] & 31, 5);
    packet->write_integer("infection-safe-haven-arrival-points", m_limit_bytes[0] & 31, 5);
    packet->write_integer("infection-suicide-points", m_limit_bytes[1] & 31, 5);
    packet->write_integer("infection-betrayal-points", m_time_bytes[0] & 31, 5);
    packet->write_integer("infection-last-man-bonus-points", m_time_bytes[1] & 31, 5);
    m_traits.encode(packet);
    m_first_zombie_traits.encode(packet);
    m_safe_haven_defender_traits.encode(packet);
    m_last_human_traits.encode(packet);
}

static __forceinline void game_engine_infection_traits_copy_limited(
    unsigned char *destination, unsigned long value, unsigned long limit, unsigned char fallback)
{
    if (value <= limit)
        *destination = value;
    else
        *destination = fallback;
}

static __forceinline long infection_read_signed5(c_bitstream *packet)
{
    unsigned long value = packet->read_integer("infection-points", 5);
    if (value & 16)
        value |= -32;
    return value;
}

// @address 0x269be0 ?decode@c_game_engine_infection_variant@@UAAXPAVc_bitstream@@@Z
void c_game_engine_infection_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    if (packet->read_bool("infection-respawn-on-haven-move"))
        m_flags_byte |= 1;
    else
        m_flags_byte &= ~1;
    game_engine_infection_traits_copy_limited((unsigned char *)&m_safe_havens, packet->read_integer("infection-safe-havens", 2), 2, 0);
    game_engine_infection_traits_copy_limited((unsigned char *)&m_next_zombie, packet->read_integer("infection-next-zombie", 2), 3, 0);
    game_engine_infection_traits_copy_limited((unsigned char *)&m_initial_zombie_count, packet->read_integer("infection-initial-zombie-count", 5), 16, 2);
    short time = (short)packet->read_integer("infection-safe-haven-movement-time", 7);
    if (time < 0 || time > 120)
        time = 30;
    m_safe_haven_movement_time = time;
    signed char value = infection_read_signed5(packet);
    if (value < -10 || value > 10)
        value = 1;
    m_points_bytes[0] = value;
    value = infection_read_signed5(packet);
    if (value >= -10 && value <= 10)
        m_points_bytes[1] = value;
    else
        m_points_bytes[1] = 0;
    value = infection_read_signed5(packet);
    if (value >= -10 && value <= 10)
        m_limit_bytes[0] = value;
    else
        m_limit_bytes[0] = 0;
    value = infection_read_signed5(packet);
    if (value >= -10 && value <= 10)
        m_limit_bytes[1] = value;
    else
        *(signed char *)&m_limit_bytes[1] = -1;
    value = infection_read_signed5(packet);
    if (value >= -10 && value <= 10)
        m_time_bytes[0] = value;
    else
        *(signed char *)&m_time_bytes[0] = -1;
    value = infection_read_signed5(packet);
    if (value >= -10 && value <= 10)
        m_time_bytes[1] = value;
    else
        m_time_bytes[1] = 0;
    m_traits.decode(packet);
    m_first_zombie_traits.decode(packet);
    m_safe_haven_defender_traits.decode(packet);
    m_last_human_traits.decode(packet);
}

// @address 0x406ec k_infection_safe_haven_movement_times
// @rdata 0x820406ec..0x82040700
const short k_infection_safe_haven_movement_times[] = { -1, 0, 5, 10, 15, 20, 30, 45, 60, 0 };
// @rdata-end
// @address 0x40700 k_game_variant_score_time_values

static __forceinline void game_engine_infection_set_enum(signed char &field, long value, long maximum, long fallback)
{
    if (value >= 0 && value <= maximum)
        field = (signed char)value;
    else
        field = (signed char)fallback;
}

static __forceinline void game_engine_infection_set_range(unsigned short &field, short value, short maximum, short fallback)
{
    if (value >= 0 && value <= maximum)
        field = value;
    else
        field = fallback;
}

// @address 0x268f38 ?set@c_game_engine_infection_variant@@QAAXPBUs_game_engine_infection_variant_definition@@@Z
void c_game_engine_infection_variant::set(const s_game_engine_infection_variant_definition *definition)
{
    game_engine_base_variant_set(this, definition, true);
    m_miscellaneous_options.m_flags.set(c_game_engine_miscellaneous_options::_game_engine_miscellaneous_option_teams_enabled, true);
    game_engine_infection_set_enum(m_safe_havens, definition->haven, 3, 1);
    game_engine_infection_set_enum(m_next_zombie, definition->next, 2, 0);
    game_engine_infection_set_enum(m_initial_zombie_count, definition->initial, 3, 0);
    game_engine_infection_set_range(m_limit, definition->limit, 50, 5);
    game_engine_infection_set_range(m_time, definition->time, 300, 30);
    m_safe_haven_movement_time = k_infection_safe_haven_movement_times[definition->movement];
    short points = k_game_variant_score_time_values[definition->points];
    if (points >= -1 && points <= 300)
        m_points = points;
    else
        m_points = 0;
    if (m_next_zombie == 0 && (definition->flags & 1))
        m_flags |= 1;
    else
        m_flags &= ~1;
    m_traits.set(game_engine_settings_player_traits_get_by_name(definition->traits), true);
}
