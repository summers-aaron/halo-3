// interfacelibxbox/c_gui_widget.obj: from the records 0x1981e8 0x482a48 0x482ad8 0x483540 0x485ee8 0x485fb0 0x489570 0x489610 0x4896b0 0x489738 0x489840 0x4898b8 0x4898c0 0x489948 0x489a40 0x489af0 0x489b58 0x489ba8 0x489bf8 0x489f08 0x48a150 0x48a288 0x48a3b0 0x48a410 0x48a510 0x48a580 0x48a608 0x48a6d8 0x48a820 0x48ac58 0x48acd0 0x48ade8 0x48ae38 0x48aeb8 0x48af38 0x48af88 0x48afd8 0x48b028 0x48b098 0x48b228 0x48b298 0x48b310 0x48b390 0x48b420 0x48b540 0x48b648 0x48b6d0 0x48b738 0x48b7a0 0x48b808 0x48b870 0x48b8d8 0x48b9f8 0x48baf0 0x48bb58 0x48bbc0 0x48bc60 0x48bce8 0x48bd90 0x48bdc8 0x48bdf8 0x48be98 0x48bf08 0x48bfa0 0x48c0c8 0x48c120 0x48c178 0x48c1e8 0x48c258 0x48c2c8 0x48c890 0x48c970 0x48cb08 0x48cc38 0x48ce38 0x48cf68 0x48d128 0x48d4c0 0x48d528 0x48d538 0x48d6c0 0x48d7b8 0x48d940 0x48da38 0x48dbe8 0x48dbf0 0x48dd88 0x48de58 0x48df28 0x48e050 0x48e0a8 0x48e160 0x4ae6d8 0x4e32e0 0x4e4c10 0x4e7d38 0x4e87a0 0x57f558
// @flags /TP /O2 /Oc
#include "interface/gui_widget_tree.h"
#include "interface/gui_list_widget_queries.h"
#include "interface/user_interface_clock.h"
#include "text/font_cache_lock.h"
#include "text/font_id_types.h"
#include "interface/gui_widget.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record/gui_start_menu_hq_service_record.h"
#include "interface/core_widget_definition.h"
#include "interface/bitmap_widget_definition.h"
#include "math/real_math_types.h"
#include <string.h>
#include "memory/static_arrays.h"
#include "tag_files/tag_groups.h"
#include "cseries/cseries.h"
#include "tag_files/string_ids.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "game/game_time_types.h"
#include "shell/shell.h"
// @address 0x48a6d8 ?get_ambient_state@c_gui_widget@@MAA?AW4e_animation_state@@XZ
// @address 0x489b58 ?get_element_handle@c_gui_widget@@UAAJXZ
// @address 0x489ba8 ?get_datasource_index@c_gui_widget@@UAAJXZ
// @address 0x4898c0 ?set_enabled@c_gui_widget@@UAAX_N@Z
// @address 0x48b8d8 ?get_unprojected_bounds@c_gui_widget@@QAAPATgui_real_rectangle2d@@PAT2@_N11@Z
// @address 0x48b9f8 ?get_container_current_bounds@c_gui_widget@@UAAPATreal_rectangle2d@@PAT2@@Z
// @address 0x48bd90 ?should_render@c_gui_widget@@UAA_NPA_N@Z
// @address 0x48bdf8 ?within_focus_chain@c_gui_widget@@UAA_NXZ
// @address 0x48baf0 ?handle_tab@c_gui_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x48bb58 ?handle_alt_tab@c_gui_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x48b028 ?set_animated_state_baseline@c_gui_widget@@UAAXPAUs_animation_transform@@@Z
#include <ppcintrinsics.h>
#include <stdlib.h>
#include <math.h>
#include "cseries/crt_xdk.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "interface/interface_constants.h"
#include "interface/user_interface_text.h"
#include "interface/user_interface_text_parser.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_model_widget_types.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "cseries/bit_operations.h"
#include "interface/c_gui_group_widget.h"
#include "interface/c_gui_button_key_widget.h"
#include "interface/gui_widget_types.h"
#include "interface/c_gui_widget.h"
// @address 0x56368 ??_7c_gui_model_widget@@6B@
// @address 0x4a8838 ??_Ec_gui_model_widget@@UAAPAXI@Z
// @address 0x4e9c0 ??_7?$c_gui_sized_text_widget@$0EAA@@@6B@
// @address 0x4ea68 ??_7?$c_gui_sized_text_widget@$0BAA@@@6B@
// @address 0x4eb10 ??_7?$c_gui_sized_text_widget@$0DA@@@6B@
// @address 0x48dd18 ??_G?$c_gui_sized_text_widget@$0DA@@@UAAPAXI@Z
// @address 0x55eb8 ??_7c_gui_group_widget@@6B@
// @address 0x55f40 ??_7c_gui_button_key_widget@@6B@
// @address 0x119a560 ?frame_period@?1??c_gui_widget_verify_animation_period@@YA_NJ@Z@4MB
#include "interface/list_widget_definition.h"
#include "math/function_definitions.h"
#include "text/font_cache_types.h"
#include "text/draw_string_types.h"
#include "interface/text_widget_definition.h"
#include "interface/runtime_text_widget_definition.h"
#include "interface/c_gui_text_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/gui_screen_in_progress.h"
#include <new>
#include "interface/gui_custom_bitmap_widget.h"
#include "interface/user_interface_text_render.h"
#include "math/real_constant_pool_access.h"
#include "math/real_math_constants.h"
#include "interface/user_interface_text_types.h"
#include "interface/user_interface_message_inlines.h"
#include "math/integer_math_templates.h"
#include "interface/user_interface_shared_constants.h"
#include "interface/gui_widget_keyframes.h"
struct s_gui_skin_definition {
    long pad0;
    s_tag_block text_widgets;
    s_tag_block bitmap_widgets;
};

class c_font_cache;

class c_font_cache;
class c_gui_text_widget;
class c_gui_button_key_widget;
class c_controller_input_message;
struct s_group_widget_definition;
struct s_button_key_definition;
struct s_model_widget_block;


template<long k_maximum_count> class c_sized_user_interface_text : public c_user_interface_text {
public:
    c_sized_user_interface_text();
    virtual void set_string(const wchar_t *string, bool parse, long parameter);
    virtual const wchar_t *get_string() { return m_string.get_string(); }
    virtual void update(long user_index);
    c_static_wchar_string<k_maximum_count> m_string;
    c_static_wchar_string<k_maximum_count> m_text;
};

template<long k_maximum_count> class c_gui_sized_text_widget : public c_gui_text_widget {
public:
    virtual unsigned int get_text_buffer_size() const { return k_maximum_count; }
private:
    virtual c_user_interface_text *get_text_internal() { return &m_text; }
    c_sized_user_interface_text<k_maximum_count> m_text;
};



struct s_widget_animation_definition
{
    long flags;
    long pad4;
    s_gui_tag_reference color;
    s_gui_tag_reference position;
    s_gui_tag_reference rotation;
    s_gui_tag_reference scale;
    s_gui_tag_reference rotation_origin;
    s_gui_tag_reference bitmap;
    s_gui_tag_reference display;
};

struct s_keyframe_block_base
{
    long time;
    short anchoring;
};

struct s_widget_component_animation_definition
{
    long flags;
    long keyframe_count;
    TAG_POINTER(const char) keyframes;
    long keyframes_definition;
    c_function_definition function;

    __forceinline const s_keyframe_block_base *get_keyframe(long index, long keyframe_size) const
    {
        return (index >= 0 && index < keyframe_count) ? (const s_keyframe_block_base *)(keyframes + index * keyframe_size) : NULL;
    }
};



inline bool c_gui_widget::should_render(bool *render_with_bounds)
{
    e_gui_widget_type type = m_type;
    bool visible = m_visible;
    if (render_with_bounds)
        *render_with_bounds = type == _gui_text_widget || type == _gui_bitmap_widget || type == _gui_model_widget;
    return visible;
}
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x4e6438 ??0c_gui_bitmap_widget@@QAA@XZ
// @address 0x491eb0 ??0c_gui_custom_bitmap_widget@@QAA@XZ



extern "C" {
 
 
 
}
// @address 0x48e160 ??0s_core_widget_definition@@QAA@XZ
#if defined(_XBOX) && !defined(__clang__)
inline s_core_widget_definition::s_core_widget_definition() : flags(0), name(0), unknown(0), font(0), bounds()
#else
s_core_widget_definition::s_core_widget_definition() : flags(0), name(0), unknown(0), font(0), bounds()
#endif
{
    s_gui_rectangle zero = {0};
    for (long rectangle = 0; rectangle < 2; rectangle++)
        bounds[rectangle] = zero;
    s_gui_tag_reference defaults(0x77616364);
    animation = defaults;
}
__forceinline s_text_widget_definition::s_text_widget_definition() : string_id(0), string_list(0), color_name(0), font(0), pad3a(0) {}
inline s_bitmap_widget_definition::s_bitmap_widget_definition() :
    blend_method(0), initial_sprite_sequence(0), initial_sprite_frame(0), pad52(0), value_override_list(0), value_identifier(0)
{
    bitmap.index = -1;
    shader.index = -1;
}
__forceinline s_text_widget_block::s_text_widget_block()
{
    s_gui_tag_reference defaults('txt3');
    text_widget_template = defaults;
}
// @address 0x48e0a8 ??0s_bitmap_widget_block@@QAA@XZ
#if defined(_XBOX) && !defined(__clang__)
inline s_bitmap_widget_block::s_bitmap_widget_block()
#else
s_bitmap_widget_block::s_bitmap_widget_block()
#endif
{
    s_gui_tag_reference defaults('bmp3');
    bitmap_widget_template = defaults;
}






template<long k_maximum_count> long c_static_wchar_string<k_maximum_count>::length() const
{
    unsigned int length;

    for (length = 0; length < k_maximum_count; length++) {
        if (m_string[length] == 0)
            break;
    }
    return length;
}
template long c_static_wchar_string<256>::length() const;

static __forceinline bool c_gui_widget_robust_values_consistent(void *const *values)
{
    return values[0] == values[1] && values[0] == values[2];
}

static __forceinline void c_gui_widget_robust_pointer_store(c_robust_pointer<c_gui_widget> *pointer, c_gui_widget *value)
{
    do {
        pointer->m_values[0] = value;
        pointer->m_values[1] = value;
        pointer->m_values[2] = value;
    } while (!c_gui_widget_robust_pointer_valid(pointer));
}

// @address 0x482a48 robust_pointer_get_repaired_value
extern "C" c_gui_widget *robust_pointer_get_repaired_value(c_robust_pointer<c_gui_widget> *pointer)
{
    if (!c_gui_widget_robust_values_consistent(pointer->m_values))
        c_gui_widget_robust_pointer_store(pointer, static_cast<c_gui_widget *>(robust_pointer_resolve_majority_value(pointer->m_values)));
    return static_cast<c_gui_widget *>(pointer->m_values[0]);
}

