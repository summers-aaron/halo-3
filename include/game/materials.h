#ifndef GAME_MATERIALS_H
#define GAME_MATERIALS_H

struct s_game_globals;
struct s_global_material_definition;

// Material types are identified by a signed 16-bit index.
// Inline bodies read from March 82448030, 82695cc8, 82795fd8 and 827929d0.
class c_global_material_type {
public:
    // March 82b6cca8: base type 0 is invalid; base type 1 is the default material.
    enum e_base_types { _invalid, _default };
    __forceinline c_global_material_type() : m_index(-1) {}
    c_global_material_type(long material_name);
    c_global_material_type(const s_game_globals *, long material_name);
    __forceinline c_global_material_type(e_base_types type) {
        switch (type) {
        case _invalid: m_index = -1; break;
        case _default: m_index = 0; break;
        }
    }
    __forceinline c_global_material_type &operator=(const c_global_material_type &other) { m_index = other.m_index; return *this; }
    __forceinline bool operator==(const c_global_material_type &other) const { return m_index == other.m_index; }
    __forceinline bool operator!=(const c_global_material_type &other) const { return m_index != other.m_index; }
    bool valid() const;
    short get_raw_material_index() const;
    s_global_material_definition *get_material(const s_game_globals *) const;
    void set_raw_material_index(const s_game_globals *, short);
    short resolve_material_name(const s_game_globals *, long);

    short m_index;
};

typedef char global_material_type_size[(sizeof(c_global_material_type) == 2) ? 1 : -1];

// Functions defined in tu/blamlibxbox/materials.cpp that other units call.

union real_point3d;

c_global_material_type global_material_resolve_underwater_material_proxy(c_global_material_type material, const real_point3d *position);
c_global_material_type object_get_default_material(long object_index);

// Data defined in tu/blamlibxbox/materials.cpp that other units use.

#include "game/material_constants.h"

#endif // GAME_MATERIALS_H
