// blamlibxbox/bitstream.obj: from the records 0xa2788 0xa2dc0 0x220330 0x2204f8 0x2205e8 0x220668 0x220790 0x2208b0 0x2209d0 0x220a50 0x220b48 0x220c70 0x220d10 0x220da0 0x220e68 0x221030 0x2210e8 0x221228 0x2212e0 0x221398 0x221480 0x221560 0x221768 0x221888 0x221998 0x2afb18 0x2afc18 0x2afd40
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include <math.h>
#include <string.h>
#include "memory/bitstream.h"


const float k_bitstream_pi = 3.1415927f;
const float k_bitstream_epsilon = 0.0001f;

// The character ranges write_string_wchar_remapped/read_string_wchar_remapped pack into 7 bits (first and last of each:
// '\0', '0'-'9', 'a'-'z', 'A'-'Z', '('-')', ' ').
// @address 0x358b0 k_bitstream_remapped_char_ranges
// @rdata 0x820358b0..0x820358bc
static const signed char k_bitstream_remapped_char_ranges[6][2] = { { 0, 0 }, { '0', '9' }, { 'a', 'z' }, { 'A', 'Z' }, { '(', ')' }, { ' ', ' ' } };
// @rdata-end

static __forceinline bool bitstream_state_is_reading(long state)
{
    return state == 3 || state == 4;
}

// @address 0xa2788 ?begin_writing@c_bitstream@@QAAXJJ_N@Z
void c_bitstream::begin_writing(long data_size_alignment, long debug_mode, bool debug_structures)
{
    switch (data_size_alignment)
    {
    case 1:
    case 2:
    case 4:
    case 8:
        m_data_size_alignment = data_size_alignment;
        break;
    default:
        m_data_size_alignment = data_size_alignment;
        break;
    }
    m_state = 1;
    m_current.current_stream_bit_position = 0;
    m_current.current_memory_bit_position = 0;
    m_position_stack_depth = 0;
    m_error_occurred = false;
    m_current.next_data = m_data;
    m_encoded_value_bits = 0;
    m_current.window = 0;
    m_current.window_bits_used = 0;
    m_number_of_bits_rewound = 0;
    m_number_of_position_resets = 0;
}

// @address 0xa2dc0 ?finish_writing@c_bitstream@@QAAXPAJ@Z
void c_bitstream::finish_writing(long *space_used)
{
    unsigned long bits_used = m_current.window_bits_used;
    if (bits_used != 64)
        m_current.window = bitstream_shift_left(m_current.window, 64 - bits_used);
    unsigned __int64 output = m_current.window;
    if (m_current.next_data + 8 <= m_data_max)
    {
        bitstream_store_word(m_current.next_data, output);
        m_current.next_data += 8;
    }
    else
    {
        while (m_current.next_data < m_data_max)
        {
            *m_current.next_data = (unsigned char)(output >> 56);
            output <<= 8;
            m_current.next_data++;
        }
    }
    m_current.current_stream_bit_position += bits_used;
    m_current.window = 0;
    m_current.window_bits_used = 0;
    m_data_size_bytes = (m_current.current_stream_bit_position + 7) / 8;
    m_data_max = m_data + m_data_size_bytes;
    if (m_data_size_bytes % m_data_size_alignment)
        m_data_size_bytes = m_data_size_bytes - m_data_size_bytes % m_data_size_alignment + m_data_size_alignment;
    m_state = 2;
}

static __forceinline unsigned int bitstream_ustrnlen(const unsigned short *string, unsigned int size)
{
    unsigned int length;
    for (length = 0; length < size; length++) {
        if (string[length] == 0)
            break;
    }
    return length;
}

