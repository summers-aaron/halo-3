#ifndef SAVED_GAMES_CONTENT_ITEM_INLINE_H
#define SAVED_GAMES_CONTENT_ITEM_INLINE_H

#include "saved_games/content_catalogue_types.h"
#include "saved_games/content_item_types.h"

__forceinline e_controller_index s_content_item::get_controller_index() const
{
    c_content_catalogue *catalogue = get_content_catalogue();
    return catalogue != 0 ? catalogue->get_controller_index() : k_no_controller;
}

#endif
