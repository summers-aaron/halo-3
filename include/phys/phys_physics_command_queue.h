#ifndef INCLUDE_PHYS_PHYSICS_COMMAND_QUEUE_H
#define INCLUDE_PHYS_PHYSICS_COMMAND_QUEUE_H
#include "phys/phys_rel_ptr32.h"
#include "phys/phys_physics_command_machine.h"
class phys_physics_command_queue {
public:
    phys_physics_command_queue(phys_physics_command *solver_buffer_start, int capacity) {
        m_start.set(solver_buffer_start);
        m_size = 0;
        m_small_capacity = capacity;
    }
    template <typename debug_command>
    __forceinline void add_command(debug_command command) {
        phys_physics_command *current = phys_add_byte_offset(m_start.val(), m_size);
        debug_command &destination = static_cast<debug_command &>(*current);
        destination = command;
        int size = (sizeof(debug_command) + 15) & ~15;
        m_size += size;
    }
    phys_rel_ptr32<phys_physics_command> m_start;
    unsigned int m_size;
    unsigned int m_small_capacity;
};
template <int NumBytes>
class phys_fixed_size_command_queue : public phys_physics_command_queue {
public:
    phys_fixed_size_command_queue()
        : phys_physics_command_queue(reinterpret_cast<phys_physics_command *>(m_buffer), NumBytes) {}
    __declspec(align(16)) unsigned char m_buffer[NumBytes];
};
typedef char phys_command_queue_size[(sizeof(phys_physics_command_queue) == 12) ? 1 : -1];
typedef char phys_fixed_command_queue_size[(sizeof(phys_fixed_size_command_queue<1024>) == 1040) ? 1 : -1];
#endif