// 0x220330: the remapped wchar writer. Every character must fall inside one of the six ranges of
// k_bitstream_remapped_char_ranges, else the whole string is written as one zero; otherwise each character is written as
// 7 bits of the ranges' compact alphabet (its offset from the range start plus all earlier ranges).
// @address 0x220330 ?write_string_wchar_remapped@c_bitstream@@QAAXPBDPBGJ@Z
void c_bitstream::write_string_wchar_remapped(const char *name, const unsigned short *string, long max_string_length)
{
    unsigned long length = bitstream_ustrnlen(string, max_string_length);
    m_encoded_value_bits += (length ? max_string_length : 1) * 7;
    bool valid = true;
    for (long index = 0; index < max_string_length; index++) {
        unsigned short character = string[index];
        bool found = false;
        for (unsigned long range_index = 0; range_index < 6; range_index++) {
            if (character >= k_bitstream_remapped_char_ranges[range_index][0] && character <= k_bitstream_remapped_char_ranges[range_index][1]) {
                found = true;
                break;
            }
        }
        if (!found)
            valid = false;
        if (string[index] == 0)
            break;
    }
    if (valid) {
        for (long index = 0; index < max_string_length; index++) {
            unsigned short character = string[index];
            long base = 0;
            long range_index = 0;
            const signed char *range = &k_bitstream_remapped_char_ranges[0][0];
            for (; range_index < 6; range_index++, range += 2) {
                if (character >= range[0] && character <= range[1]) {
                    character = (unsigned short)(character - k_bitstream_remapped_char_ranges[range_index][0] + base);
                    break;
                }
                base = base + (range[1] - range[0]) + 1;
            }
            write_integer(name, character, 7);
            if (character == 0)
                break;
        }
    } else {
        write_integer(name, 0, 7);
    }
}

// @address 0x2204f8 ?axes_compute_reference_internal@c_bitstream@@SAXPBTreal_vector3d@@PAT2@1@Z
void c_bitstream::axes_compute_reference_internal(const real_vector3d *up, real_vector3d *forward_reference, real_vector3d *left_reference)
{
  const real_vector3d forward = { 1.0f, 0.0f, 0.0f };
  const real_vector3d left = { 0.0f, 1.0f, 0.0f };
  float forward_dot = dot_product3d(up, &forward);
  float left_dot = dot_product3d(up, &left);
  if (fabsf(forward_dot) < fabsf(left_dot))
    cross_product3d(up, &forward, forward_reference);
  else
    cross_product3d(&left, up, forward_reference);
  ::normalize3d(forward_reference);
  cross_product3d(up, forward_reference, left_reference);
  ::normalize3d(left_reference);
}

// @address 0x2205e8 ?axes_to_angle_internal@c_bitstream@@SAMPBTreal_vector3d@@0@Z
float c_bitstream::axes_to_angle_internal(const real_vector3d *forward, const real_vector3d *up)
{
    real_vector3d forward_reference;
    real_vector3d left_reference;
    axes_compute_reference_internal(up, &forward_reference, &left_reference);
    float forward_amount = dot_product3d(&forward_reference, forward);
    float left_amount = dot_product3d(&left_reference, forward);
    return (float)atan2((double)left_amount, (double)forward_amount);
}

namespace {
static __forceinline real_vector3d *bitstream_rotate_vector(real_vector3d *vector, const real_vector3d *axis, float sine, float cosine)
{
    float scale = (axis->i * vector->i + axis->j * vector->j + axis->k * vector->k) * (1.0f - cosine);
    float cross_i = vector->j * axis->k - vector->k * axis->j;
    float cross_j = vector->k * axis->i - vector->i * axis->k;
    float cross_k = vector->i * axis->j - vector->j * axis->i;
    vector->i = axis->i * scale + vector->i * cosine - cross_i * sine;
    vector->j = axis->j * scale + vector->j * cosine - cross_j * sine;
    vector->k = axis->k * scale + vector->k * cosine - cross_k * sine;
    return vector;
}
}
// @address 0x220668 ?angle_to_axes_internal@c_bitstream@@SAXPBTreal_vector3d@@MPAT2@@Z
void c_bitstream::angle_to_axes_internal(const real_vector3d *up, float angle, real_vector3d *forward)
{
    real_vector3d forward_reference;
    real_vector3d left_reference;
    axes_compute_reference_internal(up, &forward_reference, &left_reference);
    *forward = forward_reference;
    float cosine;
    float sine;
    if (angle != 3.1415927f && angle != -3.1415927f) {
        sine = (float)sin((double)angle);
        cosine = (float)cos((double)angle);
    } else {
        sine = 0.0f;
        cosine = -1.0f;
    }
    bitstream_rotate_vector(forward, up, sine, cosine);
    normalize3d(forward);
}


