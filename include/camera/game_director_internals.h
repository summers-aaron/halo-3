#ifndef CAMERA_GAME_DIRECTOR_INTERNALS_H
#define CAMERA_GAME_DIRECTOR_INTERNALS_H

#include "camera/debug_director_internals.h"

class c_game_director : public c_director
{
public:
    // Restores the vtable over a director whose fields were kept.
    c_game_director() {}
    __forceinline c_game_director(long user_index) : c_director(user_index)
    {
        set_camera_mode(_camera_mode_first_person, 0.0f, 0);
    }
    virtual e_director_mode get_type() const { return _director_mode_game; }
    virtual void update(float dt);
    virtual bool should_draw_hud_saved_film() const { return true; }
    virtual bool inhibits_facing() const;
};

#endif
