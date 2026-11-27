#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_SELECTION_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_SELECTION_H

#include "interface/c_gui_screen_widget.h"
#include "multithreading/synchronized_value.h"
#include "tag_files/file_reference.h"

class c_virtual_keyboard_task;
enum e_gui_selected_item_type;

// Functions defined in tu/interfacelibxbox/gui_screen_pregame_selection.cpp.

class c_gui_screen_pregame_selection;
class c_gui_selected_item;
enum e_controller_index;
extern "C" void gui_screen_pregame_selection_enqueue_file_transfer(c_gui_screen_pregame_selection *screen,
    e_controller_index transfer_controller, const c_gui_selected_item *transfer_item);

bool parse_xml_hopper_maximum_party_size(void *screen, wchar_t *buffer, long buffer_count);
extern "C" void gui_screen_pregame_selection_update_list_ui(c_gui_screen_pregame_selection *screen);
extern "C" void gui_screen_pregame_selection_update_pending_tasks(c_gui_screen_pregame_selection *screen);


class c_gui_screen_pregame_selection : public c_gui_screen_widget {
public:
    friend void gui_screen_pregame_selection_enqueue_file_transfer(c_gui_screen_pregame_selection *screen,
        e_controller_index transfer_controller, const c_gui_selected_item *transfer_item);
    c_gui_screen_pregame_selection(long name);
    friend void gui_screen_pregame_selection_update_list_ui(c_gui_screen_pregame_selection *screen);
    friend void gui_screen_pregame_selection_update_pending_tasks(c_gui_screen_pregame_selection *screen);
    __forceinline void set_selection_type(e_gui_selected_item_type type) { m_selection_type = type; }
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual void post_initialize();
    virtual void dispose();
    virtual void reconstruct();
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool handle_list_item_chosen(
        const c_controller_input_message *message, long p2, class c_gui_list_item_widget *list_item_widget, class c_gui_data *gui_data);
    virtual void initialize_datasource();
    virtual bool handle_dialog_result(const c_dialog_result_message *dialog_result_message);
    void begin_selected_item_text_edit(e_controller_index controller_index, bool edit_name, bool edit_description);
    void begin_saved_game_file_save_changes(e_controller_index controller, const wchar_t *name, const wchar_t *description);
    void load_progress_ui();
    void dispose_progress_ui(bool force);
    void restore_focus_to_selected_category();
    void begin_deletion_of_selected_item(e_controller_index controller);
private:
    void set_initial_focused_category();
    void shift_subitem_menu();
    void offset_widget_recursive(c_gui_widget *widget, float dx, float dy);
    void update_map_image();
    long get_current_category_element_handle();
    bool is_current_category_recent_items_list();
    __forceinline bool asynchronous_operation_in_progress() const
    {
        return m_pending_delete_task != -1 || m_pending_metadata_update_task != -1 || m_pending_name_edit || m_pending_description_edit;
    }
    long m_selection_type;
    long m_selected_index;
    c_gui_data *m_current_category_datasource;
    long m_pending_category;
    long m_pending_delete_task;
    long m_pending_metadata_update_task;
    long m_pending_copy_task;
    long m_copy_file_category;
    c_synchronized_long m_sync_1aa8;
    c_synchronized_long m_sync_1aac;
    c_synchronized_long m_sync_1ab0;
    long m_pending_save_task;
    long m_pending_save_category;
    long m_progress_ui_flags;
    unsigned long m_progress_ui_request_time;
    long m_progress_title;
    long m_progress_message;
    s_file_reference m_file_reference;
    c_virtual_keyboard_task *m_virtual_keyboard_task;
    union {
        short m_keyboard_text_length;
        wchar_t m_keyboard_text[256];
    };
    bool m_pending_name_edit;
    bool m_pending_description_edit;
    long m_content_type;
    bool m_network_join_busy;
    void set_current_category(c_gui_data *data, long element_handle);
};

typedef char gui_screen_pregame_selection_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_pregame_selection) == 0x1df0))) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_SELECTION_H
