// interfacelibxbox/c_gui_bitmap_widget.obj: from the records 0x4e49e0 0x4e6438 0x4e64e8 0x4e6578 0x4e6580 0x4e6760 0x4e6908 0x4e6988 0x4e6cc0 0x4e6d20 0x4e6e08 0x4e6ed0 0x4e6f98 0x4e70c0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "render/render_texture_camera_globals.h"
#include "math/real_math_types.h"
#include "bitmaps/bitmap_access.h"
#include "interface/gui_widget_hierarchy.h"
#include "math/real_math.h"
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "interface/gui_widget_types.h"
#include "interface/core_widget_definition.h"
#include "interface/gui_data.h"
#include "interface/user_interface.h"
#include "bitmaps/bitmap_group.h"
#include "interface/interface_constants.h"
#include "objects/emblems.h"
#include "rasterizer/rasterizer_main.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/bitmap_widget_definition.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "interface/c_gui_widget.h"
#include "bitmaps/bitmap_data.h"
#include "rasterizer/rasterizer_vertex_screen_types.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "rasterizer/rasterizer_render_target_types.h"
#include "interface/controller_inline.h"
#include "interface/c_player_profile.h"
#include "rasterizer/dx9/rasterizer_dynamic_screen_geometry_types.h"
#include "rasterizer/dx9/rasterizer_dynamic_screen_calls.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "render/screen_postprocess.h"
#include "rasterizer/rasterizer_dynamic_render_targets.h"
#include "interface/gui_custom_bitmap_storage_item.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "cseries/crt_xdk.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include <math.h>
#include "interface/user_interface_shared_globals.h"
#include "rasterizer/rasterizer_dynamic_render_target_types.h"
#include "memory/data_typed_access.h"
#include "math/color_math.h"
#include "interface/c_gui_list_item_widget.h"
// @address 0x81da20 g_rasterizer_last_rectangles
// @address 0x832a88 g_rasterizer_surface_groups

static __forceinline real_vector2d c_gui_bitmap_widget_authoring_to_target_dimensions_scaling()
{
    calculate_authoring_to_target_dimensions_scaling();
    return g_interface_authoring_to_target_dimensions_scaling;
}

// @address 0x4e49e0 c_gui_bitmap_widget_clip_points_to_bounds
extern "C" void c_gui_bitmap_widget_clip_points_to_bounds(real_point2d *points, const real_rectangle2d *bounds)
{
    if (points[0].x < bounds->x0)
        points[0].x = bounds->x0;
    else if (points[0].x > bounds->x1)
        points[0].x = bounds->x1;
    if (points[0].y < bounds->y0)
        points[0].y = bounds->y0;
    else if (points[0].y > bounds->y1)
        points[0].y = bounds->y1;
    if (points[1].x < bounds->x0)
        points[1].x = bounds->x0;
    else if (points[1].x > bounds->x1)
        points[1].x = bounds->x1;
    if (points[1].y < bounds->y0)
        points[1].y = bounds->y0;
    else if (points[1].y > bounds->y1)
        points[1].y = bounds->y1;
    if (points[2].x < bounds->x0)
        points[2].x = bounds->x0;
    else if (points[2].x > bounds->x1)
        points[2].x = bounds->x1;
    if (points[2].y < bounds->y0)
        points[2].y = bounds->y0;
    else if (points[2].y > bounds->y1)
        points[2].y = bounds->y1;
    if (points[3].x < bounds->x0)
        points[3].x = bounds->x0;
    else if (points[3].x > bounds->x1)
        points[3].x = bounds->x1;
    if (points[3].y < bounds->y0)
        points[3].y = bounds->y0;
    else if (points[3].y > bounds->y1)
        points[3].y = bounds->y1;
}

// @address 0x4e6438 ??0c_gui_bitmap_widget@@QAA@XZ
c_gui_bitmap_widget::c_gui_bitmap_widget() :
    c_gui_widget(_gui_bitmap_widget), m_override_bitmap_index(-1), m_bitmap_override(-1), m_sequence_override(-1), m_definition()
{
}

