#ifndef ANIMATIONS_MIXING_BOARD_MIXING_BOARD_SLIDER_TYPES_H
#define ANIMATIONS_MIXING_BOARD_MIXING_BOARD_SLIDER_TYPES_H

#include "cseries/cseries_wrapped.h"
#include "math/short_fixed.h"
#include "math/short_fixed_storage.h"
#include "units/slider_movement_pattern_types.h"
#include "units/slider_target_position_types.h"
#include "animations/mixing_board/channels/channel_base.h"
#include "animations/mixing_board/mixing_board_definition.h"

extern "C" const t_short_fixed<13> k_mixing_board_slider_targets[4];
extern "C" const float literal_256_0f;

class c_mixing_board_slider
{
public:
    void initialize()
    {
        m_position = 0;
        m_start_position = 0;
        m_velocity = 0;
        m_initial_velocity = 0;
        m_target_position = _slider_target_position_none;
        m_movement_pattern = _slider_movement_pattern_none;
        m_parent_slider_type = _mixing_board_parent_slider_none;
        m_parent_slider_index = -1;
    }

    t_short_fixed<13> get_value() const { return m_position; }
    t_short_fixed<8> time_remaining() const { return m_initial_velocity; }
    e_slider_target_positions get_target() const { return m_target_position; }
    bool active() const { return m_target_position != _slider_target_position_none && m_initial_velocity > 0; }
    bool parent_slider_valid() const { return m_parent_slider_index != -1 && m_parent_slider_type != _mixing_board_parent_slider_none; }
    long get_parent_slider_index() const { return m_parent_slider_index; }
    c_enum<e_mixing_board_parent_slider_type, unsigned char, 0, 3> get_parent_slider_type() const { return m_parent_slider_type; }

    __forceinline void freeze()
    {
        m_start_position = m_position;
        m_velocity = 0;
        m_initial_velocity = 0;
        m_target_position = _slider_target_position_none;
    }

    __forceinline void snap_to_target(e_slider_target_positions target)
    {
        if (target != _slider_target_position_none)
        {
            freeze();
            m_target_position = target;
            m_position = k_mixing_board_slider_targets[target];
        }
    }

    void move_to_target(e_slider_target_positions target_position, e_slider_movement_pattern movement_pattern, t_short_fixed<8> duration);
    void move_to_target(e_slider_target_positions target_position, e_slider_movement_pattern movement_pattern, float duration);
    void snap_to_position(float position);
    bool update();

    t_short_fixed<13> m_position;
    t_short_fixed<13> m_start_position;
    t_short_fixed<8> m_velocity;
    t_short_fixed<8> m_initial_velocity;
    c_enum<e_slider_movement_pattern, unsigned char, 0, 7> m_movement_pattern;
    c_enum<e_slider_target_positions, unsigned char, 0, 4> m_target_position;
    c_enum<e_mixing_board_parent_slider_type, unsigned char, 0, 3> m_parent_slider_type;
    signed char m_parent_slider_index;
};

typedef char mixing_board_slider_size[(sizeof(c_mixing_board_slider) == 12) ? 1 : -1];

template<class T>
class c_mixing_board_component_iterator
{
    T *m_base;
    long m_count;
    long m_index;

public:
    c_mixing_board_component_iterator() { m_base = 0; }
    ~c_mixing_board_component_iterator() {}

    bool setup(unsigned char *base, const c_mixing_board_definition *definition, long type)
    {
        bool result = false;
        m_index = 0;
        m_base = 0;
        m_count = definition->get_component_type_count(type);
        if (m_count > 0)
        {
            long offset = definition->get_component_type_offset(type);
            void *component_storage = base + offset;
            m_base = static_cast<T *>(component_storage);
            result = true;
        }
        return result;
    }

    bool next(T **component)
    {
        bool result = false;
        if (m_base && m_index < m_count)
        {
            *component = &m_base[m_index];
            m_index++;
            result = true;
        }
        return result;
    }
};

#endif // ANIMATIONS_MIXING_BOARD_MIXING_BOARD_SLIDER_TYPES_H
