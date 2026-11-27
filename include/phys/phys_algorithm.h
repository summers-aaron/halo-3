#ifndef INCLUDE_PHYS_ALGORITHM_H
#define INCLUDE_PHYS_ALGORITHM_H


namespace phys_algorithm {
    template <class T, class less_cmp> void quick_sort_recursive(T *, int, int, less_cmp);
    template <typename T>
    __forceinline void swap(T& x, T& y) {
        T t(x);
        x = y;
        y = t;
    }
}

#endif // INCLUDE_HAVOK_HK_ALGORITHM_H