// @address 0x4e64e8 ?initialize@c_gui_bitmap_widget@@UAAXPBUs_bitmap_widget_block@@@Z
void c_gui_bitmap_widget::initialize(const struct s_bitmap_widget_block *bitmap_widget_block)
{
    if (bitmap_widget_block != 0)
    {
        const s_bitmap_widget_block *block = bitmap_widget_block;
        real_rectangle2d parent_bounds;
        robust_pointer_get_repaired_value(&m_parent)->get_authored_bounds(&parent_bounds);
        s_runtime_bitmap_widget_definition *definition = &m_definition;
        bool replace = false;
        if (block->bitmap_widget_template.index != -1)
        {
            add_definition_fields((const s_bitmap_widget_definition *)tag_get('bmp3', block->bitmap_widget_template.index),
                definition, &parent_bounds, replace);
            replace = true;
        }
        add_definition_fields(&block->definition, definition, &parent_bounds, replace);
        m_name = m_definition.name;
    }
    c_gui_widget::initialize();
}

// @address 0x4e6578 ?get_core_definition@c_gui_bitmap_widget@@UAAPAUs_runtime_core_widget_definition@@XZ
s_runtime_core_widget_definition *c_gui_bitmap_widget::get_core_definition()
{
    return &m_definition;
}

// @address 0x4e6580 ?get_current_bounds@c_gui_bitmap_widget@@UAAPATreal_rectangle2d@@PAT2@@Z
real_rectangle2d *c_gui_bitmap_widget::get_current_bounds(real_rectangle2d *bounds)
{
    c_gui_widget::get_current_bounds(bounds);
    if (!(get_core_definition()->flags & 8))
    {
        const bitmap_data *bitmap = get_current_bitmap();
        if (bitmap != 0)
        {
            float width_scale = 1.0f;
            float height_scale = 1.0f;
            real_vector2d dimensions_scale = c_gui_bitmap_widget_authoring_to_target_dimensions_scaling();
            real_vector2d aspect_scale = interface_get_authoring_aspect_conversion_scaling(interface_get_closest_authored_ui_aspect_ratio());
            if (m_definition.bitmap_definition_index != -1 &&
                (unsigned short)m_animated_state.bitmap_sequence_index != 0xffff &&
                (unsigned short)m_animated_state.bitmap_frame_index != 0xffff)
            {
                const bitmap_group *group = (const bitmap_group *)tag_get('bitm', m_definition.bitmap_definition_index);
                if (group != 0 && group->sequences.count > 0)
                {
                    real_rectangle2d *sprite_bounds = bitmap_group_get_sprite_bounds_from_sequence(m_definition.bitmap_definition_index,
                        m_animated_state.bitmap_sequence_index, m_animated_state.bitmap_frame_index);
                    if (sprite_bounds != 0)
                    {
                        width_scale = sprite_bounds->x1 - sprite_bounds->x0;
                        height_scale = sprite_bounds->y1 - sprite_bounds->y0;
                        bounds->x0 = bounds->x0 - group->sprite_spacing * aspect_scale.i * dimensions_scale.i * 0.5f;
                        bounds->y0 = bounds->y0 - group->sprite_spacing * aspect_scale.j * dimensions_scale.j * 0.5f;
                    }
                }
            }
            bounds->x1 = bitmap->width * aspect_scale.i * dimensions_scale.i * width_scale + bounds->x0;
            bounds->y1 = bitmap->height * aspect_scale.j * dimensions_scale.j * height_scale + bounds->y0;
        }
    }
    return bounds;
}

