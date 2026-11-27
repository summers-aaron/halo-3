#ifndef CSERIES_CSERIES_WRAPPED_H
#define CSERIES_CSERIES_WRAPPED_H

// Type-safe bitflag and enum storage wrappers, read method by method from the March 2007 debug build
// (`local/odin-bin/marchbody class c_flags_no_init`, `class c_flags`, `class c_enum`).
// The asserts of c_flags_no_init name ..\..\source\memory\static_arrays.h, lines 1263-1404: that is the file
// the class belongs in (it moves there once static_arrays.h is free). c_flags and c_enum carry no assert of
// their own, so their file is not attested; they stay here beside c_flags_no_init.
// These wrappers keep no debug asserts.

template<typename t_enum, typename t_storage, long k_count>
class c_flags_no_init {
public:
    __forceinline bool valid_bit(t_enum bit) const {
        return bit >= 0 && bit < k_count;
    }

    // static_arrays.h:1263
    __forceinline bool test(t_enum bit) const {
        return (m_storage & (1 << bit)) != 0;
    }

    // static_arrays.h:1274
    __forceinline bool test_range(t_enum start_bit, t_enum end_bit) const {
        unsigned long mask = ((1 << (end_bit + 1 - start_bit)) - 1) << start_bit;
        return (m_storage & mask) != 0;
    }

    // static_arrays.h:1286
    __forceinline void set(t_enum bit, bool enable) {
        if (enable)
            m_storage |= (1 << bit);
        else
            m_storage &= ~(1 << bit);
    }

    // static_arrays.h:1304
    __forceinline void toggle(t_enum bit) {
        m_storage ^= (1 << bit);
    }

    __forceinline void clear() {
        m_storage = 0;
    }

    __forceinline void set_all() {
        m_storage = static_cast<t_storage>((static_cast<unsigned __int64>(1) << k_count) - 1);
    }

    __forceinline bool is_empty() const {
        unsigned long mask = static_cast<unsigned long>((static_cast<unsigned __int64>(1) << k_count) - 1);
        return (m_storage & mask) == 0;
    }

    // March evaluates this as one conditional (its full-width instantiations fold to `true`); written as
    // a single condition; the full-width case is a separate early return instead.
    __forceinline bool valid() const {
        if (k_count == sizeof(t_storage) * 8)
            return true;
        return (m_storage & ~static_cast<unsigned long>((static_cast<unsigned __int64>(1) << k_count) - 1)) == 0;
    }

    __forceinline t_storage get_unsafe() const {
        return m_storage;
    }

    // static_arrays.h:1385
    __forceinline void set_unsafe(t_storage raw) {
        m_storage = raw;
    }

    // static_arrays.h:1395
    __forceinline c_flags_no_init &operator|=(const c_flags_no_init &other) {
        m_storage |= other.m_storage;
        return *this;
    }

    // static_arrays.h:1404
    __forceinline c_flags_no_init &operator&=(const c_flags_no_init &other) {
        m_storage &= other.m_storage;
        return *this;
    }

    __forceinline bool operator==(const c_flags_no_init &other) const {
        return m_storage == other.m_storage;
    }

    __forceinline bool operator!=(const c_flags_no_init &other) const {
        return m_storage != other.m_storage;
    }

    __forceinline c_flags_no_init operator&(const c_flags_no_init &other) const {
        c_flags_no_init result;
        result.set_unsafe(m_storage & other.m_storage);
        return result;
    }

    __forceinline c_flags_no_init operator|(const c_flags_no_init &other) const {
        c_flags_no_init result;
        result.set_unsafe(m_storage | other.m_storage);
        return result;
    }

    __forceinline c_flags_no_init operator~() const {
        c_flags_no_init result;
        result.set_unsafe(~m_storage & static_cast<t_storage>((static_cast<unsigned __int64>(1) << k_count) - 1));
        return result;
    }

    // Not in March (invented here); kept until their last users are rewritten.
    __forceinline bool is_clear() const { return m_storage == 0; }
    __forceinline operator t_storage() const { return m_storage; }
    __forceinline t_storage get() const { return m_storage; }

    t_storage m_storage;
};

template<typename t_enum, typename t_storage, long k_count>
class c_flags : public c_flags_no_init<t_enum, t_storage, k_count> {
public:
    __forceinline c_flags() { this->clear(); }
    __forceinline c_flags(t_storage raw) { this->set_unsafe(raw); }
};

template<typename t_enum, typename t_storage, long k_minimum, long k_maximum_plus_one>
class c_enum_no_init {
public:
    __forceinline operator t_enum() const { return static_cast<t_enum>(m_storage); }
    __forceinline t_enum get() const { return static_cast<t_enum>(m_storage); }
    __forceinline c_enum_no_init &operator=(t_enum v) {
        m_storage = static_cast<t_storage>(v);
        return *this;
    }
    __forceinline bool operator==(t_enum v) const { return static_cast<t_enum>(m_storage) == v; }
    __forceinline bool operator!=(t_enum v) const { return static_cast<t_enum>(m_storage) != v; }

    union {
        t_storage m_storage;
        t_storage m_value;
        t_storage value;
    };
};

template<typename t_enum, typename t_storage, long k_minimum, long k_maximum_plus_one>
class c_enum {
public:
    __forceinline c_enum() {}
    __forceinline c_enum(t_enum v) { m_storage = static_cast<t_storage>(v); }
    __forceinline operator t_enum() const { return static_cast<t_enum>(m_storage); }
    __forceinline t_enum get() const { return static_cast<t_enum>(m_storage); }
    __forceinline c_enum &operator=(t_enum v) {
        m_storage = static_cast<t_storage>(v);
        return *this;
    }
    __forceinline bool operator==(t_enum v) const { return static_cast<t_enum>(m_storage) == v; }
    __forceinline bool operator!=(t_enum v) const { return static_cast<t_enum>(m_storage) != v; }
    __forceinline bool in_range() const { return m_storage >= k_minimum && m_storage < k_maximum_plus_one; }
    __forceinline void set_raw_value(t_storage raw) { m_storage = raw; }

    union {
        t_storage m_storage;
        t_storage m_value;
        t_storage value;
    };
};

typedef char cseries_wrapped_size_flags32[(sizeof(c_flags_no_init<long, unsigned long, 32>) == 4) ? 1 : -1];
typedef char cseries_wrapped_size_flags16[(sizeof(c_flags<long, unsigned short, 16>) == 2) ? 1 : -1];
typedef char cseries_wrapped_size_enum16[(sizeof(c_enum<long, short, 0, 4>) == 2) ? 1 : -1];

#endif // CSERIES_CSERIES_WRAPPED_H