__forceinline void c_bitstream::write_logarithmic_quantized_real(const char *name, float value, float min_value, float max_value, long size_in_bits)
{
    long quantized_value = quantize_real((float)log((double)value), (float)log((double)min_value), (float)log((double)max_value), size_in_bits, false, false);
    write_integer(name, quantized_value, size_in_bits);
    m_encoded_value_bits += size_in_bits;
}

// @address 0x220790 ?write_vector@c_bitstream@@QAAXPBDPBTreal_vector3d@@MMJ@Z
void c_bitstream::write_vector(const char *name, const real_vector3d *vector, float min_magnitude, float max_magnitude, long magnitude_size_in_bits)
{
    real_vector3d direction = *vector;
    float magnitude = ::normalize3d(&direction);
    magnitude = magnitude > max_magnitude ? max_magnitude : magnitude;
    if (magnitude >= min_magnitude)
    {
        write_bool("vector-is-zero", false);
        write_unit_vector("direction", &direction);
        write_logarithmic_quantized_real("magnitude", magnitude, min_magnitude, max_magnitude, magnitude_size_in_bits);
    }
    else
    {
        write_bool("vector-is-zero", true);
    }
}


// @address 0x2208b0 ?write_axes@c_bitstream@@QAAXPBDPBTreal_vector3d@@1@Z
void c_bitstream::write_axes(const char *name, const real_vector3d *forward, const real_vector3d *up)
{
    real_vector3d quantized_up;
    if (!(fabsf(up->i) >= k_bitstream_epsilon) && !(fabsf(up->j) >= k_bitstream_epsilon) && !(fabsf(up->k - 1.0f) >= k_bitstream_epsilon)) {
        write_bool("up-is-global-up3d", true);
        quantized_up = *global_up3d;
    } else {
        unsigned long quantization = bitstream_encode_unit_vector(up);
        write_bool("up-is-global-up3d", false);
        write_integer("up-quantization", quantization, 19);
        bitstream_decode_unit_vector(quantization, &quantized_up);
    }
    long encoded = quantize_real(axes_to_angle_internal(forward, &quantized_up), -k_bitstream_pi, k_bitstream_pi, 8, true, false);
    write_integer("forward-angle", encoded, 8);
    m_encoded_value_bits += 8;
}

// @address 0x2209d0 ?read_string_wchar@c_bitstream@@QAAXPBDPAGJ@Z
void c_bitstream::read_string_wchar(const char *name, unsigned short *string, long max_string_length)
{
  long index;
  for (index = 0; index < max_string_length; index++) {
    unsigned short character = (unsigned short)read_value_internal(16);
    string[index] = character;
    if (character == 0)
      break;
  }
  if (index >= max_string_length) {
    string[max_string_length - 1] = 0;
    m_error_occurred = true;
  }
}

// @address 0x220a50 ?read_string_wchar_remapped@c_bitstream@@QAAXPBDPAGJ@Z
void c_bitstream::read_string_wchar_remapped(const char *name, unsigned short *string, long max_string_length)
{
  long index;
  for (index = 0; index < max_string_length; index++) {
    unsigned short character = (unsigned short)read_value_internal(7);
    long base = 0;
    long range_index = 0;
    const signed char *range = &k_bitstream_remapped_char_ranges[0][0];
    for (; range_index < 6; range_index++, range += 2) {
      long range_size = range[1] - range[0];
      if (character - base < range_size + 1) {
        character = (unsigned short)(k_bitstream_remapped_char_ranges[range_index][0] - base + character);
        break;
      }
      base = range_size + base + 1;
    }
    string[index] = character;
    if (character == 0)
      break;
  }
  if (index >= max_string_length) {
    string[max_string_length - 1] = 0;
    m_error_occurred = true;
  }
}

