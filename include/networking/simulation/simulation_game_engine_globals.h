#ifndef NETWORKING_SIMULATION_SIMULATION_GAME_ENGINE_GLOBALS_H
#define NETWORKING_SIMULATION_SIMULATION_GAME_ENGINE_GLOBALS_H

#include <stddef.h>
#include "memory/static_arrays.h"
#include "networking/simulation/simulation_entity_definition_types.h"
#include "networking/simulation/simulation_entity_database.h"
#include "math/real_math_types.h"

// The state is padded to 4-byte alignment, leaving two implicit bytes after round_timer.
struct __declspec(align(4)) s_game_engine_state_data
{
    unsigned short initial_teams;
    unsigned short valid_designators;
    unsigned short valid_teams;
    unsigned short active_teams;
    unsigned short ever_active_teams;
    short team_designator_to_team_index[9];
    signed char shared_team_lives[8];
    unsigned char current_state;
    bool game_finished;
    short current_round;
    short round_timer;
};

// March: simulation\game_interface\simulation_game_engine_globals.cpp.
// No data members beyond the entity-definition vptr.
class c_simulation_game_engine_globals_definition : public c_simulation_entity_definition
{
public:
    virtual long creation_data_size();
    virtual bool entity_replication_required_for_view_activation(const s_simulation_entity *entity);
    virtual bool entity_type_is_gameworld_object();
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual void entity_creation_encode(long size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update);
    virtual bool entity_creation_decode(long size, void *creation_data, c_bitstream *packet, bool final_update);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual void build_creation_data(long gamestate_index, long size, void *creation_data);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
    virtual bool gameworld_attachment_valid(long gamestate_index);
    virtual bool build_baseline_state_data(long creation_size, const void *creation_data, long state_size, void *state_data);
    virtual bool build_updated_state_data(const s_simulation_entity *entity, unsigned long *update_mask, long size, void *state);
    virtual bool update_game_entity(long gamestate_index, unsigned long update_mask, long size, const void *state_data);
    virtual bool promote_game_entity_to_authority(long gamestate_index);
    virtual void write_creation_description_to_string(const s_simulation_entity *entity,
        const s_simulation_view_telemetry_data *telemetry, long size, char *buffer);
    virtual bool create_game_entity(long gamestate_index, long creation_size, const void *creation_data,
        unsigned long update_mask, long state_size, const void *state_data);
    virtual bool delete_game_entity(long gamestate_index);

    static long global_update_minimum_required_bits(const s_entity_update_data *update_data);
    static void global_update_write_description_to_string(const s_entity_update_data *update_data, long buffer_size, char *buffer);
    static bool global_update_encode(unsigned long update_mask, unsigned long *updated_mask,
        const s_game_engine_state_data *state_data, c_bitstream *packet, long maximum_bits);
    static bool global_update_decode(unsigned long *update_mask, s_game_engine_state_data *state_data, c_bitstream *packet);

};

typedef char simulation_game_engine_globals_definition_size[(sizeof(c_simulation_game_engine_globals_definition) == sizeof(void *)) ? 1 : -1];

class c_simulation_slayer_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    ~c_simulation_slayer_engine_globals_definition() {}

    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual unsigned long initial_update_mask();
    virtual long update_minimum_required_bits(const s_simulation_entity *, const s_entity_update_data *);
    virtual bool entity_update_encode(bool initial, unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
        const void *state_data, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits,
        bool use_encoding);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual bool entity_update_decode(bool initial, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool flag);
    virtual e_simulation_entity_type entity_type();
    virtual long update_flag_count();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
};

