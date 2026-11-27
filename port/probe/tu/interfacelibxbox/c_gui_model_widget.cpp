// @address 0x761454 ?g_observer_default_field_of_view@@3MA
#include "camera/observer.h"
// interfacelibxbox/c_gui_model_widget.obj: from the records 0x4e7e60 0x4e7ef8 0x4e7f98 0x4e80b8 0x4e8138 0x4e8140 0x4e82b0 0x4e83d8
// @compiler xdk6021
// @flags /TP /O2 /Oc
#include "memory/datum_index.h"
#include "game/game_state_accessors.h"
#include "text/unicode.h"
#include "render/render_texture_camera_globals.h"
#include "interface/c_player_profile.h"
#include "interface/gui_model_widget_externs.h"
#include "game/player_color_types.h"
#include "memory/tls_globals.h"
#include "interface/controller_types.h"
#include "tag_files/tag_groups.h"
#include <string.h>
#include "game/game_globals_storage.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "game/game_types.h"
// @address 0x4e7e60 ?initialize@c_gui_model_widget@@UAAXPBUs_model_widget_block@@@Z
// @address 0x4e8138 ?get_core_definition@c_gui_model_widget@@UAAPAUs_runtime_core_widget_definition@@XZ
// @address 0x4e8140 ?add_definition_fields@c_gui_model_widget@@QAAXPBUs_model_widget_definition@@_N@Z
// @address 0x4e83d8 ?assemble_render_data@c_gui_model_widget@@UAAXPAUs_gui_widget_render_data@@PBTrectangle2d@@W4e_controller_index@@_N33@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
#include "math/real_math.h"
#include "objects/object_header.h"
#include "objects/objects.h"
#include "interface/user_interface_utilities.h"
#include "math/color_math.h"
#include "interface/c_gui_model_widget.h"
#include "interface/gui_model_widget_types.h"
#include "interface/model_widget_definition.h"
#include "interface/controller_inline.h"
#include "physics/havok_monitor_storage.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/core_widget_definition.h"
#include "math/function_definitions.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/list_widget_definition.h"
#include "interface/user_interface_messages.h"
#include "objects/object_reference_types.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_group_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_animation_transform.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_widget.h"
class c_font_cache;
struct s_widget_animation_definition;
class c_gui_button_key_widget;
class c_gui_model_widget;
struct s_group_widget_definition;
struct s_button_key_definition;
struct s_model_widget_block;
void c_gui_model_widget::initialize(const s_model_widget_block *model_widget_block)
{
    if (model_widget_block != 0)
    {
        bool require_nonzero_bounds = false;
        if (model_widget_block->model_widget_template.index != -1)
        {
            const cache_file_tag_instance *instance = &global_tag_instances[datum_index_to_absolute_index(model_widget_block->model_widget_template.index)];
            add_definition_fields((const s_model_widget_definition *)instance->definition, require_nonzero_bounds);
            require_nonzero_bounds = true;
        }
        add_definition_fields(&model_widget_block->definition, require_nonzero_bounds);
    }
    m_name = m_definition.name;
    c_gui_widget::initialize();
    g_render_texture_camera_globals->enabled = true;
}

s_runtime_core_widget_definition *c_gui_model_widget::get_core_definition()
{
    return &m_definition;
}

