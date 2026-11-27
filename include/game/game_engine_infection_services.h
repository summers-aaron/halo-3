#ifndef GAME_GAME_ENGINE_INFECTION_SERVICES_H
#define GAME_GAME_ENGINE_INFECTION_SERVICES_H

class c_game_engine_infection_variant;
struct s_game_engine_infection_variant_definition;
struct s_king_variant;
// Copy a king variant; also used by infection validation.
extern "C" void game_engine_king_variant_copy(s_king_variant *, const s_king_variant *);
extern "C" void game_engine_base_variant_set(c_game_engine_infection_variant *variant,
    const s_game_engine_infection_variant_definition *definition, bool force);

#endif
