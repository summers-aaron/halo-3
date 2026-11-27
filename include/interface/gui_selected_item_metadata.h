#ifndef INTERFACE_GUI_SELECTED_ITEM_METADATA_H
#define INTERFACE_GUI_SELECTED_ITEM_METADATA_H

#include <stddef.h>
#include "saved_games/saved_game_item_metadata_packed.h"

// Display metadata, unpacked from the saved-game record.
// Its description is wide text; the saved-game record stores UTF-8 there.
struct s_gui_selected_items_block
{
    unsigned __int64 unique_id;
    wchar_t name[16];
    wchar_t description[128];
    char author[16];
    long file_type;
    bool author_is_online;
    unsigned __int64 author_id;
    unsigned __int64 size_in_bytes;
    unsigned __int64 date;
    long length_seconds;
    long campaign_id;
    long map_id;
    long game_engine_type;
    long campaign_difficulty;
    short campaign_insertion_point;
    short campaign_survival_enabled;
    unsigned __int64 game_id;
};

typedef char gui_selected_items_block_size[(sizeof(s_gui_selected_items_block) == 0x178) ? 1 : -1];
typedef char gui_selected_items_name_offset[(offsetof(s_gui_selected_items_block, name) == 0x8) ? 1 : -1];
typedef char gui_selected_items_description_offset[(offsetof(s_gui_selected_items_block, description) == 0x28) ? 1 : -1];
typedef char gui_selected_items_author_offset[(offsetof(s_gui_selected_items_block, author) == 0x128) ? 1 : -1];
typedef char gui_selected_items_date_offset[(offsetof(s_gui_selected_items_block, date) == 0x150) ? 1 : -1];
typedef char gui_selected_items_game_id_offset[(offsetof(s_gui_selected_items_block, game_id) == 0x170) ? 1 : -1];

extern "C" void gui_selected_items_unpack_metadata(s_gui_selected_items_block *metadata, const s_saved_game_item_metadata_packed *packed);

#endif // INTERFACE_GUI_SELECTED_ITEM_METADATA_H
