// interfacelibxbox/c_gui_group_widget.obj: from the records 0x4e12b8 0x4e1460 0x4e1628 0x4e1668 0x4e1750 0x4e1868 0x4e18b8 0x4e2588
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "tag_files/tag_groups.h"
#include "shell/shell.h"
#include "interface/core_widget_definition.h"
#include "interface/gui_widget_types.h"
#include "interface/c_gui_widget.h"
#include "interface/gui_model_widget_types.h"
#include "interface/user_interface_messages.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_animation_transform.h"
#include "math/real_math_types.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_group_widget.h"
#include "interface/list_widget_definition.h"
#include "interface/group_widget_definition.h"
#include "interface/text_widget_definition.h"
#include "interface/bitmap_widget_definition.h"
#include "interface/model_widget_definition.h"
#include "interface/c_gui_button_key_widget.h"
#include "text/string_list_strings.h"
// @address 0x4e1460 ?create_and_add_children_from_definition@c_gui_group_widget@@IAAXXZ
// @address 0x4e1668 ?contains_submenu_control_definition@c_gui_group_widget@@QAA_NJPAUs_list_widget_block@@@Z
// @address 0x4e1750 ?initialize@c_gui_button_key_widget@@UAAXPBUs_button_key_block@@_N@Z
// @address 0x4e1868 ?get_string_by_string_id@c_gui_button_key_widget@@UAA_NJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4e18b8 ??0s_runtime_group_widget_definition@@QAA@XZ
// @address 0x48a150 ?create_and_add_child_text_widget@c_gui_widget@@QAAPAVc_gui_text_widget@@PBUs_text_widget_block@@@Z
// @address 0x48a288 ?create_and_add_child_bitmap_widget@c_gui_widget@@QAAPAVc_gui_bitmap_widget@@PBUs_bitmap_widget_block@@@Z
// @address 0x48b648 ?get_parent_screen@c_gui_widget@@QAAPAVc_gui_screen_widget@@XZ
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x4e8138 ?get_core_definition@c_gui_group_widget@@UAAPAUs_runtime_core_widget_definition@@XZ


struct s_text_widget_block;
struct s_bitmap_widget_block;
struct s_list_item_widget_block;
struct s_runtime_text_widget_definition;
class c_font_cache;
class c_gui_text_widget;
class c_gui_button_key_widget;
template<long k_maximum_count> class c_static_wchar_string;
namespace {
static __forceinline void group_widget_create_and_add_child_list_widget(c_gui_widget *parent, const s_list_widget_block *list_widget_block)
{
    c_gui_list_widget *list_widget = parent->create_list_widget(list_widget_block);
    if (list_widget != 0)
    {
        gui_widget_append_child(parent, list_widget);
        list_widget->initialize(list_widget_block);
    }
}
}

// @address 0x4e12b8 ?initialize@c_gui_group_widget@@UAAXPBUs_group_widget_block@@_N@Z
void c_gui_group_widget::initialize(const s_group_widget_block *group_widget_block, bool create_children)
{
    if (group_widget_block != 0)
    {
        bool require_nonzero_bounds = false;
        if (group_widget_block->group_widget_template.index != -1)
        {
            const s_group_widget_definition *template_definition = (const s_group_widget_definition *)tag_get(
                'grup', group_widget_block->group_widget_template.index);
            add_definition_fields(template_definition, require_nonzero_bounds);
            require_nonzero_bounds = true;
        }
        add_definition_fields(&group_widget_block->definition, require_nonzero_bounds);
    }
    if (create_children)
        create_and_add_children_from_definition();
    m_name = m_definition.name;
    c_gui_widget::initialize();
}

