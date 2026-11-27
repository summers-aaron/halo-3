#ifndef INCLUDE_PHYS_STEP_INFO_H
#define INCLUDE_PHYS_STEP_INFO_H
#include "phys/phys_base_types.h"
class phys_step_info {
public:
    __forceinline phys_step_info() {}
    __forceinline phys_step_info(float step_start_time, float step_end_time) { set(step_start_time, step_end_time); }
    __forceinline void set(float step_start_time, float step_end_time) {
        m_step_start_time = step_start_time;
        m_end_time = step_end_time;
        m_delta_time = step_end_time - step_start_time;
        m_inv_dt = m_delta_time == 0.0f ? 0.0f : 1.0f / m_delta_time;
    }
    __declspec(align(16)) phys_padded<float> m_step_start_time;
    phys_padded<float> m_end_time;
    phys_padded<float> m_delta_time;
    phys_padded<float> m_inv_dt;
};
#endif
