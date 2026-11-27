#ifndef GAME_GAME_ENGINE_AREA_TYPES_H
#define GAME_GAME_ENGINE_AREA_TYPES_H

#include <stddef.h>

#include "objects/multiplayer_object_types.h"
#include "objects/multiplayer_game_objects.h"
#include "game/game_engine_vip.h"
#include "cseries/global_real_argb_colors.h"

static __forceinline const real_argb_color &game_engine_area_color(e_global_real_argb_color color)
{
    return global_real_argb_colors[color];
}

// An area
// extends the old 0x50 geometry/object class with a color before the handle.
class c_area
{
public:
    __forceinline c_area() : m_object_index(-1)
    {
        m_color = game_engine_area_color(_real_argb_grey);
    }

    void set_selected(bool selected);
    bool is_selected() const;
    __forceinline long get_object_index() const { return m_object_index; }
    __forceinline void set_object_index(long object_index) { m_object_index = object_index; }
    __forceinline void get_center(real_point3d *center) const { *center = m_boundary_geometry.matrix.position; }
    __forceinline float get_radius() const { return m_boundary_geometry.radius; }
    __forceinline const s_multiplayer_object_boundary_geometry_data *get_boundary_geometry() const { return &m_boundary_geometry; }
    __forceinline const real_argb_color *get_color() const { return &m_color; }
    __forceinline void set_color(const real_argb_color &color) { m_color = color; }
    __forceinline void calculate_boundary_geometry()
    {
        if (m_object_index != -1)
            calculate_influence_boundary_geometry(m_object_index, &m_boundary_geometry, 0);
    }
    __forceinline bool player_inside(long player_index) const
    {
        return player_in_multiplayer_object_boundary(player_index, &m_boundary_geometry);
    }
    __forceinline bool point_inside(const real_point3d *point) const
    {
        return point_in_multiplayer_object_boundary(point, &m_boundary_geometry);
    }
    void render() const;

    s_multiplayer_object_boundary_geometry_data m_boundary_geometry;
    real_argb_color m_color;
    long m_object_index;
};

typedef char game_engine_area_size[(sizeof(c_area) == 0x64) ? 1 : -1];
typedef char game_engine_area_color_offset[(offsetof(c_area, m_color) == 0x50) ? 1 : -1];
typedef char game_engine_area_object_offset[(offsetof(c_area, m_object_index) == 0x60) ? 1 : -1];

#endif // GAME_GAME_ENGINE_AREA_TYPES_H
