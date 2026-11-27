#ifndef GAME_GAME_ENGINE_SPAWN_INFLUENCER_H
#define GAME_GAME_ENGINE_SPAWN_INFLUENCER_H

// Functions defined in tu/blamlibxbox/game_engine_spawn_influencer.cpp.

struct s_dynamic_spawn_influencer_collection;

// Eight-byte goal entry: March CTF assemble_spawn_influencers_for_player
// Each entry is a pair of words: a type and an object index.
struct s_netgame_goal_influencer
{
    long type;
    long object_index;
};

void game_engine_spawn_influencers_update(void);
void game_engine_spawn_influencers_build_collection_for_player(long player_index, s_dynamic_spawn_influencer_collection *collection);
void game_engine_spawn_influencer_record_player_death(long player_index);

union real_point3d;

extern "C" float game_engine_spawn_influencers_add_ally_bias(long a, long b);
extern "C" float game_engine_spawn_influencer_compute_score(const real_point3d *point, bool enabled, bool unbounded,
    float initial, float adjustment, const s_dynamic_spawn_influencer_collection *collection);

struct player_datum;
struct real_matrix4x3;
union real_vector3d;
struct s_dynamic_spawn_influencer;
struct s_equipment_spawn_influence;
struct s_multiplayer_constants;
struct s_player_spawn_influence;
struct s_projectile_spawn_influence;
struct s_vehicle_spawn_influence;
struct s_weapon_spawn_influence;

bool game_engine_build_player_spawn_influencer(struct s_player_spawn_influence const * influence, struct real_matrix4x3 const * matrix,
    struct s_dynamic_spawn_influencer * out);
bool game_engine_build_equipment_spawn_influencer(long index, struct s_dynamic_spawn_influencer * out);
void game_engine_spawn_influencers_add_enemy_forbid_influence(long player_index, const player_datum *player, s_dynamic_spawn_influencer_collection *collection);
void game_engine_spawn_influencers_add_enemy_bias_influence(long player_index, const player_datum *player, s_dynamic_spawn_influencer_collection *collection);
extern "C" void game_engine_spawn_influencers_add_teammate_influence(long player_index, const player_datum *player,
    s_dynamic_spawn_influencer_collection *collection);
void game_engine_spawn_influencers_add_dead_teammate_influence(long player_index, const player_datum *player,
    s_dynamic_spawn_influencer_collection *collection);
extern "C" void game_engine_spawn_influencers_add_enemy_weapon_influence(long player_index, const player_datum *player,
    s_dynamic_spawn_influencer_collection *collection);
extern "C" void game_engine_spawn_influencers_add_vehicle_influence(const player_datum *player, s_dynamic_spawn_influencer_collection *collection);
extern "C" void game_engine_spawn_influence_add_projectile_influence(const player_datum *player, s_dynamic_spawn_influencer_collection *collection);
void game_engine_spawn_influence_add_equipment_influence(const player_datum *player, s_dynamic_spawn_influencer_collection *collection);
void game_engine_spawn_influence_add_netgame_goal_object_influence(long player_index, s_dynamic_spawn_influencer_collection *collection);
const s_multiplayer_constants *game_engine_multiplayer_constants_try_and_get(void);
const s_weapon_spawn_influence *game_engine_weapon_spawn_influence_try_and_get(long object_index);
void game_engine_spawn_influencers_add_user_placed_influence(long player_index, s_dynamic_spawn_influencer_collection *influencers);
const s_vehicle_spawn_influence *game_engine_vehicle_spawn_influence_try_and_get(long object_index, real_vector3d *velocity, float *radius);
const s_projectile_spawn_influence *game_engine_projectile_spawn_influence_try_and_get(long object_index, real_vector3d *velocity);
const s_equipment_spawn_influence *game_engine_equipment_spawn_influence_try_and_get(long object_index);
extern "C" bool game_engine_spawn_influencer_is_enemy(long player_index, long other_player_index);
bool game_engine_build_weapon_spawn_influencer(long weapon_index, s_dynamic_spawn_influencer *influencer);
bool game_engine_build_vehicle_spawn_influencer(long object_index, s_dynamic_spawn_influencer *influencer);
bool game_engine_build_respawn_zone_influencer(long object_index, s_dynamic_spawn_influencer *influencer);
bool game_engine_build_projectile_spawn_influencer(long object_index, s_dynamic_spawn_influencer *influencer);

struct s_netgame_goal_spawn_influence;
enum e_dynamic_spawn_influencer_type;
bool game_engine_build_netgame_goal_spawn_influencer(const s_netgame_goal_spawn_influence *influence, const real_matrix4x3 *matrix,
    s_dynamic_spawn_influencer *influencer);
bool game_engine_spawn_influencer_build(e_dynamic_spawn_influencer_type type, long object_index, s_dynamic_spawn_influencer *influencer, float *duration);

#endif // GAME_GAME_ENGINE_SPAWN_INFLUENCER_H
