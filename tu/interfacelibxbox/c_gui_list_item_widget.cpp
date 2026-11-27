// interfacelibxbox/c_gui_list_item_widget.obj: from the records 0x4e7798 0x4e7828 0x4e7890 0x4e78d0 0x4e7910 0x4e7950 0x4e7990 0x4e79d0 0x4e7a10 0x4e7a90 0x4e7b38 0x4e7b70 0x4e7c38
// @compiler xdk6130
// @flags /TP /O1 /Oc
#include "math/real_math.h"
#include "interface/gui_widget_types.h"
#include "interface/core_widget_definition.h"
#include "interface/gui_data.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_widget.h"
#include "interface/gui_model_widget_types.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_group_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/list_widget_definition.h"
#include "interface/c_gui_list_item_widget.h"
// @address 0x4e7798 ??0c_gui_list_item_widget@@QAA@XZ
// @address 0x4e7828 ?initialize@c_gui_list_item_widget@@UAAXPBUs_list_item_widget_block@@@Z
// @address 0x4e7890 ?create_text_widget@c_gui_list_item_widget@@UAAPAVc_gui_text_widget@@PBUs_runtime_text_widget_definition@@@Z
// @address 0x4e78d0 ?create_bitmap_widget@c_gui_list_item_widget@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
// @address 0x4e7910 ?create_group_widget@c_gui_list_item_widget@@UAAPAVc_gui_group_widget@@PBUs_group_widget_definition@@@Z
// @address 0x4e7950 ?create_button_key_widget@c_gui_list_item_widget@@UAAPAVc_gui_button_key_widget@@PBUs_button_key_definition@@@Z
// @address 0x4e7990 ?create_list_widget@c_gui_list_item_widget@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
// @address 0x4e79d0 ?create_model_widget@c_gui_list_item_widget@@UAAPAVc_gui_model_widget@@PBUs_model_widget_block@@@Z
// @address 0x4e7a10 ?should_render@c_gui_list_item_widget@@UAA_NPA_N@Z
// @address 0x4e7a90 ?can_receive_focus@c_gui_list_item_widget@@UAA_NXZ
// @address 0x4e7b38 ?get_element_handle@c_gui_list_item_widget@@UAAJXZ
// @address 0x4e7b70 ?handle_widget_back_out@c_gui_list_item_widget@@UAA_NXZ
// @address 0x4e7c38 ?handle_widget_selected@c_gui_list_item_widget@@UAA_NXZ
// @address 0x489610 ?initialize@c_gui_widget@@UAAXXZ
// @address 0x48b648 ?get_parent_screen@c_gui_widget@@QAAPAVc_gui_screen_widget@@XZ
// @address 0x48b808 ?get_authored_bounds@c_gui_widget@@QAAPATreal_rectangle2d@@PAT2@@Z
// @address 0x4e3600 ??0s_list_widget_block@@QAA@XZ
// @address 0x4e1668 ?contains_submenu_control_definition@c_gui_group_widget@@QAA_NJPAUs_list_widget_block@@@Z
// @address 0x489570 ??0c_gui_widget@@QAA@W4e_gui_widget_type@@@Z
// @address 0x489bf8 ?add_definition_fields@c_gui_widget@@SAXW4e_gui_widget_type@@PBUs_core_widget_definition@@PAUs_runtime_core_widget_definition@@PBTreal_rectangle2d@@_N@Z

class c_font_cache;
struct s_widget_animation_definition;

class c_gui_text_widget;
class c_gui_button_key_widget;
struct s_runtime_text_widget_definition;
struct s_group_widget_definition;
struct s_button_key_definition;
struct s_model_widget_block;


c_gui_list_item_widget::c_gui_list_item_widget() : c_gui_widget(_gui_list_item_widget)
{
    m_list_item_index = -1;
}

void c_gui_list_item_widget::initialize(const s_list_item_widget_block *list_item_widget_block)
{
    real_rectangle2d bounds;
    if (list_item_widget_block)
    {
        robust_pointer_get_repaired_value(&m_parent)->get_authored_bounds(&bounds);
        c_gui_widget::add_definition_fields(m_type, (const s_core_widget_definition *)list_item_widget_block,
            (s_runtime_core_widget_definition *)&m_definition, &bounds, false);
        m_name = m_definition.name;
        m_definition.item_label = list_item_widget_block->item_label;
    }
    c_gui_widget::initialize();
}

