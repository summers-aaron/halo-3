#ifndef CAMERA_DEBUG_DIRECTOR_INTERNALS_H
#define CAMERA_DEBUG_DIRECTOR_INTERNALS_H

#include "camera/director_vtables.h"
#include "camera/observer_director.h"

class c_orbiting_camera;

class c_debug_director : public c_director
{
public:
    c_debug_director() {}
    __forceinline c_debug_director(long user_index) : c_director(user_index)
    {
        m_camera_index = 0;
        m_spare_flag0 = false;
        changed_camera();
    }
    virtual e_director_mode get_type() const { return _director_mode_debug; }
    virtual void update(float);
    virtual bool should_draw_hud() const;
    virtual bool should_draw_hud_saved_film() const;
    virtual bool inhibits_facing() const;
    virtual bool inhibits_input() const;
private:
    void changed_camera();
    long m_camera_index;
    bool m_inhibits_facing;
    bool m_spare_flag0;
};

typedef char debug_director_size[(sizeof(void *) != 4 || ((sizeof(c_debug_director) == 0x154))) ? 1 : -1];

#endif // CAMERA_DEBUG_DIRECTOR_INTERNALS_H
