// interfacelibxbox/user_interface_data.obj: from the records 0x4a0c78 0x4a0d98 0x4a0df8 0x4a0e50 0x4a0e58 0x4a0e60 0x4a0e80 0x4a0ea0 0x4a0ec0 0x4a0ed8 0x4a0f58 0x4a0fc0 0x4a0ff8 0x4a1098 0x4a10f0 0x4a1160 0x4a1170 0x4a1198 0x4a11d0 0x4a1420 0x4a14b0 0x4a1540 0x4a15d8 0x4a1668 0x4a16e8 0x4a1780 0x4a1818 0x4a18b8 0x4a1958 0x4a19f8 0x4a1a08 0x4a1a48 0x4a1b10 0x4a1b88 0x4c2070
// @flags /TP /O2 /J /Oc
#include "memory/static_arrays.h"
#include "interface/gui_data.h"
#include "interface/user_interface_data.h"
// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
// @address 0x1ec288 ?data_dispose@@YAXPAUs_data_array@@@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x7c1b88 g_user_interface_allocation
#include <string.h>
#include "memory/data_types.h"
#include "tag_files/tag_groups.h"
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"
#include "memory/data.h"
#include "interface/user_interface_memory_allocation.h"
#include "game/player_configuration.h"
#include "tag_files/tag_types.h"


struct s_integer_column { long name; long value; };
struct s_text_column { long name; unsigned short value[16]; };
struct s_string_id_column { long name; long value; };
struct s_datasource_block
{
    c_typed_tag_block<s_integer_column> integers;
    c_typed_tag_block<s_text_column> texts;
    c_typed_tag_block<s_string_id_column> string_ids;
    long invoked_control;
};
struct s_datasource_definition
{
    char header[16];
    c_typed_tag_block<s_datasource_block> elements;
};

class c_gui_selected_item;

#include "interface/c_gui_data_array_test.h"

static __forceinline long data_previous_index(s_data_array *data, long index)
{
    long result = -1;
    long absolute_index = (index == -1 ? data->first_unallocated : (unsigned short)index) - 1;
    if (absolute_index >= 0 && absolute_index < data->first_unallocated)
    {
        short *datum = (short *)(data->data + absolute_index * data->size);
        do
        {
            if (*datum != 0)
            {
                result = absolute_index | ((unsigned short)*datum << 16);
                break;
            }
            datum = (short *)((char *)datum - data->size);
        } while (absolute_index-- >= 0);
    }
    return result;
}

// @address 0x4a0c78 ?get_maximum_item_count@c_gui_data@@UAAJXZ
long c_gui_data::get_maximum_item_count()
{
    return get_current_item_count();
}

void c_gui_data::dispose()
{
}

void c_gui_data::update()
{
}

bool c_gui_data::is_busy()
{
    return false;
}

bool c_gui_data::get_integer_value(long element_handle, long value_name, long *value)
{
    return false;
}

bool c_gui_data::get_qword_value(long element_handle, long value_name, unsigned __int64 *value)
{
    return false;
}

bool c_gui_data::get_real_value(long element_handle, long value_name, float *value)
{
    return false;
}

bool c_gui_data::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    return false;
}

bool c_gui_data::get_string_id_value(long element_handle, long value_name, long *value)
{
    return false;
}

bool c_gui_data::get_player_appearance(long element_handle, s_player_appearance *appearance)
{
    return false;
}

const c_gui_selected_item *c_gui_data::get_gui_selected_item(long element_handle) const
{
    return 0;
}

bool c_gui_data::get_invoked_control(long element_handle, long *control_name)
{
    return false;
}

void c_gui_data::clear_disabled_elements()
{
}

void c_gui_data::set_disabled_element(long element_handle, long reason)
{
}

void c_gui_data::disable_all_elements(long reason)
{
}

void c_gui_data::enable_element(long element_handle, long reason)
{
}

void c_gui_data::delete_element(long element_handle)
{
}

bool c_gui_data::get_element(long element_handle, void *element, long element_size)
{
    return false;
}

void c_gui_data::set_element(long element_handle, const void *element, long element_size)
{
}

