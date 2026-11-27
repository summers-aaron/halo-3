#ifndef SAVED_GAMES_CONTENT_ITEM_H
#define SAVED_GAMES_CONTENT_ITEM_H

// Functions defined in tu/blamlibxbox/content_item.cpp that other units call.

extern "C" unsigned int content_item_get_string_length(const unsigned short *string);

struct s_content_item;
struct s_saved_game_item_metadata_packed;

extern "C" bool content_item_try_and_get_metadata(const s_content_item *content_item, s_saved_game_item_metadata_packed *metadata);

class c_content_catalogue;
enum e_game_content_type;

extern "C" bool content_item_create(s_content_item *item, e_game_content_type content_type, c_content_catalogue *catalogue,
    const wchar_t *name, const wchar_t *display_name, long flags, long maximum_size, bool synchronous);

#endif // SAVED_GAMES_CONTENT_ITEM_H
