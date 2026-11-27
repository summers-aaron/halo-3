#ifndef GAME_MAP_VARIANT_H
#define GAME_MAP_VARIANT_H

#include <stddef.h>

#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays.h"
#include "math/real_math_types.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "objects/object_types.h"
#include "saved_games/saved_game_file_types.h"

class c_bitstream;
enum e_object_type;
enum e_metadata_file_type;

#include "game/map_variant_object_properties_types.h"

enum e_variant_object_placement_flags {
    _variant_object_placement_occupied_slot_bit = 0,
    _variant_object_placement_edited_bit = 1,
    _variant_object_placement_registered_bit = 2,
    _variant_object_placement_runtime_candidate_bit = 3,
    _variant_object_placement_scenario_object_removed_bit = 5,
    _variant_object_placement_runtime_created_bit = 6,
    _variant_object_placement_preserve_helper_bit = 7,
    _variant_object_placement_spawn_from_object_bit = 8,
    _variant_object_placement_spawn_relative_bit = 9,
    k_variant_object_placement_flags_count = 10
};


enum e_object_removal_origin {
    _object_removal_origin_0 = 0,
    _object_removal_origin_1 = 1,
    _object_removal_origin_2 = 2
};

#include "saved_games/saved_game_item_metadata.h"


struct s_variant_object_datum {
    s_variant_object_datum();
    c_flags<e_variant_object_placement_flags, unsigned short, k_variant_object_placement_flags_count> flags;
    short reuse_timeout;
    long object_index;
    long helper_object_index;
    long quota_index;
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    c_object_identifier parent_object_identifier;
    s_variant_multiplayer_object_properties_definition properties;
};

struct s_variant_quota {
    s_variant_quota() { initialize_to_default(); }
    void initialize_to_default() {
        object_definition_index = -1;
        minimum_count = 0;
        maximum_count = 0;
        placed_on_map = 0;
        maximum_allowed = 255;
        price = -1.0f;
    }

    long object_definition_index;
    unsigned char minimum_count;
    unsigned char maximum_count;
    unsigned char placed_on_map;
    unsigned char maximum_allowed;
    float price;
};

class c_map_variant {
public:
    c_map_variant();
    void create_default(e_map_id map_id);
    void initialize_for_scenario();
    void adjust_active_object_count(long quota_index, long notify);
    long get_object_index(e_object_type object_type, long scenario_index) const;
    bool get_object_scenario_properties(long object_index, e_object_type *object_type, long *scenario_index);
    bool register_scenario_object(long object_index, bool from_game_options);
    void register_scenario_object(e_object_type type, long scenario_index);
    void register_object(long object_index, long placement_index);
    void remove_object(long variant_object_index, e_object_removal_origin origin);
    bool get_variant_object_datum(long object_index, s_variant_object_datum *variant_object) const;
    void set_variant_object_datum(long variant_object_index, const s_variant_object_datum *variant_object, bool edited);
    void save_to(c_map_variant *destination) const;
    void encode(c_bitstream *packet) const;
    void apply_multiplayer_properties(long variant_object_index, long object_index) const;
    void apply_multiplayer_properties(const s_variant_multiplayer_object_properties_definition *properties, long object_index) const;
    long find_definition_index(long variant_object_index);
    static void find_definition_index(long object_index, long placement_index, long notify);
    static void setup_object_properties(s_variant_object_datum *variant_object);

    union {
        s_saved_game_item_metadata m_metadata;
        s_saved_game_item_metadata metadata;
        struct {
            // Leading bytes of the metadata: the unique id plus the first six
            // wide characters of the metadata name. The display name follows at
            // the UI's read offset (see parse_variant_name_string).
            unsigned __int64 unique_id;
            wchar_t name_prefix[6];
            wchar_t name[16];
        };
    };
    union {
        short m_version;
        unsigned short version;
    };
    union {
        short m_number_of_scenario_objects;
        short scenario_object_count;
    };
    union {
        short m_number_of_variant_objects;
        short variant_object_count;
    };
    union {
        short m_number_of_placeable_object_quotas;
        short quota_count;
    };
    union {
        long map_id;
        e_map_id m_map_id;
    };
    real_rectangle3d m_bounds;
    union {
        long m_game_engine_subtype;
        long game_engine_subtype;
    };
    union {
        float m_maximum_budget;
        float budget_limit;
        float maximum_budget;
    };
    union {
        float m_spent_budget;
        float budget_spent;
        float current_budget;
    };
    unsigned char m_flag;
    union {
        bool m_built_in;
        bool built_in;
    };
    long m_original_map_signature_hash;
    s_variant_object_datum objects[640];
    short object_type_start[14];
    c_static_array<s_variant_quota, 256> quotas;
    long m_simulation_indices[80];
};

typedef char map_variant_size_metadata[(sizeof(s_saved_game_item_metadata) == 0xf8) ? 1 : -1];
typedef char map_variant_size_mp_props[(sizeof(s_variant_multiplayer_object_properties_definition) == 24) ? 1 : -1];
typedef char map_variant_size_obj_datum[(sizeof(s_variant_object_datum) == 0x54) ? 1 : -1];
typedef char map_variant_size_quota[(sizeof(s_variant_quota) == 12) ? 1 : -1];
typedef char map_variant_size_variant[(sizeof(c_map_variant) == 0xe090) ? 1 : -1];
typedef char map_variant_off_name[(offsetof(c_map_variant, name) == 20) ? 1 : -1];
typedef char map_variant_off_objects[(offsetof(c_map_variant, objects) == 0x130) ? 1 : -1];
typedef char map_variant_off_quotas[(offsetof(c_map_variant, quotas) == 0xd34c) ? 1 : -1];

#endif // GAME_MAP_VARIANT_H
