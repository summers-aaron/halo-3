// interfacelibxbox/gui_selected_items.obj: from the records 0x221ac8 0x491d68 0x491e58 0x491e98
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x56c64 ??_7c_gui_simple_selected_item@@6B@
// @address 0x4ed94 ??_7c_load_pregame_selection_screen_message@@6B@
#include "saved_games/saved_game_item_metadata.h"
#include "interface/gui_selected_item.h"
#include "text/utf8_conversion.h"
#include "text/unicode.h"
#include "saved_games/saved_game_files_operations.h"
#include "interface/gui_simple_selected_item.h"
#include "interface/gui_selected_items.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_selection.h"

typedef char gui_simple_selected_item_metadata_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_simple_selected_item, m_metadata) == 0x18) )) ? 1 : -1];
typedef char gui_simple_selected_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_simple_selected_item) == 0x190) )) ? 1 : -1];

enum { _string_id_pregame_selection = 0x100ac };

// @address 0x221ac8 ?get_description_ascii@c_gui_simple_selected_item@@UBAPADPADJ@Z
char *c_gui_simple_selected_item::get_description_ascii(char *destination, long size) const {
    wchar_string_to_utf8_string(get_description(), destination, size, 0);
    return destination;
}

// @address 0x491e58 ?get_metadata@c_gui_simple_selected_item@@UBAPBXPAX@Z
const void *c_gui_simple_selected_item::get_metadata(void *metadata) const {
    *static_cast<s_gui_selected_items_block *>(metadata) = m_metadata;
    return metadata;
}
c_load_pregame_selection_screen_message::c_load_pregame_selection_screen_message(e_controller_index controller,
    e_window_index window, long layered_position, e_gui_selected_item_type selection_type) :
    c_load_screen_message(_string_id_pregame_selection, controller, window, layered_position),
    m_selection_type(selection_type)
{
}

// @address 0x491e98 ?apply_initial_state@c_load_pregame_selection_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_pregame_selection_screen_message::apply_initial_state(c_gui_screen_widget *screen) const {
    if (!screen->running_in_codeless_mode()) {
        c_gui_screen_pregame_selection *selection_screen = (c_gui_screen_pregame_selection *)screen;
        selection_screen->set_selection_type(m_selection_type);
    }
}

// @address 0x491d68 ??0c_gui_simple_selected_item@@QAA@W4e_gui_selected_item_type@@PB_W1W4e_special_item_type@c_gui_selected_item@@W4e_gui_selected_item_location@@JJ@Z
c_gui_simple_selected_item::c_gui_simple_selected_item(e_gui_selected_item_type type, const wchar_t *name, const wchar_t *description,
    c_gui_selected_item::e_special_item_type special_type, e_gui_selected_item_location location, long, long)
{
    m_type = type;
    m_special_type = special_type;
    m_location = location;
    m_flag14 = true;
    m_flag15 = false;
    m_flag16 = false;
    s_saved_game_item_metadata packed;
    saved_game_files_initialize_metadata(&packed, -1, -1, name, description, 0, 0);
    gui_selected_items_unpack_metadata(&m_metadata, (const s_saved_game_item_metadata_packed *)(&packed));
    ustrnzcpy_inline(m_metadata.description, description, 128);
}
