#ifndef NETWORKING_SIMULATION_SIMULATION_ENTITY_DEFINITION_H
#define NETWORKING_SIMULATION_SIMULATION_ENTITY_DEFINITION_H

// Bungie: simulation\simulation_entity_definition.cpp. Member names from the March 2007 asserts there
// ("m_bitstream==NULL", "m_able_to_write_update", "(m_update_written_mask|m_update_skipped_mask|
// m_update_overflowed_mask)==m_update_mask", "!TEST_BIT(m_update_considered_mask, ...)").

class c_bitstream;

// Writes one entity update as a run of optional components (simulation_game_*.obj entity_update_encode).
class c_entity_update_encode_helper
{
public:
    c_entity_update_encode_helper() : m_bitstream(0) {}
    ~c_entity_update_encode_helper() {}

    bool make_room_for_update(c_bitstream *bitstream, long required_leave_space_bits,
        long update_component_first_index, long update_component_count, unsigned long update_mask);
    // March defines finish_update and skip_component in simulation_entity_definition.cpp (asserts at lines 61
    // and 194); both merge a single mask update in place.
    void finish_update(unsigned long *update_mask_written) { *update_mask_written |= m_update_written_mask; }
    bool write_component_header(long update_component_index, const char *update_component_name);
    void finish_component();
    void skip_component() { m_update_skipped_mask |= 1 << m_current_update_component_index; }

    // Public only because two units still read m_update_written_mask directly (see the lane log).
    c_bitstream *m_bitstream;
    unsigned long m_update_component_first_index;
    unsigned long m_update_component_count;
    unsigned long m_update_mask;
    unsigned long m_update_considered_mask;
    unsigned long m_update_written_mask;
    unsigned long m_update_skipped_mask;
    unsigned long m_update_overflowed_mask;
    bool m_able_to_write_update;
    long m_current_update_component_index;
    const char *m_current_update_component_name;
    long m_required_leave_space_bits;
    long m_current_leave_space_bits;
};

typedef char simulation_entity_definition_size_helper[(sizeof(void *) != 4 || ((sizeof(c_entity_update_encode_helper) == 0x34) )) ? 1 : -1];

#endif