void c_gui_model_widget::add_definition_fields(const s_model_widget_definition *model_widget_definition, bool require_nonzero_bounds)
{
    long bounds[6];
    robust_pointer_get_repaired_value(&m_parent)->get_authored_bounds((real_rectangle2d *)bounds);
    ::c_gui_widget::add_definition_fields((e_gui_widget_type)m_type, (s_core_widget_definition const *)model_widget_definition,
        (s_runtime_core_widget_definition *)&m_definition, (real_rectangle2d const *)bounds, require_nonzero_bounds);
    const s_model_widget_definition *definition = model_widget_definition;
    for (long i = 0; i < definition->elements.count(); i++)
    {
        const s_model_widget_tag_element *source = &definition->elements[i];
        m_definition.elements[i].identifier = source->identifier;
        m_definition.elements[i].field_of_view = source->body.bounds4.x0;
        m_definition.elements[i].lower = source->body.bounds4.x1;
        m_definition.elements[i].upper = source->body.bounds4.y0;
        m_definition.elements[i].scale = source->body.bounds4.y1;
        m_definition.elements[i].height_lower = source->body.bounds20.x0;
        m_definition.elements[i].height_upper = source->body.bounds20.x1;
        m_definition.elements[i].height_scale = source->body.bounds20.y0;
        m_definition.elements[i].rotation_scale = source->body.bounds20.y1;
        if (source->body.distance_function_block.count > 0)
            m_definition.elements[i].distance_function = (const c_function_definition *)source->body.distance_function_block.address;
        if (source->body.height_function_block.count > 0)
            m_definition.elements[i].height_function = (const c_function_definition *)source->body.height_function_block.address;
    }
}




#if !defined(_XBOX) || defined(__clang__)
// tmp/probe/tu/interfacelibxbox/user_interface.cpp (GAME-CHANGES "High-Hz camera"): this main-loop pass's time in the
// console's 60 Hz frames; 1 up to HALO3_FPS=60.
float user_interface_pass_console_frames();
#endif

// @address 0x4e82b0 ?handle_camera_input@c_gui_model_widget@@QAA_NMM@Z
bool c_gui_model_widget::handle_camera_input(float angle_delta, float position_factor)
{
#if !defined(_XBOX) || defined(__clang__)
    // tmp/probe (GAME-CHANGES "High-Hz camera"): gui_screen_widget_spin_models_with_dpad steps the model one degree
    // (one zoom step) for each alt-stick event, and the event manager posts one every main-loop pass the right stick is
    // held: one a console frame, so 60 degrees a second. Above 60 Hz a pass is part of a console frame; it steps that
    // part, so the model turns 60 degrees a second at any frame rate and by an even step every frame. Up to 60 Hz the
    // steps are the console's.
    const float console_frames = user_interface_pass_console_frames();
    angle_delta *= console_frames;
    position_factor *= console_frames;
#endif
    s_runtime_model_widget_element *slot = get_current_model_element();
    float displacement = slot->scale * position_factor;
    float span = slot->upper - slot->lower;
    float position = m_zoom + displacement;
    float angle = m_yaw + angle_delta;
    m_yaw = angle;
    // match: 0x4e82b0: value range assumption guides branch lowering
    __assume(m_yaw == angle);
    m_zoom = position;
    while (m_yaw > 360.0f)
        m_yaw = m_yaw - 360.0f;
    while (m_yaw < 0.0f)
        m_yaw = m_yaw + 360.0f;

    float bounded;
    if (position > 0.0f)
        bounded = position;
    else
        bounded = 0.0f;
    float result;
    if (bounded < span)
    {
        if (position > 0.0f)
            result = position;
        else
            result = 0.0f;
    }
    else
    {
        result = span;
    }
    m_zoom = result;
    c_gui_screen_widget *parent = get_parent_screen();
    if (parent != 0)
    {
        bool status = true;
        if (start_menu_pane_is_valid_type(parent) == 0)
            return status;
        ((c_start_menu_pane_screen_widget *)parent)->m_translation = m_zoom;
        ((c_start_menu_pane_screen_widget *)parent)->m_rotation = m_yaw;
        return status;
    }
    return true;
}

// @address 0x4e7ef8 ?post_initialize@c_gui_model_widget@@UAAXXZ
void c_gui_model_widget::post_initialize()
{
    float position = 0.0f;
    float angle = 0.0f;
    c_gui_widget::post_initialize();
    reset_player_appearance();
    c_gui_screen_widget *parent = get_parent_screen();
    if (parent != 0 && start_menu_pane_is_valid_type(parent) != 0)
    {
        position = ((c_start_menu_pane_screen_widget *)parent)->m_translation;
        angle = ((c_start_menu_pane_screen_widget *)parent)->m_rotation;
    }
    m_zoom = position;
    m_yaw = angle;
    handle_camera_input(0.0f, 0.0f);
}

