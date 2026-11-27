#ifndef INTERFACE_GUI_SELECTED_ITEM_H
#define INTERFACE_GUI_SELECTED_ITEM_H
#include "interface/gui_selected_items.h"
class __declspec(novtable) c_gui_selected_item
{
public:
    __forceinline e_gui_selected_item_type get_selection_type() const;
    enum e_special_item_type { _special_item_type_none = -1 };
    virtual bool is_valid() const;
    virtual e_gui_selected_item_location get_location() const;
    virtual bool get_file_path(char *path, long size) const;
    virtual const wchar_t *get_title() const;
    virtual char *get_description_ascii(char *buffer, long size) const;
    virtual const wchar_t *get_description() const;
    virtual const void *get_metadata(void *metadata) const;
    virtual bool get_flag14() const;
    virtual bool get_enabled() const;
    virtual bool get_corrupt() const;

    long m_reserved_4;
    e_gui_selected_item_type m_type;
    e_special_item_type m_special_type;
};
#endif
