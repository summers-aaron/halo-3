#ifndef GAME_GAME_ENGINE_AREA_SET_TYPES_H
#define GAME_GAME_ENGINE_AREA_SET_TYPES_H

#include "game/game_engine_area_types.h"
#include "game/game_engine.h"
#include "game/area_cycle_options_types.h"
#include "memory/static_arrays.h"

// This header supplies the template methods. CTF,
// Assault, Infection and Territories agree on the four words after the areas:
// count, team designator, object type mask and object match flags.
template<class t_area, long k_area_count>
class c_area_set
{
    friend class c_ctf_engine;
    friend class c_territories_engine;
public:
    __forceinline c_area_set() : m_area_count(0), m_area_type_flags(0), m_area_flags(0) {}
    __forceinline t_area *begin() { return m_areas.begin(); }
    __forceinline t_area *end() { return m_areas.begin() + m_area_count; }
    __forceinline long get_area_count() { return m_area_count; }
    __forceinline t_area &operator[](long index) { return m_areas[index]; }
    __forceinline bool exist() const { return m_area_count > 0; }
    __forceinline long get_absolute_index(t_area *area) { return area - &m_areas[0]; }
    __forceinline void update() { update_areas(); }
    void initialize(long type_flags, long area_flags, e_multiplayer_team_designator team_designator);
    void select_area(e_area_cycle_options cycle);
    t_area *find_selected_area();
    bool player_in_set(long player_index, t_area **area_out);
    __forceinline bool point_in_set(const real_point3d *point, t_area **area_out)
    {
        bool result = false;
        if (game_engine_in_round()) {
            t_area *selected_area = find_selected_area();
            if (selected_area != 0) {
                result = point_in_multiplayer_object_boundary(point, (const s_multiplayer_object_boundary_geometry_data *)selected_area);
            } else {
                for (long index = 0; index < m_area_count; index++) {
                    result = point_in_multiplayer_object_boundary(point, (const s_multiplayer_object_boundary_geometry_data *)&m_areas[index]);
                    if (result) {
                        if (area_out != 0)
                            *area_out = &m_areas[index];
                        break;
                    }
                }
            }
        }
        return result;
    }
protected:
    void update_areas();
public:
    c_static_array<t_area, k_area_count> m_areas;
    long m_area_count;
    long m_team_designator;
    long m_area_type_flags;
    long m_area_flags;
};

typedef char game_engine_area_set_ten_size[(sizeof(c_area_set<c_area, 10>) == 0x3f8) ? 1 : -1];
typedef char game_engine_area_set_three_size[(sizeof(c_area_set<c_area, 3>) == 0x13c) ? 1 : -1];
typedef char game_engine_area_set_twelve_size[(sizeof(c_area_set<c_area, 12>) == 0x4c0) ? 1 : -1];

#endif // GAME_GAME_ENGINE_AREA_SET_TYPES_H
