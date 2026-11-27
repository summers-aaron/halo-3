// interfacelibxbox/c_gui_text_widget.obj: from the records 0x4e4cb0 0x4e4d70 0x4e4e08 0x4e5518 0x4e55b8 0x4e5638 0x4e56c0 0x4e5910 0x4e5db8 0x4e5f40 0x4e5fb0 0x4e61a0 0x4e61e0 0x4e6228 0x4e62a0
// @flags /TP /O2 /Oc
#include "interface/c_gui_text_widget.h"
#include "interface/gui_widget.h"
#include "interface/core_widget_definition.h"
#include "interface/c_gui_screen_widget.h"
#include "math/real_math_types.h"
#include <string.h>
#include "math/real_math.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "shell/shell.h"
#include "interface/gui_animation_transform.h"
#include "interface/user_interface.h"
#include "text/unicode.h"
#include "rasterizer/rasterizer_main.h"
#include "interface/user_interface_utilities.h"
#include "text/text_group.h"
#include "interface/c_gui_list_widget.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "interface/gui_data.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/gui_widget_hierarchy.h"
#include "math/color_math.h"
#include "memory/datum_index.h"
#include "cseries/global_real_argb_colors.h"
// @address 0x4e4cb0 ??0c_gui_text_widget@@QAA@XZ
// @address 0x4e4d70 ?initialize@c_gui_text_widget@@UAAXPBUs_text_widget_block@@@Z
// @address 0x4e5518 ?set_string_from_string_id@c_gui_text_widget@@AAA_NPAV?$c_static_wchar_string@$0EAA@@@PAVc_gui_widget@@J_N@Z
// @address 0x4e55b8 ?update_render_state@c_gui_text_widget@@UAAXK@Z
// @address 0x4e5638 ?set_animated_state_baseline@c_gui_text_widget@@UAAXPAUs_animation_transform@@@Z
// @address 0x4e5db8 ?add_definition_fields@c_gui_text_widget@@SAXPBUs_text_widget_definition@@PAUs_runtime_text_widget_definition@@PATreal_rectangle2d@@_N@Z
// @address 0x4e5fb0 ?set_text_internal@c_gui_text_widget@@AAAXPB_WPBUs_text_source_data@1@_N@Z
// @address 0x4e61a0 ?get_text@c_gui_text_widget@@UAAPB_WXZ
// @address 0x4e61e0 ?set_text@c_gui_text_widget@@UAAXPB_W@Z
// @address 0x4e6228 ?set_text_from_string_id@c_gui_text_widget@@UAAXPAVc_gui_screen_widget@@J@Z


#include "interface/gui_text_widget_colors.h"
// @address 0x3ac90 global_real_argb_colors

// @address 0x3acc0 k_argb_red
// @rdata 0x8203acc0..0x8203acd0
static const real_argb_color k_argb_red = { 1.0f, 1.0f, 0.0f, 0.0f };
// @rdata-end

// @address 0x3ad00 k_argb_yellow
// @rdata 0x8203ad00..0x8203ad10
static const real_argb_color k_argb_yellow = { 1.0f, 1.0f, 1.0f, 0.0f };
// @rdata-end

// @address 0x74b750 g_gui_text_widget_color_1
const real_argb_color *g_gui_text_widget_color_1 = &k_argb_red;
// @address 0x74b760 g_gui_text_widget_color_6
const real_argb_color *g_gui_text_widget_color_6 = &k_argb_yellow;


#include "interface/user_interface_text_types.h"

#include "interface/text_widget_definition.h"

#include "interface/user_interface_text_render.h"


static __forceinline real_rgb_color *c_gui_text_widget_set_real_rgb_color(real_rgb_color *color, float red, float green, float blue)
{
    color->red = red;
    color->green = green;
    color->blue = blue;
    return color;
}
static __forceinline real_argb_color *c_gui_text_widget_set_real_argb_color(real_argb_color *color, float alpha, float red, float green, float blue)
{
    color->alpha = alpha;
    c_gui_text_widget_set_real_rgb_color(&color->rgb, red, green, blue);
    return color;
}
static __forceinline void c_gui_text_widget_set_animated_state_baseline_base(s_animation_transform *baseline)
{
    c_gui_text_widget_set_real_argb_color(&baseline->color, 1.0f, 1.0f, 1.0f, 1.0f);
    set_real_point3d(&baseline->position, 0.0f, 0.0f, 0.0f);
    set_real_vector2d(&baseline->scale, 1.0f, 1.0f);
    set_real_point2d(&baseline->rotation_origin, 0.0f, 0.0f);
    baseline->rotation_angle = 0.0f;
    baseline->rotation_sine = 0.0f;
    baseline->rotation_cosine = 1.0f;
    set_real_point2d(&baseline->rotation_pivot, 0.0f, 0.0f);
    set_real_point2d(&baseline->texture_offset, 0.0f, 0.0f);
    baseline->bitmap_sequence_index = 0;
    baseline->bitmap_frame_index = 0;
    baseline->font = 0;
}

