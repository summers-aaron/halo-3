#ifndef CAMERA_CAMERA_PAN_NAVIGATOR_H
#define CAMERA_CAMERA_PAN_NAVIGATOR_H

class c_camera_pan_navigator {
public:
    bool set_parameters(float duration, float acceleration_time, float deceleration_time, float initial_scale, float final_scale);
    float evaluate(long ticks);
private:
    float m_initial_velocity;
    float m_velocity;
    float m_acceleration;
    float m_deceleration;
    short m_acceleration_ticks;
    short m_deceleration_ticks;
    short m_total_ticks;
    bool m_valid;
};

#endif
