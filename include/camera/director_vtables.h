#ifndef CAMERA_DIRECTOR_VTABLES_H
#define CAMERA_DIRECTOR_VTABLES_H

#include "camera/camera_mode_types.h"
#include "camera/observer_command.h"
#include <stddef.h>

class c_camera;

enum e_director_mode
{
    _director_mode_game,
    _director_mode_saved_film,
    _director_mode_observer,
    _director_mode_debug,
    _director_mode_unused,
    _director_mode_editor
};

struct s_camera_interface
{
    int (**dispatch)(s_camera_interface *self);
    char m_camera_storage[0x2c];
    unsigned long m_flags;
};

class c_director
{
public:
    __forceinline c_director() {}
    c_director(long user_index);
    virtual e_director_mode get_type() const = 0;
    virtual void update(float);
    virtual bool should_draw_hud() const { return true; }
    virtual bool should_draw_hud_saved_film() const { return true; }
    virtual bool inhibits_facing() const { return false; }
    virtual bool inhibits_input() const { return false; }
    virtual void handle_deleted_player(long player_index);
    virtual void handle_deleted_object(long object_index);
    virtual bool can_use_camera_mode(e_camera_mode) const { return true; }
    bool set_camera_mode(e_camera_mode camera_mode, float blend, unsigned char force);
public:
    union {
        mutable s_camera_interface m_camera;
#ifdef HALO3_64BIT
        unsigned char m_camera_storage[0x60];
#else
        unsigned char m_camera_storage[0x4c];
#endif
    };
    s_observer_command stored_command;
    __forceinline const s_observer_command *get_last_observer_command() const { return &stored_command; }
    __forceinline c_camera *get_camera() const { return (c_camera *)&m_camera; }
    float blend;
    long m_user_index;
    long watched_player_index;
    union
    {
        long m_output_state;
        bool scoreboard_animation_started;
    };
public:
    static long player_get_next_player_with_a_unit(long reference_player_index, long current_player_index,
        long preferred_player_index, long direction, bool same_team_only, bool live_unit_only, bool wrap,
        long *out_unit_index);
};

typedef char director_size[(sizeof(void *) != 4 || ((sizeof(c_director) == 0x14c))) ? 1 : -1];
typedef char director_command_offset[(sizeof(void *) != 4 || ((offsetof(c_director, stored_command) == 0x50))) ? 1 : -1];
typedef char director_user_offset[(sizeof(void *) != 4 || ((offsetof(c_director, m_user_index) == 0x140))) ? 1 : -1];

#endif // CAMERA_DIRECTOR_VTABLES_H