class c_font_cache;


c_gui_text_widget::c_gui_text_widget() : c_gui_widget(_gui_text_widget)
{
}

void c_gui_text_widget::initialize(const struct s_text_widget_block *text_widget_block)
{
    if (text_widget_block != 0)
    {
        const s_text_widget_block *block = text_widget_block;
        real_rectangle2d parent_bounds;
        robust_pointer_get_repaired_value(&m_parent)->get_authored_bounds(&parent_bounds);
        s_runtime_text_widget_definition *definition = &this->definition;
        bool replace = false;
        if (block->text_widget_template.index != -1)
        {
            long template_index = datum_index_to_absolute_index(block->text_widget_template.index);
            add_definition_fields((const s_text_widget_definition *)global_tag_instances[template_index].definition, definition, &parent_bounds,
                replace);
            replace = true;
        }
        add_definition_fields(&block->definition, definition, &parent_bounds, replace);
        user_interface_precache_font_charset(this->definition.font);
    }
    m_name = this->definition.core.name;
    c_gui_widget::initialize();
}

bool c_gui_text_widget::set_string_from_string_id(c_static_wchar_string<1024> *static_wchar_string, c_gui_widget *gui_widget, long string_id, bool force)
{
    bool result = false;
    s_text_source_data source_data;
    if (string_id != -1 && string_id != 0)
    {
        if (gui_widget->get_string_by_string_id(string_id, static_wchar_string))
        {
            source_data.type = 1;
            source_data.source_id = string_id;
            set_text_internal(static_wchar_string->get_string(), &source_data, force);
            result = true;
        }
    }
    return result;
}

void c_gui_text_widget::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    c_gui_screen_widget *parent_screen = get_parent_screen();
    if (parent_screen != 0 && !parent_screen->transitioning_out())
    {
        get_text_internal()->update(-1);
    }
}

void c_gui_text_widget::set_animated_state_baseline(s_animation_transform *baseline)
{
    c_gui_text_widget_set_animated_state_baseline_base(baseline);
    c_gui_text_widget_set_real_argb_color(&baseline->color, 1.0f, 1.0f, 1.0f, 1.0f);
    baseline->font = definition.font;
}

// @address 0x4e56c0 ?compute_layout_bounds@c_gui_text_widget@@AAAXPBTrectangle2d@@Treal_point2d@@MMMMMPAT3@PATreal_rectangle2d@@3PAM@Z
void c_gui_text_widget::compute_layout_bounds(const rectangle2d *rectangle, real_point2d origin, float x,
    float y, float z, float scale_x, float scale_y, real_point2d *out_origin, real_rectangle2d *out_flat, real_rectangle2d *out_rectangle,
    float *out_scale)
{
    s_runtime_core_widget_definition *core_definition = get_core_definition();
    float origin_dx = x - core_definition->current_bounds.x0;
    float origin_dy = y - core_definition->current_bounds.y0;
    gui_real_rectangle2d bounds;
    get_unprojected_bounds(&bounds, true, true, false);
    origin.x += origin_dx;
    origin.y += origin_dy;
    real_point3d min_point;
    real_point3d max_point;
    min_point.z = z;
    max_point.z = z;
    float average = (scale_x + scale_y) * 0.5f;
    c_user_interface_text *text_source = get_text_internal();
    min_point.x = text_source->layout_origin.x + bounds.points[0].x;
    min_point.y = text_source->layout_origin.y + bounds.points[0].y;
    max_point.x = text_source->layout_origin.x + bounds.points[3].x;
    max_point.y = text_source->layout_origin.y + bounds.points[3].y;
    gui_real_rectangle2d::transform_real_point3d(&min_point, rectangle);
    gui_real_rectangle2d::transform_real_point3d(&max_point, rectangle);

    real_rectangle2d flat_bounds;
    flat_bounds.x0 = min_point.x;
    flat_bounds.x1 = max_point.x;
    flat_bounds.y0 = min_point.y;
    flat_bounds.y1 = max_point.y;
    float width = bounds.points[3].x - bounds.points[0].x;
    float scale;
    if (width > 0.0f)
        scale = (flat_bounds.x1 - flat_bounds.x0) / width;
    else
        scale = 1.0f;
    scale *= average;

    set_real_point3d(&min_point, bounds.points[0].x, bounds.points[0].y, z);
    set_real_point3d(&max_point, bounds.points[3].x, bounds.points[3].y, z);
    gui_real_rectangle2d::transform_real_point3d(&min_point, rectangle);
    gui_real_rectangle2d::transform_real_point3d(&max_point, rectangle);

    real_rectangle2d output_bounds;
    output_bounds.x0 = min_point.x;
    output_bounds.y0 = min_point.y;
    output_bounds.x1 = max_point.x;
    output_bounds.y1 = max_point.y;

    if (out_origin != 0)
        *out_origin = origin;
    if (out_flat != 0)
        *out_flat = flat_bounds;
    if (out_rectangle != 0)
        *out_rectangle = output_bounds;
    if (out_scale != 0)
        *out_scale = scale;
}