// @address 0x482ad8 ??$c_gui_widget_robust_pointer_valid@Vc_gui_widget@@@@YA_NPAV?$c_robust_pointer@Vc_gui_widget@@@@@Z
template<typename t_type> bool c_gui_widget_robust_pointer_valid(c_robust_pointer<t_type> *pointer)
{
    return c_gui_widget_robust_values_consistent(pointer->m_values);
}
template bool c_gui_widget_robust_pointer_valid<c_gui_widget>(c_robust_pointer<c_gui_widget> *pointer);




// @address 0x483540 ?apply_initial_state@c_saved_film_load_in_progress_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_saved_film_load_in_progress_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    c_gui_screen_in_progress *in_progress_screen = static_cast<c_gui_screen_in_progress *>(screen);

    long title = m_title;
    long msg = m_message;

    in_progress_screen->m_title = title;
    in_progress_screen->m_message = msg;
    if (!m_respond_to_controller_events)
        in_progress_screen->m_responds_to_controller_events = false;
    in_progress_screen->m_show_progress = m_show_progress;
    gui_widget_set_alternate_state(screen, m_alternate_state);
}

static __forceinline void c_gui_widget_offset_real_rectangle2d(real_rectangle2d *rectangle, float dx, float dy)
{
    rectangle->x0 += dx;
    rectangle->y0 += dy;
    rectangle->x1 += dx;
    rectangle->y1 += dy;
}

__forceinline void c_gui_widget::apply_relative_positioning(real_rectangle2d *bounds, const real_rectangle2d *parent_bounds)
{
    if (parent_bounds)
        c_gui_widget_offset_real_rectangle2d(bounds, parent_bounds->x0, parent_bounds->y0);
}


// @address 0x485fb0 ?update@c_gui_screen_widget@@UAAXK@Z
void c_gui_screen_widget::update(unsigned long current_milliseconds)
{
    c_gui_widget *widget = this;
    widget->c_gui_widget::update(current_milliseconds);
    e_controller_index driving_controller = widget->get_driving_controller();
    if (driving_controller != _controller_index_none && driving_controller != k_any_controller &&
        !controller_get(driving_controller)->in_use())
        static_cast<c_gui_screen_widget *>(widget)->transition_out_with_transition_type(0, _screen_transition_normal);
}

// @address 0x489570 ??0c_gui_widget@@QAA@W4e_gui_widget_type@@@Z
c_gui_widget::c_gui_widget(e_gui_widget_type type) :
    m_type(type), m_visible(true), m_enabled(true), m_animation_looping(false), m_unknown(false),
    m_parent(0), m_children(0), m_next(0), m_previous(0),
    m_controller_mask(0), m_last_animated_milliseconds(0)
{
}

// 0x489610
void c_gui_widget::initialize()
{
    m_controller_mask |= 16;

    s_runtime_core_widget_definition *definition = get_core_definition();
    real_rectangle2d bounds;
    get_current_bounds(&bounds);
    scale_and_position_rectangle_for_display_resolution(m_type, &bounds, (e_widget_anchoring)definition->unknown);
    definition->current_bounds = bounds;
}

// @address 0x4896b0 ?post_initialize@c_gui_widget@@UAAXXZ
void c_gui_widget::post_initialize()
{
    for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget)
            child->post_initialize();
    }
    m_controller_mask &= ~(1ui64 << 4);
}


// @address 0x489738 ?dispose@c_gui_widget@@UAAXXZ
void c_gui_widget::dispose()
{
    c_gui_screen_widget *screen = m_type == _gui_screen_widget ? (c_gui_screen_widget *)this : get_parent_screen();
    if (screen != 0) {
    c_gui_widget *focused = screen->m_suppress_focus ? 0 : screen->m_focused_widget;
        if (focused == this) {
            for (c_gui_widget *ancestor = get_parent(); ancestor != 0; ancestor = ancestor->get_parent()) {
                if (ancestor->can_receive_focus()) {
                    screen->transfer_focus_without_animations(ancestor, true, true);
                    break;
                }
            }
        }
    }
    for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget)
            child->dispose();
    }
}

// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z
void c_gui_widget::set_visible(bool visible)
{
    m_visible = visible;
    for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget)
            child->set_visible(visible);
    }
}

// @address 0x4898b8 ?get_enabled@c_gui_widget@@UBA_NXZ
bool c_gui_widget::get_enabled() const
{
    return m_enabled;
}

// March c_gui_widget.obj 0x8369c598 (/Od returns constant 0): the base string lookup never resolves.
// @address 0x42d298 ?get_string_by_string_id@c_gui_widget@@UAA_NJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_widget::get_string_by_string_id(long string_identifier, c_static_wchar_string<1024> *buffer)
{
    return false;
}

// @address 0x4898c0 ?set_enabled@c_gui_widget@@UAAX_N@Z
void c_gui_widget::set_enabled(bool enabled)
{
    m_enabled = enabled;
    for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget)
            child->set_enabled(enabled);
    }
}

// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
e_controller_index c_gui_widget::get_any_responding_controller() const
{
    long controller_mask = get_controller_mask();
    if ((controller_mask & 1) && controller_get(_controller0)->in_use())
        return _controller0;
    if ((controller_mask & 2) && controller_get(_controller1)->in_use())
        return _controller1;
    if ((controller_mask & 4) && controller_get(_controller2)->in_use())
        return _controller2;
    if ((controller_mask & 8) && controller_get(_controller3)->in_use())
        return _controller3;
    if (controller_mask & 1)
        return _controller0;
    if (controller_mask & 2)
        return _controller1;
    if (controller_mask & 4)
        return _controller2;
    if (controller_mask & 8)
        return _controller3;
    return _controller0;
}


// 0x489b58
long c_gui_widget::get_element_handle()
{
    c_gui_widget *list_item = gui_widget_get_parent_list_item_widget(this);
    if (list_item != 0)
        return list_item->get_element_handle();
    return -1;
}

// 0x489ba8
long c_gui_widget::get_datasource_index()
{
    c_gui_widget *list = gui_widget_get_parent_list_widget(this);
    if (list != 0)
        return list->get_datasource_index();
    return -1;
}

// 0x489bf8
static __forceinline unsigned long c_gui_widget_count_driving_bits(long value)
{
    unsigned long bits = value;
    bits = ((bits >> 1) & 0x55555555) + (bits & 0x55555555);
    bits = ((bits >> 2) & 0x33333333) + (bits & 0x33333333);
    bits = ((bits >> 4) & 0x0f0f0f0f) + (bits & 0x0f0f0f0f);
    bits = ((bits >> 8) & 0x00ff00ff) + (bits & 0x00ff00ff);
    bits = (bits >> 16) + (bits & 0xffff);
    return bits;
}

// @address 0x489a40 ?get_driving_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
e_controller_index c_gui_widget::get_driving_controller() const
{
    e_controller_index result = _controller_index_none;
    long controller_count = c_gui_widget_count_driving_bits(get_controller_mask());
    if (controller_count == 1)
        result = get_any_responding_controller();
    else if (controller_count == 4)
        result = k_any_controller;
    return result;
}

// @address 0x489af0 ?controller_can_drive@c_gui_widget@@QAA_NW4e_controller_index@@@Z
bool c_gui_widget::controller_can_drive(e_controller_index controller_index)
{
    return controller_index == 255 || get_driving_controller() == 255 || get_driving_controller() == controller_index;
}

void c_gui_widget::add_definition_fields(e_gui_widget_type widget_type, const s_core_widget_definition *source_definition,
    s_runtime_core_widget_definition *dest_definition, const real_rectangle2d *parent_bounds, bool require_nonzero_bounds)
{
    e_authored_ui_aspect_ratio aspect_ratio = interface_get_closest_authored_ui_aspect_ratio();

    dest_definition->flags = (source_definition->flags & 2) ? source_definition->flags : dest_definition->flags | source_definition->flags;
    if (source_definition->name != 0 && source_definition->name != -1)
        dest_definition->name = source_definition->name;
    if (source_definition->unknown >= 0 && source_definition->unknown < 10 && source_definition->unknown != 0)
        dest_definition->unknown = source_definition->unknown;
    if (source_definition->font != 0)
        dest_definition->font = source_definition->font;

    bool relative_positioning = widget_type != _gui_screen_widget;
    s_gui_rectangle zero_rectangle = {0};
    real_rectangle2d zero_bounds = {0};
    real_rectangle2d bounds = {0};
    if (memcmp(&zero_rectangle, &source_definition->bounds[aspect_ratio], sizeof(s_gui_rectangle)) != 0)
    {
        bounds.x0 = (float)source_definition->bounds[aspect_ratio].values[1];
        bounds.x1 = (float)source_definition->bounds[aspect_ratio].values[3];
        bounds.y0 = (float)source_definition->bounds[aspect_ratio].values[0];
        bounds.y1 = (float)source_definition->bounds[aspect_ratio].values[2];
    }
    else
    {
        real_vector2d scaling = interface_get_authoring_aspect_conversion_scaling(aspect_ratio);
        bounds.x0 = (float)source_definition->bounds[0].values[1];
        bounds.x1 = (float)source_definition->bounds[0].values[3];
        bounds.y0 = (float)source_definition->bounds[0].values[0];
        bounds.y1 = (float)source_definition->bounds[0].values[2];
        bounds.x0 *= scaling.i;
        bounds.x1 *= scaling.i;
        bounds.y0 *= scaling.j;
        bounds.y1 *= scaling.j;
    }

    if (!require_nonzero_bounds || memcmp(&zero_bounds, &bounds, sizeof(real_rectangle2d)) != 0)
    {
        if (relative_positioning)
            apply_relative_positioning(&bounds, parent_bounds);
        dest_definition->current_bounds = bounds;
        dest_definition->authored_bounds = bounds;
    }

    if (source_definition->animation.index != -1 && source_definition->animation.index != 0)
        dest_definition->animation_definition_index = source_definition->animation.index;
}

// @address 0x489f08 ?create_and_add_children_from_skin_definition@c_gui_widget@@QAAXPBUs_gui_skin_definition@@JF@Z
void c_gui_widget::create_and_add_children_from_skin_definition(const s_gui_skin_definition *skin_definition, long animation_index, short font_index)
{
    for (long text_widget_index = 0; text_widget_index < skin_definition->text_widgets.count; text_widget_index++)
    {
        const s_text_widget_block *text_widget_block = &((const s_text_widget_block *)skin_definition->text_widgets.address)[text_widget_index];
        s_text_widget_block text_widget_block_copy;
        if ((text_widget_block->definition.core.flags & 0x20000) && (animation_index != -1 || font_index != 0))
        {
            memcpy(&text_widget_block_copy, text_widget_block, sizeof(s_text_widget_block));
            text_widget_block = &text_widget_block_copy;
            if (animation_index != -1)
                text_widget_block_copy.definition.core.animation.index = animation_index;
            if (font_index != 0)
                text_widget_block_copy.definition.core.font = font_index;
        }
        create_and_add_child_text_widget(text_widget_block);
    }

    for (long bitmap_widget_index = 0; bitmap_widget_index < skin_definition->bitmap_widgets.count; bitmap_widget_index++)
    {
        const s_bitmap_widget_block *bitmap_widget_block = &((const s_bitmap_widget_block *)skin_definition->bitmap_widgets.address)[bitmap_widget_index];
        s_bitmap_widget_block bitmap_widget_block_copy;
        if ((bitmap_widget_block->definition.core.flags & 0x200) && (animation_index != -1 || font_index != 0))
        {
            memcpy(&bitmap_widget_block_copy, bitmap_widget_block, sizeof(s_bitmap_widget_block));
            bitmap_widget_block = &bitmap_widget_block_copy;
            if (animation_index != -1)
                bitmap_widget_block_copy.definition.core.animation.index = animation_index;
            if (font_index != 0)
                bitmap_widget_block_copy.definition.core.font = font_index;
        }
        create_and_add_child_bitmap_widget(bitmap_widget_block);
    }
}

