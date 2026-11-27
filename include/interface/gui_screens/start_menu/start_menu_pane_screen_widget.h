#ifndef INTERFACE_GUI_SCREENS_START_MENU_START_MENU_PANE_SCREEN_WIDGET_H
#define INTERFACE_GUI_SCREENS_START_MENU_START_MENU_PANE_SCREEN_WIDGET_H

// The start menu pane (Bungie's interface\gui_screens\start_menu\start_menu_pane_screen_widget.cpp; the March 2007
// debug build asserts there at line 81). Method list, const qualifiers and access from the March debug map and
// the pane subclasses. The class has no vtable of its own (every pane is a derived class whose
// constructor inlines this one); slot 50 (controller_event_should_go_to_parent, protected) is its one new virtual.
// Fields: March's setup() (0x8365f530), get_target_xuid(), set_service_record_identity() and
// set_automatic_button_key(); the service record identity below, which shrank from
// 0x74 to 0x5c); translation and rotation (0x1afc/0x1b00, not in March) are what load_pane receives from a pane.

#include "interface/c_gui_screen_widget.h"
#include "networking/online/online_service_record.h"

class c_gui_list_item_widget;
class c_gui_data;
class c_controller_input_message;

class __declspec(novtable) c_start_menu_pane_screen_widget : public c_gui_screen_widget {
public:
    __forceinline c_start_menu_pane_screen_widget(long name) : c_gui_screen_widget(name) {}
    __forceinline c_start_menu_pane_screen_widget(long name, long automatic_button_key) :
        c_gui_screen_widget(name),
        m_translation(0.0f),
        m_rotation(0.0f),
        m_automatic_button_key(automatic_button_key)
    {
    }
    virtual void update(unsigned long current_milliseconds); // slot 23
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource); // slot 42
    virtual bool handle_controller_input_message(const c_controller_input_message *message); // slot 43
protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message); // slot 50

public:
    void close_current_subpane();
    void close_start_menu();
    // Copies the owner and target identity into the pane.
    __forceinline void setup(const s_player_identifier *owner_player_identifier, const s_player_identifier *target_player_identifier,
        const unsigned __int64 *target_xuid, const s_service_record_identity *service_record_identity)
    {
        m_owner_player_identifier = *owner_player_identifier;
        m_target_player_identifier = *target_player_identifier;
        m_target_xuid = *target_xuid;
        m_service_record_identity = *service_record_identity;
    }

protected:
    unsigned __int64 get_owner_xuid() const;
    // Accessors for the target identity and automatic button key.
    __forceinline unsigned __int64 get_target_xuid() const { return m_target_xuid; }
    __forceinline const s_service_record_identity *get_service_record_identity() { return &m_service_record_identity; }
    __forceinline void set_service_record_identity(const s_service_record_identity *identity) { m_service_record_identity = *identity; }
    __forceinline void set_automatic_button_key(long automatic_button_key) { m_automatic_button_key = automatic_button_key; }

public:
    // Units read some of these directly; Bungie's access for data members is not attested.
    s_player_identifier m_owner_player_identifier;          // March: setup()'s first argument
    s_player_identifier m_target_player_identifier;         // March: setup()'s second argument
    unsigned __int64 m_target_xuid;                         // March: get_target_xuid
    s_service_record_identity m_service_record_identity;    // March: get/set_service_record_identity
    float m_translation;                                    // use: passed to load_pane
    float m_rotation;                                       // use: passed to load_pane
    long m_automatic_button_key;                            // March: set_automatic_button_key
};

typedef char start_menu_pane_screen_widget_size[(sizeof(void *) != 4 || ((sizeof(c_start_menu_pane_screen_widget) == 0x1b08) )) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/start_menu_pane_screen_widget.cpp that other units call.


extern "C" unsigned char start_menu_pane_is_valid_type(const c_gui_widget *widget);

#endif // INTERFACE_GUI_SCREENS_START_MENU_START_MENU_PANE_SCREEN_WIDGET_H