__forceinline float c_bitstream::read_logarithmic_quantized_real(const char *name, float min_value, float max_value, long size_in_bits)
{
    long maximum_value = (1 << size_in_bits) - 1;
    long quantized_value = read_integer(name, size_in_bits);
    float value;
    if (quantized_value == 0)
    {
        value = min_value;
    }
    else if (quantized_value >= maximum_value)
    {
        value = max_value;
    }
    else
    {
        float log_min_value = (float)log(min_value);
        float log_max_value = (float)log(max_value);
        value = (float)exp(dequantize_real(quantized_value, log_min_value, log_max_value, size_in_bits, false));
    }
    return value;
}

// @address 0x220b48 ?read_vector@c_bitstream@@QAAXPBDPATreal_vector3d@@MMJ@Z
void c_bitstream::read_vector(const char *name, real_vector3d *vector, float min_magnitude, float max_magnitude, long magnitude_size_in_bits)
{
    if (!read_bool("is-zero"))
    {
        real_vector3d direction;
        read_unit_vector("direction", &direction);
        float magnitude = read_logarithmic_quantized_real("magnitude", min_magnitude, max_magnitude, magnitude_size_in_bits);
        vector->i = direction.i * magnitude;
        vector->j = direction.j * magnitude;
        vector->k = direction.k * magnitude;
    }
    else
    {
        *vector = *global_zero_vector3d;
    }
}

// the reader of the axes pair: up is the default up or a 19-bit quantized unit
// vector, forward is an 8-bit angle about up
// @address 0x220c70 ?read_axes@c_bitstream@@QAAXPBDPATreal_vector3d@@1@Z
void c_bitstream::read_axes(const char *name, real_vector3d *forward, real_vector3d *up)
{
    if (read_bool("is-default-up"))
    {
        *up = *global_up3d;
    }
    else
    {
        unsigned long quantized_up = read_value_internal(19);
        bitstream_decode_unit_vector(quantized_up, up);
    }
    float angle = dequantize_real(read_integer("forward-angle", 8), -k_bitstream_pi, k_bitstream_pi, 8, true);
    angle_to_axes_internal(up, angle, forward);
}

// @address 0x220d10 ?compare_quantized_reals@c_bitstream@@SA_NMMMMJ_N0@Z
bool c_bitstream::compare_quantized_reals(float first, float second, float minimum, float maximum, long size_in_bits, bool exact_midpoint, bool wrap)
{
  bool result = false;
  long steps = (1 << size_in_bits) - 1;
  if (exact_midpoint)
    steps -= steps % 2;
  float range = maximum - minimum;
  float delta = fabsf(second - first);
  float step = range / (float)steps;
  if (delta < step)
    result = true;
  else if (wrap)
    result = fabsf(delta - range) < step;
  return result;
}


// @address 0x220e68 ?compare_vectors@c_bitstream@@SA_NPBTreal_vector3d@@0MMJ@Z
bool c_bitstream::compare_vectors(
    const real_vector3d *first, const real_vector3d *second, float min_magnitude, float max_magnitude, long magnitude_size_in_bits)
{
    bool result = false;
    real_vector3d direction1 = *first;
    real_vector3d direction2 = *second;
    float magnitude1 = normalize3d(&direction1);
    float magnitude2 = normalize3d(&direction2);
    bool zero1 = magnitude1 < min_magnitude;
    bool zero2 = magnitude2 < min_magnitude;
    bool minimum1 = !zero1 && c_bitstream::compare_logarithmic_quantized_reals(magnitude1, min_magnitude, min_magnitude, max_magnitude, magnitude_size_in_bits);
    bool minimum2 = !zero2 && c_bitstream::compare_logarithmic_quantized_reals(magnitude2, min_magnitude, min_magnitude, max_magnitude, magnitude_size_in_bits);
    if (zero1 && zero2)
        result = true;
    else if ((!zero1 || minimum2) && (!zero2 || minimum1))
    {
        result = compare_unit_vectors(&direction1, &direction2);
        result = result && c_bitstream::compare_logarithmic_quantized_reals(magnitude1, magnitude2, min_magnitude, max_magnitude, magnitude_size_in_bits);
    }
    return result;
}

