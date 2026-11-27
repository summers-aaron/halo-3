#ifndef CAMERA_SAVED_FILM_DIRECTOR_INTERNALS_H
#define CAMERA_SAVED_FILM_DIRECTOR_INTERNALS_H

#include "camera/director_globals_types.h"
#include "camera/camera_types.h"

class c_observer_director : public c_director
{
public:
    c_observer_director() {}
    c_observer_director(long user_index);
    virtual e_director_mode get_type() const { return _director_mode_observer; }
    virtual void update(float elapsed_time);
    virtual bool should_draw_hud() const;
    virtual bool should_draw_hud_saved_film() const;
    virtual bool inhibits_facing() const { return true; }
    virtual bool inhibits_input() const { return true; }
    virtual void find_any_player();
    long m_camera_mode;
    long m_mode;
    unsigned char m_initial_camera;
    bool m_follow_any_player;
};

class c_saved_film_director : public c_observer_director
{
public:
    c_saved_film_director() {}
    __forceinline c_saved_film_director(long user_index) : c_observer_director(user_index)
    {
        set_camera_mode(_camera_mode_first_person, 0.0f, 0);
        m_adjusting_playback_speed = false;
    }
    virtual e_director_mode get_type() const { return _director_mode_saved_film; }
    virtual void update(float elapsed_time);
    virtual void find_any_player();
    bool m_adjusting_playback_speed;
};

typedef char saved_film_director_playback_offset[(sizeof(void *) != 4 || ((offsetof(c_saved_film_director, m_adjusting_playback_speed) == 0x158))) ? 1 : -1];

#endif
