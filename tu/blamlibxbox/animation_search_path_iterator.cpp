// blamlibxbox/animation_search_path_iterator.obj: from the records 0xe8a20
// @flags /TP /O2 /Oc

#include "cseries/cseries.h"
#include "animations/animation_search_path_iterator.h"

static const long k_string_id_none = 200;

// @address 0xe8a20 ?get_next_weapon_type@c_animation_search_path_iterator@@QAAPBVc_weapon_type@@PAJ00@Z
const c_weapon_type *c_animation_search_path_iterator::get_next_weapon_type(
    long *result_mode, long *result_weapon_class, long *result_weapon_type)
{
    const c_weapon_type *result = 0;
    while (!result && m_state < 8)
    {
        unsigned long state = m_state;
        if (!m_mode)
        {
            bool skip = false;
            if (TEST_BIT(state, 2))
            {
                if (m_mode_name != k_string_id_none)
                    *result_mode = k_string_id_none;
                else
                    skip = true;
            }
            else
            {
                *result_mode = m_mode_name;
            }
            if (!skip)
            {
                m_mode = m_graph->find_mode(*result_mode);
                m_weapon_class = 0;
            }
        }
        const c_animation_mode *mode = m_mode;
        if (mode)
        {
            if (!m_weapon_class)
            {
                bool skip = false;
                if (TEST_BIT(state, 1))
                {
                    if (m_weapon_class_name != k_string_id_none)
                        *result_weapon_class = k_string_id_none;
                    else
                        skip = true;
                }
                else
                {
                    *result_weapon_class = m_weapon_class_name;
                }
                if (!skip)
                    m_weapon_class = mode->find_class(*result_weapon_class);
            }
            const c_weapon_class *weapon_class = m_weapon_class;
            if (weapon_class)
            {
                bool skip = false;
                if (TEST_BIT(state, 0))
                {
                    if (m_weapon_type_name != k_string_id_none)
                        *result_weapon_type = k_string_id_none;
                    else
                        skip = true;
                }
                else
                {
                    *result_weapon_type = m_weapon_type_name;
                }
                if (!skip)
                    result = weapon_class->find_type(*result_weapon_type);
                m_state = state + 1;
                if (!TEST_BIT(m_state, 0))
                {
                    m_weapon_class = 0;
                    if (!TEST_BIT(m_state, 1))
                        m_mode = 0;
                }
            }
            else
            {
                m_state = state + 2;
                m_state &= ~FLAG(0);
                if (!TEST_BIT(m_state, 1))
                    m_mode = 0;
            }
        }
        else
        {
            m_state = state + 4;
            m_state &= ~FLAG(1);
            m_state &= ~FLAG(0);
            m_weapon_class = 0;
        }
    }
    return result;
}
