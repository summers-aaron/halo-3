#ifndef INTERFACE_GUI_MODEL_WIDGET_TYPES_H
#define INTERFACE_GUI_MODEL_WIDGET_TYPES_H

#include "interface/gui_widget.h"
#include "interface/core_widget_definition.h"
#include "game/player_types.h"
#include <stddef.h>

class c_function_definition;
struct s_model_widget_block;
struct s_model_widget_definition;
struct s_gui_model_widget_render_data : s_gui_widget_render_data
{
    long object_index;
    s_player_appearance player_appearance;
    real_point3d camera_position;
    real_vector3d camera_up;
    real_vector3d camera_forward;
    float field_of_view;
    unsigned long tint;
};

typedef char model_widget_render_tint_offset[(offsetof(s_gui_model_widget_render_data, tint) == 0x78) ? 1 : -1];

// Runtime camera settings copied from the tag element: field of view, distance range and step,
// height range and step, rotation step. The tag's two function blocks are resolved to
// pointers at runtime; the rest of each 12-byte block slot is never read.
struct s_runtime_model_widget_element
{
    long identifier;
    float field_of_view;
    float lower;
    float upper;
    float scale;
    float height_lower;
    float height_upper;
    float height_scale;
    float rotation_scale;
    const c_function_definition *distance_function;
    long pad28;
    long pad2c;
    const c_function_definition *height_function;
    long pad34;
    long pad38;
};

// March 8371b460 constructs this runtime definition as one subobject.
// It holds the two resolved camera elements.
struct s_runtime_model_widget_definition : s_runtime_core_widget_definition
{
    s_runtime_model_widget_element elements[2];
};

// Model widget state based on c_gui_widget.
// Holds the runtime definition and player appearance.
class c_gui_model_widget : public c_gui_widget
{
public:
    c_gui_model_widget();
    virtual ~c_gui_model_widget() {}
    virtual void initialize(const s_model_widget_block *definition);
    virtual void post_initialize();
    virtual s_runtime_core_widget_definition *get_core_definition();
    virtual void assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
        e_controller_index local_controller_index, bool apply_translation, bool apply_scale, bool apply_rotation);
    void add_definition_fields(const s_model_widget_definition *definition, bool require_nonzero_bounds);
    s_runtime_model_widget_element *get_current_model_element();
    __forceinline s_runtime_model_widget_element *find_model_element(long key);
    void reset_player_appearance();
    void set_player_appearance(const s_player_appearance *appearance, long appearance_index);
    bool handle_camera_input(float angle_delta, float position_delta);
    const s_player_appearance *get_player_appearance() const { return &m_player_appearance; }
    static void render(s_gui_model_widget_render_data *render_data);

    s_runtime_model_widget_definition m_definition;
    s_player_appearance m_player_appearance;
    long m_appearance_index;
    float m_yaw;
    float m_zoom;
};

typedef char runtime_model_widget_definition_size[(sizeof(void *) != 4 || sizeof(s_runtime_model_widget_definition) == 0xac) ? 1 : -1];

typedef char runtime_model_widget_element_size[(sizeof(void *) != 4 || ((sizeof(s_runtime_model_widget_element) == 0x3c) )) ? 1 : -1];
typedef char model_widget_elements_offset[(sizeof(void *) != 4
    || ((offsetof(c_gui_model_widget, m_definition) + offsetof(s_runtime_model_widget_definition, elements) == 0xfc) )) ? 1 : -1];
typedef char model_widget_appearance_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_model_widget, m_player_appearance) == 0x174) )) ? 1 : -1];
typedef char model_widget_choice_offset[(sizeof(void *) != 4
    || ((offsetof(c_gui_model_widget, m_player_appearance) + offsetof(s_player_appearance, player_model_choice) == 0x178) )) ? 1 : -1];
typedef char model_widget_yaw_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_model_widget, m_yaw) == 0x198) )) ? 1 : -1];
typedef char model_widget_size[(sizeof(void *) != 4 || ((sizeof(c_gui_model_widget) == 0x1a0) )) ? 1 : -1];

#endif // INTERFACE_GUI_MODEL_WIDGET_TYPES_H