// 0x48a150
c_gui_text_widget *c_gui_widget::create_and_add_child_text_widget(const s_text_widget_block *text_widget_block)
{
    s_runtime_text_widget_definition text_widget_definition;
    real_rectangle2d parent_bounds;
    get_parent()->get_authored_bounds(&parent_bounds);

    bool was_templated = false;
    if (text_widget_block->text_widget_template.index != -1)
    {
        const s_text_widget_definition *template_definition = (const s_text_widget_definition *)tag_get(
            'txt3', text_widget_block->text_widget_template.index);
        c_gui_text_widget::add_definition_fields(template_definition, &text_widget_definition, &parent_bounds, was_templated);
        was_templated = true;
    }
    c_gui_text_widget::add_definition_fields(&text_widget_block->definition, &text_widget_definition, &parent_bounds, was_templated);

    c_gui_text_widget *text_widget = create_text_widget(&text_widget_definition);
    if (text_widget)
    {
        gui_widget_append_child(this, text_widget);
        text_widget->initialize(text_widget_block);
    }
    return text_widget;
}





// @address 0x4ae6d8 ?create_bitmap_widget@c_gui_start_menu_hq_service_record@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_start_menu_hq_service_record::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _string_id_flavor_image)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_custom_bitmap_widget), 0, 0);
        if (memory != 0)
            return new(memory) c_gui_custom_bitmap_widget();
    }
    else
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_bitmap_widget), 0, 0);
        if (memory != 0)
            return new(memory) c_gui_bitmap_widget();
    }
    return 0;
}

// @address 0x48ade8 ?create_bitmap_widget@c_gui_widget@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_widget::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_bitmap_widget), 0, 0);
    return memory ? new(memory) c_gui_bitmap_widget : 0;
}



// @address 0x48ae38 ?create_group_widget@c_gui_widget@@UAAPAVc_gui_group_widget@@PBUs_group_widget_definition@@@Z
c_gui_group_widget *c_gui_widget::create_group_widget(const s_group_widget_definition *definition)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_group_widget), 0, 0);
    if (memory)
    {
        return new(memory) c_gui_group_widget;
    }
    return 0;
}



// @address 0x48aeb8 ?create_button_key_widget@c_gui_widget@@UAAPAVc_gui_button_key_widget@@PBUs_button_key_definition@@@Z
c_gui_button_key_widget *c_gui_widget::create_button_key_widget(const s_button_key_definition *definition)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_button_key_widget), 0, 0);
    if (memory)
    {
        return new(memory) c_gui_button_key_widget;
    }
    return 0;
}

// @address 0x48af38 ?create_list_widget@c_gui_widget@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
c_gui_list_widget *c_gui_widget::create_list_widget(const s_list_widget_block *definition)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_list_widget), 0, 0);
    return memory ? new(memory) c_gui_list_widget : 0;
}

// @address 0x48af88 ?create_list_item_widget@c_gui_widget@@UAAPAVc_gui_list_item_widget@@PBUs_list_item_widget_block@@@Z
c_gui_list_item_widget *c_gui_widget::create_list_item_widget(const s_list_item_widget_block *definition)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_list_item_widget), 0, 0);
    return memory ? new(memory) c_gui_list_item_widget : 0;
}


// @address 0x48afd8 ?create_model_widget@c_gui_widget@@UAAPAVc_gui_model_widget@@PBUs_model_widget_block@@@Z
c_gui_model_widget *c_gui_widget::create_model_widget(const s_model_widget_block *definition)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_model_widget), 0, 0);
    return memory ? new(memory) c_gui_model_widget : 0;
}

// 0x48a288
c_gui_bitmap_widget *c_gui_widget::create_and_add_child_bitmap_widget(const s_bitmap_widget_block *bitmap_widget_block)
{
    s_runtime_bitmap_widget_definition bitmap_widget_definition;
    real_rectangle2d parent_bounds;
    get_parent()->get_authored_bounds(&parent_bounds);

    bool was_templated = false;
    if (bitmap_widget_block->bitmap_widget_template.index != -1)
    {
        const s_bitmap_widget_definition *template_definition = (const s_bitmap_widget_definition *)tag_get(
            'bmp3', bitmap_widget_block->bitmap_widget_template.index);
        c_gui_bitmap_widget::add_definition_fields(template_definition, &bitmap_widget_definition, &parent_bounds, was_templated);
        was_templated = true;
    }
    c_gui_bitmap_widget::add_definition_fields(&bitmap_widget_block->definition, &bitmap_widget_definition, &parent_bounds, was_templated);

    c_gui_bitmap_widget *bitmap_widget = create_bitmap_widget(&bitmap_widget_definition);
    if (bitmap_widget)
    {
        gui_widget_append_child(this, bitmap_widget);
        bitmap_widget->initialize(bitmap_widget_block);
    }
    return bitmap_widget;
}

// @address 0x48a3b0 gui_widget_create_and_add_child_list_widget
extern "C" c_gui_widget *gui_widget_create_and_add_child_list_widget(c_gui_widget *parent, const void *source)
{
    c_gui_widget *child = parent->create_list_widget((const s_list_widget_block *)source);
    if (child) {
        gui_widget_append_child(parent, child);
        ((c_gui_bitmap_widget *)child)->initialize((const s_bitmap_widget_block *)source);
    }
    return child;
}

// @address 0x48a410 ?create_and_add_child_list_item_widgets@c_gui_widget@@QAAXPBUs_tag_block@@J@Z
void c_gui_widget::create_and_add_child_list_item_widgets(const s_tag_block *list_items_block, long gui_skin_tag_index)
{
    if (gui_skin_tag_index != -1)
    {
        const s_gui_skin_definition *gui_skin = (const s_gui_skin_definition *)tag_get('skn3', gui_skin_tag_index);
        for (long list_item_index = 0; list_item_index < list_items_block->count; list_item_index++)
        {
            const s_list_item_widget_block *list_item_block = &((const s_list_item_widget_block *)list_items_block->address)[list_item_index];
            c_gui_screen_widget *parent_screen = get_parent_screen();
            c_gui_list_item_widget *list_item_widget = parent_screen ?
                parent_screen->create_list_item_widget(list_item_block) :
                create_list_item_widget(list_item_block);
            if (!list_item_widget)
                break;

            gui_widget_append_child(this, list_item_widget);
            list_item_widget->m_list_item_index = list_item_index;
            list_item_widget->initialize(list_item_block);
            list_item_widget->create_and_add_children_from_skin_definition(gui_skin,
                list_item_widget->m_definition.animation_definition_index,
                (short)list_item_widget->get_core_definition()->font);
        }
    }
}

// @address 0x48a510 ?update@c_gui_widget@@UAAXK@Z
void c_gui_widget::update(unsigned long current_milliseconds)
{
    c_gui_widget *child = get_children();
    while (child != 0) {
        c_gui_widget *next = child->get_next();
        if (child != 0 && child->m_type != _gui_screen_widget)
            child->update(current_milliseconds);
        child = next;
    }
}

// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
void c_gui_widget::update_render_state(unsigned long current_milliseconds)
{
    if (m_visible) {
        for (c_gui_widget *child = get_children(); child; child = child->get_next()) {
            if (child->m_type != _gui_screen_widget)
                child->update_render_state(current_milliseconds);
        }
    }
}

// @address 0x48a608 ?update_animation@c_gui_widget@@QAAXK@Z
void c_gui_widget::update_animation(unsigned long current_milliseconds)
{
    for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget)
            child->update_animation(current_milliseconds);
    }
    animate(current_milliseconds);
    if (can_be_disposed() && can_all_children_be_disposed()) {
        if (m_type == _gui_group_widget || m_type == _gui_button_key_widget) {
            c_gui_group_widget *group = static_cast<c_gui_group_widget *>(this);
            group->m_dispose_as_display_group = true;
        } else if (m_type == _gui_list_widget) {
            c_gui_list_widget *list = (c_gui_list_widget *)this;
            if (list->m_is_submenu)
                list->m_is_submenu_that_needs_disposal = true;
        }
    }
}


// 0x48a6d8 ?get_ambient_state@c_gui_widget@@MAA?AW4e_animation_state@@XZ
e_animation_state c_gui_widget::get_ambient_state()
{
    c_gui_list_widget *list = gui_widget_get_parent_list_widget(this);
    if (list != 0 && list->m_submenu_owner_list_item != 0) {
        int state = (within_focus_chain() == 0) + 28;
        if (state != -1)
            return (e_animation_state)state;
    }

    bool selected = within_focus_chain();
    if (!selected && m_type != _gui_list_widget && list != 0 && (bool)(_rotl(list->m_definition.flags, 26) & 1)) {
        c_gui_widget *list_item = m_type == _gui_list_item_widget ? this : (c_gui_widget *)gui_widget_get_parent_list_item_widget(this);
        if (list_item != 0) {
            long focused = list->m_focused_item_index;
            long scroll = list->m_scroll_position;
            long item_index = ((c_gui_list_item_widget *)list_item)->get_list_item_index();
            selected = focused - scroll == item_index;
        }
    }

    if (get_enabled()) {
        if (m_unknown != 0)
            return (e_animation_state)((selected == 0) + 4);
        return (e_animation_state)(selected == 0);
    }
    return (e_animation_state)((selected == 0) + 2);
}

