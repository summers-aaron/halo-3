#ifndef EFFECTS_EDITABLE_PROPERTY_H
#define EFFECTS_EDITABLE_PROPERTY_H

#include "math/function_definitions.h"
#include "math/real_math_types.h"

class c_particle_state_list;

class c_editable_property_base
{
public:
    template<class T> __forceinline float evaluate(const T *states) const
    {
        if (m_flags & 0x20)
            return m_constant;
        float value = m_function.evaluate_scalar(states->get_state_value(m_input), states->get_state_value(m_range));
        long modifier_input = m_modifier_input;
        if (modifier_input != -1)
        {
            switch (m_modifier)
            {
            case 1: value += states->get_state_value(modifier_input); break;
            case 2: value *= states->get_state_value(modifier_input); break;
            }
        }
        return value;
    }
    __forceinline bool is_constant_over_time() const { return (m_flags & 0x80) != 0; }
    static __forceinline bool property_value_is_initialized(const float *value)
    {
        return *(const long *)value != -1;
    }
    template<class T> static bool property_value_is_initialized(const T *value) { return *reinterpret_cast<const long *>(value) != -1; }
protected:
    unsigned char m_input;
    unsigned char m_range;
    unsigned char m_modifier;
    unsigned char m_modifier_input;
    c_function_definition m_function;
    float m_constant;
    unsigned char m_flags;
};

template<> float c_editable_property_base::evaluate<c_particle_state_list>(const c_particle_state_list *states) const;

template<class T> class c_editable_property : public c_editable_property_base {};

class c_contrail_states;
class c_light_volume_states;
template<> class c_editable_property<c_contrail_states> : public c_editable_property_base
{
public:
    float evaluate(const c_contrail_states *states) const;
};
template<> class c_editable_property<c_light_volume_states> : public c_editable_property_base
{
public:
    float evaluate(const c_light_volume_states *states) const;
};

template<class T> class c_editable_property_type_base;
template<> class c_editable_property_type_base<real_point2d> : public c_editable_property_base
{
private:
    real_point2d m_start;
    real_point2d m_end;
};
template<> class c_editable_property<real_point2d> : public c_editable_property_type_base<real_point2d> {};

template<> class c_editable_property_type_base<real_vector3d> : public c_editable_property_base
{
protected:
    const real_vector3d interpolate(float t) const;
public:
    __forceinline real_vector3d interpolate_public(float t) const { return interpolate(t); }
private:
    real_vector3d m_start;
    real_vector3d m_end;
};
template<> class c_editable_property_type_base<real_point3d> : public c_editable_property_base
{
protected:
    const real_point3d interpolate(float t) const
    {
        real_point3d result;
        result.x = m_start.x * (1.0f - t) + m_end.x * t;
        result.y = m_start.y * (1.0f - t) + m_end.y * t;
        result.z = m_start.z * (1.0f - t) + m_end.z * t;
        return result;
    }
private:
    real_point3d m_start;
    real_point3d m_end;
};
template<> class c_editable_property<real_point3d> : public c_editable_property_type_base<real_point3d>
{
public:
    const real_point3d evaluate(const c_particle_state_list *states) const
    {
        return this->interpolate(c_editable_property_base::evaluate(states));
    }
};

template<> class c_editable_property<real_vector3d> : public c_editable_property_type_base<real_vector3d>
{
public:
    const real_vector3d evaluate(const c_particle_state_list *states) const
    {
        return this->interpolate(c_editable_property_base::evaluate(states));
    }
};

typedef char editable_property_size[(sizeof(c_editable_property_base) == 0x20) ? 1 : -1];
#endif
