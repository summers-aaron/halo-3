#ifndef SAVED_GAMES_CONTENT_ITEM_STORAGE_H
#define SAVED_GAMES_CONTENT_ITEM_STORAGE_H
#include "saved_games/content_catalogue_types.h"
extern s_content_catalogues g_content_catalogue_globals;
extern "C" long content_catalogue_new_item(c_content_catalogue *catalogue, const wchar_t *display_name, const wchar_t *name,
    long content_type, bool no_profile_transfer, long maximum_size, bool synchronous);
#endif