// @address 0x4e6760 ?update_render_state@c_gui_bitmap_widget@@UAAXK@Z
void c_gui_bitmap_widget::update_render_state(unsigned long current_milliseconds)
{
    this->c_gui_widget::update_render_state(current_milliseconds);
    unsigned int flags = get_core_definition()->flags;
    bool bitmap_from_data = (flags >> 6) & 1;
    bool sequence_from_data = (flags >> 7) & 1;
    if (m_sequence_override != -1)
        m_animated_state.bitmap_sequence_index = (short)m_sequence_override;
    if (m_bitmap_override != -1)
    {
        m_animated_state.bitmap_frame_index = (short)m_bitmap_override;
    }
    else if (bitmap_from_data || sequence_from_data)
    {
        c_gui_data *data = 0;
        long element_handle = -1;
        c_gui_screen_widget *screen = this->get_parent_screen();
        long data_name = m_definition.value_identifier;
        if (screen && m_definition.value_override_list != 0 && m_definition.value_override_list != -1)
        {
            c_gui_widget *list = 0;
            if (m_definition.value_override_list == 0x10007)
            {
                if (!screen->m_suppress_focus && screen->m_focused_widget)
                    list = gui_widget_get_parent_list_widget(screen->m_focused_widget);
            }
            else
            {
                list = ((c_gui_widget *)screen)->get_child_widget(_gui_list_widget, m_definition.value_override_list);
            }
            if (list)
            {
                data = ((c_gui_list_widget *)list)->get_data();
                element_handle = ((c_gui_list_widget *)list)->get_focused_element_handle();
            }
        }
        else
        {
            data = gui_widget_get_parent_list_widget(this)
                ? static_cast<c_gui_list_widget *>(gui_widget_get_parent_list_widget(this))->get_data()
                : 0;
            element_handle = get_element_handle();
        }
        long value;
        if (data && element_handle != -1 &&
            data->get_integer_value(element_handle, data_name, &value))
        {
            if (bitmap_from_data)
                m_animated_state.bitmap_frame_index = (short)value;
            else if (sequence_from_data)
                m_animated_state.bitmap_sequence_index = (short)value;
        }
        else
        {
            m_animated_state.bitmap_sequence_index = -1;
            m_animated_state.bitmap_frame_index = -1;
        }
    }
}

void c_gui_bitmap_widget::set_animated_state_baseline(s_animation_transform *baseline)
{
    c_gui_widget::set_animated_state_baseline(baseline);
    baseline->bitmap_sequence_index = m_definition.initial_sprite_sequence;
    baseline->bitmap_frame_index = m_definition.initial_sprite_frame;
}

// @address 0x4e6ed0 ?get_ambient_state@c_gui_bitmap_widget@@MAA?AW4e_animation_state@@XZ
e_animation_state c_gui_bitmap_widget::get_ambient_state()
{
    int ambient_state = -1;
    c_gui_widget *parent_list = gui_widget_get_parent_list_widget(this);
    if (parent_list != 0)
    {
        const c_gui_list_widget *list = (const c_gui_list_widget *)parent_list;
        if (this == list->m_more_items_preceeding_indicator && parent_list->get_enabled())
            ambient_state = (((c_gui_list_widget *)parent_list)->list_has_more_elements_preceeding() == 0) + 18;
        else if (this == list->m_more_items_following_indicator && parent_list->get_enabled())
            ambient_state = (((c_gui_list_widget *)parent_list)->list_has_more_elements_following() == 0) + 18;
    }
    if (ambient_state == -1)
        ambient_state = c_gui_widget::get_ambient_state();
    return (e_animation_state)ambient_state;
}

// @address 0x4e6cc0 ?within_focus_chain@c_gui_bitmap_widget@@UAA_NXZ
bool c_gui_bitmap_widget::within_focus_chain()
{
    c_gui_widget *list_item = gui_widget_get_parent_list_item_widget(this);
    if (list_item != 0)
        return list_item->within_focus_chain();
    return c_gui_widget::within_focus_chain();
}

// @address 0x4e6d20 ?add_definition_fields@c_gui_bitmap_widget@@SAXPBUs_bitmap_widget_definition@@PAUs_runtime_bitmap_widget_definition@@PATreal_rectangle2d@@_N@Z
void c_gui_bitmap_widget::add_definition_fields(const s_bitmap_widget_definition *source_definition,
    s_runtime_bitmap_widget_definition *dest_definition, real_rectangle2d *positioning_bounds, bool replace)
{
    c_gui_widget::add_definition_fields(_gui_bitmap_widget, &source_definition->core, dest_definition, positioning_bounds, replace);
    if (source_definition->bitmap.index != -1 && source_definition->bitmap.index != 0)
        dest_definition->bitmap_definition_index = source_definition->bitmap.index;
    if (source_definition->shader.index != -1 && source_definition->shader.index != 0)
        dest_definition->shader_definition_index = source_definition->shader.index;
    short blend_method = (short)source_definition->blend_method;
    if (blend_method >= 0 && blend_method < 13 && blend_method != 0)
        dest_definition->blend_method = blend_method;
    if ((short)source_definition->initial_sprite_sequence > 0)
        dest_definition->initial_sprite_sequence = source_definition->initial_sprite_sequence;
    if ((short)source_definition->initial_sprite_frame > 0)
        dest_definition->initial_sprite_frame = source_definition->initial_sprite_frame;
    if (source_definition->value_identifier > 0)
        dest_definition->value_identifier = source_definition->value_identifier;
    if (source_definition->value_override_list > 0)
        dest_definition->value_override_list = source_definition->value_override_list;
}