void c_gui_text_widget::add_definition_fields(const s_text_widget_definition *source_definition,
                  s_runtime_text_widget_definition *dest_definition,
                  real_rectangle2d *positioning_bounds, bool replace)
{
    const s_user_interface_shared_globals *globals = (const s_user_interface_shared_globals *)((char *)::user_interface_shared_tag_globals_try_and_get());
    c_gui_widget::add_definition_fields(_gui_text_widget, &source_definition->core, &dest_definition->core, positioning_bounds, replace);
    if (source_definition->string_id != 0 && source_definition->string_id != -1)
        dest_definition->string_id = source_definition->string_id;
    if (source_definition->string_list != 0 && source_definition->string_list != -1)
        dest_definition->string_list = source_definition->string_list;
    if (globals)
    {
        real_argb_color zero = real_argb_color();
        bool color_is_zero = memcmp(&dest_definition->color, &zero, sizeof(zero)) == 0;
        if (source_definition->color_name != 0 || color_is_zero)
        {
            if (source_definition->color_name == 0 && globals->text_colors.count() > 0)
                dest_definition->color = globals->text_colors[0].color;
            else
            {
                for (long index = 0; index < globals->text_colors.count(); ++index)
                {
                    const s_named_text_color *color = &globals->text_colors[index];
                    if (source_definition->color_name == color->name)
                    {
                        dest_definition->color = color->color;
                        break;
                    }
                }
            }
        }
    }
    if (source_definition->font >= 0 && source_definition->font < 11 && source_definition->font != 0)
        dest_definition->font = source_definition->font;
}

real_argb_color *c_gui_text_widget::get_cumulative_color_tint(real_argb_color *color)
{
    *color = m_animated_state.color;
    color->alpha = definition.color.alpha * color->alpha;
    color->rgb.red = definition.color.rgb.red * color->rgb.red;
    color->rgb.green = definition.color.rgb.green * color->rgb.green;
    color->rgb.blue = definition.color.rgb.blue * color->rgb.blue;
    return color;
}

void c_gui_text_widget::set_text_internal(const wchar_t *text, const s_text_source_data *source_data, bool force)
{
    bool changed = false;
    if (source_data->type != text_source.type)
    {
        changed = true;
    }
    else
    {
        switch (source_data->type)
        {
        case -1:
            break;
        case 0:
            changed = true;
            break;
        case 1:
            changed = source_data->source_id != text_source.source_id;
            break;
        case 2:
            changed = source_data->source_id != text_source.source_id ||
                source_data->string_id != text_source.string_id ||
                wcscmp(text, get_text_internal()->get_string()) != 0;
            break;
        default:
            // match: 0x4e5fb0: unreachable default lets the type switch lower to a biased jump table (addi 1, bctr) instead of cmplwi compare chains
            __assume(0);
        }
    }
    if (changed || force)
    {
        bool overflowed = false;
        bool has_tags = false;
        c_gui_screen_widget *parent_screen = get_parent_screen();
        if (parent_screen != 0)
        {
            if (text != 0)
            {
                const wchar_t *open = wcschr(text, L'<');
                const wchar_t *close = open != 0 ? wcschr(open, L'>') : 0;
                has_tags = close != 0;
            }
            get_text_internal()->set_string(text, has_tags, parent_screen->get_screen_index());
            if (get_text_internal()->has_overflowed())
                overflowed = true;
            if (overflowed)
                definition.core.flags |= 0x200000;
            else
                definition.core.flags &= ~0x200000;
            text_source = *source_data;
        }
    }
}