// @address 0x221030 ?compare_axes@c_bitstream@@SA_NPBTreal_vector3d@@000@Z
bool c_bitstream::compare_axes(const real_vector3d *forward1, const real_vector3d *up1, const real_vector3d *forward2, const real_vector3d *up2)
{
    bool result = false;
    if (compare_unit_vectors(up1, up2))
    {
        float angle1 = axes_to_angle_internal(forward1, up1);
        float angle2 = axes_to_angle_internal(forward2, up1);
        result = compare_quantized_reals(angle1, angle2, -3.1415927f, 3.1415927f, 8, true, true);
    }
    return result;
}

// @address 0x2210e8 ?reset@c_bitstream@@QAAXJ@Z
void c_bitstream::reset(long state)
{
  m_state = state;
  m_current.current_stream_bit_position = 0;
  m_current.current_memory_bit_position = 0;
  m_position_stack_depth = 0;
  m_error_occurred = false;
  m_encoded_value_bits = 0;
  m_current.window = 0;
  m_current.window_bits_used = 0;
  m_current.next_data = m_data;
  if (state == 1) {
    m_number_of_bits_rewound = 0;
    m_number_of_position_resets = 0;
    return;
  }
  if (bitstream_state_is_reading(state)) {
    unsigned __int64 value = 0;
    long size;
    if (m_current.next_data + 8 <= m_data_max) {
      value = bitstream_load_word(m_current.next_data);
      size = 64;
      m_current.next_data += 8;
    } else if (m_current.next_data < m_data_max) {
      size = 0;
      while (m_current.next_data < m_data_max) {
        value = (value << 8) | *m_current.next_data++;
        size += 8;
      }
      value <<= (unsigned long)(64 - size);
    }
    m_current.current_stream_bit_position = size;
    m_current.window = value;
    if ((long)(value >> 32) == 'debg') {
      read_value_internal(32);
      m_error_occurred = true;
    }
  }
}

// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
void c_bitstream::write_bits_internal(const unsigned char *data, long size_in_bits)
{
    if ((unsigned long)size_in_bits >= 64)
    {
        unsigned long word_count = (unsigned long)size_in_bits / 64;
        size_in_bits -= word_count * 64;
        do
        {
            unsigned __int64 value = bitstream_load_word(data);
            data += sizeof(unsigned __int64);
            write_value_internal(value, 64);
        } while (--word_count != 0);
    }
    if (size_in_bits != 0)
    {
        unsigned __int64 value = 0;
        long bits_remaining = size_in_bits;
        if (size_in_bits >= 8)
        {
            unsigned long byte_count = (unsigned long)size_in_bits / 8;
            bits_remaining = size_in_bits - byte_count * 8;
            do
            {
                value = value << 8 | *data;
                data++;
            } while (--byte_count != 0);
        }
        if (bits_remaining != 0)
        {
            long unused_bits = 8 - bits_remaining;
            value = (*data >> unused_bits) | (value << bits_remaining);
        }
        write_value_internal(value, size_in_bits);
    }
}


