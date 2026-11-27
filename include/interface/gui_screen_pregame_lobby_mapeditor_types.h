#ifndef INTERFACE_GUI_SCREEN_PREGAME_LOBBY_MAPEDITOR_TYPES_H
#define INTERFACE_GUI_SCREEN_PREGAME_LOBBY_MAPEDITOR_TYPES_H

#include "game/game_options.h"
#include <stddef.h>

struct s_game_preferences_color { unsigned char b[4]; };
// Mode snapshots use these copy boundaries within the persisted setup record.
// Theater game options contain an eight-byte-aligned game instance identifier.
struct __declspec(align(8)) s_game_preferences_mode_settings
{

    union { unsigned char campaign_settings[28]; int campaign_words[7]; };
    s_game_preferences_color matchmaking_settings;
    unsigned char multiplayer_settings[58648];
    unsigned char map_editor_settings[57768];
    unsigned char theater_settings[64064];
};

struct s_gui_game_setup_storage {
    struct s_campaign_settings {
        bool valid;
        e_campaign_id campaign_id;
        e_map_id map_id;
        long difficulty_level;
        char remaining_campaign_settings[0x20 - 0x10];
    };
    struct s_multiplayer_settings {
        struct s_game_variant_settings {
            __forceinline s_game_variant_settings() {}
            long flags;
            c_game_variant variant;
            signed char delete_pending;
            long delete_type;
            int : 32;
            int : 32;
            char path[0x100];
        };
        struct s_map_variant_settings {
            __forceinline s_map_variant_settings() {}
            c_map_variant variant;
        };
        s_multiplayer_settings();
        s_game_variant_settings game_variant;
        s_map_variant_settings map_variant;
    };
    struct s_map_editor_settings {
        char map_variant_file[0x118];
        c_map_variant map_variant;
    };
    struct s_theater_settings {
        bool valid;
        long campaign_id;
        char film_description[0x338 - 8];
        game_options options;
        long game_engine_index;
        long game_variant_index;
    };
    s_gui_game_setup_storage();
    enum e_file_image { _file_image };
    __forceinline explicit s_gui_game_setup_storage(e_file_image) {}
    union {
        unsigned char storage[0x2c120];
        s_game_preferences_mode_settings serialized_settings;
        struct {
            s_campaign_settings campaign_settings;
            s_multiplayer_settings multiplayer_settings;
            s_map_editor_settings map_editor_settings;
            s_theater_settings theater_settings;
        };
    };
};

typedef char gui_game_setup_snapshot_layout_size[(sizeof(s_game_preferences_mode_settings) == 0x2c120) ? 1 : -1];
typedef char gui_game_setup_size[(sizeof(s_gui_game_setup_storage) == 0x2c120) ? 1 : -1];
typedef char gui_game_setup_variant_offset[(offsetof(s_gui_game_setup_storage, multiplayer_settings)
    + offsetof(s_gui_game_setup_storage::s_multiplayer_settings::s_game_variant_settings, variant) == 0x24) ? 1 : -1];
typedef char gui_game_setup_map_editor_offset[(offsetof(s_gui_game_setup_storage, map_editor_settings) == 0xe428) ? 1 : -1];
typedef char gui_game_setup_theater_offset[(offsetof(s_gui_game_setup_storage, theater_settings) == 0x1c5d0) ? 1 : -1];
typedef char gui_game_setup_delete_type_offset[(offsetof(s_gui_game_setup_storage, multiplayer_settings)
    + offsetof(s_gui_game_setup_storage::s_multiplayer_settings::s_game_variant_settings, delete_type) == 0x28c) ? 1 : -1];
typedef char gui_game_setup_path_offset[(offsetof(s_gui_game_setup_storage, multiplayer_settings)
    + offsetof(s_gui_game_setup_storage::s_multiplayer_settings::s_game_variant_settings, path) == 0x298) ? 1 : -1];

#endif