// 0x48a820
void c_gui_widget::animate(unsigned long current_milliseconds)
{
    bool ambient_animated = false;
    bool focus_animated = false;
    const __int64 ambient_states = 0x3c0000ffi64 << 6;
    const __int64 focus_states = 0x33fcfi64 << 14;
    const unsigned __int64 transient_states = 0x300c003f;

    calculate_authoring_to_target_dimensions_scaling();
    real_vector2d aspect_ratio_scale = g_interface_authoring_to_target_dimensions_scaling;

    m_animated_state.animation_state_flags &= ~transient_states;
    m_last_animated_milliseconds = current_milliseconds;
    set_animated_state_baseline(&m_animated_state);

    unsigned __int64 animation_state_flags = m_animated_state.animation_state_flags;
    if (animation_state_flags & ambient_states)
    {
        for (long state_index = 0; state_index < 36; state_index++)
        {
            __int64 state_bit = 1i64 << state_index;
            if ((state_bit & ambient_states) && (state_bit & animation_state_flags))
            {
                e_animation_state animation_state = (e_animation_state)state_index;
                unsigned long start_time = get_animation_state_timer(animation_state);
                const s_widget_animation_definition *animation_definition = get_widget_animation_definition(animation_state);
                bool finished = false;
                gui_widget_apply_animation_state(this, animation_state, start_time, current_milliseconds, animation_definition,
                    &m_animated_state, &aspect_ratio_scale, false, false, &finished);
                ambient_animated = true;
                if (finished)
                {
                    switch (state_index)
                    {
                    case _animation_state_transition_from_screen:
                    case _animation_state_transition_back_from_screen:
                    case _animation_state_list_display_group_transition_in:
                    case _animation_state_list_display_group_transition_out:
                    case _animation_state_custom_transition_out0:
                    case _animation_state_custom_transition_out1:
                        finished = false;
                        m_animation_looping = true;
                        break;
                    }
                    if (!finished)
                        m_animated_state.animation_state_flags |= 1ui64 << state_index;
                    else
                        m_animated_state.animation_state_flags &= ~(1ui64 << state_index);
                }
                break;
            }
        }
    }
    else
    {
        c_gui_screen_widget *parent_screen = get_parent_screen();
        if (parent_screen && parent_screen != this && (parent_screen->m_name == _string_id_scoreboard || parent_screen->m_name == _string_id_scoreboard_half))
            m_animation_looping = parent_screen->m_animation_looping;
    }

    s_animation_transform saved_state = m_animated_state;
    if (m_animated_state.animation_state_flags & focus_states)
    {
        for (long state_index = 0; state_index < 36; state_index++)
        {
            __int64 state_bit = 1i64 << state_index;
            if ((state_bit & focus_states) && (state_bit & m_animated_state.animation_state_flags))
            {
                e_animation_state animation_state = (e_animation_state)state_index;
                unsigned long start_time = get_animation_state_timer(animation_state);
                const s_widget_animation_definition *animation_definition = get_widget_animation_definition(animation_state);
                bool finished = false;
                gui_widget_apply_animation_state(this, animation_state, start_time, current_milliseconds, animation_definition,
                    &m_animated_state, &aspect_ratio_scale, false, ambient_animated, &finished);
                focus_animated = true;
                bool done = finished;
                if (done)
                {
                    if (state_index == 15 || state_index == 27)
                    {
                        done = false;
                        m_animation_looping = true;
                    }
                    if (!done)
                        m_animated_state.animation_state_flags |= state_bit;
                    else
                        m_animated_state.animation_state_flags &= ~state_bit;
                    if (!done)
                        saved_state.animation_state_flags |= 1i64 << state_index;
                    else
                        saved_state.animation_state_flags &= ~state_bit;
                }
                saved_state.start_time = 0;
                m_animated_state.start_time = 0;
            }
        }
    }

    if (!(m_animated_state.animation_state_flags & focus_states))
    {
        unsigned long start_time = m_animated_state.start_time;
        e_animation_state ambient_state = get_ambient_state();
        const s_widget_animation_definition *animation_definition = get_widget_animation_definition(ambient_state);
        bool finished = false;
        if (focus_animated)
            m_animated_state = saved_state;
        if (!m_animated_state.start_time)
            m_animated_state.start_time = current_milliseconds;
        gui_widget_apply_animation_state(this, ambient_state, start_time, current_milliseconds, animation_definition, &m_animated_state,
            &aspect_ratio_scale, false, ambient_animated, &finished);
        m_animated_state.animation_state_flags |= 1ui64 << ambient_state;
    }
}

// @address 0x48ac58 ?animate_recursively@c_gui_widget@@IAAXK@Z
void c_gui_widget::animate_recursively(unsigned long current_milliseconds)
{
    animate(current_milliseconds);
    for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget)
            child->animate_recursively(current_milliseconds);
    }
}



static __forceinline float c_gui_widget_get_rendered_depth(c_gui_widget *widget)
{
    return widget->get_animated_state()->position.z;
}

// @address 0x48b420 ?assemble_render_data@c_gui_widget@@UAAXPAUs_gui_widget_render_data@@PBTrectangle2d@@W4e_controller_index@@_N33@Z
void c_gui_widget::assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
    e_controller_index local_controller_index, bool a, bool b, bool c)
{
    s_gui_widget_render_data *layout = (s_gui_widget_render_data *)render_data;
    real_rectangle2d current_bounds;
    layout->type = m_type;
    get_current_bounds(&current_bounds);
    get_unprojected_bounds((gui_real_rectangle2d *)layout->corners, a, b, c);
    real_point2d *corner = layout->corners;
    unsigned int remaining = 4;
    do {
        real_point3d point = { corner->x, corner->y, c_gui_widget_get_rendered_depth(this) };
        // match: 0x48b420: pointer __restrict qualifier prevents alias reload
        real_point2d *__restrict out = corner;
        gui_real_rectangle2d::transform_real_point3d(&point, window_bounds);
        out->x = point.x;
        out->y = point.y;
        corner++;
    } while (--remaining != 0);
    layout->local_controller_index = local_controller_index;
    layout->flags = 0;

    bool screen_flag = false;
    c_gui_screen_widget *screen = get_parent_screen();
    while (screen) {
        c_gui_screen_widget *parent = screen->get_parent_screen();
        if (parent == screen) {
            screen_flag = screen->m_render_in_screenshot;
            break;
        }
        screen = parent;
    }
    if (screen_flag)
        layout->flags |= 32;
    else
        layout->flags &= ~32;
}




// @address 0x48b540 gui_widget_render_data
extern "C" void gui_widget_render_data(long user_index, const s_gui_widget_render_data *render_data, const rectangle2d *window_bounds, bool is_screenshot)
{
    bool render = true;
    bool render_in_screenshot = ((render_data->flags >> 5) & 1) != 0;
    if (is_screenshot && !render_in_screenshot)
        render = false;

    if (render) {
        switch (render_data->type) {
        case _gui_text_widget:
            user_interface_text_render((const s_user_interface_text_render_data *)render_data, window_bounds);
            break;
        case _gui_bitmap_widget:
            c_gui_bitmap_widget_render(static_cast<const s_gui_bitmap_widget_render_data *>(render_data), window_bounds);
            break;
        case _gui_model_widget: {
            const s_gui_model_widget_render_data *model_render_data = static_cast<const s_gui_model_widget_render_data *>(render_data);
            s_gui_bitmap_widget_render_data bitmap_render_data;
            bitmap_render_data.type = _gui_bitmap_widget;
            bitmap_render_data.projected_bounds = model_render_data->projected_bounds;
            bitmap_render_data.local_controller_index = model_render_data->local_controller_index;
            bitmap_render_data.tint = model_render_data->tint;
            bitmap_render_data.flags = 2;
            bitmap_render_data.texture_offset.y = 0.0f;
            bitmap_render_data.texture_offset.x = 0.0f;
            bitmap_render_data.blend_method = 3;
            bitmap_render_data.shader_definition_index = -1;
            c_gui_bitmap_widget_render(&bitmap_render_data, window_bounds);
            break;
        }
        }
    }
}

// @address 0x48b098 ?start_animation_at_time@c_gui_widget@@QAAXW4e_animation_state@@K_N@Z
void c_gui_widget::start_animation_at_time(e_animation_state animation_state, unsigned long milliseconds, bool recursive)
{
    struct s_time_field {
        unsigned __int64 mask;
        unsigned __int64 cleared;
        long *time;
    };

    unsigned long time;
    if (milliseconds == -1) {
        time = g_user_interface_globals.milliseconds.peek();
    } else {
        time = milliseconds;
    }
    unsigned __int64 state_mask = 1ui64 << animation_state;
    unsigned __int64 flags = m_animated_state.animation_state_flags;
    unsigned __int64 screen_mask = 0x3c0000ffui64 << 6;
    s_time_field fields[4] = {
        { 0x3c0000ffui64 << 6, 0x3c0000ffui64 << 6, &get_animated_state()->screen_transition_start_time },
        { 0x33f03ui64 << 14, 0x33f03ui64 << 14, &get_animated_state()->previous_control_start_time },
        { 0x30000, 0x30000, &get_animated_state()->next_control_start_time },
        { 0x300000, 0x300000, &get_animated_state()->indicator_start_time },
    };
    for (unsigned int i = 0; i < 4; i++) {
        if ((state_mask & fields[i].mask) != 0) {
            flags &= ~fields[i].cleared;
            if (fields[i].mask == screen_mask && m_type != _gui_screen_widget && get_parent_screen())
                *fields[i].time = get_parent_screen()->get_animated_state()->screen_transition_start_time;
            else
                *fields[i].time = time;
        }
    }
    m_animated_state.animation_state_flags = flags | state_mask;
    if (recursive) {
        for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
            if (child->m_type != _gui_screen_widget)
                child->start_animation_at_time(animation_state, milliseconds, recursive);
        }
    }
    animate(m_last_animated_milliseconds);
}

// @address 0x48b228 ?set_full_animation_state@c_gui_widget@@QAAXPBUs_animation_transform@@_N@Z
void c_gui_widget::set_full_animation_state(const s_animation_transform *state, bool recursive)
{
    m_animated_state = *state;
    if (recursive) {
        for (c_gui_widget *child = get_children(); child != 0; child = child->get_next())
            child->set_full_animation_state(state, recursive);
    }
}

// @address 0x48b298 ?can_be_disposed@c_gui_widget@@QAA?B_NXZ
const bool c_gui_widget::can_be_disposed()
{
    bool result = m_animation_looping;
    if (result) {
        for (c_gui_widget *child = get_children(); child != 0; child = child->get_next())
            result = child->can_be_disposed() & result;
    }
    return result;
}

// @address 0x48b310 ?can_all_children_be_disposed@c_gui_widget@@QAA?B_NXZ
const bool c_gui_widget::can_all_children_be_disposed()
{
    bool result = true;
    for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget)
            result = child->can_be_disposed() & result;
    }
    return result;
}

const s_widget_animation_definition *c_gui_widget::get_widget_animation_definition(e_animation_state state)
{
    const s_widget_animation_definition *result = 0;
    const s_runtime_core_widget_definition *definition = get_core_definition();
    if (definition->animation_definition_index != -1)
    {
        const s_tag_reference *animations = (const s_tag_reference *)tag_get(0x77616364, definition->animation_definition_index);
        long animation_index = animations[state].index;
        if (animation_index != -1)
            result = (const s_widget_animation_definition *)tag_get(0x7767616e, animation_index);
    }
    return result;
}

// @address 0x48b648 ?get_parent_screen@c_gui_widget@@QAAPAVc_gui_screen_widget@@XZ
c_gui_screen_widget *c_gui_widget::get_parent_screen()
{
    c_gui_widget *widget = get_parent();
    while (widget != 0 && widget->m_type != _gui_screen_widget)
        widget = widget->get_parent();
    if (widget != 0)
        return widget->m_type == _gui_screen_widget ? (c_gui_screen_widget *)widget : 0;
    return m_type == _gui_screen_widget ? (c_gui_screen_widget *)this : (c_gui_screen_widget *)widget;
}

