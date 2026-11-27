// Title mouse input runs on the main thread, after user_interface_update has settled focus.
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/event_manager_types.h"
#include "interface/gui_screens/start_menu/gui_screen_start_menu.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/gui_widget_hierarchy.h"
#include "interface/interface_constants.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_clock.h"
#include "interface/user_interface_window_manager.h"
#include "memory/tls_globals.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "text/font_cache_types.h"

#include "d3d/vulkan_device.h"
#include "input/pointer_viewport.h"
#include "input/port_input.h"
#include "xam/guide.h"
#include "xam/xam_ui.h"

namespace {

struct pointer_hit {
    rectangle2d viewport;
    real_point2d point; // in the responding window, as in assemble_render_data
};

bool locate_pointer(const port_ui_mouse_event &event, e_window_index window, pointer_hit &hit)
{
    const int width = c_rasterizer::render_globals.width, height = c_rasterizer::render_globals.height;
    int target_w = width, target_h = height;
    port_vk_presented_extent(&target_w, &target_h);
    float u, v;
    if (!port_pointer_in_frame(event.x, event.y, event.width, event.height, target_w, target_h, &u, &v))
        return false;

    // The UI renderer already applied the authoring scale/anchoring to current_bounds.
    // Its window is in display pixels (including a split-screen origin); HD is 1:1.
    hit.viewport = g_window_manager.m_window_bounds[window];
    const float x = u * (float)width / g_interface_aspect_ratio_scaling.i;
    const float y = v * (float)height / g_interface_aspect_ratio_scaling.j;
    if (x < hit.viewport.x0 || x >= hit.viewport.x1 || y < hit.viewport.y0 || y >= hit.viewport.y1)
        return false;
    hit.point.x = x - hit.viewport.x0;
    hit.point.y = y - hit.viewport.y0;
    return true;
}

bool contains(c_gui_widget *widget, const pointer_hit &hit)
{
    // The base renderer takes get_current_bounds through the widget's animated scale,
    // rotation, translation and depth projection, yielding a parallelogram in window pixels.
    s_gui_widget_render_data data;
    widget->c_gui_widget::assemble_render_data(&data, &hit.viewport, _controller0, true, true, true);
    const real_point2d *p = data.corners;
    const float ax = p[2].x - p[0].x, ay = p[2].y - p[0].y;
    const float bx = p[1].x - p[0].x, by = p[1].y - p[0].y;
    const float dx = hit.point.x - p[0].x, dy = hit.point.y - p[0].y;
    const float det = ax * by - ay * bx;
    if (det == 0.0f)
        return false;
    const float u = (dx * by - dy * bx) / det, v = (ax * dy - ay * dx) / det;
    return u >= 0 && u < 1 && v >= 0 && v < 1;
}

// Some retail rows are only an anchor (the Main Menu's has zero width). The
// visible text/bitmap skin is part of that row's hit area. A nested list starts
// its own ownership, so its artwork cannot select the outer item.
c_gui_list_item_widget *find_row(c_gui_widget *widget, const pointer_hit &hit, c_gui_list_item_widget *owner = nullptr)
{
    if (!widget->get_enabled() || !widget->should_render(nullptr))
        return nullptr;
    const auto type = widget->get_type();
    if (type == _gui_list_widget) {
        auto *list = static_cast<c_gui_list_widget *>(widget);
        if (list->non_interactive() || list->is_submenu_that_needs_disposal() ||
            list->is_animation_active(_animation_state_unload_as_submenu))
            return nullptr;
        owner = nullptr;
    } else if (type == _gui_list_item_widget) {
        auto *item = static_cast<c_gui_list_item_widget *>(widget);
        auto *list = gui_widget_get_parent_list_widget(item);
        const long index = item->get_list_item_index();
        if (!list || !item->can_receive_focus() || index < 0 ||
            index + list->get_scroll_position() >= list->get_selectable_item_count())
            return nullptr;
        owner = list->has_active_submenu() ? nullptr : item;
    }
    for (auto *child = widget->get_children(); child; child = child->get_next()) {
        // Nested screens have their own window-manager stack entry and input ownership.
        if (child->get_type() == _gui_screen_widget)
            continue;
        if (auto *item = find_row(child, hit, owner))
            return item;
    }
    if (owner && (type == _gui_list_item_widget || type == _gui_bitmap_widget || type == _gui_text_widget) &&
        widget->get_animated_state()->color.alpha > 0.0f && contains(widget, hit))
        return owner;
    return nullptr;
}

void controller_event(e_event_type type, e_controller_component component)
{
    s_event_record event = { type, _controller0, component, (short)(type == _event_type_button_press ? 255 : -1) };
    g_window_manager.handle_global_controller_event(&event);
}

// Start/Pause displays one tab title. Its parent screen owns that horizontal list
// while the current pane receives controller messages.
bool handle_tab_title(c_gui_screen_widget *screen, const pointer_hit &hit, port_ui_mouse_action action)
{
    auto *parent = screen->get_parent_screen();
    if (!parent || parent->get_name() != _start_menu_string_start_menu)
        return false;
    auto *start = static_cast<c_start_menu_screen_widget *>(parent);
    if (!start->m_root_pane_shown || start->m_breadcrumbs.count() != 1)
        return false;
    auto *tabs = start->get_child_list_widget(_start_menu_string_start_menu_sidebar);
    if (!tabs || !tabs->get_data() || !find_row(tabs, hit))
        return false;
    const bool next = action == port_ui_mouse_choose
        ? tabs->get_data()->get_next_element_handle(tabs->get_focused_element_handle()) != -1
        : action == port_ui_mouse_down;
    controller_event(next ? _event_type_tab_right : _event_type_tab_left,
                     next ? _controller_component_dpad_right : _controller_component_dpad_left);
    return true;
}

bool handle_pointer(c_gui_screen_widget *screen, e_window_index window, const port_ui_mouse_event &event)
{
    if (event.action == port_ui_mouse_back) {
        controller_event(_event_type_button_press, _controller_component_button_b);
        return true;
    }
    if (screen->m_suppress_focus)
        return false;
    c_font_cache_scope_lock render_lock; // the window manager publishes viewport and widget render state under this lock
    pointer_hit hit;
    const bool wheel = event.action == port_ui_mouse_up || event.action == port_ui_mouse_down;
    auto *focused = screen->get_focused_widget();
    auto *list = focused ? gui_widget_get_parent_list_widget(focused) : nullptr;
    auto *root = list && list->is_submenu() ? static_cast<c_gui_widget *>(list) : screen;
    const bool inside = locate_pointer(event, window, hit);
    auto *item = inside ? find_row(root, hit) : nullptr;
    if (!item && inside && root == screen && event.action != port_ui_mouse_move && handle_tab_title(screen, hit, event.action))
        return true;
    if (item && focused != item) {
        auto *hovered = gui_widget_get_parent_list_widget(item);
        if (!wheel || hovered != list) {
            screen->transfer_focus_to_list(hovered, item->get_element_handle(), true, true);
            if (screen->get_focused_widget() != item)
                return false;
            list = hovered;
            g_user_interface_globals.last_input.set(g_user_interface_globals.milliseconds.peek());
            window_manager_play_input_sound(screen->m_core_definition.sound_override_index,
                _event_type_tab_down, _controller_component_dpad_down);
        }
    }
    if (item && event.action == port_ui_mouse_choose) {
        controller_event(_event_type_button_press, _controller_component_none); // A
        return true;
    }
    if (wheel && list) {
        const bool up = event.action == port_ui_mouse_up;
        if (list->scrolls_horizontally())
            controller_event(up ? _event_type_tab_left : _event_type_tab_right,
                             up ? _controller_component_dpad_left : _controller_component_dpad_right);
        else
            controller_event(up ? _event_type_tab_up : _event_type_tab_down,
                             up ? _controller_component_dpad_up : _controller_component_dpad_down);
        return true;
    }
    return false;
}

} // namespace

void port_ui_mouse_tick()
{
    e_window_index window = _console_window;
    c_gui_screen_widget *screen = nullptr;
    if (g_window_manager.m_active_screens && !guide_showing() && !port_xam_ui_showing()) {
        if (tls_game_globals() && tls_player_mapping_globals())
            screen = g_window_manager.get_responding_screen(_controller0, &window);
        else
            screen = g_window_manager.get_responding_screen_in_window(_controller0, window);
    }
    port_input_set_ui_cursor_mode(screen != nullptr);

    // Messages are consumed at the next UI update. Keep subsequent actions ordered and
    // discard input aimed at a screen that was replaced (including dialog overlays).
    static long previous_screen = -1;
    const long index = screen ? screen->get_screen_index() : -1;
    const bool ready = screen && index == previous_screen && !screen->transitioning_in() &&
        !screen->transitioning_out() && !screen->m_reload_next_frame;
    previous_screen = index;
    port_ui_mouse_event event;
    while (screen && port_input_next_ui_mouse_event(&event))
        if (ready && handle_pointer(screen, window, event))
            break;
}