const bitmap_data *c_gui_bitmap_widget::get_current_bitmap() const
{
    const bitmap_data *bitmap = 0;
    long bitmap_index = m_definition.bitmap_definition_index;
    if (m_override_bitmap_index != -1)
        bitmap_index = m_override_bitmap_index;
    if (bitmap_index != -1)
    {
        long sequence_index = get_animated_state()->bitmap_sequence_index;
        long frame_index = get_animated_state()->bitmap_frame_index;
        const bitmap_group *group = (const bitmap_group *)tag_get('bitm', m_definition.bitmap_definition_index);
        if (group)
        {
            if (sequence_index >= 0 &&
                sequence_index < group->sequences.count)
            {
                if (frame_index >= 0 &&
                    frame_index <
                        ((const bitmap_sequence *)group->sequences.address)[sequence_index].sprites.count)
                    bitmap = bitmap_group_get_bitmap_from_sequence(m_definition.bitmap_definition_index,
                        sequence_index, frame_index);
            }
            else if (frame_index >= 0 &&
                     frame_index < group->bitmaps.count)
            {
                bitmap = (const bitmap_data *)tag_block_get_element_with_size(&group->bitmaps, frame_index, sizeof(bitmap_data));
            }
        }
    }
    return bitmap;
}

// @address 0x4e6f98 ?get_bitmap_and_hardware_format@@YAPAUbitmap_data@@JJJPAVc_rasterizer_texture_ref@@@Z
struct bitmap_data * get_bitmap_and_hardware_format(long bitmap_tag, long sequence_index, long frame_index, class c_rasterizer_texture_ref * texture)
{
    bitmap_data *bitmap = 0;
    if (bitmap_tag != -1)
    {
        const bitmap_group *group = (const bitmap_group *)tag_get('bitm', bitmap_tag);
        if (group)
        {
            const bitmap_data *hardware_bitmap;
            if (group->sequences.count > 0)
            {
                if (sequence_index >= 0 && sequence_index < group->sequences.count && frame_index >= 0 &&
                    frame_index < ((const bitmap_sequence *)group->sequences.address)[sequence_index].sprites.count)
                {
                    long bitmap_index = bitmap_group_get_bitmap_index_from_sequence(bitmap_tag, (short)sequence_index, (short)frame_index);
                    if (::bitmap_group_get_xenon_hardware_format(bitmap_tag, bitmap_index, texture,
                            &hardware_bitmap, (__int64 *)(0)))
                        bitmap = bitmap_group_get_bitmap_from_sequence(bitmap_tag, (short)sequence_index, (short)frame_index);
                }
            }
            else if (frame_index >= 0 && frame_index < group->bitmaps.count)
            {
                if (::bitmap_group_get_xenon_hardware_format(bitmap_tag, frame_index, texture,
                        &hardware_bitmap, (__int64 *)(0)))
                    bitmap = ((bitmap_data *)group->bitmaps.address) + frame_index;
            }
        }
    }
    return bitmap;
}