extern "C" {
}

// @address 0x4e7f98 ?reset_player_appearance@c_gui_model_widget@@QAAXXZ
void c_gui_model_widget::reset_player_appearance()
{
    c_gui_screen_widget *screen = get_parent_screen();
    if (screen != 0)
    {
        e_controller_index controller_index = ((c_gui_widget *)screen)->get_any_responding_controller();
        if ((unsigned long)controller_index <= 3)
        {
            const c_player_profile_interface *profile = controller_get(controller_index)->get_player_profile_interface();
            s_player_appearance appearance;
            memset(&appearance, 0, sizeof(appearance));
            appearance.primary_color = (signed char)profile->customization.primary_change_color;
            appearance.secondary_color = (signed char)profile->customization.secondary_change_color;
            appearance.tertiary_color = (signed char)profile->customization.tertiary_change_color;
            unsigned int model = profile->customization.player_model_choice;
            if (model >= 2)
                model = 0;
            appearance.model_choice = (signed char)model;
            appearance.emblem = profile->customization.emblem_info;
            int choice = 0;
            do {
                int index = 0;
                do {
                    appearance.selections[choice][index] = profile->get_model_customization_selection((e_player_model_choice)choice, index);
                    index++;
                } while (index < 4);
                choice++;
            } while (choice < 2);
            ustrnzcpy_inline(appearance.service_tag, profile->get_desired_service_tag(), 4);
            set_player_appearance(&appearance, -1);
        }
    }
}

extern "C" {
}

// @address 0x4e80b8 ?set_player_appearance@c_gui_model_widget@@QAAXPBUs_player_appearance@@J@Z
void c_gui_model_widget::set_player_appearance(const s_player_appearance *appearance, long appearance_index)
{
    if (memcmp(&m_player_appearance, appearance, sizeof(s_player_appearance)) != 0 || m_appearance_index != appearance_index)
    {
        memcpy(&m_player_appearance, appearance, sizeof(s_player_appearance));
        m_appearance_index = appearance_index;
        user_interface_update_main_menu_character_model(appearance);
    }
}


// @address 0x74b910 g_havok_monitor_globals



static __forceinline unsigned long c_gui_model_widget_get_pixel_tint(c_gui_widget *widget, real_argb_color *tint)
{
    return real_argb_color_to_pixel32(widget->get_cumulative_color_tint(tint));
}


static __forceinline void c_gui_model_widget_set_biped_change_colors(long object_index, e_game_team team_index, const s_player_appearance *appearance)
{
    real_rgb_color colors[4];
    network_get_player_colors(team_index, false, (const c_enum<e_player_color_index, char, -1, 32> *)&appearance->primary_color, colors);
    if (object_header_get(object_index)->object_type == 0)
        object_set_base_change_color_by_index(object_index, 0, &colors[0]);
    if (object_header_get(object_index)->object_type == 0)
        object_set_base_change_color_by_index(object_index, 1, &colors[1]);
    if (object_header_get(object_index)->object_type == 0)
        object_set_base_change_color_by_index(object_index, 2, &colors[2]);
}

// @rdata 0x82024fd4
static const float k_model_degrees_to_radians = 0.017453292f;
// @rdata 0x820334a8
static const float k_model_minimum_camera_distance = 0.01f;
// @rdata 0x8203867c
static const float k_model_radians_to_degrees = 57.29578f;
// @rdata 0x820221fc
static const float k_model_unit_range = 1.0f;
// @rdata 0x8202490c
static const float k_model_distance_epsilon = 0.0001f;