class c_simulation_juggernaut_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    ~c_simulation_juggernaut_engine_globals_definition() {}

    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data);
    virtual const char *entity_type_name();
    virtual unsigned long initial_update_mask();
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *written_update_mask,
        long state_data_size, const void *state_data, const struct s_simulation_view_telemetry_data *telemetry, class c_bitstream *packet,
        long maximum_bits, bool flag);
    virtual bool entity_update_decode(bool initial, unsigned long *update_mask, long state_data_size, void *state_data, class c_bitstream *packet, bool flag);
    virtual e_simulation_entity_type entity_type();
    virtual long state_data_size();
    virtual long update_flag_count();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
};

class c_simulation_king_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    ~c_simulation_king_engine_globals_definition() {}

    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask, long state_data_size,
        void *state_data, c_bitstream *packet, bool final_update);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data);
    virtual const char *entity_type_name();
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask,
        unsigned long *updated_mask, long state_data_size, const void *state_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet,
        long maximum_bits, bool final_update);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual e_simulation_entity_type entity_type();
    virtual long state_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
};

class c_simulation_vip_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    virtual const char *entity_type_name();
    virtual unsigned long initial_update_mask();
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual long update_flag_count();
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
        const void *state_data, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits,
        bool final_update);
    virtual bool entity_update_decode(bool initial, unsigned long *update_mask, long state_size, void *state_data, c_bitstream *stream, bool unknown);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual e_simulation_entity_type entity_type();
    virtual long state_data_size();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
};

class c_simulation_oddball_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    ~c_simulation_oddball_engine_globals_definition() {}

    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data);
    virtual const char *entity_type_name();
    virtual bool entity_state_lossy_compare(void *, void *, long);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *written_update_mask,
        long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet,
        long maximum_bits, bool flag);
    virtual e_simulation_entity_type entity_type();
    virtual long state_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask,
        long state_data_size, void *state_data, c_bitstream *packet, bool final_update);
};

class c_simulation_territories_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    ~c_simulation_territories_engine_globals_definition() {}

    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask, long state_data_size,
        void *state_data, c_bitstream *packet, bool final_update);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data);
    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual unsigned long initial_update_mask();
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask,
        unsigned long *written_update_mask, long state_data_size, const void *state_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet,
        long maximum_bits, bool final_update);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual e_simulation_entity_type entity_type();
    virtual long update_flag_count();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
};

class c_simulation_infection_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data);
    virtual const char *entity_type_name();
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask,
        unsigned long *updated_mask, long state_data_size, const void *state_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet,
        long maximum_bits, bool final_update);
    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask,
        long state_data_size, void *state_data, c_bitstream *packet, bool use_encoding);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual e_simulation_entity_type entity_type();
    virtual long state_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
};

class c_simulation_assault_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    ~c_simulation_assault_engine_globals_definition() {}

    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data);
    virtual long state_data_size();
    virtual const char *entity_type_name();
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
        const void *state_data, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits,
        bool final_update);
    virtual bool entity_update_decode(
        bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool use_encoding);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual e_simulation_entity_type entity_type();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
};

class c_simulation_ctf_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
public:
    ~c_simulation_ctf_engine_globals_definition() {}

    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual unsigned long initial_update_mask();
    virtual long update_minimum_required_bits(const s_simulation_entity *, const s_entity_update_data *);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *written_update_mask,
        long state_data_size, void const *state_data, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet,
        long maximum_bits, bool use_encoding);
    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data,
        class c_bitstream *packet, bool use_encoding);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual e_simulation_entity_type entity_type();
    virtual long update_flag_count();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
};

class c_simulation_sandbox_engine_globals_definition : public c_simulation_game_engine_globals_definition
{
    c_simulation_sandbox_engine_globals_definition();
    ~c_simulation_sandbox_engine_globals_definition() {}

public:
    static c_simulation_sandbox_engine_globals_definition &instance();

    virtual e_simulation_entity_type entity_type();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask, long size,
        void *state, c_bitstream *packet, bool final_update);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data);
    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask,
        unsigned long *updated_mask, long state_data_size, const void *state_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet,
        long maximum_bits, bool is_replicated);
    virtual void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer);
};

