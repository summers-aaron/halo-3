#ifndef MAIN_MAIN_RENDER_TYPES_H
#define MAIN_MAIN_RENDER_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_observer_result;

struct s_main_render_view_layout
{
    long view_count;
    long layout;
    long current_camera_index;
    long current_user_index;
    const s_observer_result *current_observer_result;
};

struct s_main_pregame_frame_colors
{
    real_rgb_color background;
    real_rgb_color text;
    real_rgb_color shadow;
};

typedef char main_render_view_layout_size[(sizeof(void *) != 4 || ((sizeof(s_main_render_view_layout) == 0x14))) ? 1 : -1];
typedef char main_render_view_layout_camera_offset[(offsetof(s_main_render_view_layout, current_camera_index) == 8) ? 1 : -1];
typedef char main_render_view_layout_user_offset[(offsetof(s_main_render_view_layout, current_user_index) == 12) ? 1 : -1];
typedef char main_render_view_layout_observer_offset[(offsetof(s_main_render_view_layout, current_observer_result) == 16) ? 1 : -1];
typedef char main_render_pregame_frame_colors_size[(sizeof(s_main_pregame_frame_colors) == 0x24) ? 1 : -1];

enum e_main_pregame_frame
{
    _main_pregame_frame_none = 0,
    _main_pregame_frame_normal = 1,
    _main_pregame_frame_crash = 4,
    _main_pregame_frame_crash_upload_succeeded = 5
};

// March main_render.cpp: render camera and PVS iteration interfaces.
class c_player_render_camera_iterator {
public:
    long m_camera_count;
    long m_window_arrangement;
    long m_current_camera_index;
    long m_current_user_index;
    const s_observer_result *m_current_observer_result;
public:
    bool next();
    __forceinline long get_window_count() const { return m_camera_count; }
    __forceinline long get_window_arrangement() const { return m_window_arrangement; }
    __forceinline long get_user_index() const { return m_current_user_index; }
    __forceinline const s_observer_result *get_observer_result() const { return m_current_observer_result; }
};
class c_pvs_cluster_iterator {
    unsigned long *m_cluster_bits[16];
    long m_cluster_counts[16];
    long m_structure_bsp_index;
    long m_cluster_index;
    bool valid_cluster_reference() const;
    bool visible_cluster_reference() const;
    __forceinline bool go_to_next_cluster();
public:
    bool next();
    __forceinline long get_structure_bsp_index() const { return m_structure_bsp_index; }
    __forceinline long get_cluster_index() const { return m_cluster_index; }
    __forceinline long get_cluster_count(long structure_bsp_index) const { return m_cluster_counts[structure_bsp_index]; }
    __forceinline void reset() { m_structure_bsp_index = 0; m_cluster_index = -1; }
};

typedef char main_render_camera_iterator_size[(sizeof(void *) != 4 || ((sizeof(c_player_render_camera_iterator) == 0x14))) ? 1 : -1];
typedef char main_render_camera_arrangement_offset[(offsetof(c_player_render_camera_iterator, m_window_arrangement) == 4) ? 1 : -1];
typedef char main_render_pvs_iterator_size[(sizeof(void *) != 4 || ((sizeof(c_pvs_cluster_iterator) == 0x88))) ? 1 : -1];

#endif