void c_gui_model_widget::assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
    e_controller_index local_controller_index, bool apply_translation, bool apply_scale, bool apply_rotation)
{
    c_gui_widget::assemble_render_data(render_data, window_bounds, local_controller_index, apply_translation, apply_scale, apply_rotation);
    bool is_ui_shell = game_is_ui_shell();
    long object_index = user_interface_find_menu_character(&m_player_appearance);
    if (object_index != NONE)
    {
        s_runtime_model_widget_element *slot = get_current_model_element();
        float distance_range = slot->upper - slot->lower;
        float height_range = slot->height_upper - slot->height_lower;
        float abs_distance_range = distance_range < 0.0f ? -distance_range : distance_range;
        float zoom_fraction = abs_distance_range > k_model_distance_epsilon ? m_zoom / distance_range : 0.0f;
        const float &unit_range = static_cast<float>(k_model_unit_range);
        float distance_fraction = slot->distance_function ? slot->distance_function->evaluate_scalar(zoom_fraction, unit_range) : zoom_fraction;
        float height_fraction = slot->height_function ? slot->height_function->evaluate_scalar(zoom_fraction, unit_range) : zoom_fraction;
        float raw_distance = slot->lower + distance_fraction * distance_range;
        float distance = raw_distance > k_model_minimum_camera_distance ? raw_distance : k_model_minimum_camera_distance;
        float height = slot->height_lower + height_fraction * height_range;

        union
        {
            real_point3d object_center;
            real_vector3d camera_forward;
        };
        object_center = object_get(object_index)->bounding_center;

        real_vector3d camera_offset;
        {
        real_vector3d object_forward;
        object_get_orientation(object_index, &object_forward, NULL);
        object_forward.i = unit_range;
        object_forward.j = 0.0f;
        object_forward.k = 0.0f;

        camera_offset.i = object_forward.i * distance;
        camera_offset.j = object_forward.j * distance;
        camera_offset.k = object_forward.k * distance;

        float angle_radians = m_yaw * k_model_degrees_to_radians;
        float sine = (float)sin(angle_radians);
        float cosine = (float)cos(angle_radians);
        float rotated_i = object_forward.i * cosine - object_forward.j * sine;
        float rotated_j = object_forward.i * sine + object_forward.j * cosine;
        object_forward.i = rotated_i;
        object_forward.j = rotated_j;
        object_forward.k = 0.0f;
        fast_normalize3d(&object_forward);

        g_havok_monitor_globals.lock_count_4c++;
        object_set_position_direct(object_index, NULL, &object_forward, global_up3d, NULL, false);
        g_havok_monitor_globals.lock_count_4c--;
        }

        {
        real_point3d camera_position;
        camera_position.x = object_center.x + camera_offset.i;
        camera_position.y = object_center.y + camera_offset.j;
        camera_position.z = object_center.z + camera_offset.k;

        vector_from_points3d(&camera_position, &object_center, &camera_forward);
        fast_normalize3d(&camera_forward);

        real_vector3d camera_up = *global_up3d;
        camera_position.z += height;

        s_gui_model_widget_render_data *model_render_data = (s_gui_model_widget_render_data *)render_data;
        model_render_data->object_index = object_index;
        memcpy(&model_render_data->player_appearance, &m_player_appearance, sizeof(m_player_appearance));
        model_render_data->camera_position = camera_position;
        model_render_data->camera_up = camera_up;
        model_render_data->camera_forward = camera_forward;
        model_render_data->field_of_view = slot->field_of_view != 0.0f ? slot->field_of_view * k_model_degrees_to_radians : g_observer_default_field_of_view;

        real_argb_color tint;
        model_render_data->tint = c_gui_model_widget_get_pixel_tint(this, &tint);

        if (is_ui_shell)
            c_gui_model_widget_set_biped_change_colors(object_index, (e_game_team)m_appearance_index, &m_player_appearance);

        render_texture_camera_set_position_and_direction(true, &model_render_data->camera_position, &model_render_data->camera_forward,
            &model_render_data->camera_up, model_render_data->field_of_view *k_model_radians_to_degrees);
        }
    }
}
