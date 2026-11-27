#ifndef INCLUDE_SAVED_GAMES_CONTENT_CATALOGUE_OPERATIONS_H
#define INCLUDE_SAVED_GAMES_CONTENT_CATALOGUE_OPERATIONS_H

#include "saved_games/game_content_types.h"
class c_content_catalogue;

extern "C" long content_catalogue_new_item(c_content_catalogue *catalogue, const wchar_t *display_name, const wchar_t *name,
    long content_type, bool no_profile_transfer, long maximum_size, bool synchronous);
#include "saved_games/content_catalogue_flush.h"
extern "C" bool content_catalogue_get_item_container_path(
    const c_content_catalogue *catalogue, long content_item_index, char *buffer, long buffer_size, long flag);
extern "C" long content_catalogue_open_or_create_item(c_content_catalogue *catalogue, const wchar_t *display_name, const wchar_t *name,
    e_game_content_type content_type, bool unknown_flag, long maximum_size, long unknown);

#endif // INCLUDE_SAVED_GAMES_CONTENT_CATALOGUE_OPERATIONS_H
