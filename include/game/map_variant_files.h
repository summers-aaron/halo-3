#ifndef GAME_MAP_VARIANT_FILES_H
#define GAME_MAP_VARIANT_FILES_H

class c_map_variant;
extern "C" unsigned __int64 map_variant_compute_unique_id(const c_map_variant *variant);
extern "C" void game_engine_set_map_info_name(c_map_variant *info, const wchar_t *source);

#endif