// @address 0x4e6988 ?assemble_render_data@c_gui_bitmap_widget@@UAAXPAUs_gui_widget_render_data@@PBTrectangle2d@@W4e_controller_index@@_N33@Z
void c_gui_bitmap_widget::assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
    e_controller_index local_controller_index, bool a, bool b, bool c)
{
    c_gui_widget::assemble_render_data(render_data, window_bounds, local_controller_index, a, b, c);
    s_gui_bitmap_widget_render_data *bitmap_render_data = static_cast<s_gui_bitmap_widget_render_data *>(render_data);
    s_player_appearance appearance;
    unsigned long definition_flags = m_definition.flags;
    bool scaled = (definition_flags >> 4) & 1;
    bool player_emblem = (definition_flags >> 5) & 1;
    if (scaled)
        bitmap_render_data->flags |= 4;
    else
        bitmap_render_data->flags &= ~4;
    if (player_emblem)
        bitmap_render_data->flags |= 8;
    else
        bitmap_render_data->flags &= ~8;
    long bitmap_definition_index = m_override_bitmap_index;
    if (bitmap_definition_index == -1)
        bitmap_definition_index = m_definition.bitmap_definition_index;
    bitmap_render_data->bitmap_definition_index = bitmap_definition_index;
    if (!scaled)
    {
        if (player_emblem)
        {
            bool found = get_parent_screen()->get_emblem_info_for_widget(this, &bitmap_render_data->emblem);
            if (!found)
            {
                long element_handle = get_element_handle();
                if (element_handle != -1)
                {
                    c_gui_widget *list = gui_widget_get_parent_list_widget(this);
                    if (list)
                    {
                        c_gui_data *data = (c_gui_data *)((c_gui_list_widget *)list)->get_data();
                        if (data && data->get_player_appearance(element_handle, &appearance))
                        {
                            found = true;
                            bitmap_render_data->emblem = appearance.emblem;
                        }
                    }
                }
            }
            if (!found)
            {
                if (bitmap_render_data->local_controller_index >= _controller0 && bitmap_render_data->local_controller_index < k_number_of_controllers)
                {
                    found = true;
                    bitmap_render_data->emblem = controller_get(bitmap_render_data->local_controller_index)->get_player_profile_interface()->get_emblem_info();
                }
            }
            if (found)
                bitmap_render_data->flags |= 0x10;
            else
                bitmap_render_data->flags &= ~0x10;
        }
        else
        {
            bitmap_render_data->sprite.sequence_index = m_animated_state.bitmap_sequence_index;
            bitmap_render_data->sprite.frame_index = m_animated_state.bitmap_frame_index;
        }
    }
    {
        real_argb_color tint;
        const real_argb_color *cumulative_tint = get_cumulative_color_tint(&tint);
        bitmap_render_data->tint = real_a_rgb_color_to_pixel32(cumulative_tint->alpha, &cumulative_tint->rgb);
    }
    bitmap_render_data->texture_offset = m_animated_state.texture_offset;
    bitmap_render_data->blend_method = PIN(m_definition.blend_method, 0, 12);
    bitmap_render_data->shader_definition_index = m_definition.shader_definition_index;
    bool clip_to_window = (bitmap_render_data->flags >> 2) & 1;
    if (clip_to_window)
    {
        real_rectangle2d bounds;
        bounds.x0 = (float)window_bounds->x0;
        bounds.x1 = (float)window_bounds->x1;
        bounds.y0 = (float)window_bounds->y0;
        bounds.y1 = (float)window_bounds->y1;
        c_gui_bitmap_widget_clip_points_to_bounds(bitmap_render_data->corners, &bounds);
    }
}

static __forceinline float c_gui_bitmap_widget_quad_width(const gui_real_rectangle2d *bounds)
{
    real_vector2d edge;
    edge.i = bounds->points[2].x - bounds->points[0].x;
    edge.j = bounds->points[2].y - bounds->points[0].y;
    return sqrtf(edge.i * edge.i + edge.j * edge.j);
}

static __forceinline float c_gui_bitmap_widget_quad_height(const gui_real_rectangle2d *bounds)
{
    real_vector2d edge;
    edge.i = bounds->points[1].x - bounds->points[0].x;
    edge.j = bounds->points[1].y - bounds->points[0].y;
    return sqrtf(edge.i * edge.i + edge.j * edge.j);
}

static __forceinline s_dynamic_render_target *c_gui_bitmap_widget_get_dynamic_target()
{
    long index = g_dynamic_render_target_indices[1];
    return index != NONE ? datum_get_typed<s_dynamic_render_target>(g_dynamic_render_targets, index) : NULL;
}

// @rdata 0x8202490c
static const float c_gui_bitmap_widget_minimum_dimension = 0.0001f;
// @rdata 0x82026484
static const float c_gui_bitmap_widget_default_blur = 1.5f;

static __forceinline const bitmap_data *c_gui_bitmap_widget_precache_bitmap(const bitmap_data *bitmap, float priority)
{
    return bitmap;
}

static __forceinline void c_gui_bitmap_widget_offset_bounds(unsigned long remaining_corners, gui_real_rectangle2d *bounds, const real_point2d *origin,
                                                            bool offset)
{
    if (offset)
    {
        for (unsigned long corner = 0; corner < remaining_corners; ++corner)
        {
            bounds->points[corner].x += origin->x;
            bounds->points[corner].y += origin->y;
        }
    }
}

