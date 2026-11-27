#ifndef INTERFACE_GUI_SIMPLE_SELECTED_ITEM_H
#define INTERFACE_GUI_SIMPLE_SELECTED_ITEM_H
#include "interface/gui_selected_items.h"
#include "interface/gui_selected_item.h"
#include <string.h>
class c_gui_simple_selected_item : public c_gui_selected_item
{
public:
    // For derived items whose out-of-line constructor fills every field.
    __forceinline c_gui_simple_selected_item() {}

    __forceinline c_gui_simple_selected_item(const c_gui_simple_selected_item &other)
    {
        m_type = other.m_type;
        m_special_type = other.m_special_type;
        m_location = other.m_location;
        m_flag14 = other.m_flag14;
        m_flag15 = other.m_flag15;
        m_flag16 = other.m_flag16;
        memcpy(&m_metadata, &other.m_metadata, sizeof(m_metadata));
    }

    // A saved-file item: clears the metadata block, then copies the caller's block in.
    __forceinline c_gui_simple_selected_item(e_gui_selected_item_type type, const s_gui_selected_items_block *metadata,
        e_gui_selected_item_location location, unsigned char enabled, unsigned char corrupt)
    {
        m_type = type;
        m_flag15_byte = enabled;
        m_location = location;
        m_special_type = c_gui_selected_item::_special_item_type_none;
        m_flag14 = true;
        m_flag16_byte = corrupt;
        memset(&m_metadata, 0, sizeof(m_metadata));
        memcpy(&m_metadata, metadata, sizeof(m_metadata));
    }

    __forceinline c_gui_simple_selected_item(e_gui_selected_item_type type, const s_saved_game_item_metadata_packed *metadata,
        e_gui_selected_item_location location, bool corrupt, bool created)
    {
        m_type = type;
        m_location = location;
        m_flag15 = corrupt;
        m_special_type = _special_item_type_none;
        m_flag14 = true;
        m_flag16 = created;
        memset(&m_metadata, 0, sizeof(m_metadata));
        gui_selected_items_unpack_metadata(&m_metadata, metadata);
    }

    c_gui_simple_selected_item(e_gui_selected_item_type type, const wchar_t *name, const wchar_t *description,
        c_gui_selected_item::e_special_item_type special_type, e_gui_selected_item_location location, long, long);

    c_gui_simple_selected_item &operator=(const c_gui_simple_selected_item &other)
    {
        m_type = other.m_type;
        m_special_type = other.m_special_type;
        m_location = other.m_location;
        m_flag14 = other.m_flag14;
        m_flag15 = other.m_flag15;
        m_flag16 = other.m_flag16;
        m_metadata = other.m_metadata;
        return *this;
    }

    virtual bool is_valid() const;
    virtual e_gui_selected_item_location get_location() const;
    virtual bool get_file_path(char *path, long size) const { return false; }
    virtual const wchar_t *get_title() const;
    // Converts the description to UTF-8.
    virtual char *get_description_ascii(char *buffer, long size) const;
    virtual const wchar_t *get_description() const;    /* slot 5 */
    virtual const void *get_metadata(void *metadata) const;

    virtual bool get_flag14() const;
    virtual bool get_enabled() const;
    virtual bool get_corrupt() const;

    e_gui_selected_item_location m_location;
    bool m_flag14;
    union { bool m_flag15; unsigned char m_flag15_byte; };
    union { bool m_flag16; unsigned char m_flag16_byte; };
    s_gui_selected_items_block m_metadata;
};

#endif