// @address 0x4a0d98 ?initialize@c_gui_data_array@@UAA_NJ@Z
bool c_gui_data_array::initialize(long name)
{
    if (m_list_data)
    {
        data_make_valid(m_list_data);
        return c_gui_data::initialize(name);
    }
    else
        return false;
}
// @address 0x4a0df8 ??1c_gui_data_array@@UAA@XZ
c_gui_data_array::~c_gui_data_array()
{
    if (m_list_data)
    {
        data_dispose(m_list_data);
        m_list_data = 0;
    }
}
// @address 0x4a0e50 ?add_element@c_gui_data_array@@UAAJXZ
long c_gui_data_array::add_element()
{
    return datum_new(m_list_data);
}
// @address 0x4a0e58 ?delete_element@c_gui_data_array@@UAAXJ@Z
void c_gui_data_array::delete_element(long element_handle)
{
    ::datum_delete(m_list_data, element_handle);
}
// @address 0x4a0e60 ?get_maximum_item_count@c_gui_data_array@@UAAJXZ
long c_gui_data_array::get_maximum_item_count()
{
    long result = m_list_data ? m_list_data->maximum_count : 0;
    return result;
}
// @address 0x4a0e80 ?get_current_item_count@c_gui_data_array@@UAAJXZ
long c_gui_data_array::get_current_item_count()
{
    long result = m_list_data ? m_list_data->actual_count : 0;
    return result;
}
// @address 0x4a0ea0 ?get_first_element_handle@c_gui_data_array@@UAAJXZ
long c_gui_data_array::get_first_element_handle()
{
    long result = m_list_data ? ::data_next_index(m_list_data, -1) : -1;
    return result;
}
// @address 0x4a0ec0 ?get_next_element_handle@c_gui_data_array@@UAAJJ@Z
long c_gui_data_array::get_next_element_handle(long element_handle)
{
    long result = m_list_data ? ::data_next_index(m_list_data, element_handle) : -1;
    return result;
}
// @address 0x4a0ed8 ?get_previous_element_handle@c_gui_data_array@@UAAJJ@Z
long c_gui_data_array::get_previous_element_handle(long element_handle)
{
    long result = -1;
    if (m_list_data)
        result = data_previous_index(m_list_data, element_handle);
    return result;
}
// @address 0x4a0f58 ?get_element@c_gui_data_array@@UAA_NJPAXJ@Z
bool c_gui_data_array::get_element(long element_handle, void *element, long element_size)
{
    bool result = false;
    if (m_list_data)
    {
        void *datum = datum_try_and_get(m_list_data, element_handle);
        if (datum)
        {
            memcpy(element, datum, element_size);
            result = true;
        }
    }
    return result;
}
// @address 0x4a0fc0 ?set_element@c_gui_data_array@@UAAXJPBXJ@Z
void c_gui_data_array::set_element(long element_handle, const void *element, long element_size)
{
    long copy_size = element_size - sizeof(short);
    if (copy_size > 0)
    {
        char *datum = (char *)datum_get(m_list_data, element_handle);
        memcpy(datum + sizeof(short), (const char *)element + sizeof(short), copy_size);
    }
}
// @address 0x4a0ff8 ?initialize@c_gui_data_array_test@@UAA_NJ@Z
bool c_gui_data_array_test::initialize(long name)
{
    internal_critical_section_enter(19);
    s_data_array *data = data_new("test data array", 101, 4, 0, get_user_interface_allocation());
    _InterlockedDecrement(&g_synch_globals.critical_sections[19].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[19].critical_section);
    m_list_data = data;
    if (data)
        return c_gui_data_array::initialize(name);
    return false;
}
// @address 0x4a1098 ?get_integer_value@c_gui_data_array_test@@UAA_NJJPAJ@Z
bool c_gui_data_array_test::get_integer_value(long element_handle, long value_name, long *value)
{
    if (m_list_data && datum_try_and_get(m_list_data, element_handle))
    {
        *value = (unsigned short)element_handle;
        return true;
    }
    else
        return false;
}
static __forceinline const char *user_interface_data_string_id_get_string_const(long string_id)
{
    return "blah blah";
}
// @address 0x484d20 ?print@?$c_static_wchar_string@$0EAA@@@QAAPB_WPB_WZZ
// @address 0x4a10f0 ?get_text_value@c_gui_data_array_test@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_data_array_test::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *buffer)
{
    if (m_list_data && datum_try_and_get(m_list_data, element_handle))
    {
        const char *value_name_string = user_interface_data_string_id_get_string_const(value_name);
        buffer->print(L"%hs #%d", value_name_string, (element_handle & 0xffff));
        return true;
    }
    return false;
}
// @address 0x4a1160 ?get_string_id_value@c_gui_data_array_test@@UAA_NJJPAJ@Z
bool c_gui_data_array_test::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool result = true;
    *value = value_name;
    return result;
}
// @address 0x4a1170 ?initialize@c_gui_tag_datasource@@UAA_NJ@Z
bool c_gui_tag_datasource::initialize(long name)
{
    bool result = m_definition_index != -1 ? c_gui_data::initialize(name) : false;
    return result;
}
// @address 0x4a1198 ?get_current_item_count_internal@c_gui_tag_datasource@@MAAJXZ
long c_gui_tag_datasource::get_current_item_count_internal()
{
    long result = 0;
    const s_datasource_definition *definition = get_definition();
    if (definition)
        result = definition->elements.count();
    return result;
}
// @address 0x4a11d0 ?get_column_names@c_gui_tag_datasource@@UAAXQAJPAJ@Z
void c_gui_tag_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    for (long handle = get_first_element_handle(); handle != -1; handle = get_next_element_handle(handle))
    {
        s_datasource_block element;
        if (get_element(handle, &element, sizeof(element)))
        {
            for (long index = 0; index < element.integers.count(); index++)
            {
                s_integer_column *column = &element.integers[index];
                bool found = false;
                for (long name_index = 0; name_index < *column_count; name_index++)
                    found |= (column_names[name_index] - column->name) == 0;
                if (!found)
                {
                    column_names[*column_count] = column->name;
                    (*column_count)++;
                }
            }
            for (long index = 0; index < element.texts.count(); index++)
            {
                s_text_column *column = &element.texts[index];
                bool found = false;
                for (long name_index = 0; name_index < *column_count; name_index++)
                    found |= (column_names[name_index] - column->name) == 0;
                if (!found)
                {
                    column_names[*column_count] = column->name;
                    (*column_count)++;
                }
            }
            for (long index = 0; index < element.string_ids.count(); index++)
            {
                s_string_id_column *column = &element.string_ids[index];
                bool found = false;
                for (long name_index = 0; name_index < *column_count; name_index++)
                    found |= (column_names[name_index] - column->name) == 0;
                if (!found)
                {
                    column_names[*column_count] = column->name;
                    (*column_count)++;
                }
            }
        }
    }
}
// @address 0x4a1420 ?get_element@c_gui_tag_datasource@@UAA_NJPAXJ@Z
bool c_gui_tag_datasource::get_element(long element_handle, void *element, long element_size)
{
    const s_datasource_definition *definition = get_definition();
    bool result = false;
    if (definition && element_handle >= 0 && element_handle < definition->elements.count())
    {
        memcpy(element, &definition->elements[element_handle], sizeof(s_datasource_block));
        result = true;
    }
    return result;
}
// @address 0x4a14b0 ?get_integer_value@c_gui_tag_datasource@@UAA_NJJPAJ@Z
bool c_gui_tag_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    s_datasource_block element;
    if (get_element(element_handle, &element, sizeof(element)))
    {
        for (long index = 0; index < element.integers.count(); index++)
        {
            s_integer_column *column = &element.integers[index];
            if (column->name == value_name)
            {
                *value = column->value;
                result = true;
                break;
            }
        }
    }
    return result;
}
// @address 0x4a1540 ?get_text_value@c_gui_tag_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_tag_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *buffer)
{
    bool result = false;
    s_datasource_block element;
    if (get_element(element_handle, &element, sizeof(element)))
    {
        for (long index = 0; index < element.texts.count(); index++)
        {
            s_text_column *column = &element.texts[index];
            if (column->name == value_name)
            {
                buffer->print(L"%hs", column->value);
                result = true;
                break;
            }
        }
    }
    return result;
}
// @address 0x4a15d8 ?get_string_id_value@c_gui_tag_datasource@@UAA_NJJPAJ@Z
bool c_gui_tag_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    s_datasource_block element;
    if (get_element(element_handle, &element, sizeof(element)))
    {
        for (long index = 0; index < element.string_ids.count(); index++)
        {
            s_string_id_column *column = &element.string_ids[index];
            if (column->name == value_name)
            {
                *value = column->value;
                result = true;
                break;
            }
        }
    }
    return result;
}
// @address 0x4a1668 ?get_invoked_control@c_gui_tag_datasource@@UAA_NJPAJ@Z
bool c_gui_tag_datasource::get_invoked_control(long element_handle, long *control_name)
{
    bool result = false;
    s_datasource_block element;
    if (get_element(element_handle, &element, sizeof(element)) && element.invoked_control != -1 && element.invoked_control != 0)
    {
        *control_name = element.invoked_control;
        result = true;
    }
    return result;
}
// @address 0x4a16e8 ?get_current_item_count@c_gui_ordered_data@@UAAJXZ
long c_gui_ordered_data::get_current_item_count()
{
    long item_count = 0;
    if (m_disabled_element_count > 0)
    {
        for (long element_handle = 0; element_handle < get_current_item_count_internal(); element_handle++)
        {
            if (is_enabled(element_handle))
                item_count++;
        }
        return item_count;
    }
    return get_current_item_count_internal();
}
// @address 0x4a1780 ?get_first_element_handle@c_gui_ordered_data@@UAAJXZ
long c_gui_ordered_data::get_first_element_handle()
{
    long element_handle = 0;
    long maximum_count = get_current_item_count_internal();
    if (m_disabled_element_count > 0)
    {
        while (element_handle >= 0 && element_handle < maximum_count && !is_enabled(element_handle))
            element_handle++;
    }
    if (element_handle < 0 || element_handle >= maximum_count)
        element_handle = -1;
    return element_handle;
}
// @address 0x4a1818 ?get_next_element_handle@c_gui_ordered_data@@UAAJJ@Z
long c_gui_ordered_data::get_next_element_handle(long element_handle)
{
    long maximum_count = get_current_item_count_internal();
    element_handle++;
    if (m_disabled_element_count > 0)
    {
        while (element_handle >= 0 && element_handle < maximum_count && !is_enabled(element_handle))
            element_handle++;
    }
    if (element_handle < 0 || element_handle >= maximum_count)
        element_handle = -1;
    return element_handle;
}
// @address 0x4a18b8 ?get_previous_element_handle@c_gui_ordered_data@@UAAJJ@Z
long c_gui_ordered_data::get_previous_element_handle(long element_handle)
{
    long maximum_count = get_current_item_count_internal();
    element_handle--;
    if (m_disabled_element_count > 0)
    {
        while (element_handle >= 0 && element_handle < maximum_count && !is_enabled(element_handle))
            element_handle--;
    }
    if (element_handle < 0 || element_handle >= maximum_count)
        element_handle = -1;
    return element_handle;
}
// @address 0x4a1958 ?is_enabled@c_gui_ordered_data@@AAA_NJ@Z
bool c_gui_ordered_data::is_enabled(long element_handle)
{
    bool result = true;
    long value = -1;
    for (long index = 0; index < m_disabled_element_count; index++)
    {
        if (get_string_id_value(element_handle, m_disabled_elements[index].element_handle, &value) && value == m_disabled_elements[index].reason)
        {
            result = false;
            break;
        }
    }
    return result;
}
// @address 0x4a19f8 ?clear_disabled_elements@c_gui_ordered_data@@UAAXXZ
void c_gui_ordered_data::clear_disabled_elements()
{
    m_disabled_element_count = 0;
}
// @address 0x4a1a08 ?set_disabled_element@c_gui_ordered_data@@UAAXJJ@Z
void c_gui_ordered_data::set_disabled_element(long element_handle, long reason)
{
    if (m_disabled_element_count + 1 >= 0 && (unsigned long)(m_disabled_element_count + 1) < k_maximum_ordered_elements)
    {
        m_disabled_elements[m_disabled_element_count].element_handle = element_handle;
        m_disabled_elements[m_disabled_element_count].reason = reason;
        m_disabled_element_count++;
    }
}
// @address 0x4a1a48 ?disable_all_elements@c_gui_ordered_data@@UAAXJ@Z
void c_gui_ordered_data::disable_all_elements(long reason)
{
    clear_disabled_elements();
    for (long element_handle = get_first_element_handle(); element_handle != -1; element_handle = get_next_element_handle(element_handle))
    {
        long value = -1;
        if (get_string_id_value(element_handle, reason, &value) && value != -1)
            set_disabled_element(reason, value);
    }
}
// @address 0x4a1b10 ?enable_element@c_gui_ordered_data@@UAAXJJ@Z
void c_gui_ordered_data::enable_element(long element_handle, long reason)
{
    for (long index = 0; index < m_disabled_element_count; index++)
    {
        if (m_disabled_elements[index].element_handle == element_handle && m_disabled_elements[index].reason == reason)
        {
            m_disabled_element_count--;
            memmove(&m_disabled_elements[index], &m_disabled_elements[index + 1], (m_disabled_element_count - index) * sizeof(s_disabled_element));
            break;
        }
    }
}
// @address 0x4a1b88 ?initialize@c_gui_data_proxy@@UAA_NXZ
bool c_gui_data_proxy::initialize()
{
    bool result = c_gui_data::initialize(m_base_datasource->get_name());
    m_base_datasource->set_name(0x10008);
    return result;
}
// @address 0x4c2070 ?initialize@c_gui_data@@UAA_NJ@Z
bool c_gui_data::initialize(long name)
{
    set_name(name);
    return true;
}