struct s_juggernaut_engine_state_data { s_game_engine_state_data globals; short juggernaut_player; unsigned char juggernaut_timer; bool visited_zone; };

extern "C" {
extern const char k_juggernaut_player_component_name[], k_juggernaut_timer_component_name[], k_juggernaut_visited_zone_component_name[];
}

struct s_king_engine_state_data
{
    s_game_engine_state_data globals;
    short first;
    unsigned short second;
};

extern "C" {
extern const char k_king_players_in_hill_component_name[];
extern const char k_king_hill_timer_component_name[];
}

struct s_vip_engine_state_data
{
    s_game_engine_state_data globals;
    short team_vip[8];
    unsigned char zone_data[12];
    long defending_team;
};

extern "C" {
extern const char k_vip_team_vip_component_name[];
extern const char k_vip_zone_data_component_name[];
extern const char k_vip_defending_team_component_name[];
}

struct s_oddball_engine_state_data
{
    s_game_engine_state_data globals;
    c_static_array<long, 5> spawn_timers;
    c_static_array<real_point3d, 5> spawn_positions;
    c_static_array<short, 5> reset_timers;
    c_static_array<short, 5> owner_player_indices;
    c_static_array<char, 5> ball_seconds;
};

struct s_territories_engine_state_data : s_game_engine_state_data
{
    long territory_controller_team[8];
    unsigned char territory_capture_time[8][8];
    char territory_state[8];
    long defending_team;
    long sudden_death_time;
    long grace_period_time;
};

struct s_assault_engine_state_data
{
    s_game_engine_state_data globals;
    long defensive_team;
    short bomb_reset_timers[9];
    short bomb_arming_timers[9];
    unsigned char bomb_weapon_flags[9];
    long player_that_placed_bombs[9];
    unsigned short helper_flags;
};

// Named from the ctf state components (entity_update_encode/decode, 0x4275e8/0x427888); 0xb0 bytes.
// The game engine baseline initializes the emblem-player array at offset 0x88.
struct s_ctf_engine_state_data
{
    s_game_engine_state_data globals;
    long defensive_team;
    long flag_swap_timer;
    long touch_return_timers[9];
    short flag_reset_timers[9];
    unsigned short : 16, : 16, : 16, : 16, : 16, : 16, : 16, : 16, : 16;
    unsigned char flag_weapon_flags[9];
    long flag_emblem_player[9];
    unsigned short helper_flags;
};

struct s_infection_engine_state_data
{
    s_game_engine_state_data globals;
    unsigned short zombie_bitvector;
    unsigned short first_zombie_bitvector;
    long safe_haven_timer;
    unsigned short players_in_safe_haven_bitvector;
    unsigned short zombie_team_bitvector;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char simulation_game_engine_state_size[(sizeof(s_game_engine_state_data) == 0x2c) ? 1 : -1];
typedef char simulation_infection_engine_state_size[(sizeof(long) != 4 || sizeof(s_infection_engine_state_data) == 0x38) ? 1 : -1];
typedef char simulation_ctf_engine_state_size[(sizeof(s_ctf_engine_state_data) == 0xb0) ? 1 : -1];
typedef char simulation_ctf_engine_state_helper_flags[(offsetof(s_ctf_engine_state_data, helper_flags) == 0xac) ? 1 : -1];
typedef char simulation_ctf_engine_state_flag_weapon_flags[(offsetof(s_ctf_engine_state_data, flag_weapon_flags) == 0x7c) ? 1 : -1];
#endif

extern "C" {
extern const char k_territory_exists_component_name[];   // "territory-exists"
extern const char k_territories_defending_team_exists_name[];   // "territories-defending-team-exists"
extern const char k_territories_sudden_death_time_exists_name[];   // "territories-sudden-death-time-exists"
extern const char k_territories_grace_period_time_exists_name[];   // "territories-grace-period-time-exists"
}

#endif