const wchar_t *c_gui_text_widget::get_text(void)
{
    return get_text_internal()->get_string();
}

void c_gui_text_widget::set_text(const wchar_t *text)
{
    if (text != 0)
    {
        s_text_source_data source_data;
        source_data.type = 0;
        set_text_internal(text, &source_data, true);
    }
}

void c_gui_text_widget::set_text_from_string_id(c_gui_screen_widget *gui_screen_widget, long string_id)
{
    c_static_wchar_string<1024> string;
    if (gui_screen_widget != 0)
    {
        if (gui_screen_widget->get_string_by_string_id(string_id, &string))
        {
            set_text(string.get_string());
        }
    }
}

static __forceinline void c_gui_text_widget_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    string->m_string[0] = 0;
    const s_user_interface_shared_globals *globals = (const s_user_interface_shared_globals *)((char *)::user_interface_shared_tag_globals_try_and_get());
    if (globals != 0 && globals->global_strings.index != -1)
        text_group_get_string(globals->global_strings.index, string_id, string);
}

// 0x4e4e08 (c_gui_text_widget::update)
// @address 0x4e4e08 ?update@c_gui_text_widget@@UAAXK@Z
void c_gui_text_widget::update(unsigned long current_milliseconds)
{
    
    this->c_gui_widget::update(current_milliseconds);
    if (!m_visible)
        return;
    c_gui_list_item_widget *list_item = gui_widget_get_parent_list_item_widget(this);
    c_gui_widget *list = gui_widget_get_parent_list_widget(this);
    long *definition_flags = (long *)get_core_definition();
    bool left_justified = (*definition_flags & 8) != 0;
    bool centered = !left_justified && (*definition_flags & 0x10) != 0;
    bool render_uppercase = (*definition_flags & 0x40) != 0;
    bool use_text = (*definition_flags & 0x80) != 0;
    bool use_string_id = (*definition_flags & 0x100) != 0;
    bool use_string_list = (*definition_flags & 0x200) != 0;
    bool use_integer = (*definition_flags & 0x400) != 0;
    bool use_item_label = (*definition_flags & 0x800) != 0;
    bool selected = (*definition_flags & 0x1000) != 0 && list_item != 0 && list_item->within_focus_chain() &&
        list != 0 && list->get_enabled();
    bool found = false;
    bool was_selected = (*definition_flags & 0x100000) != 0;
    bool unknown_flag2 = (*definition_flags & 0x40000) == 0;
    long justification = left_justified ? 0 : (centered == 0) + 1;
    long drop_shadow_style = (*definition_flags & 0x8000) ? 1 : (*definition_flags & 0x10000) ? 0 : 2;
    this->get_text_internal()->set_render_uppercase(render_uppercase);
    this->get_text_internal()->set_drop_shadow_style(drop_shadow_style);
    this->get_text_internal()->set_justification(justification);
    this->get_text_internal()->set_font(m_animated_state.font);
    this->get_text_internal()->set_unknown_flag2(unknown_flag2);
    if (robust_pointer_get_repaired_value(&this->m_parent) == 0)
        return;
    c_gui_widget *parent = robust_pointer_get_repaired_value(&this->m_parent);
    c_gui_widget *screen = parent->m_type == 4 ? robust_pointer_get_repaired_value(&this->m_parent) : (c_gui_widget *)this->get_parent_screen();
    if (screen == 0)
        return;
    c_gui_text_widget::s_text_source_data source_data;
    c_static_wchar_string<1024> string;
    bool force = selected || was_selected;
    if (use_item_label && list_item != 0)
        found = this->set_string_from_string_id(&string, screen, list_item->m_definition.item_label, force);
    if (!found)
    {
        c_gui_screen_widget *parent_screen = this->get_parent_screen();
        c_gui_data *data = 0;
        long element_handle = -1;
        if (list != 0)
        {
            data = ((c_gui_list_widget *)list)->get_data();
            element_handle = get_element_handle();
        }
        if (parent_screen != 0 && definition.string_id != 0 && definition.string_id != -1)
        {
            c_gui_widget *named_list;
            if (definition.string_id == _string_id_focused_list)
            {
                named_list = 0;
                if (!parent_screen->m_suppress_focus && parent_screen->m_focused_widget != 0)
                    named_list = gui_widget_get_parent_list_widget(parent_screen->m_focused_widget);
            }
            else
            {
                named_list = ((c_gui_widget *)parent_screen)->get_child_widget(_gui_list_widget, definition.string_id);
            }
            if (named_list != 0)
            {
                data = ((c_gui_list_widget *)named_list)->get_data();
                element_handle = ((c_gui_list_widget *)named_list)->get_focused_element_handle();
            }
        }
        if (data != 0)
        {
            
            long name = definition.string_list;
            if (use_integer)
            {
                unsigned long value = 0;
                if (data->get_integer_value(element_handle, name, (long *)&value))
                {
                    source_data.type = 2;
                    source_data.source_id = name;
                    source_data.string_id = element_handle;
                    string.print(L"%d", value);
                    this->set_text_internal(string.get_string(), &source_data, force);
                    found = true;
                }
            }
            if (!found && use_text && data->get_text_value(element_handle, name, &string))
            {
                source_data.type = 2;
                source_data.source_id = name;
                source_data.string_id = element_handle;
                this->set_text_internal(string.get_string(), &source_data, force);
                found = true;
            }
            if (!found && use_string_id)
            {
                long string_id = -1;
                if (data->get_string_id_value(element_handle, name, &string_id))
                    found = this->set_string_from_string_id(&string, screen, string_id, force);
            }
            if (!found && use_string_list)
            {
                const s_user_interface_shared_globals *globals =
                    (const s_user_interface_shared_globals *)((char *)::user_interface_shared_tag_globals_try_and_get());
                if (globals != 0 && globals->global_strings.index != -1)
                {
                    long string_id = -1;
                    if (data->get_string_id_value(element_handle, name, &string_id) &&
                        multilingual_unicode_string_list_test_string(globals->global_strings.index, string_id) &&
                        text_group_get_string(globals->global_strings.index, string_id, &string))
                    {
                        source_data.type = 1;
                        source_data.source_id = string_id;
                        this->set_text_internal(string.get_string(), &source_data, force);
                        found = true;
                    }
                }
            }
            if (!found && (use_text || use_string_id || use_string_list || use_integer))
            {
                source_data.type = 0;
                source_data.source_id = name;
                this->set_text_internal(string.get_string(), &source_data, force);
                found = true;
            }
        }
    }
    if (!found)
        found = this->set_string_from_string_id(&string, screen, definition.string_list, force);
    if (selected && found)
    {
        c_static_wchar_string<1024> prefix;
        c_static_wchar_string<1024> decorated;
        c_gui_text_widget_global_string_get(0x10019, &prefix);
        decorated.append(prefix.get_string());
        decorated.append(string.get_string());
        c_gui_text_widget_global_string_get(0x1001a, &prefix);
        decorated.append(prefix.get_string());
        this->set_text_internal(decorated.get_string(), &source_data, force);
    }
    if (selected)
        *definition_flags |= 0x100000;
    else
        *definition_flags &= ~0x100000;
}


