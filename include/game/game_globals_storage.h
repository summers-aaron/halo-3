#ifndef GAME_GAME_GLOBALS_STORAGE_H
#define GAME_GAME_GLOBALS_STORAGE_H

#include <stddef.h>

#include "objects/object_reference_types.h"
#include "game/game_options.h"

struct s_game_globals;

struct game_globals_storage {
    union {
        bool initializing;
        bool initialized;
    };
    union {
        bool map_active;
        bool map_initialized;
        bool active;
    };
    unsigned long active_structure_bsp_mask;
    unsigned long active_designer_zone_mask;
    unsigned long active_cinematic_zone_mask;
    game_options options;
    union {
        bool in_progress;
        bool game_in_progress;
    };
    union {
        bool game_lost;
        bool lost;
        unsigned char game_lost_state;
    };
    long revert_ticks;
    union {
        bool game_finished;
        // game_engine readers explicitly normalize the stored byte.
        unsigned char game_finished_state;
        bool finished;
    };
    bool finish_immediate;
    long finish_ticks;
    long ragdoll_count;
    unsigned char : 8;
    bool game_had_an_update_tick_this_frame;
    unsigned long cluster_pvs[16][8];
    unsigned long cluster_pvs_local[16][8];
    unsigned long cluster_activation[16][8];
    bool scripted_camera_pvs;
    short scripted_type;
    union {
        long scripted_object;
        s_cluster_reference scripted_cluster;
    };
};

typedef game_globals_storage s_game_globals_storage;

typedef char game_globals_storage_size[(sizeof(game_globals_storage) == 0xfe40) ? 1 : -1];
typedef char game_globals_storage_off_options[(offsetof(game_globals_storage, options) == 0x10) ? 1 : -1];
typedef char game_globals_storage_off_in_progress[(offsetof(game_globals_storage, in_progress) == 0xf820) ? 1 : -1];
typedef char game_globals_storage_off_finished_state[(offsetof(game_globals_storage, game_finished_state) == 0xf828) ? 1 : -1];
typedef char game_globals_storage_off_clusters[(offsetof(game_globals_storage, cluster_activation) == 0xfc38) ? 1 : -1];

// Thread-local runtime game state (TLS slot 0x831df680), owned by game.cpp (March 2007 debug name and type).
extern __declspec(thread) game_globals_storage *game_globals;

#endif // GAME_GAME_GLOBALS_STORAGE_H
