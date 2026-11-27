#ifndef MEMORY_BITSTREAM_H
#define MEMORY_BITSTREAM_H
// Bit-level serialization with a 64-bit window and a four-entry position stack.

#include <stddef.h>
#include "math/real_math_types.h"

#ifndef _XTL_
extern "C" void *XMemCpy(void *pDest, const void *pSrc, size_t count);
#endif
float dequantize_real(long quantized, float minimum, float maximum, long size_in_bits, bool exact_midpoint);
long quantize_real(float value,
        float minimum,
        float maximum,
        long size_in_bits,
        bool exact_midpoint,
        bool exact_endpoints);
static __forceinline unsigned long bitstream_strnlen(const char *s, unsigned long maximum)
{
    unsigned long length = 0;
    while (length < maximum && *s++)
        ++length;
    return length;
}
union real_vector3d;
extern "C" long bitstream_encode_unit_vector(const union real_vector3d *vector);
void bitstream_decode_unit_vector(long quantization, union real_vector3d *vector);

// The stream keeps its 64-bit words big-endian in memory, as the PowerPC ld/std of the Xbox build do: the byte-wise
// paths (the tail of a buffer, finish_writing near the end) are MSB-first, so on a little-endian host every 64-bit word
// access swaps, or a word written whole and read byte-wise (the last partial word of every stream) comes back scrambled.
#if defined(_XBOX) && !defined(__clang__)
#define bitstream_load_word(data) (*(const unsigned __int64 *)(data))
#define bitstream_store_word(data, value) (*(unsigned __int64 *)(data) = (value))
#else
static inline unsigned __int64 bitstream_load_word(const unsigned char *data)
{
    return __builtin_bswap64(*(const unsigned __int64 *)data);
}
static inline void bitstream_store_word(unsigned char *data, unsigned __int64 value)
{
    *(unsigned __int64 *)data = __builtin_bswap64(value);
}
#endif

// The 64-bit window is shifted by counts that reach 64 (an empty window shifted by a whole word, a full one emptied).
// PowerPC sld/srd take the count modulo 128, so a count of 64 gives 0, and the stream relies on it; x86 takes it
// modulo 64 (undefined in C++), which leaves stale bits in the window and corrupts raw 64-bit payloads.
#if defined(_XBOX) && !defined(__clang__)
#define bitstream_shift_left(value, count) ((value) << (count))
#define bitstream_shift_right(value, count) ((value) >> (count))
#else
static inline unsigned __int64 bitstream_shift_left(unsigned __int64 value, unsigned long count)
{
    return count < 64 ? value << count : 0;
}
static inline unsigned __int64 bitstream_shift_right(unsigned __int64 value, unsigned long count)
{
    return count < 64 ? value >> count : 0;
}
#endif

struct s_bitstream_position
{
    long current_stream_bit_position;
    long current_memory_bit_position;
    unsigned __int64 window;
    long window_bits_used;
    unsigned char *next_data;
};

class c_bitstream
{
public:
    void begin_writing(long data_size_alignment, long debug_mode, bool debug_structures);
    void read_vector(const char *name,
        union real_vector3d *vector,
        float min_value,
        float max_value,
        long size_in_bits);
    void write_vector(const char *name,
        const union real_vector3d *vector,
        float min_value,
        float max_value,
        long size_in_bits);
    void read_axes(const char *name, union real_vector3d *forward, union real_vector3d *up);
    void write_axes(const char *name, const union real_vector3d *forward, const union real_vector3d *up);
    void read_string_wchar(const char *name, unsigned short *string, long max_string_length);
    void read_string_wchar_remapped(const char *name, unsigned short *string, long max_string_length);
    void write_string_wchar_remapped(const char *name, const unsigned short *string, long max_string_length);
    void write_string_wchar(const char *name, const unsigned short *string, long maximum_length);
    static bool compare_quantized_reals(float first,
        float second,
        float minimum,
        float maximum,
        long size_in_bits,
        bool exact_midpoint,
        bool wrap);
    static bool compare_vectors(const union real_vector3d *first,
        const union real_vector3d *second,
        float min_magnitude,
        float max_magnitude,
        long magnitude_size_in_bits);
    static bool compare_logarithmic_quantized_reals(float first,
        float second,
        float minimum,
        float maximum,
        long size_in_bits);
    static bool compare_axes(const union real_vector3d *forward1,
        const union real_vector3d *up1,
        const union real_vector3d *forward2,
        const union real_vector3d *up2);
    static bool compare_unit_vectors(const union real_vector3d *first, const union real_vector3d *second);
    static void axes_compute_reference_internal(const union real_vector3d *up,
        union real_vector3d *forward_reference,
        union real_vector3d *left_reference);
    static float axes_to_angle_internal(const union real_vector3d *forward, const union real_vector3d *up);
    static void angle_to_axes_internal(const union real_vector3d *up, float angle, union real_vector3d *forward);