// @address 0x2212e0 ?read_bits_internal@c_bitstream@@AAAXPAEJ@Z
void c_bitstream::read_bits_internal(unsigned char *data, long size_in_bits)
{
    if ((unsigned long)size_in_bits >= 64)
    {
        unsigned long word_count = (unsigned long)size_in_bits / 64;
        size_in_bits -= word_count * 64;
        do
        {
            bitstream_store_word(data, bitstream_read_window_bits(this, 64));
            data += sizeof(unsigned __int64);
        } while (--word_count != 0);
    }
    if (size_in_bits != 0)
    {
        unsigned __int64 value = bitstream_read_window_bits(this, size_in_bits) << (unsigned long)(64 - size_in_bits);
        if (size_in_bits >= 8)
        {
            unsigned long byte_count = (unsigned long)size_in_bits / 8;
            size_in_bits -= byte_count * 8;
            do
            {
                *data++ = (unsigned char)(value >> 56);
                value <<= 8;
            } while (--byte_count != 0);
        }
        if (size_in_bits != 0)
            *data = (unsigned char)(value >> 56);
    }
}

void c_bitstream::write_bit_internal(bool value)
{
  if (m_current.window_bits_used < sizeof(m_current.window) * 8) {
    m_current.current_memory_bit_position++;
    m_current.window = (m_current.window << 1) | (unsigned __int64)(value & 1);
    m_current.window_bits_used++;
  } else {
    m_current.current_memory_bit_position++;
    long bits_available = 64 - m_current.window_bits_used;
    unsigned __int64 wide_value = value & 1;
    unsigned __int64 old_window = m_current.window;
    m_current.window = wide_value;
    unsigned __int64 output = (old_window << bits_available) | (wide_value >> (1 - bits_available));
    m_current.window_bits_used = 1 - bits_available;
    if (m_current.next_data + 8 <= m_data_max) {
      bitstream_store_word(m_current.next_data, output);
      m_current.next_data += 8;
    } else {
      while (m_current.next_data < m_data_max) {
        *m_current.next_data = (unsigned char)(output >> 56);
        output <<= 8;
        m_current.next_data++;
      }
    }
    m_current.current_stream_bit_position += 64;
  }
}

void c_bitstream::write_value_internal(unsigned long value, long size_in_bits)
{
    long bits_used = m_current.window_bits_used;
    if (size_in_bits <= 64 - bits_used)
    {
        m_current.current_memory_bit_position += size_in_bits;
        m_current.window = (m_current.window << size_in_bits) | (unsigned __int64)value;
        m_current.window_bits_used = bits_used + size_in_bits;
    }
    else
    {
        m_current.current_memory_bit_position += size_in_bits;
        long bits_available = 64 - bits_used;
        unsigned __int64 wide_value = value;
        unsigned __int64 output = (m_current.window << bits_available) | (wide_value >> (size_in_bits - bits_available));
        m_current.window_bits_used = size_in_bits - bits_available;
        m_current.window = wide_value;
        if (m_current.next_data + 8 <= m_data_max)
        {
            bitstream_store_word(m_current.next_data, output);
            m_current.next_data += 8;
        }
        else
        {
            while (m_current.next_data < m_data_max)
            {
                *m_current.next_data = (unsigned char)(output >> 56);
                output <<= 8;
                m_current.next_data++;
            }
        }
        m_current.current_stream_bit_position += 64;
    }
}

// @address 0x221560 ?write_value_internal@c_bitstream@@QAAX_KJ@Z
void c_bitstream::write_value_internal(unsigned __int64 value, long size_in_bits)
{
    long bits_used = m_current.window_bits_used;
    if (size_in_bits <= 64 - bits_used)
    {
        m_current.window_bits_used = bits_used + size_in_bits;
        m_current.current_memory_bit_position += size_in_bits;
        m_current.window = bitstream_shift_left(m_current.window, size_in_bits) | value;
    }
    else
    {
        long bits_available = 64 - bits_used;
        unsigned __int64 old_window = m_current.window;
        m_current.window = value;
        m_current.current_memory_bit_position += size_in_bits;
        long bits_remaining = size_in_bits - bits_available;
        m_current.window_bits_used = bits_remaining;
        unsigned __int64 output = (old_window << bits_available) | bitstream_shift_right(value, bits_remaining);
        if (m_current.next_data + 8 <= m_data_max)
        {
            bitstream_store_word(m_current.next_data, output);
            m_current.next_data += 8;
        }
        else
        {
            while (m_current.next_data < m_data_max)
            {
                *m_current.next_data = (unsigned char)(output >> 56);
                output <<= 8;
                m_current.next_data++;
            }
        }
        m_current.current_stream_bit_position += 64;
    }
}