void c_gui_group_widget::create_and_add_children_from_definition()
{
    c_gui_widget *widget = this;
    for (long list_widget_index = 0; list_widget_index < m_definition.list_widgets.count; list_widget_index++)
    {
        s_list_widget_block *list_widget_block = &((s_list_widget_block *)m_definition.list_widgets.address)[list_widget_index];
        bool is_submenu = (list_widget_block->definition.core.flags & 8) != 0;
        if ((list_widget_block->definition.core.flags & 2) == 0 && list_widget_block->list_widget_template.index != -1)
        {
            s_list_widget_definition *template_definition = (s_list_widget_definition *)tag_get('lst3', list_widget_block->list_widget_template.index);
            is_submenu |= (template_definition->core.flags & 8) != 0;
        }
        if (!is_submenu)
            group_widget_create_and_add_child_list_widget(widget, list_widget_block);
    }
    for (long text_widget_index = 0; text_widget_index < m_definition.text_widgets.count; text_widget_index++)
    {
        const s_text_widget_block *text_widget = &((const s_text_widget_block *)m_definition.text_widgets.address)[text_widget_index];
        widget->create_and_add_child_text_widget((const s_text_widget_block *)text_widget);
    }
    for (long bitmap_widget_index = 0; bitmap_widget_index < m_definition.bitmap_widgets.count; bitmap_widget_index++)
    {
        const s_bitmap_widget_block *bitmap_widget = &((const s_bitmap_widget_block *)m_definition.bitmap_widgets.address)[bitmap_widget_index];
        widget->create_and_add_child_bitmap_widget((const s_bitmap_widget_block *)bitmap_widget);
    }
    c_gui_group_widget *self = this;
    for (int model_widget_index = 0; model_widget_index < self->m_definition.model_widgets.count; model_widget_index++)
    {
        const s_model_widget_block *model_widget_block = &((const s_model_widget_block *)self->m_definition.model_widgets.address)[model_widget_index];
        c_gui_model_widget *model_widget = widget->create_model_widget(model_widget_block);
        if (model_widget == 0)
            continue;
        gui_widget_append_child(widget, model_widget);
        model_widget->initialize(model_widget_block);
    }
}

// @address 0x4e1628 ?create_list_item_widget@c_gui_group_widget@@UAAPAVc_gui_list_item_widget@@PBUs_list_item_widget_block@@@Z
c_gui_list_item_widget *c_gui_group_widget::create_list_item_widget(const s_list_item_widget_block *list_item_widget_block)
{
    return get_parent_screen()->create_list_item_widget(list_item_widget_block);
}

bool c_gui_group_widget::contains_submenu_control_definition(long submenu_name,
    s_list_widget_block *submenu_definition)
{
    bool found = false;
    if (submenu_name != -1 && submenu_name != 0)
    {
        for (long index = 0; index < m_definition.list_widgets.count; ++index)
        {
            s_list_widget_block *list_widget = &((s_list_widget_block *)m_definition.list_widgets.address)[index];
            bool is_submenu = (list_widget->definition.core.flags & 8) != 0;
            s_list_widget_definition *referenced_definition = 0;
            if ((list_widget->definition.core.flags & 2) == 0 && list_widget->list_widget_template.index != -1)
            {
                s_list_widget_definition *template_definition = static_cast<s_list_widget_definition *>(
                    tag_get('lst3', list_widget->list_widget_template.index));
                is_submenu |= (template_definition->core.flags & 8) != 0;
            }
            if (is_submenu && (list_widget->definition.core.name == submenu_name ||
                ((list_widget->definition.core.name == -1 || list_widget->definition.core.name == 0) &&
                 referenced_definition != 0 && referenced_definition->core.name == submenu_name)))
            {
                *submenu_definition = *list_widget;
                found = true;
                break;
            }
            else
            {
                continue;
            }
        }
    }
    return found;
}

// @address 0x4e1750 ?initialize@c_gui_button_key_widget@@UAAXPBUs_button_key_block@@_N@Z
void c_gui_button_key_widget::initialize(const s_button_key_block *button_key_block, bool create_children)
{
    if (button_key_block && button_key_block->button_key_template.index != -1)
    {
        const s_button_key_definition *definition =
            (const s_button_key_definition *)global_tag_instances[(unsigned short)button_key_block->button_key_template.index].definition;
        real_rectangle2d authored_bounds;
        get_parent()->get_authored_bounds(&authored_bounds);
        c_gui_widget::add_definition_fields(m_type, (const s_core_widget_definition *)definition,
            &m_definition, &authored_bounds, false);
        if (definition->text_widgets.count > 0 && definition->text_widgets.address)
            m_definition.text_widgets = definition->text_widgets;
        if (definition->bitmap_widgets.count > 0 && definition->bitmap_widgets.address)
            m_definition.bitmap_widgets = definition->bitmap_widgets;
        if (definition->string_list.index != -1 && definition->string_list.index != 0)
            m_definition.string_list_index = definition->string_list.index;
    }
    if (create_children)
        create_and_add_children_from_definition();
    m_name = m_definition.name;
    c_gui_widget::initialize();
}