// @address 0x4e62a0 ?get_text_bounds@c_gui_text_widget@@QAA_NPAVc_font_cache@@PATrectangle2d@@@Z
bool c_gui_text_widget::get_text_bounds(c_font_cache *font_cache, rectangle2d *rectangle)
{
    bool result = false;
    rectangle2d screen_bounds;
    XVIDEO_MODE *video_mode = rasterizer_main_get_video_mode();
    screen_bounds.y0 = 0;
    if (video_mode->fIsWideScreen != 0)
    {
        screen_bounds.x0 = 0;
        screen_bounds.x1 = (short)c_rasterizer::render_globals.width;
        screen_bounds.y1 = (short)c_rasterizer::render_globals.height;
    }
    else
    {
        screen_bounds.x0 = 0;
        screen_bounds.x1 = 640;
        screen_bounds.y1 = 480;
    }

    real_rectangle2d flat_bounds;
    compute_layout_bounds(&screen_bounds, m_animated_state.rotation_pivot, m_animated_state.position.x,
        m_animated_state.position.y, m_animated_state.position.z,
        m_animated_state.scale.i, m_animated_state.scale.j, 0, &flat_bounds, 0, 0);
    if (flat_bounds.y1 - flat_bounds.y0 > 0.0f && flat_bounds.x1 - flat_bounds.x0 > 0.0f)
    {
        rectangle2d text_rectangle;
        text_rectangle.y0 = (short)flat_bounds.y0;
        text_rectangle.x0 = (short)flat_bounds.x0;
        text_rectangle.y1 = (short)flat_bounds.y1;
        text_rectangle.x1 = (short)flat_bounds.x1;
        result = ((c_user_interface_text *)get_text_internal())
                     ->compute_text_bounds(font_cache, 0, &text_rectangle, (float)definition.core.font, &text_rectangle, rectangle);
    }
    return result;
}

