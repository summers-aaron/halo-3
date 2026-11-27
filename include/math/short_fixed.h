#ifndef MATH_SHORT_FIXED_H
#define MATH_SHORT_FIXED_H

// A signed 16-bit fixed-point value with k_bits fraction bits: mixing-board weights and slider positions use
// t_short_fixed<13>, facial animation and slider times t_short_fixed<8>. Every method is inline in the March 2007
// debug build (`local/odin-bin/marchbody class t_short_fixed`); no assert names its header, so the location is
// a guess (recorded in the lane report). import_real and export_real keep March's function-local constants.
// import_real is force-inlined while export_real stays a plain inline function.

template<long k_bits>
class t_short_fixed {
public:
    t_short_fixed() {}
    t_short_fixed(long value) : m_value((short)value) {}
    t_short_fixed(const t_short_fixed<k_bits> &other) : m_value(other.m_value) {}
    ~t_short_fixed() {}

    t_short_fixed<k_bits> &operator=(const t_short_fixed<k_bits> &other) { m_value = other.m_value; return *this; }
    t_short_fixed<k_bits> &operator=(int value) { m_value = (short)value; return *this; }
    t_short_fixed<k_bits> &operator-=(const t_short_fixed<k_bits> &other) { m_value = m_value - other.m_value; return *this; }
    t_short_fixed<k_bits> &operator*=(const t_short_fixed<k_bits> &other)
    {
        long product = m_value * other.m_value;
        m_value = (short)(product >> k_bits);
        return *this;
    }

    bool operator==(int value) const { return m_value == value; }
    bool operator!=(int value) const { return m_value != value; }
    bool operator<(int value) const { return m_value < value; }
    bool operator>(int value) const { return m_value > value; }
    bool operator<(const t_short_fixed<k_bits> &other) const { return m_value < other.m_value; }
    bool operator>(const t_short_fixed<k_bits> &other) const { return m_value > other.m_value; }

    __forceinline void import_real(float value)
    {
        static const float s_real_one = (float)k_fixed_one;
        m_value = (short)(value * s_real_one);
    }
    float export_real() const
    {
        static const float s_inv_real_one = 1.0f / (float)k_fixed_one;
        return (float)m_value * s_inv_real_one;
    }

    static const short k_fixed_one = (short)(1 << k_bits);
    static const short k_fixed_half = (short)(1 << (k_bits - 1));

    short m_value;
};
typedef char short_fixed_size[(sizeof(t_short_fixed<13>) == 2) ? 1 : -1];

template<long k_bits> __forceinline t_short_fixed<k_bits> operator-(const t_short_fixed<k_bits> &a, const t_short_fixed<k_bits> &b)
{
    t_short_fixed<k_bits> result(a); result.m_value -= b.m_value; return result;
}

#endif // MATH_SHORT_FIXED_H