// @address 0x48b6d0 gui_widget_get_parent_group_widget
extern "C" c_gui_widget *gui_widget_get_parent_group_widget(c_gui_widget *widget)
{
    c_gui_widget *ancestor;
    for (ancestor = widget->get_parent(); ancestor; ancestor = ancestor->get_parent()) {
        if (ancestor->m_type == 3)
            return ancestor;
    }
    if (widget->m_type == 3)
        ancestor = widget;
    return ancestor;
}

// @address 0x48b738 gui_widget_get_parent_list_widget
extern "C" c_gui_list_widget *gui_widget_get_parent_list_widget(c_gui_widget *widget)
{
    c_gui_widget *ancestor;

    for (ancestor = robust_pointer_get_repaired_value(&widget->m_parent); ancestor; ancestor = robust_pointer_get_repaired_value(&ancestor->m_parent)) {
        if (ancestor->m_type == 7)
            return (c_gui_list_widget *)ancestor;
    }
    if (widget->m_type == 7)
        ancestor = widget;
    return (c_gui_list_widget *)ancestor;
}

// @address 0x48b7a0 gui_widget_get_parent_list_item_widget
extern "C" c_gui_list_item_widget *gui_widget_get_parent_list_item_widget(c_gui_widget *widget)
{
    c_gui_widget *ancestor;

    for (ancestor = robust_pointer_get_repaired_value(&widget->m_parent); ancestor; ancestor = robust_pointer_get_repaired_value(&ancestor->m_parent)) {
        if (ancestor->m_type == 5)
            return (c_gui_list_item_widget *)ancestor;
    }
    if (widget->m_type == 5)
        ancestor = widget;
    return (c_gui_list_item_widget *)ancestor;
}

real_rectangle2d *c_gui_widget::get_authored_bounds(real_rectangle2d *out) {
    s_runtime_core_widget_definition *source = get_core_definition();
    *out = source->authored_bounds;
    return out;
}

// @address 0x48b870 ?get_current_bounds@c_gui_widget@@UAAPATreal_rectangle2d@@PAT2@@Z
real_rectangle2d *c_gui_widget::get_current_bounds(real_rectangle2d *out) {
    s_runtime_core_widget_definition *source = get_core_definition();
    *out = source->current_bounds;
    return out;
}

// @address 0x48b8d8 ?get_unprojected_bounds@c_gui_widget@@QAAPATgui_real_rectangle2d@@PAT2@_N11@Z
gui_real_rectangle2d *c_gui_widget::get_unprojected_bounds(gui_real_rectangle2d *bounds, bool apply_translation, bool apply_scale, bool apply_rotation)
{
    s_animation_transform *transform = get_animated_state();
    real_rectangle2d current;
    get_current_bounds(&current);
    bounds->points[0].x = current.x0;
    bounds->points[0].y = current.y0;
    bounds->points[1].x = current.x0;
    bounds->points[1].y = current.y1;
    bounds->points[2].x = current.x1;
    bounds->points[2].y = current.y0;
    bounds->points[3].x = current.x1;
    bounds->points[3].y = current.y1;
    if (apply_scale)
        (bounds)
            ->scale_about_local_point(&transform->rotation_origin, &transform->scale);
    if (apply_rotation)
        (bounds)
            ->rotate_about_local_point(&transform->rotation_pivot, transform->rotation_sine, transform->rotation_cosine);
    if (apply_translation) {
        float dy = transform->position.y;
        float dx = transform->position.x;
        for (long i = 0; i < 4; i++) {
            bounds->points[i].x += dx;
            bounds->points[i].y += dy;
        }
    }
    return bounds;
}

// @address 0x48b9f8 ?get_container_current_bounds@c_gui_widget@@UAAPATreal_rectangle2d@@PAT2@@Z
real_rectangle2d *c_gui_widget::get_container_current_bounds(real_rectangle2d *bounds)
{
    get_current_bounds(bounds);
    for (c_gui_widget *child = get_children(); child != 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget) {
            real_rectangle2d child_bounds;
            child->get_container_current_bounds(&child_bounds);
            bounds->x0 = bounds->x0 <= child_bounds.x0 ? bounds->x0 : child_bounds.x0;
            bounds->y0 = bounds->y0 <= child_bounds.y0 ? bounds->y0 : child_bounds.y0;
            bounds->x1 = bounds->x1 > child_bounds.x1 ? bounds->x1 : child_bounds.x1;
            bounds->y1 = bounds->y1 > child_bounds.y1 ? bounds->y1 : child_bounds.y1;
        }
    }
    return bounds;
}

// @address 0x48baf0 ?handle_tab@c_gui_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_widget::handle_tab(const c_controller_input_message *message)
{
    if (get_parent() != 0)
        return get_parent()->handle_tab(message);
    return 0;
}

// @address 0x48bb58 ?handle_alt_tab@c_gui_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_widget::handle_alt_tab(const c_controller_input_message *message)
{
    if (get_parent() != 0)
        return get_parent()->handle_alt_tab(message);
    return 0;
}

// @address 0x48bbc0 gui_widget_append_child
extern "C" void gui_widget_append_child(c_gui_widget *parent, c_gui_widget *child)
{
    if (parent->get_children()) {
        c_gui_widget *last = parent->get_children();
        while (last->get_next())
            last = last->get_next();
        last->set_next(child);
        child->set_previous(last);
    } else {
        parent->set_children(child);
    }
    child->set_parent(parent);
}

// match: 0x48bc60: optimize("s") keeps the 0x70 manual-save frame (default emits __savegprlr + 0x80 frame)
#pragma optimize("s", on)
// @address 0x48bc60 gui_widget_remove_child
extern "C" void gui_widget_remove_child(c_gui_widget *parent, c_gui_widget *child)
{
    c_gui_widget *previous = child->get_previous();
    c_gui_widget *next = child->get_next();
    if (previous != 0)
        previous->set_next(next);
    if (next != 0)
        next->set_previous(previous);
    if (parent->get_children() == child)
        parent->set_children(next);
    child->set_parent(0);
}
// match: 0x48bc60: restores default optimization after the size-tuned function above
#pragma optimize("t", on)

// match: 0x48bce8: optimize("s") keeps the 0x70 manual-save frame (default emits __savegprlr + 0x80 frame)
#pragma optimize("s", on)
// @address 0x48bce8 ?delete_all_children@c_gui_widget@@QAAXXZ
void c_gui_widget::delete_all_children()
{
    c_gui_widget *child = get_children();
    set_children(0);
    while (child != 0) {
        c_gui_widget *next = child->get_next();
        child->set_parent(0);
        child->dispose();
        child->~c_gui_widget();
        user_interface_free(child);
        child = next;
    }
}
// match: 0x48bce8: restores default optimization after the size-tuned function above
#pragma optimize("t", on)

// @address 0x48bdc8 ?get_cumulative_color_tint@c_gui_widget@@UAAPATreal_argb_color@@PAT2@@Z
real_argb_color *c_gui_widget::get_cumulative_color_tint(real_argb_color *tint)
{
    *tint = m_animated_state.color;
    return tint;
}



// @address 0x48bdf8 ?within_focus_chain@c_gui_widget@@UAA_NXZ
bool c_gui_widget::within_focus_chain()
{
    bool result = false;
    c_gui_widget *screen_widget = this;
    if (m_type != _gui_screen_widget)
        screen_widget = get_parent_screen();
    if (screen_widget != 0 && !((c_gui_screen_widget *)screen_widget)->m_suppress_focus) {
        c_gui_widget *focused = ((c_gui_screen_widget *)screen_widget)->m_focused_widget;
        if (focused != 0) {
            if (m_type != _gui_list_item_widget && gui_widget_get_parent_list_item_widget(this) != 0)
                result = gui_widget_is_descendant_or_self(focused, (c_gui_widget *)gui_widget_get_parent_list_item_widget(this));
            else
                result = gui_widget_is_descendant_or_self(focused, this);
        }
    }
    return result;
}

