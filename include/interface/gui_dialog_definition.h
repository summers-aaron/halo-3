#ifndef INTERFACE_GUI_DIALOG_DEFINITION_H
#define INTERFACE_GUI_DIALOG_DEFINITION_H
struct s_gui_dialog_definition
{
    long name;
    short description_flags;
    long title;
    long description;
    long choices[4];
    long button_key;
    short initial_choice;
    short b_button_action;
};
typedef char gui_dialog_definition_size[(sizeof(s_gui_dialog_definition) == 40) ? 1 : -1];
#endif
