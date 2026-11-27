#ifndef CAMERA_EDITOR_DIRECTOR_H
#define CAMERA_EDITOR_DIRECTOR_H

// Functions defined in tu/blamlibxbox/editor_director.cpp.

#include "camera/debug_director_internals.h"

class c_editor_director : public c_director
{
public:
    c_editor_director() {}
    c_editor_director(long user_index);
    virtual e_director_mode get_type() const { return _director_mode_editor; }
    virtual void update(float);
    virtual bool should_draw_hud() const { return false; }
    virtual bool should_draw_hud_saved_film() const { return false; }
    virtual void handle_deleted_object(long object_index);
    long m_unit_focus;
    bool m_scripted;
    bool m_use_roll;
};

typedef char editor_director_focus_offset[(sizeof(void *) != 4 || ((offsetof(c_editor_director, m_unit_focus) == 0x14c))) ? 1 : -1];

#endif // CAMERA_EDITOR_DIRECTOR_H