// @address 0x4e5910 ?assemble_render_data@c_gui_text_widget@@UAAXPAUs_gui_widget_render_data@@PBTrectangle2d@@W4e_controller_index@@_N33@Z
void c_gui_text_widget::assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
    e_controller_index local_controller_index, bool a, bool b, bool c)
{
    c_gui_widget::assemble_render_data(render_data, window_bounds, local_controller_index, a, b, c);
    real_argb_color color;
    bool use_debug_color = (m_controller_mask & (1 << 5)) != 0;
    if (use_debug_color)
    {
        const real_argb_color *selected_color;
        switch (m_type)
        {
        case 0:
            selected_color = &global_real_argb_colors[_real_argb_white];
            break;
        case 1:
            selected_color = g_gui_text_widget_color_1;
            break;
        case 2:
            selected_color = global_real_argb_green;
            break;
        case 3:
        case 4:
            selected_color = global_real_argb_blue;
            break;
        case 5:
            selected_color = &global_real_argb_colors[_real_argb_cyan];
            break;
        case 6:
            selected_color = g_gui_text_widget_color_6;
            break;
        case 7:
            selected_color = global_real_argb_magenta;
            break;
        case 8:
            selected_color = global_real_argb_orange;
            break;
        default:
            selected_color = &global_real_argb_colors[_real_argb_grey];
            break;
        }
        color = *selected_color;
    }
    else
    {
        get_cumulative_color_tint(&color);
    }
    real_argb_color shadow_color;
    shadow_color.alpha = 1.0f;
    shadow_color.rgb.blue = 0.0f;
    shadow_color.rgb.green = 0.0f;
    shadow_color.rgb.red = 0.0f;
    const s_user_interface_shared_globals *globals = user_interface_shared_tag_globals_try_and_get();
    if (globals != 0 && globals->shadow_color.alpha > 0.0f)
        shadow_color = globals->shadow_color;
    c_user_interface_text *text = get_text_internal();
    float shadow_alpha = 0.0f;
    if (text->get_drop_shadow_style() != 0)
        shadow_alpha = color.alpha * shadow_color.alpha;
    s_gui_text_widget_extra_large_render_data *text_render_data = static_cast<s_gui_text_widget_extra_large_render_data *>(render_data);
    text = get_text_internal();
    text_render_data->font = text->get_font();
    text = get_text_internal();
    text_render_data->style = text->get_style();
    text = get_text_internal();
    text_render_data->justification = text->get_justification();
    text = get_text_internal();
    text_render_data->shadow_style = text->get_drop_shadow_style();
    text_render_data->color = real_a_rgb_color_to_pixel32(color.alpha, &color.rgb);
    text_render_data->shadow_color = real_a_rgb_color_to_pixel32(shadow_alpha, &shadow_color.rgb);
    text = get_text_internal();
    text_render_data->wrap_horizontally = text->get_wrap_horizontally();
    text = get_text_internal();
    text_render_data->align_vertically = text->get_align_vertically();
    text_render_data->tab_count = 16;
    text_render_data->rotation = m_animated_state.rotation_angle;
    get_text_internal()->get_tab_stops(text_render_data->tabs, &text_render_data->tab_count);
    compute_layout_bounds(window_bounds, m_animated_state.rotation_pivot, m_animated_state.position.x,
        m_animated_state.position.y, m_animated_state.position.z,
        m_animated_state.scale.i, m_animated_state.scale.j, &text_render_data->rotation_origin, &text_render_data->bounds,
        &text_render_data->clip_bounds, &text_render_data->scale);
    wchar_t *render_text = text_render_data->text;
    long text_length = get_text_buffer_size();
    ustrnzcpy(render_text, get_text(), text_length);
    if (get_text_internal()->get_render_uppercase())
        ustrnupr(render_text, NUMBEROF(text_render_data->text));
}
