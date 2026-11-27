#ifndef GAME_GAME_ENGINE_DEFINITION_SETTERS_H
#define GAME_GAME_ENGINE_DEFINITION_SETTERS_H
struct oddball_unit_slayer_variant;
struct oddball_unit_slayer_definition;
extern "C" void game_engine_slayer_variant_set_from_definition(oddball_unit_slayer_variant *, const oddball_unit_slayer_definition *);
class c_game_engine_oddball_variant;
struct s_game_engine_oddball_variant_definition;
extern "C" void game_engine_oddball_variant_set_from_definition(
    c_game_engine_oddball_variant *variant, const s_game_engine_oddball_variant_definition *definition);
class c_game_engine_vip_variant;
struct s_game_engine_vip_settings_definition;
extern "C" void game_engine_territories_variant_set_from_definition(
    c_game_engine_vip_variant *variant, const s_game_engine_vip_settings_definition *definition);
extern "C" void game_engine_king_variant_set_from_definition(void *storage, const void *definition);
#endif