unsigned long c_bitstream::read_value_internal(long size_in_bits)
{
    unsigned long result;
    long bits_used = m_current.window_bits_used;
    if (size_in_bits <= 64 - bits_used) {
        result = (unsigned long)bitstream_shift_right(m_current.window, (unsigned long)(64 - size_in_bits));
        m_current.current_memory_bit_position += size_in_bits;
        m_current.window_bits_used = bits_used + size_in_bits;
        m_current.window = bitstream_shift_left(m_current.window, size_in_bits);
    } else {
        unsigned __int64 value = 0;
        long size;
        unsigned __int64 old_window = m_current.window;
        if (m_current.next_data + 8 <= m_data_max) {
            value = bitstream_load_word(m_current.next_data);
            size = 64;
            m_current.next_data += 8;
        } else if (m_current.next_data < m_data_max) {
            size = 0;
            while (m_current.next_data < m_data_max) {
                value = (value << 8) | *m_current.next_data++;
                size += 8;
            }
            value <<= (unsigned long)(64 - size);
        }
        long remaining = bits_used + size_in_bits - 64;
        m_current.current_stream_bit_position += size;
        m_current.current_memory_bit_position += size_in_bits;
        m_current.window_bits_used = remaining;
        result = (unsigned long)(value >> (unsigned long)(64 - remaining)) | (unsigned long)(old_window >> (unsigned long)(64 - size_in_bits));
        m_current.window = value << remaining;
    }
    return result;
}

// @address 0x220da0 ?compare_logarithmic_quantized_reals@c_bitstream@@SA_NMMMMJ@Z
bool c_bitstream::compare_logarithmic_quantized_reals(float first, float second, float minimum, float maximum, long size_in_bits)
{
    bool result = false;
    if (first >= 0.0f && second >= 0.0f) {
        float c = (float)log((double)maximum);
        float b = (float)log((double)minimum);
        float a = (float)log((double)first);
        result = c_bitstream::compare_quantized_reals(a, (float)log((double)second), b, c, size_in_bits, false, false);
    }
    return result;
}

// @address 0x221888 bitstream_read_window_bits
extern "C" unsigned __int64 bitstream_read_window_bits(c_bitstream *stream, long size_in_bits)
{
    unsigned __int64 result;
    long bits_used = stream->m_current.window_bits_used;
    if (size_in_bits <= 64 - bits_used) {
        result = bitstream_shift_right(stream->m_current.window, (unsigned long)(64 - size_in_bits));
        stream->m_current.current_memory_bit_position += size_in_bits;
        stream->m_current.window_bits_used = bits_used + size_in_bits;
        stream->m_current.window = bitstream_shift_left(stream->m_current.window, size_in_bits);
    } else {
        unsigned __int64 value = 0;
        long size;
        unsigned __int64 old_window = stream->m_current.window;
        if (stream->m_current.next_data + 8 <= stream->m_data_max) {
            value = bitstream_load_word(stream->m_current.next_data);
            size = 64;
            stream->m_current.next_data += 8;
        } else if (stream->m_current.next_data < stream->m_data_max) {
            size = 0;
            while (stream->m_current.next_data < stream->m_data_max) {
                value = (value << 8) | *stream->m_current.next_data++;
                size += 8;
            }
            value <<= (unsigned long)(64 - size);
        }
        long remaining = bits_used + size_in_bits - 64;
        stream->m_current.current_stream_bit_position += size;
        stream->m_current.current_memory_bit_position += size_in_bits;
        stream->m_current.window_bits_used = remaining;
        result = (value >> (unsigned long)(64 - remaining)) | (old_window >> (unsigned long)(64 - size_in_bits));
        stream->m_current.window = bitstream_shift_left(value, remaining);
    }
    return result;
}