// @address 0x48be98 gui_widget_is_descendant_or_self
extern "C" bool gui_widget_is_descendant_or_self(c_gui_widget *object, c_gui_widget *target)
{
    bool result;
    if (target == object) {
        result = true;
    } else {
        result = false;
        for (c_gui_widget *parent = object->get_parent(); parent; parent = parent->get_parent()) {
            if (parent == target) {
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x48bf08 ?get_deepest_widget_that_can_receive_focus@c_gui_widget@@QAAPAV1@XZ
c_gui_widget *c_gui_widget::get_deepest_widget_that_can_receive_focus()
{
    c_gui_widget *result = 0;
    for (c_gui_widget *child = get_children(); child != 0 && result == 0; child = child->get_next()) {
        if (child->m_type != _gui_screen_widget)
            result = child->get_deepest_widget_that_can_receive_focus();
    }
    if (result == 0 && can_receive_focus())
        result = this;
    return result;
}

// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
c_gui_widget *c_gui_widget::get_child_widget(e_gui_widget_type type, long name)
{
    if (m_type == type && m_name == name)
        return this;
    c_gui_widget *result = 0;
    bool recursive = false;
    for (;;) {
        for (c_gui_widget *child = get_children(); child != 0 && result == 0; child = child->get_next()) {
            if (recursive) {
                result = child->get_child_widget(type, name);
            } else if (child->m_type == type && child->m_name == name) {
                result = child;
            }
        }
        if (result == 0 && !recursive) {
            recursive = true;
            continue;
        } else {
            break;
        }
    }
    return result;
}

// @address 0x48c0c8 gui_widget_get_previous_sibling_of_type
extern "C" c_gui_widget *gui_widget_get_previous_sibling_of_type(c_gui_widget *widget, e_gui_widget_type type)
{
    c_gui_widget *previous = widget->get_previous();
    while (previous != 0 && previous->m_type != type)
        previous = previous->get_previous();
    return previous;
}

// @address 0x48c120 gui_widget_get_next_sibling_of_type
extern "C" c_gui_widget *gui_widget_get_next_sibling_of_type(c_gui_widget *widget, e_gui_widget_type type)
{
    c_gui_widget *next;

    for (next = robust_pointer_get_repaired_value(&widget->m_next); next; next = robust_pointer_get_repaired_value(&next->m_next)) {
        if (next->m_type == type)
            break;
    }
    return next;
}

// @address 0x48c178 gui_widget_get_previous_list_item
extern "C" c_gui_widget *gui_widget_get_previous_list_item(c_gui_widget *widget, bool require_valid)
{
    c_gui_widget *result = gui_widget_get_previous_sibling_of_type(widget, _gui_list_item_widget);
    if (result) {
        if (!require_valid)
            return result;
        if (result->get_element_handle() == -1)
            return 0;
    }
    return result;
}

// @address 0x48c1e8 gui_widget_get_next_list_item
extern "C" c_gui_widget *gui_widget_get_next_list_item(c_gui_widget *widget, bool require_valid)
{
    c_gui_widget *result = gui_widget_get_next_sibling_of_type(widget, _gui_list_item_widget);
    if (result) {
        if (!require_valid)
            return result;
        if (result->get_element_handle() == -1)
            return 0;
    }
    return result;
}

// @address 0x48c258 gui_widget_set_alternate_state
extern "C" void gui_widget_set_alternate_state(c_gui_widget *widget, unsigned char value)
{
    widget->m_unknown = value;
    for (c_gui_widget *child = widget->get_children(); child != 0; child = child->get_next())
        gui_widget_set_alternate_state(child, value);
}

// @address 0x48c890 ?get_animation_state_timer@c_gui_widget@@AAAKW4e_animation_state@@@Z
unsigned long c_gui_widget::get_animation_state_timer(e_animation_state state)
{
    unsigned __int64 state_mask = 1ui64 << state;
    if ((state_mask & 0x300c003fui64) != 0)
        return m_animated_state.start_time;
    if ((state_mask & (0x3c0000ffui64 << 6)) != 0) {
        c_gui_widget *widget = this;
        if (m_type != _gui_screen_widget)
            widget = get_parent_screen();
        return widget->get_animated_state()->screen_transition_start_time;
    }
    if ((state_mask & (0x33f03ui64 << 14)) != 0)
        return m_animated_state.previous_control_start_time;
    if ((state_mask & 0x30000) != 0)
        return m_animated_state.next_control_start_time;
    return m_animated_state.indicator_start_time;
}

static __forceinline void c_gui_widget_scale_real_rectangle2d_for_xenon_scaler(real_rectangle2d *rectangle)
{
    calculate_authoring_to_target_dimensions_scaling();
    rectangle->x0 *= g_interface_authoring_to_target_dimensions_scaling.i;
    rectangle->x1 *= g_interface_authoring_to_target_dimensions_scaling.i;
    rectangle->y0 *= g_interface_authoring_to_target_dimensions_scaling.j;
    rectangle->y1 *= g_interface_authoring_to_target_dimensions_scaling.j;
}

// @address 0x48c970 ?scale_and_position_rectangle_for_display_resolution@@YAXW4e_gui_widget_type@@PATreal_rectangle2d@@W4e_widget_anchoring@@@Z
void scale_and_position_rectangle_for_display_resolution(e_gui_widget_type widget_type, real_rectangle2d *rectangle, e_widget_anchoring anchoring)
{
    real_rectangle2d scaled_rectangle = *rectangle;
    float x_offset = 0.0f;
    float y_offset = 0.0f;

    c_gui_widget_scale_real_rectangle2d_for_xenon_scaler(&scaled_rectangle);

    switch (anchoring)
    {
    case 2:
        y_offset += rectangle->y0 - scaled_rectangle.y0;
        break;
    case 3:
        y_offset += rectangle->y1 - scaled_rectangle.y1;
        break;
    case 4:
        x_offset += rectangle->x0 - scaled_rectangle.x0;
        break;
    case 5:
        x_offset += rectangle->x1 - scaled_rectangle.x1;
        break;
    case 6:
        x_offset += rectangle->x0 - scaled_rectangle.x0;
        y_offset += rectangle->y0 - scaled_rectangle.y0;
        break;
    case 7:
        x_offset += rectangle->x1 - scaled_rectangle.x1;
        y_offset += rectangle->y0 - scaled_rectangle.y0;
        break;
    case 8:
        x_offset += rectangle->x1 - scaled_rectangle.x1;
        y_offset += rectangle->y1 - scaled_rectangle.y1;
        break;
    case 9:
        x_offset += rectangle->x0 - scaled_rectangle.x0;
        y_offset += rectangle->y1 - scaled_rectangle.y1;
        break;
    }

    c_gui_widget_offset_real_rectangle2d(&scaled_rectangle, x_offset, y_offset);
    *rectangle = scaled_rectangle;
}
// @address 0x48cb08 gui_widget_get_anchor_offset
extern "C" void gui_widget_get_anchor_offset(c_gui_widget *widget, unsigned long anchoring, real_point2d *offset)
{
    real_rectangle2d bounds;
    widget->get_current_bounds(&bounds);
    float width = bounds.x1 - bounds.x0;
    float height = bounds.y1 - bounds.y0;
    float half_width = width * 0.5f;
    float half_height = height * 0.5f;
    switch (anchoring) {
    case 0: offset->x = 0.0f; offset->y = 0.0f; break;
    case 1: offset->x = half_width; offset->y = half_height; break;
    case 2: offset->x = half_width; offset->y = 0.0f; break;
    case 3: offset->x = half_width; offset->y = height; break;
    case 4: offset->x = 0.0f; offset->y = half_height; break;
    case 5: offset->x = width; offset->y = half_height; break;
    case 6: offset->x = 0.0f; offset->y = 0.0f; break;
    case 7: offset->x = width; offset->y = 0.0f; break;
    case 8: offset->x = width; offset->y = height; break;
    case 9: offset->x = 0.0f; offset->y = height; break;
    default: offset->x = 0.0f; offset->y = 0.0f; break;
    }
}

static __forceinline float gui_keyframe_transition_progress(long transition_type, const c_function_definition *custom_function,
    const c_function_definition *function, float time)
{
    if (transition_type == 1)
        function = custom_function;
    float progress = 0.0f;
    if ((const void *)function->m_data.address && function->m_data.size > 0)
        progress = function->map_to_output_range_legacy(function->evaluate_legacy(time, 1.0f));
    return progress;
}

static __forceinline void gui_keyframe_anchor_point(c_gui_widget *widget, short anchoring, const real_point2d *authored_point,
    real_point2d *point)
{
    if (anchoring >= 0 && anchoring < 10 && anchoring != 0)
        gui_widget_get_anchor_offset(widget, anchoring, point);
    else
        *point = *authored_point;
}

// @address 0x48cf68 gui_widget_apply_scale_animation
extern "C" void gui_widget_apply_scale_animation(c_gui_widget *widget, s_animation_transform *transform, float time, bool combine,
    const s_gui_scale_keyframe *start, const s_gui_scale_keyframe *end, const c_function_definition *function)
{
    float progress = gui_keyframe_transition_progress(start->transition_type, start->custom_function, function, time);

    transform->scale.i = ((end->scale.i - start->scale.i) * progress + start->scale.i) * (combine ? transform->scale.i : 1.0f);
    transform->scale.j = ((end->scale.j - start->scale.j) * progress + start->scale.j) * (combine ? transform->scale.j : 1.0f);

    real_point2d start_origin;
    gui_keyframe_anchor_point(widget, start->anchoring, &start->origin, &start_origin);
    real_point2d end_origin;
    gui_keyframe_anchor_point(widget, end->anchoring, &end->origin, &end_origin);
    transform->rotation_origin.x =
        (end_origin.x - start_origin.x) * progress + (combine ? transform->rotation_origin.x : 0.0f) + start_origin.x;
    transform->rotation_origin.y =
        (end_origin.y - start_origin.y) * progress + (combine ? transform->rotation_origin.y : 0.0f) + start_origin.y;
}

// @address 0x48ce38 gui_widget_apply_position_animation
extern "C" void gui_widget_apply_position_animation(s_animation_transform *transform, float time, bool combine,
    const real_vector2d *aspect_ratio_scale, const s_gui_position_keyframe *start, const s_gui_position_keyframe *end,
    const c_function_definition *function)
{
    float progress = gui_keyframe_transition_progress(start->transition_type, start->custom_function, function, time);

    transform->position.x = (end->position.x - start->position.x) * progress + start->position.x + (combine ? transform->position.x : 0.0f);
    transform->position.y = (end->position.y - start->position.y) * progress + start->position.y + (combine ? transform->position.y : 0.0f);
    // match: 0x48ce38: storing z through a local pointer keeps the x and y reloads after the z store
    float *position_z = &transform->position.z;
    *position_z = (end->position.z - start->position.z) * progress + start->position.z + (combine ? transform->position.z : 0.0f);

    // match: 0x48ce38: pointer __restrict qualifier prevents alias reload
    const real_vector2d *__restrict scale = aspect_ratio_scale;
    transform->position.x = scale->i * transform->position.x;
    transform->position.y = scale->j * transform->position.y;
}



// @rdata 0x8202867c
static const float k_c_gui_widget_radians_to_degrees = 57.295779513f;
// @rdata 0x82024fd4
static const float k_c_gui_widget_degrees_to_radians = 0.017453292519943295f;
static __forceinline float c_gui_widget_rotation_progress(const s_gui_rotation_keyframe *start, const c_function_definition *function, float time)
{
    if (start->transition_type == 1) function = start->custom_function;
    float progress = 0.0f;
    if ((const void *)function->m_data.address && function->m_data.size > 0)
        progress = function->map_to_output_range_legacy(function->evaluate_legacy(time, 1.0f));
    return progress;
}
// @address 0x48d128 gui_widget_apply_rotation_animation
extern "C" void gui_widget_apply_rotation_animation(c_gui_widget *widget, s_animation_transform *transform, float time, bool combine,
    const s_gui_rotation_keyframe *start, const s_gui_rotation_keyframe *end, const c_function_definition *function)
{
    float progress = c_gui_widget_rotation_progress(start, function, time);

    transform->rotation_angle = ((end->rotation - start->rotation) * progress + start->rotation +
                                    (combine ? transform->rotation_angle * k_c_gui_widget_radians_to_degrees : 0.0f)) *
        k_c_gui_widget_degrees_to_radians;
    transform->rotation_sine = sinf(transform->rotation_angle);
    transform->rotation_cosine = cosf(transform->rotation_angle);

    real_point2d start_origin;
    if (start->anchoring >= 0)
        if (start->anchoring < 10)
            if (start->anchoring != 0)
                gui_widget_get_anchor_offset(widget, start->anchoring, &start_origin);
            else
                start_origin = start->origin;
        else
            start_origin = start->origin;
    else
        start_origin = start->origin;
    real_point2d end_origin;
    if (end->anchoring >= 0 && end->anchoring < 10 && end->anchoring != 0)
        gui_widget_get_anchor_offset(widget, end->anchoring, &end_origin);
    else
        end_origin = end->origin;
    transform->rotation_pivot.x = (end_origin.x - start_origin.x) * progress + (combine ? transform->rotation_pivot.x : 0.0f) + start_origin.x;
    transform->rotation_pivot.y = (end_origin.y - start_origin.y) * progress + (combine ? transform->rotation_pivot.y : 0.0f) + start_origin.y;
}

// @address 0x48cc38 gui_widget_apply_color_animation
extern "C" void gui_widget_apply_color_animation(s_animation_transform *transform, float time, bool combine,
    const s_gui_color_keyframe *start, const s_gui_color_keyframe *end, const c_function_definition *function)
{
    if (start->transition_type == 1)
        function = start->custom_function;
    float progress = 0.0f;
    if ((const void *)function->m_data.address && function->m_data.size > 0)
        progress = function->map_to_output_range_legacy(function->evaluate_legacy(time, 1.0f));

    transform->color.alpha = ((end->color.alpha - start->color.alpha) * progress + start->color.alpha) * (combine ? transform->color.alpha : 1.0f);
    transform->color.alpha = PIN(transform->color.alpha, 0.0f, 1.0f);
    transform->color.rgb.red = ((end->color.rgb.red - start->color.rgb.red) * progress + start->color.rgb.red) * (combine ? transform->color.rgb.red : 1.0f);
    transform->color.rgb.red = PIN(transform->color.rgb.red, 0.0f, 1.0f);
    transform->color.rgb.green = ((end->color.rgb.green - start->color.rgb.green) * progress + start->color.rgb.green) *
        (combine ? transform->color.rgb.green : 1.0f);
    transform->color.rgb.green = PIN(transform->color.rgb.green, 0.0f, 1.0f);
    transform->color.rgb.blue = ((end->color.rgb.blue - start->color.rgb.blue) * progress + start->color.rgb.blue) *
        (combine ? transform->color.rgb.blue : 1.0f);
    transform->color.rgb.blue = PIN(transform->color.rgb.blue, 0.0f, 1.0f);
}


static __forceinline unsigned long c_gui_widget_ustrnlen(const wchar_t *string, unsigned long maximum)
{
    unsigned long length = 0;
    while (length < maximum && string[length])
        length++;
    return length;
}




// @address 0x48d4c0 robust_pointer_initialize_to_null
extern "C" c_robust_pointer<c_gui_widget> *robust_pointer_initialize_to_null(c_robust_pointer<c_gui_widget> *pointer, c_gui_widget *value)
{
    c_gui_widget_robust_pointer_store(pointer, 0);
    robust_pointer_set_value(pointer, 0);
    return pointer;
}

// @address 0x48d538 ?set_string@?$c_sized_user_interface_text@$0EAA@@@UAAXPB_W_NJ@Z
template<> void c_sized_user_interface_text<1024>::set_string(const wchar_t *string, bool parse, long parameter)
{
    m_flags &= ~(1 << 3);
    m_flags &= ~(1 << 4);
    if (string && string[0]) {
        if (parse) {
            bool truncated;
            wchar_t buffer[1024];
            user_interface_text_parse_xml(string, buffer, 1024, &truncated, parameter);
            m_text.set(buffer);
            SET_BIT(m_flags, 3, truncated);
            SET_BIT(m_flags, 4, string_contains_special_characters(buffer));
        } else {
            m_text.set(string);
            if (c_gui_widget_ustrnlen(string, 1024) == 1024)
                m_flags |= 1 << 3;
#if !defined(_XBOX) || defined(__clang__)
            SET_BIT(m_flags, 4, string_contains_special_characters(string));
#endif
        }
        m_flags |= 1 << 5;
    } else {
        m_text.m_string[0] = 0;
        m_string.m_string[0] = 0;
        m_flags &= ~(1 << 5);
    }
}

// @address 0x48d6c0 ?update@?$c_sized_user_interface_text@$0EAA@@@UAAXJ@Z
template<> void c_sized_user_interface_text<1024>::update(long user_index)
{
    if (m_text.length() > 0) {
        bool valid = true;
        wchar_t buffer[1024];
        ustrnzcpy(buffer, m_text, 1024);
        if ((long)m_flags & (1 << 4))
            valid = valid && user_interface_parse_string(user_index, buffer, 1024);
        if (valid && wcsncmp(m_string, buffer, 1024) != 0) {
            c_user_interface_draw_string draw;
            c_font_cache cache;
            if (draw.precache_wchar_string_for_font(&cache, buffer, (e_font_id)m_font))
                m_string.set(buffer);
        }
    }
}

// @address 0x48d7b8 ?set_string@?$c_sized_user_interface_text@$0BAA@@@UAAXPB_W_NJ@Z
template<> void c_sized_user_interface_text<256>::set_string(const wchar_t *string, bool parse, long parameter)
{
    m_flags &= ~(1 << 3);
    m_flags &= ~(1 << 4);
    if (string && string[0]) {
        if (parse) {
            bool truncated;
            wchar_t buffer[256];
            user_interface_text_parse_xml(string, buffer, 256, &truncated, parameter);
            m_text.set(buffer);
            SET_BIT(m_flags, 3, truncated);
            SET_BIT(m_flags, 4, string_contains_special_characters(buffer));
        } else {
            m_text.set(string);
            if (c_gui_widget_ustrnlen(string, 256) == 256)
                m_flags |= 1 << 3;
#if !defined(_XBOX) || defined(__clang__)
            SET_BIT(m_flags, 4, string_contains_special_characters(string));
#endif
        }
        m_flags |= 1 << 5;
    } else {
        m_text.m_string[0] = 0;
        m_string.m_string[0] = 0;
        m_flags &= ~(1 << 5);
    }
}



// @address 0x48d940 ?update@?$c_sized_user_interface_text@$0BAA@@@UAAXJ@Z
template<> void c_sized_user_interface_text<256>::update(long user_index)
{
    if (m_text.length() > 0) {
        bool valid = true;
        wchar_t buffer[256];
        ustrnzcpy(buffer, m_text, 256);
        if ((long)m_flags & (1 << 4))
            valid = valid && user_interface_parse_string(user_index, buffer, 256);
        if (valid && wcsncmp(m_string, buffer, 256) != 0) {
            c_user_interface_draw_string draw;
            c_font_cache cache;
            if (draw.precache_wchar_string_for_font(&cache, buffer, (e_font_id)m_font))
                m_string.set(buffer);
        }
    }
}

// @address 0x48da38 ?set_string@?$c_sized_user_interface_text@$0DA@@@UAAXPB_W_NJ@Z
template<> void c_sized_user_interface_text<48>::set_string(const wchar_t *string, bool parse, long parameter)
{
    m_flags &= ~(1 << 3);
    m_flags &= ~(1 << 4);
    if (string && string[0]) {
        if (parse) {
            bool truncated;
            wchar_t buffer[48];
            user_interface_text_parse_xml(string, buffer, 48, &truncated, parameter);
            m_text.set(buffer);
            SET_BIT(m_flags, 3, truncated);
            SET_BIT(m_flags, 4, string_contains_special_characters(buffer));
        } else {
            m_text.set(string);
            if (c_gui_widget_ustrnlen(string, 48) == 48)
                m_flags |= 1 << 3;
#if !defined(_XBOX) || defined(__clang__)
            SET_BIT(m_flags, 4, string_contains_special_characters(string));
#endif
        }
        m_flags |= 1 << 5;
    } else {
        m_text.m_string[0] = 0;
        m_string.m_string[0] = 0;
        m_flags &= ~(1 << 5);
    }
}

static __forceinline long c_gui_widget32_wchar_length(const wchar_t *string, unsigned long maximum)
{
    const wchar_t *s = string;
    unsigned long length = 0;
    do {
        if (!*s)
            break;
        length++;
        s++;
    } while (length < maximum);
    return length;
}
// @address 0x48dbf0 ?update@?$c_sized_user_interface_text@$0DA@@@UAAXJ@Z
template<> void c_sized_user_interface_text<48>::update(long user_index)
{
    if (c_gui_widget32_wchar_length(m_text, 48) > 0) {
        bool valid = true;
        wchar_t buffer[48];
        ustrnzcpy(buffer, m_text, 48);
        if ((long)m_flags & (1 << 4))
            valid = valid && user_interface_parse_string(user_index, buffer, 48);
        if (valid && wcsncmp(m_string, buffer, 48) != 0) {
            c_user_interface_draw_string draw;
            c_font_cache cache;
            if (draw.precache_wchar_string_for_font(&cache, buffer, (e_font_id)m_font))
                m_string.set(buffer);
        }
    }
}

// @address 0x48dd88 ??0?$c_sized_user_interface_text@$0EAA@@@QAA@XZ
template<> c_sized_user_interface_text<1024>::c_sized_user_interface_text()
{
    set_string(L"", false, -1);
}

// @address 0x48de58 ??0?$c_sized_user_interface_text@$0BAA@@@QAA@XZ
template<> c_sized_user_interface_text<256>::c_sized_user_interface_text()
{
    set_string(L"", false, -1);
}

// @address 0x48df28 ??0?$c_sized_user_interface_text@$0DA@@@QAA@XZ
template<> c_sized_user_interface_text<48>::c_sized_user_interface_text()
{
    set_string(L"", false, -1);
}


// @address 0x48acd0 ?create_text_widget@c_gui_widget@@UAAPAVc_gui_text_widget@@PBUs_runtime_text_widget_definition@@@Z
c_gui_text_widget *c_gui_widget::create_text_widget(const s_runtime_text_widget_definition *definition)
{
    if (definition->core.flags & 0x4000) {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_sized_text_widget<1024>), 0, 0);
        if (memory)
            return new (memory) c_gui_sized_text_widget<1024>();
    } else if (definition->core.flags & 0x2000) {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_sized_text_widget<256>), 0, 0);
        if (memory)
            return new (memory) c_gui_sized_text_widget<256>();
    } else {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_sized_text_widget<48>), 0, 0);
        if (memory)
            return new (memory) c_gui_sized_text_widget<48>();
    }
    return 0;
}