    long read_signed_integer(const char *name, long size_in_bits)
    {
        long range = 1 << (size_in_bits - 1);    // March: feeds ASSERT("value >= -range && value < range") only
        unsigned long value = read_value_internal(size_in_bits);
        if (size_in_bits < 32 && (value & (1 << (size_in_bits - 1))))
            value |= ~((1i64 << size_in_bits) - 1);
        long result = value;
        return result;
    }
    // Unit vectors use a 19-bit cube-face encoding.
    void read_unit_vector(const char *name, union real_vector3d *vector)
    {
        long quantized = read_integer(name, 19);
        bitstream_decode_unit_vector(quantized, vector);
    }
    void write_unit_vector(const char *name, const union real_vector3d *vector)
    {
        write_value_internal((unsigned long)bitstream_encode_unit_vector(vector), 19);
    }
    const unsigned char *get_data(long *data_length) const { *data_length = m_data_size_bytes; return m_data; }
    void begin_reading() { reset(3); }
    void finish_reading() { m_state = 5; }
    void discard_remaining_data()
    {
        m_current.current_stream_bit_position = m_data_size_bytes << 3;
        m_current.window = 0;
        m_current.window_bits_used = 0;
    }
    long get_space_used_in_bytes() const
    {
        return (m_current.current_stream_bit_position + m_current.window_bits_used + 7) / 8;
    }
    void write_logarithmic_quantized_real(const char *name,
        float value,
        float min_value,
        float max_value,
        long size_in_bits);
    float read_logarithmic_quantized_real(const char *name, float min_value, float max_value, long size_in_bits);
    void write_raw_data(const char *name, const void *data, long size_in_bits)
    {
        write_bits_internal((const unsigned char *)data, size_in_bits);
    }
    void write_integer(const char *name, unsigned long value, long size_in_bits)
    {
        write_value_internal(value, size_in_bits);
    }
    void write_signed_integer(const char *name, long value, long size_in_bits)
    {
        write_value_internal((unsigned long)(value & ((1 << size_in_bits) - 1)), size_in_bits);
    }
    void write_bool(const char *name, bool value) { write_bit_internal(value); }
    void read_raw_data(const char *name, void *data, long size_in_bits)
    {
        read_bits_internal((unsigned char *)data, size_in_bits);
    }
    unsigned long read_integer(const char *name, long size_in_bits) { return read_value_internal(size_in_bits); }
    bool read_bool(const char *name) { return read_bit_internal(); }
    float read_quantized_real(const char *name, float minimum, float maximum, long size_in_bits, bool exact_midpoint)
    {
        long value = read_value_internal(size_in_bits);
        return dequantize_real(value, minimum, maximum, size_in_bits, exact_midpoint);
    }

