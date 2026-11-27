#ifndef MEMORY_BASIC_BUFFER_H
#define MEMORY_BASIC_BUFFER_H

template <typename t_type>
class c_basic_buffer
{
public:
    __forceinline c_basic_buffer() : m_address(0), m_size(0) { }
    __forceinline c_basic_buffer(t_type *address, unsigned long size) : m_address(address), m_size(size) { }
    __forceinline c_basic_buffer<t_type> slice(unsigned long offset, unsigned long length) const {
        unsigned long usable = m_size - offset;
        unsigned long count = length <= usable ? length : usable;
        c_basic_buffer<t_type> original = *this;
        return c_basic_buffer<t_type>((t_type *)((char *)original.m_address + offset), count);
    }
    enum e_uninitialized { _uninitialized };
    __forceinline c_basic_buffer(e_uninitialized) { }
    __forceinline c_basic_buffer(c_basic_buffer buffer, unsigned long offset, unsigned long size)
        : m_address((t_type *)((char *)buffer.m_address + offset)), m_size(size) {}
    __forceinline t_type *get_buffer() const { return m_address; }
    bool empty() const { return m_size == 0; }
    void *get_range(unsigned long offset, unsigned long size) const { return (char *)m_address + offset; }
    template <typename t_element>
    t_element *get_as_type_from_offset(unsigned long offset) const { return (t_element *)((char *)m_address + offset); }
    __forceinline c_basic_buffer<t_type> get_sub_buffer(unsigned long offset, unsigned long size) const {
        c_basic_buffer<t_type> result;
        result.m_address = (t_type *)get_range(offset, size);
        result.m_size = size;
        return result;
    }
    __forceinline void set_buffer(t_type *buffer, unsigned long size)
    {
        m_address = buffer;
        m_size = size;
    }
    t_type *m_address;
    unsigned long m_size;
};

#endif // MEMORY_BASIC_BUFFER_H
