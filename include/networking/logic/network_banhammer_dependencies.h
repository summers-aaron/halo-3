#ifndef NETWORKING_LOGIC_NETWORK_BANHAMMER_DEPENDENCIES_H
#define NETWORKING_LOGIC_NETWORK_BANHAMMER_DEPENDENCIES_H

#include "saved_games/saved_game_item_metadata.h"
#include "networking/network_time_types.h"

#if defined(_XBOX) && !defined(__clang__)
extern "C" s_network_time_override banhammer_network_time_globals;
#endif

// match: preserve the banhammer caller's signed index view; the unsigned
// declaration in saved_game_files_operations.h changes register allocation.
extern "C" void saved_game_files_initialize_metadata(s_saved_game_item_metadata_record *out, long index, int value, const wchar_t *name,
    const wchar_t *description, unsigned __int64 identifier, unsigned __int64 size_in_bytes);

#endif // NETWORKING_LOGIC_NETWORK_BANHAMMER_DEPENDENCIES_H
