#ifndef ANIMATIONS_MIXING_BOARD_MIXING_BOARD_DEFINITION_H
#define ANIMATIONS_MIXING_BOARD_MIXING_BOARD_DEFINITION_H

#include <stddef.h>

#include "motor/mixing_board_component_types.h"

class c_animation_manager;

// March g_mixing_board_component_sizes (get_component_offset's assert, mixing_board_definition.cpp:114):
// the size of one component of each type, indexed like component_types.
extern "C" long g_mixing_board_component_sizes[5];

// The layout of one kind of mixing board (biped, vehicle, ...): March asserts in
// animations\mixing_board\mixing_board_definition.h (get_component_type_count :85, get_component_type_offset :91,
// validate_component_index :97) and mixing_board_definition.cpp (get_component_offset :109..114).
// Layout: each 300-byte entry holds the name
// "biped_mixing_board_definition", the board's total size 0x304 (March get_total_size reads +4), three callbacks
// (unit_animation_setup_mixing_board_channels 0x348518, unit_animation_snap_mixing_sliders 0x3483b0, which
// c_animation_manager::reset_interpolators calls, and biped_locate_auxilary_animations 0x121058), the five
// component types (sliders, channels, auxilary, aiming, facial: offset and count each), and up to twelve component
// names per type (sliders "aim_on", "look_on", ...; channels "primary", ...; the unused rest zero).
class c_mixing_board_definition
{
public:
    long get_component_type_count(long component_type) const { return component_types[component_type].count; }
    long get_component_type_offset(long component_type) const { return component_types[component_type].offset; }
    static long get_component_size(long component_type) { return g_mixing_board_component_sizes[component_type]; }
    long get_total_size() const { return total_size; }
    // Byte offset of one component: the type base offset plus the index times the per-type size.
    long get_component_offset(long component_type, long component_index) const
    {
        return get_component_type_offset(component_type) + component_index * g_mixing_board_component_sizes[component_type];
    }

    const char *name;
    long total_size;
    void (*setup_channels)(c_animation_manager *manager);
    void (*reset_interpolators)(long object_index);
    void (*locate_auxilary_animations)(long object_index);
    s_mixing_board_component_type component_types[5];
    const char *component_names[5][12];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char mixing_board_definition_size[(sizeof(void *) != 4 || (sizeof(c_mixing_board_definition) == 300)) ? 1 : -1];
typedef char mixing_board_definition_components_offset[(sizeof(void *) != 4 || (offsetof(c_mixing_board_definition, component_types) == 0x14)) ? 1 : -1];
#endif

// The mixing board definitions, one per board type.
extern c_mixing_board_definition g_mixing_board_definition_list[3];

static __forceinline const c_mixing_board_definition *mixing_board_definition_get_by_type(unsigned long type)
{
    const c_mixing_board_definition *definition = 0;
    if (type <= 2)
        definition = &g_mixing_board_definition_list[type];
    return definition;
}

#endif // ANIMATIONS_MIXING_BOARD_MIXING_BOARD_DEFINITION_H