// @address 0x221998 bitstream_has_data
extern "C" bool bitstream_has_data(c_bitstream *stream)
{
    return stream->m_data_size_bytes != -1;
}

// 0x2afb18: the encoding bitstream_decode_unit_vector decodes: the face of the cube the vector points through, then its two other
// components on that face, 8 bits each
// @address 0x2afb18 bitstream_encode_unit_vector
long bitstream_encode_unit_vector(const real_vector3d *vector)
{
    float i = vector->i;
    float j = vector->j;
    float abs_i = fabsf(i);
    float abs_j = fabsf(j);
    float k = vector->k;
    float abs_k = fabsf(k);
    long face;
    float u;
    float v;
    if (abs_i > abs_j)
    {
        if (abs_i > abs_k)
        {
            face = i > 0.0f ? 0 : 3;
            float scale = k_real_one / abs_i;
            v = scale * j;
            u = scale * k;
        }
        else
        {
            face = k > 0.0f ? 2 : 5;
            float scale = k_real_one / abs_k;
            v = scale * i;
            u = scale * j;
        }
    }
    else
    {
        if (abs_j > abs_k)
        {
            face = j > 0.0f ? 1 : 4;
            float scale = k_real_one / abs_j;
            v = scale * i;
            u = scale * k;
        }
        else
        {
            face = k > 0.0f ? 2 : 5;
            float scale = k_real_one / abs_k;
            v = scale * i;
            u = scale * j;
        }
    }
    long quantized_u = quantize_real(u, -1.0f, k_real_one, 8, true, false);
    long quantized_v = quantize_real(v, -1.0f, k_real_one, 8, true, false);
    return ((quantized_u << 8 | quantized_v) << 3) | face;
}

static __forceinline void bitstream_copy_vector3d(real_vector3d *dst, const real_vector3d *src) { *dst = *src; }
// @address 0x2afc18 ?bitstream_decode_unit_vector@@YAXJPATreal_vector3d@@@Z
void bitstream_decode_unit_vector(long quantization, real_vector3d *vector)
{
  float a = dequantize_real((quantization >> 3) & 255, -1.0f, 1.0f, 8, true);
  float b = dequantize_real((quantization >> 11) & 255, -1.0f, 1.0f, 8, true);
  switch (quantization & 7) {
  case 0: vector->j = a; vector->k = b; vector->i = 1.0f; break;
  case 3: vector->j = a; vector->k = b; vector->i = -1.0f; break;
  case 1: vector->i = a; vector->k = b; vector->j = 1.0f; break;
  case 4: vector->i = a; vector->k = b; vector->j = -1.0f; break;
  case 2: vector->i = a; vector->j = b; vector->k = 1.0f; break;
  case 5: vector->i = a; vector->j = b; vector->k = -1.0f; break;
  default: bitstream_copy_vector3d(vector, global_up3d); break;
  }
  normalize3d(vector);
}

// @address 0x2afd40 ?compare_unit_vectors@c_bitstream@@SA_NPBTreal_vector3d@@0@Z
bool c_bitstream::compare_unit_vectors(const real_vector3d *first, const real_vector3d *second)
{
    float first_i = fabsf(first->i);
    float first_j = fabsf(first->j);
    float first_k = fabsf(first->k);
    float second_i = fabsf(second->i);
    float second_j = fabsf(second->j);
    float second_k = fabsf(second->k);
    float first_max = first_i > first_j ? first_i : first_j;
    float second_max = second_i > second_j ? second_i : second_j;
    first_max = first_max > first_k ? first_max : first_k;
    second_max = second_max > second_k ? second_max : second_k;
    bool result = compare_quantized_reals(first->i / first_max, second->i / second_max, -1.0f, 1.0f, 8, true, false);
    result = result && compare_quantized_reals(first->j / first_max, second->j / second_max, -1.0f, 1.0f, 8, true, false);
    result = result && compare_quantized_reals(first->k / first_max, second->k / second_max, -1.0f, 1.0f, 8, true, false);
    return result;
}