    __forceinline void read_string(const char *name, char *string, long maximum_length)
    {
        long index;
        for (index = 0; index < maximum_length; index++)
        {
            unsigned char character = (unsigned char)read_value_internal(8);
            string[index] = character;
            if ((signed char)character == 0)
                break;
        }
        if (index >= maximum_length)
        {
            string[maximum_length - 1] = 0;
            m_error_occurred = true;
        }
    }
    __forceinline void write_string(const char *name, const char *string, long maximum_length)
    {
        unsigned long length = bitstream_strnlen(string, maximum_length);
        m_encoded_value_bits += ((length ? maximum_length - 1 : 0) + 1) * 8;
        for (long index = 0; index < maximum_length; index++)
        {
            unsigned char character = (unsigned char)string[index];
            write_value_internal((unsigned long)character, 8);
            if (!character)
                break;
        }
    }
    __forceinline void write_quantized_real(const char *name,
        float value,
        float minimum,
        float maximum,
        long size_in_bits,
        bool exact_midpoint,
        bool exact_endpoints)
    {
        unsigned long quantized = quantize_real(
            value, minimum, maximum, size_in_bits, exact_midpoint, exact_endpoints);
        write_value_internal(quantized, size_in_bits);
        m_encoded_value_bits += size_in_bits;
    }
    __forceinline void write_point3d(const char *name, const long_point3d *point, long axis_encoding_size_in_bits)
    {
        for (long axis = 0; axis < 3; axis++)
        {
            write_value_internal((unsigned long)point->n[axis], axis_encoding_size_in_bits);
            m_encoded_value_bits += axis_encoding_size_in_bits;
        }
    }
    void reset(long state);
    void finish_writing(long *space_used);
    void set_data(unsigned char *data, long data_size_bytes)
    {
        m_data = data;
        m_data_max = data + data_size_bytes;
        m_data_size_bytes = data_size_bytes;
        reset(0);
    }
    void begin_writing(long data_size_alignment = 1)
    {
        m_data_size_alignment = data_size_alignment;
        if (m_data_size_alignment != 1 && m_data_size_alignment != 2 && m_data_size_alignment != 4)
            m_data_size_alignment = 1;
        m_state = 1; m_current.current_stream_bit_position = 0; m_current.current_memory_bit_position = 0;
        m_position_stack_depth = 0; m_error_occurred = false; m_current.next_data = m_data; m_encoded_value_bits = 0;
        m_current.window = 0; m_current.window_bits_used = 0; m_number_of_bits_rewound = 0; m_number_of_position_resets = 0;
    }
    bool reading() const { return m_state == 2 || m_state == 3; }
    bool read_only_for_consistency() const { return m_state == 4; }
    bool overflowed() const { return m_current.current_stream_bit_position > m_data_size_bytes * 8; }
    bool error_occurred() const { bool result = overflowed(); if (m_error_occurred) result = true; return result; }
    bool would_overflow(long size_in_bits) const
    {
        return m_current.current_stream_bit_position + size_in_bits > m_data_size_bytes * 8;
    }
    long get_space_left_in_bits() const
    {
        return m_data_size_bytes * 8 - m_current.window_bits_used - m_current.current_stream_bit_position;
    }
    long get_space_used_in_bits() const { return m_current.current_stream_bit_position + m_current.window_bits_used; }
    void append(const c_bitstream *was_writing)
    {
        write_bits_internal(was_writing->m_data, was_writing->m_current.current_stream_bit_position);
        m_encoded_value_bits += was_writing->m_current.current_stream_bit_position;
        m_number_of_bits_rewound += was_writing->m_number_of_bits_rewound;
        m_number_of_position_resets += was_writing->m_number_of_position_resets;
    }
    void push_position()
    {
        XMemCpy(&m_position_stack[m_position_stack_depth], &m_current, sizeof(m_current));
        m_position_stack_depth++;
    }
    void pop_position(bool restore)
    {
        --m_position_stack_depth;
        if (restore)
            XMemCpy(&m_current, &m_position_stack[m_position_stack_depth], sizeof(m_current));
    }

    void write_value_internal(unsigned long value, long size_in_bits);
    void write_value_internal(unsigned __int64 value, long size_in_bits);

private:
    void write_bits_internal(const unsigned char *data, long size_in_bits);
    void read_bits_internal(unsigned char *data, long size_in_bits);
    void write_bit_internal(bool value);
    bool read_bit_internal();
    unsigned long read_value_internal(long size_in_bits);

public:
    unsigned char *m_data;
    unsigned char *m_data_max;
    long m_data_size_bytes;
    long m_data_size_alignment;
    long m_state;
    bool m_error_occurred;
    s_bitstream_position m_current;
    // Sum of the declared sizes of the quantized, point, axis and string values written; integers and bools are not counted.
    long m_encoded_value_bits;
    long m_position_stack_depth;
    s_bitstream_position m_position_stack[4];
    long m_number_of_bits_rewound;
    long m_number_of_position_resets;
};

// Attach a packet to a caller buffer with byte alignment, as the film and
// simulation-queue encoders do before begin_writing.
static __forceinline void bitstream_construct(c_bitstream *packet, unsigned char *buffer, long buffer_size)
{
    packet->m_data_size_alignment = 1;
    packet->m_data = buffer;
    packet->m_data_max = buffer + buffer_size;
    packet->m_data_size_bytes = buffer_size;
    packet->reset(0);
}

// Reads up to 64 bits from the window.
extern "C" unsigned __int64 bitstream_read_window_bits(c_bitstream *bitstream, long size_in_bits);

// Reads 32 window bits into 4 bytes, most significant first.
static __forceinline void read_bytes32(c_bitstream *stream, unsigned char *bytes)
{
    unsigned __int64 value = bitstream_read_window_bits(stream, 32) << 32;
    for (unsigned long i = 4; i != 0; i--) {
        *bytes++ = (unsigned char)(value >> 56);
        value <<= 8;
    }
}

typedef char bitstream_position_size_must_be_24[(sizeof(void *) != 4 || ((sizeof(s_bitstream_position) == 24) )) ? 1 : -1];
typedef char bitstream_size_must_be_160[(sizeof(void *) != 4 || ((sizeof(c_bitstream) == 160) )) ? 1 : -1];
typedef char bitstream_current_offset[
    (sizeof(void *) != 4 || offsetof(c_bitstream, m_current) == 0x18) ? 1 : -1];

#endif // MEMORY_BITSTREAM_H