// @address 0x4e70c0 c_gui_bitmap_widget_render
extern "C" void c_gui_bitmap_widget_render(const s_gui_bitmap_widget_render_data *render_data, const rectangle2d *window_bounds)
{
    real_point2d window_origin;
    window_origin.x = (float)window_bounds->x0;
    window_origin.y = (float)window_bounds->y0;
    float width = c_gui_bitmap_widget_quad_width(&render_data->projected_bounds);
    float height = c_gui_bitmap_widget_quad_height(&render_data->projected_bounds);
    if (width > c_gui_bitmap_widget_minimum_dimension && height > c_gui_bitmap_widget_minimum_dimension)
    {
        short display_width = (short)c_rasterizer::render_globals.width;
        short display_height = (short)c_rasterizer::render_globals.height;
        rasterizer_main_get_video_mode();
        float screen_width = (float)display_width;
        float screen_height = (float)display_height;
        c_rasterizer_texture_ref texture;
        c_rasterizer_texture_ref secondary_texture;
        const bitmap_data *bitmap = NULL;
        const bitmap_data *secondary_bitmap = NULL;
        long shader_index = NONE;
        float u0 = 0.0f, texture_top = 0.0f, u1 = 1.0f, texture_bottom = 1.0f;
        bool flipped = false;
        if ((bool)((render_data->flags >> 1) & 1))
        {
            s_dynamic_render_target *target = c_gui_bitmap_widget_get_dynamic_target();
            if (target)
            {
                float aspect_ratio = width / height;
                float target_aspect_ratio = g_render_texture_camera_globals->aspect_ratio;
                if (target_aspect_ratio > c_gui_bitmap_widget_minimum_dimension && aspect_ratio > c_gui_bitmap_widget_minimum_dimension)
                {
                    if (target_aspect_ratio > aspect_ratio)
                    {
                        const float &ratio = aspect_ratio / target_aspect_ratio;
                        const float &border = 1.0f - ratio;
                        u0 = border * 0.5f;
                        u1 = 1.0f - u0;
                    }
                    else if (target_aspect_ratio < aspect_ratio)
                    {
                        const float &border = 1.0f - target_aspect_ratio / aspect_ratio;
                        texture_top = border * 0.5f;
                        texture_bottom = 1.0f - texture_top;
                    }
                    texture = target->accumulation_texture;
                }
            }
        }
        else if ((bool)((render_data->flags >> 2) & 1))
        {
            if (screen_width > c_gui_bitmap_widget_minimum_dimension && screen_height > c_gui_bitmap_widget_minimum_dimension)
            {
                float inverse_width = 1.0f / (float)(window_bounds->x1 - window_bounds->x0);
                float inverse_height = 1.0f / (float)(window_bounds->y1 - window_bounds->y0);
                u0 = (render_data->corners[0].x + 0.5f) * inverse_width;
                texture_top = (render_data->corners[2].y + 0.5f) * inverse_height;
                u1 = (render_data->corners[3].x + 0.5f) * inverse_width;
                texture_bottom = (render_data->corners[3].y + 0.5f) * inverse_height;
                const s_user_interface_shared_globals *globals = user_interface_shared_tag_globals_try_and_get();
                float blur_x = globals ? globals->blur_x : c_gui_bitmap_widget_default_blur;
                float blur_y = globals ? globals->blur_y : c_gui_bitmap_widget_default_blur;
                c_screen_postprocess::setup_rasterizer_for_postprocess(false);
                c_rasterizer::e_surface surface = c_screen_postprocess::blur_display(blur_x, blur_y);
                c_rasterizer::set_render_target(0, c_rasterizer::_surface_display);
                c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
                c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
                texture = c_rasterizer_texture_ref((D3DBaseTexture *)g_rasterizer_surface_groups[surface].get_specialized_surface(surface)->resource);
            }
        }
        else if ((bool)((render_data->flags >> 3) & 1))
        {
            bool valid_emblem =
                (bool)((render_data->flags >> 4) & 1) ? emblem_set_render_constants_for_emblem((const emblem_info *)&render_data->emblem) : false;
            if (valid_emblem)
            {
                bitmap = get_bitmap_and_hardware_format(render_data->bitmap_definition_index, 0, render_data->emblem.background, &texture);
                secondary_bitmap = get_bitmap_and_hardware_format(render_data->bitmap_definition_index, 0, render_data->emblem.foreground, &secondary_texture);
                shader_index = 63;
            }
        }
        else if ((bool)(render_data->flags & 1))
        {
            long bitmap_index = render_data->custom_storage_index;
            if (bitmap_index != NONE)
            {
                c_gui_custom_bitmap_storage_item *item = gui_custom_bitmap_storage_try_and_get(&g_gui_custom_bitmap_storage_data.data, bitmap_index);
                if (item && item->m_bitmap_ready)
                {
                    bitmap = &item->m_bitmap;
                    if (bitmap)
                    {
                        texture = c_rasterizer_texture_ref(bitmap->texture_ref);
                        shader_index = 88;
                    }
                }
            }
        }
        else if (render_data->bitmap_definition_index != NONE)
        {
            bitmap = get_bitmap_and_hardware_format(render_data->bitmap_definition_index, render_data->sprite.sequence_index, render_data->sprite.frame_index,
                                                    &texture);
            if (render_data->sprite.sequence_index != NONE && render_data->sprite.frame_index != NONE)
            {
                real_rectangle2d *bounds =
                    bitmap_group_get_sprite_bounds_from_sequence(
                        render_data->bitmap_definition_index, render_data->sprite.sequence_index, render_data->sprite.frame_index);
                if (bounds)
                {
                    u0 = bounds->x0;
                    texture_top = bounds->y0;
                    u1 = bounds->x1;
                    texture_bottom = bounds->y1;
                }
            }
            flipped = bitmap ? TEST_BIT(bitmap->flags, 2) : false;
        }

        bool bitmaps_ready = true;
        if (bitmap)
            bitmaps_ready &= c_gui_bitmap_widget_precache_bitmap(bitmap, 0.0f) != NULL;
        bitmaps_ready &= secondary_bitmap ? c_gui_bitmap_widget_precache_bitmap(secondary_bitmap, 0.0f) != NULL : true;
        if (bitmaps_ready)
        {
            rasterizer_dynamic_screen_geometry_parameters parameters;
            gui_real_rectangle2d bounds = render_data->projected_bounds;
            memset(&parameters, 0, sizeof(parameters));
            c_gui_bitmap_widget_offset_bounds(4, &bounds, &window_origin, true);
            rasterizer_vertex_screen vertices[4];
            vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = render_data->tint;
            vertices[0].u = render_data->texture_offset.x + u0;
            vertices[0].v = render_data->texture_offset.y + texture_top;
            vertices[0].x = bounds.points[0].x;
            vertices[0].y = bounds.points[0].y;
            vertices[flipped ? 3 : 1].u = render_data->texture_offset.x + u1;
            vertices[flipped ? 3 : 1].v = render_data->texture_offset.y + texture_top;
            vertices[1].x = bounds.points[2].x;
            vertices[1].y = bounds.points[2].y;
            vertices[2].u = render_data->texture_offset.x + u1;
            vertices[2].v = render_data->texture_offset.y + texture_bottom;
            vertices[2].x = bounds.points[3].x;
            vertices[2].y = bounds.points[3].y;
            vertices[flipped ? 1 : 3].u = render_data->texture_offset.x + u0;
            vertices[flipped ? 1 : 3].v = render_data->texture_offset.y + texture_bottom;
            vertices[3].x = bounds.points[1].x;
            vertices[3].y = bounds.points[1].y;
            calculate_aspect_ratio_scaling();
            real_vector2d scale = g_interface_aspect_ratio_scaling;
            rasterizer_scale_screen_vertices(vertices, 4, &scale);
            parameters.texture_address[0] = render_data->texture_offset.x != 0.0f || render_data->texture_offset.y != 0.0f;
            parameters.scale1.i = parameters.scale1.j = 1.0f;
            parameters.scale0.i = parameters.scale0.j = 1.0f;
            unsigned long sampler_mask = 0;
            if (texture.valid())
            {
                parameters.textures[0] = texture;
                sampler_mask |= 1;
            }
            if (secondary_texture.valid())
            {
                parameters.textures[1] = secondary_texture;
                sampler_mask |= 2;
                parameters.texture_address[1] = parameters.texture_address[0];
                parameters.secondary_scale = parameters.scale1;
            }
            parameters.alpha_blend_mode = (short)PIN(render_data->blend_method, 0, 12);
            parameters.explicit_shader_index = shader_index;
            rasterizer_draw_dynamic_screen_quad(&parameters, vertices);
            c_rasterizer::clear_sampler_textures(sampler_mask);
        }
    }
}
