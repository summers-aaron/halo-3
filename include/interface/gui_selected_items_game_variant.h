#ifndef INTERFACE_GUI_SELECTED_ITEMS_GAME_VARIANT_H
#define INTERFACE_GUI_SELECTED_ITEMS_GAME_VARIANT_H

#include "game/game_types.h"
#include "interface/gui_selected_item_metadata.h"
#include "interface/gui_simple_selected_item.h"
#include "tag_files/file_reference.h"

class c_gui_game_variant_selected_item : public c_gui_simple_selected_item {
public:
    c_gui_game_variant_selected_item();
    // A listed variant: the display metadata, where it lives, and the file to load it from (none for a built-in).
    c_gui_game_variant_selected_item(const s_gui_selected_items_block *metadata, e_gui_selected_item_location location,
        long controller, const s_file_reference *file, long builtin, long sort, bool corrupt, bool is_new);
    static long get_game_engine_item_count(e_game_engine_type game_engine_type);
    virtual bool get_file_path(char *buffer, long size) const;
    __forceinline long get_special_item_type() const { return m_special_type; }
    __forceinline long get_sort_order() const { return sort; }
    bool get_game_variant(class c_game_variant *game_variant) const;
    __forceinline e_game_engine_type get_game_engine_type() const
    {
        return (e_game_engine_type)m_metadata.game_engine_type;
    }
private:
    long controller;
    long builtin;
    s_file_reference file;
    long sort;
};
typedef char gui_game_variant_selected_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_game_variant_selected_item) == 0x2b0))) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/gui_selected_items_game_variant.cpp.

int game_variant_category_sort_proc(const void *p0, const void *p1);

extern "C" int game_variant_selected_item_sort_proc(const void *a, const void *b);

#endif // INTERFACE_GUI_SELECTED_ITEMS_GAME_VARIANT_H
