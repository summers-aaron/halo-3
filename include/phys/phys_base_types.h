#ifndef INCLUDE_PHYS_BASE_TYPES_H
#define INCLUDE_PHYS_BASE_TYPES_H

// Havok base value wrappers.
// Method sets and bodies follow the March 2007 debug build (marchbody class hkBool / hkEnum / hkPadSpu).

// A bool held in one byte.
class phys_bool {
public:
    __forceinline phys_bool() {}
    __forceinline phys_bool(bool b) : m_bool_value(static_cast<char>(b)) {}
    __forceinline operator bool() const { return m_bool_value != 0; }
    __forceinline phys_bool &operator=(bool b) {
        m_bool_value = static_cast<char>(b);
        return *this;
    }
    // March compares the stored byte (sign-extended) with b, both as int.
    __forceinline phys_bool operator==(bool b) const { return phys_bool(m_bool_value == b); }
    __forceinline phys_bool operator!=(bool b) const { return phys_bool(m_bool_value != b); }

private:
    char m_bool_value;
};

// An enum value stored in a smaller integer.
template <typename ENUM_VALUE_TYPE, typename STORAGE_TYPE>
class phys_enum {
public:
    __forceinline phys_enum() {}
    __forceinline phys_enum(ENUM_VALUE_TYPE e) : m_storage(static_cast<STORAGE_TYPE>(e)) {}
    __forceinline operator ENUM_VALUE_TYPE() const { return static_cast<ENUM_VALUE_TYPE>(m_storage); }
    __forceinline void operator=(ENUM_VALUE_TYPE e) { m_storage = static_cast<STORAGE_TYPE>(e); }
    // March narrows e to the storage type before comparing.
    __forceinline phys_bool operator==(ENUM_VALUE_TYPE e) const { return phys_bool(m_storage == static_cast<STORAGE_TYPE>(e)); }
    __forceinline phys_bool operator!=(ENUM_VALUE_TYPE e) const { return phys_bool(m_storage != static_cast<STORAGE_TYPE>(e)); }

private:
    STORAGE_TYPE m_storage;
};

// A set of flag bits stored in an integer. No March rows; the methods are the ones tree units call.
template <typename FLAG_BITS_TYPE, typename STORAGE_TYPE>
class phys_flags {
public:
    __forceinline phys_flags() {}
    __forceinline phys_flags(STORAGE_TYPE s) : m_storage(s) {}
    __forceinline void clear() { m_storage = 0; }
    __forceinline void flags_set_all(STORAGE_TYPE s) { m_storage = s; }
    __forceinline void flags_or_with(STORAGE_TYPE s) { m_storage |= s; }
    __forceinline void flags_and_with(STORAGE_TYPE s) { m_storage &= s; }
    __forceinline STORAGE_TYPE get() const { return m_storage; }
    __forceinline STORAGE_TYPE get(STORAGE_TYPE mask) const { return m_storage & mask; }
    __forceinline bool any_is_set(STORAGE_TYPE mask) const { return (m_storage & mask) != 0; }
    __forceinline bool all_are_set(STORAGE_TYPE mask) const { return (m_storage & mask) == mask; }

private:
    STORAGE_TYPE m_storage;
};

// A value of type T (on this platform no padding is added).
template <typename T>
class phys_padded {
public:
    __forceinline phys_padded() {}
    __forceinline phys_padded(T v) : m_storage(v) {}
    __forceinline void operator=(T v) { m_storage = v; }
    __forceinline T val() const { return m_storage; }
    __forceinline T operator->() const { return m_storage; }
    // March's conversion calls val().
    __forceinline operator T() const { return val(); }

private:
    template <typename U> friend class phys_fixed_array;
    T m_storage;
};

typedef char havok_size_hkBool[(sizeof(phys_bool) == 1) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_BASE_TYPES_H
