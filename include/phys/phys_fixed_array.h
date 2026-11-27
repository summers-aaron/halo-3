#ifndef INCLUDE_PHYS_FIXED_ARRAY_H
#define INCLUDE_PHYS_FIXED_ARRAY_H

template <typename T>
class phys_fixed_array {
public:
    T *m_data;
    int m_size;

    __forceinline T &operator[](int index) { return m_data[index]; }
    __forceinline T *begin() { return m_data; }
    __forceinline const T &operator[](int index) const { return m_data[index]; }
    __forceinline int getSize() const { return m_size; }
    __forceinline const T *begin() const { return m_data; }

protected:
    // March 2007: protected and empty (??0?$hkFixedArray@...@@IAA@XZ); hkLocalBuffer sets the pointer.
    __forceinline phys_fixed_array() {}
};

#endif
