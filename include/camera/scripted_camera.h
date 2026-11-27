#ifndef CAMERA_SCRIPTED_CAMERA_H
#define CAMERA_SCRIPTED_CAMERA_H

#include "camera/camera_types.h"

class c_scripted_camera : public c_camera
{
public:
    c_scripted_camera() {}

    virtual e_camera_mode get_type() const { return _camera_mode_scripted; }
    virtual e_director_perspective get_perspective() const;
    virtual void update(long user_index, float dt, s_observer_command *result);
};

#endif // CAMERA_SCRIPTED_CAMERA_H
