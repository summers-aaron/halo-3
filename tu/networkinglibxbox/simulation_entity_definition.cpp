// OBJ: from the records 0x446e88 0x446ef8 0x446f80
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "memory/bitstream.h"
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ




// the 64-bit window internals (no name in the March 2007 build)


bool c_entity_update_encode_helper::make_room_for_update(c_bitstream *bitstream,
    long required_leave_space_bits, long update_component_first_index,
    long update_component_count, unsigned long update_mask)
{
    m_bitstream = bitstream;
    m_update_component_first_index = update_component_first_index;
    m_update_component_count = update_component_count;
    m_update_mask = update_mask;
    m_required_leave_space_bits = required_leave_space_bits;
    m_current_leave_space_bits = m_required_leave_space_bits + update_component_count;
    m_update_considered_mask = 0;
    m_update_written_mask = 0;
    m_update_skipped_mask = 0;
    m_update_overflowed_mask = 0;
    m_current_update_component_index = -1;
    m_current_update_component_name = 0;
    m_able_to_write_update = m_bitstream->get_space_left_in_bits() >= m_current_leave_space_bits;
    return m_able_to_write_update;
}

bool c_entity_update_encode_helper::write_component_header(long update_component_index, const char *update_component_name)
{
    bool result = false;
    m_current_update_component_index = update_component_index;
    m_current_update_component_name = update_component_name;
    m_update_considered_mask |= 1 << m_current_update_component_index;
    m_bitstream->push_position();
    if (m_update_mask & (1 << m_current_update_component_index))
    {
        m_bitstream->write_bool(m_current_update_component_name, true);
        m_update_written_mask |= 1 << m_current_update_component_index;
        result = true;
    }
    return result;
}

void c_entity_update_encode_helper::finish_component()
{
    bool discard_component = false;
    if (m_update_written_mask & (1 << m_current_update_component_index))
    {
        if (m_update_skipped_mask & (1 << m_current_update_component_index))
            discard_component = true;
        else if (m_bitstream->get_space_left_in_bits() < m_current_leave_space_bits)
        {
            m_update_overflowed_mask |= 1 << m_current_update_component_index;
            discard_component = true;
        }
    }
    if (discard_component)
    {
        m_update_written_mask &= ~(1 << m_current_update_component_index);
        m_bitstream->pop_position(true);
    }
    else
        m_bitstream->pop_position(false);
    if (!(m_update_written_mask & (1 << m_current_update_component_index)))
        m_bitstream->write_bool(m_current_update_component_name, false);
    --m_current_leave_space_bits;
    m_current_update_component_index = -1;
    m_current_update_component_name = 0;
}