bool c_gui_button_key_widget::get_string_by_string_id(long string_identifier, c_static_wchar_string<1024> *buffer)
{
    bool result = false;
    if (string_identifier != 0 && string_identifier != -1 && m_definition.string_list_index != -1)
    {
        text_group_get_string(m_definition.string_list_index, string_identifier, buffer);
        result = true;
    }
    return result;
}

// March 0x83715ca8: the base constructor, each block zero-filled, the string list -1, then each block emptied.
s_runtime_group_widget_definition::s_runtime_group_widget_definition()
    : list_widgets(), text_widgets(), bitmap_widgets(), model_widgets(), string_list_index(-1)
{
    list_widgets.count = 0;
    list_widgets.address = 0;
    text_widgets.count = 0;
    text_widgets.address = 0;
    bitmap_widgets.count = 0;
    bitmap_widgets.address = 0;
    model_widgets.count = 0;
    model_widgets.address = 0;
}

// @address 0x4e2588 ?add_definition_fields@c_gui_list_widget@@QAAXPBUs_list_widget_definition@@_N@Z
void c_gui_list_widget::add_definition_fields(const s_list_widget_definition *definition, bool use_defaults)
{
    real_rectangle2d parent_bounds;
    robust_pointer_get_repaired_value(&m_parent)->get_authored_bounds(&parent_bounds);
    c_gui_widget::add_definition_fields((e_gui_widget_type)m_type, &definition->core,
        &m_definition, &parent_bounds, use_defaults);
    if (definition->datasource != -1 && definition->datasource != 0)
    {
        m_datasource_name = m_definition.datasource = definition->datasource;
    }
    if (definition->skin.index != -1 && definition->skin.index != 0)
    {
        m_definition.skin_index = definition->skin.index;
    }
    if (definition->rows != 0)
    {
        m_definition.rows = definition->rows;
    }
    if (definition->items.count > 0 && definition->items.address)
    {
        m_definition.list_items = definition->items;
    }
    if (definition->previous.index != -1 && definition->previous.index != 0)
    {
        m_definition.more_items_preceeding_bitmap_index = definition->previous.index;
    }
    if (definition->next.index != -1 && definition->next.index != 0)
    {
        m_definition.more_items_following_bitmap_index = definition->next.index;
    }
}

// @address 0x4e7890 ?create_text_widget@c_gui_group_widget@@UAAPAVc_gui_text_widget@@PBUs_runtime_text_widget_definition@@@Z
c_gui_text_widget *c_gui_group_widget::create_text_widget(const s_runtime_text_widget_definition *definition)
{
    return get_parent_screen()->create_text_widget(definition);
}

// @address 0x4e78d0 ?create_bitmap_widget@c_gui_group_widget@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_group_widget::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    return get_parent_screen()->create_bitmap_widget(definition);
}

// @address 0x4e7910 ?create_group_widget@c_gui_group_widget@@UAAPAV1@PBUs_group_widget_definition@@@Z
c_gui_group_widget *c_gui_group_widget::create_group_widget(const s_group_widget_definition *definition)
{
    return get_parent_screen()->create_group_widget(definition);
}

// @address 0x4e7950 ?create_button_key_widget@c_gui_group_widget@@UAAPAVc_gui_button_key_widget@@PBUs_button_key_definition@@@Z
c_gui_button_key_widget *c_gui_group_widget::create_button_key_widget(const s_button_key_definition *definition)
{
    return get_parent_screen()->create_button_key_widget(definition);
}

// @address 0x4e7990 ?create_list_widget@c_gui_group_widget@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
c_gui_list_widget *c_gui_group_widget::create_list_widget(const s_list_widget_block *definition)
{
    return get_parent_screen()->create_list_widget(definition);
}

// @address 0x4e79d0 ?create_model_widget@c_gui_group_widget@@UAAPAVc_gui_model_widget@@PBUs_model_widget_block@@@Z
c_gui_model_widget *c_gui_group_widget::create_model_widget(const s_model_widget_block *definition)
{
    return get_parent_screen()->create_model_widget(definition);
}
