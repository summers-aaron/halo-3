#ifndef RENDER_RENDER_H
#define RENDER_RENDER_H

// Functions defined in tu/graphicslibxbox/render.cpp.

struct render_camera;
struct render_projection;
struct s_shader_extern_info;

void initialize_default_shader_extern_info(s_shader_extern_info *info);
void render_setup_window(const render_camera *camera, const render_projection *projection);
void render_setup_occlusion_constants(long user_index);

struct real_matrix4x3;

void combine_projection_and_view_matrix(const real_matrix4x3 *view, const float (*const projection)[4], float *const out);

struct s_render_text_style;

extern "C" void render_draw_pregame_text(const s_render_text_style *style, bool simple);
extern "C" void render_initialize_loading_screen(void);
extern "C" void render_setup_fade_constants(long user_index, float near_distance, float far_distance);
extern "C" bool render_is_load_recently_complete(void);
extern "C" bool render_is_loading_complete(void);

// March: every member static (render.obj owns the data; only what cleaned units use is declared).
class c_render_globals
{
public:
    static float get_frame_time() { return m_frame_time; }
    static void set_frame_time(float time) { m_frame_time = time; }
    static long get_frame_index() { return m_frame_index; }
    static void set_frame_index(long index) { m_frame_index = index; }
    static bool get_skinning_constants_owned() { return m_skinning_constants_owned; }
    static void set_skinning_constants_owned(bool owned) { m_skinning_constants_owned = owned; }
    static bool get_distortion_active() { return m_distortion_active; }
    static void set_distortion_active(bool active) { m_distortion_active = active; }
    static bool get_depth_fade_active() { return m_depth_fade_active; }
    static void set_depth_fade_active(bool active) { m_depth_fade_active = active; }

private:
    static float m_frame_time;
    static unsigned long m_frame_index;
    static bool m_skinning_constants_owned;
    static bool m_distortion_active;
    static bool m_depth_fade_active;
};

// Data defined in tu/graphicslibxbox/render.cpp that other units use.

class c_restricted_section;

extern c_restricted_section g_restricted_render_memory_section;
extern "C" const float k_render_min_field_of_view[];

void render_initialize(void);
void render_dispose(void);
void render_initialize_for_new_map(void);
void render_dispose_from_old_map(void);
void render_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);

void render_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
#endif // RENDER_RENDER_H
