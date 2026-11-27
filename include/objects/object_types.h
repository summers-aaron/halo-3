#ifndef OBJECTS_OBJECT_TYPES_H
#define OBJECTS_OBJECT_TYPES_H

#include "cseries/cseries_types.h"

#include <string.h>
#include "objects/object_type_enums.h"

enum {
    _object_mask_all = 0xffffffffUL,
    _object_mask_biped = (1UL << _object_type_biped),
    _object_mask_vehicle = (1UL << _object_type_vehicle),
    _object_mask_weapon = (1UL << _object_type_weapon),
    _object_mask_equipment = (1UL << _object_type_equipment),
    _object_mask_terminal = (1UL << _object_type_terminal),
    _object_mask_projectile = (1UL << _object_type_projectile),
    _object_mask_scenery = (1UL << _object_type_scenery),
    _object_mask_machine = (1UL << _object_type_machine),
    _object_mask_control = (1UL << _object_type_control),
    _object_mask_sound_scenery = (1UL << _object_type_sound_scenery),
    _object_mask_crate = (1UL << _object_type_crate),
    _object_mask_creature = (1UL << _object_type_creature),
    _object_mask_giant = (1UL << _object_type_giant),
    _object_mask_effect_scenery = (1UL << _object_type_effect_scenery),
    _object_mask_item = _object_mask_weapon | _object_mask_equipment,
    _object_mask_device = _object_mask_terminal | _object_mask_machine | _object_mask_control,
    _object_mask_unit = _object_mask_biped | _object_mask_vehicle | _object_mask_giant
};

// March asserts name _object_source_sky (objects.cpp 2296); the other sources are not attested by value.
enum e_object_source {
    _object_source_sky = 4
};

class c_object_identifier {
public:
    long m_unique_id;
    short m_origin_bsp_index;
    char m_type;
    signed char m_source;

    __forceinline void clear() {
        m_type = -1;
        m_source = -1;
        m_origin_bsp_index = -1;
        m_unique_id = -1;
    }

    __forceinline e_object_type get_type() const { return (e_object_type)m_type; }
    __forceinline long get_source() const { return m_source; }

    // March 82730e20 / 82730f18 (objects.cpp, out of line there).
    __forceinline void create_from_sky(e_object_type type, long sky_index) {
        m_type = (char)type;
        m_source = _object_source_sky;
        m_origin_bsp_index = -1;
        m_unique_id = sky_index;
    }
    __forceinline long get_sky_identifier() const { return m_unique_id; }

    long find_object_index() const;
    bool is_equal(const c_object_identifier *other) const;
    struct s_scenario_object *find_scenario_object_from_scenario(struct scenario *scenario, long *out_index) const;
};

struct s_damage_owner {
    long player_index;
    long object_index;
    char team;
    unsigned char reporting_info;
    __forceinline void set(long object_index, long player_index, long team)
    {
        memset(this, 0, sizeof(*this));
        this->object_index = object_index;
        this->player_index = player_index;
        this->team = (char)team;
    }
};

typedef char obj_types_size_object_id[(sizeof(c_object_identifier) == 8) ? 1 : -1];
typedef char obj_types_size_damage_owner[(sizeof(s_damage_owner) == 12) ? 1 : -1];


// Functions defined in tu/objectslibxbox/object_types.cpp that other units call.

struct object_placement_data;
struct real_matrix4x3;
struct real_orientation;
union real_point3d;
union real_vector3d;
struct s_scenario_object;
template<long k_bit_count> class c_static_flags;

bool object_type_update(long object_index);
bool object_type_compute_function_value(long object_index, long p1, long p2, float *p3, bool *p4, bool *p5);
void object_type_fix_transform(long object_index, real_point3d *position, real_vector3d *forward, real_vector3d *up);
void object_type_fix_transform_from_physics(long object_index, real_matrix4x3 *transform);
void object_type_delete(long object_index);
void object_type_adjust_placement(object_placement_data *placement);
bool object_type_new(long object_index, object_placement_data *placement, bool *out_of_objects);
void object_type_place(long object_index,const s_scenario_object *scenario_object);
void object_type_unplace(long object_index);
void object_type_preprocess_node_orientations(long object_index, const c_static_flags<255> *node_mask, long node_count,
    real_orientation *node_orientations);
void object_type_fix_transform_to_physics(long object_index, real_matrix4x3 *transform);

// Data defined in tu/objectslibxbox/object_types.cpp that other units use.

struct object_type_definition;

extern object_type_definition *first_object_type_definition;
extern object_type_definition *object_type_definitions[14];

// Look up the definition for an object type.
__forceinline object_type_definition *object_type_definition_get(e_object_type type) { return object_type_definitions[type]; }

// The object types whose handle_deleted_object / handle_deleted_player is set (object_types_initialize builds them).
extern "C" {
extern unsigned long g_object_types_deleted_object_mask;
extern unsigned long g_object_types_deleted_player_mask;
}
void object_types_initialize();

#endif // OBJECTS_OBJECT_TYPES_H