// @address 0x48e050 robust_pointer_set_value
extern "C" void robust_pointer_set_value(c_robust_pointer<c_gui_widget> *pointer, c_gui_widget *value)
{
    c_gui_widget_robust_pointer_store(pointer, value);
}

// @address 0x4e32e0 gui_list_widget_find_item_by_element_handle
extern "C" c_gui_widget *gui_list_widget_find_item_by_element_handle(c_gui_widget *list_widget, long index)
{
    c_gui_widget *result = 0;
    if (index != -1) {
        for (c_gui_widget *current = gui_widget_find_child_by_name(list_widget,
            _gui_list_item_widget); current; current = gui_widget_get_next_list_item(current, true)) {
            if (current->get_element_handle() == index) {
                result = current;
                break;
            }
        }
    }
    return result;
}

// @address 0x4e4c10 ??0s_animation_transform@@QAA@XZ
s_animation_transform::s_animation_transform() :
    screen_transition_start_time(0), previous_control_start_time(0), next_control_start_time(0), indicator_start_time(0),
    animation_state_flags(0),
    color(), position(), scale(), rotation_origin(), rotation_angle(0.0f), rotation_sine(0.0f), rotation_cosine(1.0f),
    rotation_pivot(), texture_offset(), bitmap_sequence_index(0), bitmap_frame_index(0), font(0)
{
    color.alpha = 1.0f;
    color.rgb.red = 1.0f;
    color.rgb.green = 1.0f;
    color.rgb.blue = 1.0f;
    scale.j = 1.0f;
    scale.i = 1.0f;
    texture_offset.y = 0.0f;
    texture_offset.x = 0.0f;
}



// @address 0x4e87a0 ??0c_xenon_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z
c_xenon_message::c_xenon_message(long value, e_controller_index controller, e_window_index window, long) :
    c_message(_ui_xenon_message, _start_menu_string_start_menu, controller, window), m_xenon_message_type((e_xenon_message_type)value), m_event_value(-1)
{
}