c_gui_text_widget *c_gui_list_item_widget::create_text_widget(const s_runtime_text_widget_definition *definition)
{
    return get_parent_screen()->create_text_widget(definition);
}

c_gui_bitmap_widget *c_gui_list_item_widget::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    return get_parent_screen()->create_bitmap_widget(definition);
}

c_gui_group_widget *c_gui_list_item_widget::create_group_widget(const s_group_widget_definition *definition)
{
    return get_parent_screen()->create_group_widget(definition);
}

c_gui_button_key_widget *c_gui_list_item_widget::create_button_key_widget(const s_button_key_definition *definition)
{
    return get_parent_screen()->create_button_key_widget(definition);
}

c_gui_list_widget *c_gui_list_item_widget::create_list_widget(const s_list_widget_block *definition)
{
    return get_parent_screen()->create_list_widget(definition);
}

c_gui_model_widget *c_gui_list_item_widget::create_model_widget(const s_model_widget_block *definition)
{
    return get_parent_screen()->create_model_widget(definition);
}

c_gui_list_item_widget *c_gui_list_item_widget::create_list_item_widget(const s_list_item_widget_block *definition)
{
    return get_parent_screen()->create_list_item_widget(definition);
}

bool c_gui_list_item_widget::should_render(bool *render_with_bounds)
{
    bool render = c_gui_widget::should_render(render_with_bounds);
    bool result = render & valid();
    return result;
}

// match: 0x4e7a90: favor-speed through EOF keeps cr6 compares + tail-duplicated epilogues, not rlwinm. + shared epilogue
#pragma optimize("t", on)
// no dev name
bool c_gui_list_item_widget::can_receive_focus()
{
    c_gui_list_widget *list = (c_gui_list_widget *)gui_widget_get_parent_list_widget(this);
    // match: 0x4e7a90: preserve the old view's unsigned flags read; the canonical header's signed shift adds 4 bytes (check 1).
    if (list != 0 && (bool)((unsigned long)list->m_definition.flags >> 7 & 1))
        return false;
    if ((m_controller_mask & 0x10) != 0)
        return true;
    return valid();
}

long c_gui_list_item_widget::get_element_handle()
{
    long list_item_index = m_list_item_index;
    return ((c_gui_list_widget *)gui_widget_get_parent_list_widget(this))->get_element_handle_from_list_item_index(list_item_index);
}

bool c_gui_list_item_widget::handle_widget_back_out()
{
    bool result = false;
    c_gui_widget *list_widget = gui_widget_get_parent_list_widget(this);
    c_gui_list_widget *list = (c_gui_list_widget *)list_widget;
    if (list != 0 && list->m_is_submenu)
    {
        c_gui_widget *outer = gui_widget_get_parent_list_widget(list_widget);
        outer->start_animation_at_time(_animation_state_unload_submenu_focused, -1, false);
        for (c_gui_widget *child = robust_pointer_get_repaired_value(&outer->m_children); child != 0; child = robust_pointer_get_repaired_value(&child->m_next))
        {
            child->start_animation_at_time(
                (e_animation_state)(_animation_state_unload_submenu_focused +
                    (child != ((c_gui_list_widget *)outer)->m_submenu_owner_list_item)), -1, true);
        }
        list_widget->start_animation_at_time(_animation_state_unload_as_submenu, -1, true);
        result = true;
    }
    return result;
}


__forceinline bool c_gui_list_item_widget::invokes_submenu(long *submenu_name)
{
    bool invokes = false;
    long element_handle = get_element_handle();
    c_gui_widget *parent_list = gui_widget_get_parent_list_widget(this);
    c_gui_data *data = ((c_gui_list_widget *)parent_list)->get_data();
    if (data && data->get_invoked_control(element_handle, submenu_name))
    {
        c_gui_group_widget *parent_group = parent_list ? (c_gui_group_widget *)gui_widget_get_parent_group_widget(parent_list) : 0;
        s_list_widget_block definition;
        if (parent_group && parent_group->contains_submenu_control_definition(*submenu_name, &definition))
        {
            invokes = true;
        }
    }
    return invokes;
}

// ?handle_widget_selected@c_gui_list_item_widget@@UAA_NXZ (the virtual)
bool c_gui_list_item_widget::handle_widget_selected()
{
    bool handled = false;
    long submenu_name;
    if (invokes_submenu(&submenu_name) && gui_widget_get_parent_list_widget(this))
    {
        handled = (gui_widget_get_parent_list_widget(this))->invoke_submenu(submenu_name, -1, true);
    }
    return handled;
}
