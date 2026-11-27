#ifndef MEMORY_ALLOCATION_DESCRIPTION_H
#define MEMORY_ALLOCATION_DESCRIPTION_H

class c_raw_storage_buffer_iterator;
struct s_data_array;

class c_allocation_description
{
public:
    virtual unsigned long get_unaligned_size() const = 0;
    virtual void assign_storage(c_raw_storage_buffer_iterator &iterator) const = 0;
};

struct s_allocation_description_holder
{
    const c_allocation_description &description;
};

typedef char allocation_description_size[(sizeof(void *) != 4 || ((sizeof(c_allocation_description) == 4) )) ? 1 : -1];

template<typename T> class c_basic_buffer;
class c_wrapped_flags;

class c_buffer_allocation_description : public c_allocation_description
{
public:
    __forceinline c_buffer_allocation_description(c_basic_buffer<void> &buffer, unsigned long size, unsigned long alignment_bits)
        : m_buffer(&buffer), m_size(size), m_alignment_bits(alignment_bits) {}
    virtual unsigned long get_unaligned_size() const;
    virtual void assign_storage(c_raw_storage_buffer_iterator &iterator) const;
private:
    c_basic_buffer<void> *m_buffer;
    unsigned long m_size;
    unsigned long m_alignment_bits;
};

class c_wrapped_flags_allocation_description : public c_allocation_description
{
public:
    __forceinline c_wrapped_flags_allocation_description(c_wrapped_flags &flags, unsigned long count)
        : m_flags(&flags), m_bit_count(count) {}
    virtual unsigned long get_unaligned_size() const;
    virtual void assign_storage(c_raw_storage_buffer_iterator &iterator) const;
private:
    c_wrapped_flags *m_flags;
    unsigned long m_bit_count;
};

class c_data_array_allocation_description : public c_allocation_description
{
public:
    __forceinline c_data_array_allocation_description(s_data_array *&data, const char *name, long maximum_count, long datum_size, long alignment_bits)
        : m_data(&data), m_name(name), m_maximum_count(maximum_count), m_datum_size(datum_size), m_alignment_bits(alignment_bits) {}
    virtual unsigned long get_unaligned_size() const;
    virtual void assign_storage(c_raw_storage_buffer_iterator &iterator) const;
private:
    s_data_array **m_data;
    const char *m_name;
    long m_maximum_count;
    long m_datum_size;
    long m_alignment_bits;
};

typedef char data_array_allocation_description_size[(sizeof(void *) != 4 || sizeof(c_data_array_allocation_description) == 0x18) ? 1 : -1];
typedef char allocation_description_holder_size[(sizeof(void *) != 4 || sizeof(s_allocation_description_holder) == 4) ? 1 : -1];
typedef char buffer_allocation_description_size[(sizeof(void *) != 4 || sizeof(c_buffer_allocation_description) == 16) ? 1 : -1];
typedef char wrapped_flags_allocation_description_size[(sizeof(void *) != 4 || sizeof(c_wrapped_flags_allocation_description) == 12) ? 1 : -1];

#endif // MEMORY_ALLOCATION_DESCRIPTION_H