// @address 0x57f558 robust_pointer_resolve_majority_value
extern "C" void *robust_pointer_resolve_majority_value(void **values)
{
    void *a = values[0];
    void *b = values[1];
    if (c_gui_widget_robust_values_consistent(values) ? true : (a == b || false))
        return a;

    // no two copies agree: when every copy is null or a single bit away from null, the value was null
    void *c = values[2];
    if (b == c)
        return b;
    if (c == a)
        return c;
    return (!a || count_bits<unsigned long>(reinterpret_cast<uintptr_t>(a)) == 1)
        && (!b || count_bits<unsigned long>(reinterpret_cast<uintptr_t>(b)) == 1)
        && (!c || count_bits<unsigned long>(reinterpret_cast<uintptr_t>(c)) == 1) ? 0 : a;
}

// match: 0x4e7d38: stores go through a local pointer to m_definition; direct member stores change the base register and scheduling.

// @address 0x4e7d38 ??0c_gui_model_widget@@QAA@XZ
c_gui_model_widget::c_gui_model_widget() : c_gui_widget(_gui_model_widget)
{
    s_runtime_model_widget_definition *definition;
    definition = &m_definition;
    definition->elements[0].distance_function = 0;
    definition->elements[0].field_of_view = 0.0f;
    definition->elements[0].pad28 = 0;
    definition->elements[0].lower = 0.0f;
    definition->elements[0].height_function = 0;
    definition->elements[0].upper = 0.0f;
    definition->elements[0].pad34 = 0;
    definition->elements[0].scale = 0.0f;
    definition->elements[1].distance_function = 0;
    definition->elements[0].height_lower = 0.0f;
    definition->elements[1].pad28 = 0;
    definition->elements[0].height_upper = 0.0f;
    definition->elements[1].height_function = 0;
    definition->elements[0].height_scale = 0.0f;
    definition->elements[1].pad34 = 0;
    definition->elements[0].rotation_scale = 0.0f;
    definition->elements[1].field_of_view = 0.0f;
    definition->elements[1].lower = 0.0f;
    definition->elements[1].upper = 0.0f;
    definition->elements[1].scale = 0.0f;
    definition->elements[1].height_lower = 0.0f;
    definition->elements[1].height_upper = 0.0f;
    definition->elements[1].height_scale = 0.0f;
    definition->elements[1].rotation_scale = 0.0f;
    memset((char *)&m_player_appearance, 0, sizeof(m_player_appearance));
    m_yaw = 0.0f;
    m_zoom = 0.0f;
    m_appearance_index = -1;
    memset(m_definition.elements, 0, sizeof(m_definition.elements));
}

// @address 0x485ee8 gui_screen_widget_reload_and_update_datasources
extern "C" void gui_screen_widget_reload_and_update_datasources(c_gui_screen_widget *p0)
{
    int i = 0;
    if (p0->m_reload_next_frame)
    {
        p0->m_reload_next_frame = false;
        p0->dispose();
        p0->reconstruct();
        p0->initialize();
        p0->post_initialize();
        p0->start_animation_at_time(_animation_state_transition_to_screen, 0xffffffffu, true);
    }
    if (p0->m_datasource_count > 0)
    {
        c_gui_data **children = p0->m_datasource;
        do
        {
            children[i]->update();
            i++;
        } while (i < p0->m_datasource_count);
    }
}


static __forceinline long c_gui_widget_get_animation_period(const s_widget_component_animation_definition *component, long keyframe_size)
{
    long period = component->keyframe_count > 1 ? *(const long *)(component->keyframes + (component->keyframe_count - 1) * keyframe_size) : 0x7fffffff;
    return period;
}

static __forceinline bool c_gui_widget_verify_animation_period(long period)
{
    bool result = true;
    // match: 0x48c2c8: a literal divisor makes frame_period a constant; the variable divisor keeps its one-time guarded initialization (guard 0x119a564)
    const real frames_per_second = 60.0f;
    static real const frame_period = 1000.0f / frames_per_second;
    if ((float)period <= frame_period)
        result = false;
    return result;
}


// @address 0x48c2c8 gui_widget_apply_animation_state
extern "C" void gui_widget_apply_animation_state(c_gui_widget *widget, e_animation_state animation_state, unsigned long start_time,
    unsigned long current_milliseconds,
    const s_widget_animation_definition *animation_definition, s_animation_transform *transform, real_vector2d *aspect_ratio_scale,
    bool initialize, bool combine_states, bool *finished)
{
    long active_components = 0;
    if (animation_definition != NULL)
    {
        for (long component_index = 0; component_index < 7; component_index++)
        {
            bool component_finished = true;
            const s_widget_component_animation_definition *component;
            long keyframe_size;
            switch (component_index)
            {
            case 0:
                if (animation_definition->color.index != -1)
                    component = (const s_widget_component_animation_definition *)tag_get(
                        animation_definition->color.group, animation_definition->color.index);
                else
                    component = NULL;
                keyframe_size = 32;
                break;
            case 1:
                if (animation_definition->position.index != -1)
                    component = (const s_widget_component_animation_definition *)tag_get(
                        animation_definition->position.group, animation_definition->position.index);
                else
                    component = NULL;
                keyframe_size = 28;
                break;
            case 2:
                if (animation_definition->scale.index != -1)
                    component = (const s_widget_component_animation_definition *)tag_get(
                        animation_definition->scale.group, animation_definition->scale.index);
                else
                    component = NULL;
                keyframe_size = 36;
                break;
            case 3:
                if (animation_definition->rotation.index != -1)
                    component = (const s_widget_component_animation_definition *)tag_get(
                        animation_definition->rotation.group, animation_definition->rotation.index);
                else
                    component = NULL;
                keyframe_size = 32;
                break;
            case 4:
                if (animation_definition->rotation_origin.index != -1)
                    component = (const s_widget_component_animation_definition *)tag_get(
                        animation_definition->rotation_origin.group, animation_definition->rotation_origin.index);
                else
                    component = NULL;
                keyframe_size = 24;
                break;
            case 5:
                if (animation_definition->bitmap.index != -1)
                    component = (const s_widget_component_animation_definition *)tag_get(
                        animation_definition->bitmap.group, animation_definition->bitmap.index);
                else
                    component = NULL;
                keyframe_size = 20;
                break;
            case 6:
                if (animation_definition->display.index != -1)
                    component = (const s_widget_component_animation_definition *)tag_get(
                        animation_definition->display.group, animation_definition->display.index);
                else
                    component = NULL;
                keyframe_size = 20;
                break;
            default:
                // match: 0x48c2c8: unreachable hint omits dead branch check
                __assume(0);
            }
            if (component == NULL)
                continue;

            long period = c_gui_widget_get_animation_period(component, keyframe_size);
            const c_function_definition *function = &component->function;
            if (!c_gui_widget_verify_animation_period(period))
                continue;

            long animation_time = (long)(current_milliseconds - start_time);
            long loop_count = period > 0 ? animation_time / period : 0;
            long keyframe_count = component->keyframe_count;
            bool looping = (component->flags & 1) != 0;
            bool ping_pong = (!looping && (component->flags & 2) != 0) ? true : false;
            bool play_once = (!looping && !ping_pong) ? true : false;
            long direction = ping_pong ? ((loop_count & 1) ? -1 : 1) : 1;
            if (play_once)
                component_finished = loop_count >= 1;
            else
                component_finished = false;
            if (component_finished)
                animation_time = period;
            else
                animation_time -= loop_count * period;

            long keyframe_indices[2];
            if (keyframe_count > 1)
            {
                if (direction > 0)
                {
                    if (play_once && loop_count > 1)
                    {
                        keyframe_indices[0] = keyframe_count - 1;
                        keyframe_indices[1] = keyframe_indices[0];
                    }
                    else
                    {
                        keyframe_indices[0] = 0;
                        keyframe_indices[1] = keyframe_indices[0] + 1;
                        while (keyframe_indices[1] < keyframe_count - 1)
                        {
                            const s_keyframe_block_base *keyframe = component->get_keyframe(keyframe_indices[1], keyframe_size);
                            if (animation_time <= keyframe->time)
                                break;
                            keyframe_indices[0]++;
                            keyframe_indices[1]++;
                        }
                    }
                }
                else
                {
                    keyframe_indices[0] = keyframe_count - 1;
                    keyframe_indices[1] = keyframe_indices[0] - 1;
                    while (keyframe_indices[1] > 0)
                    {
                        const s_keyframe_block_base *keyframe = component->get_keyframe(keyframe_indices[1], keyframe_size);
                        long remaining = period - keyframe->time;
                        if (animation_time <= remaining)
                            break;
                        keyframe_indices[0]--;
                        keyframe_indices[1]--;
                    }
                }
            }
            else
            {
                keyframe_indices[1] = 0;
                keyframe_indices[0] = keyframe_indices[1];
            }

            const s_keyframe_block_base *keyframes[2];
            keyframes[0] = component->get_keyframe(keyframe_indices[0], keyframe_size);
            keyframes[1] = component->get_keyframe(keyframe_indices[1], keyframe_size);
            if (keyframes[0] == NULL || keyframes[1] == NULL)
                continue;

            long keyframe_delta = (keyframes[1]->time - keyframes[0]->time) >= 0 ? (keyframes[1]->time - keyframes[0]->time)
                                                                                 : -(keyframes[1]->time - keyframes[0]->time);
            float progress = keyframe_delta > 0 ? fabsf((float)(animation_time - keyframes[0]->time) / (float)keyframe_delta) : 0.0f;

            switch (component_index)
            {
            case 0:
                gui_widget_apply_color_animation(transform, progress, combine_states, (const s_gui_color_keyframe *)keyframes[0],
                    (const s_gui_color_keyframe *)keyframes[1], function);
                break;
            case 1:
                gui_widget_apply_position_animation(transform, progress, combine_states, aspect_ratio_scale,
                    (const s_gui_position_keyframe *)keyframes[0], (const s_gui_position_keyframe *)keyframes[1], function);
                break;
            case 2:
                gui_widget_apply_scale_animation(widget, transform, progress, combine_states, (const s_gui_scale_keyframe *)keyframes[0],
                    (const s_gui_scale_keyframe *)keyframes[1], function);
                break;
            case 3:
                gui_widget_apply_rotation_animation(widget, transform, progress, combine_states,
                    (const s_gui_rotation_keyframe *)keyframes[0], (const s_gui_rotation_keyframe *)keyframes[1], function);
                break;
            case 4:
                gui_widget_animate_rotation_origin(transform, progress, combine_states, (const s_gui_texture_offset_keyframe *)keyframes[0],
                    (const s_gui_texture_offset_keyframe *)keyframes[1], function);
                break;
            case 5:
                gui_widget_animate_bitmap(transform, progress, combine_states, (const s_gui_bitmap_keyframe *)keyframes[0],
                    (const s_gui_bitmap_keyframe *)keyframes[1], function);
                break;
            case 6:
                transform->font = PIN(keyframes[0]->anchoring, 0, 10);
                break;
            default:
                // match: 0x48c2c8: unreachable hint omits dead branch check
                __assume(0);
            }

            active_components |= (1 << component_index);
            *finished |= component_finished;
        }
    }
    if (active_components == 0)
        *finished = true;
}
