#ifndef GAME_GAME_ENGINE_VARIANT_LOOKUP_H
#define GAME_GAME_ENGINE_VARIANT_LOOKUP_H

enum e_game_engine_type;
class c_game_variant;

bool game_engine_tag_defined_variant_get_built_in_variant(e_game_engine_type engine_type, long index, c_game_variant *game_variant);

template<long k_size> class c_static_wchar_string;

bool game_engine_tag_defined_variant_get_strings(
    e_game_engine_type engine_type, long index, c_static_wchar_string<48> *name, c_static_wchar_string<256> *description);
#endif // GAME_GAME_ENGINE_VARIANT_LOOKUP_H
