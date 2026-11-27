#ifndef INCLUDE_PHYS_ALGORITHM_SORT_H
#define INCLUDE_PHYS_ALGORITHM_SORT_H

#include "phys/phys_base_types.h"

namespace phys_algorithm {
    template <typename T>
    class less {
    public:
        __forceinline phys_bool operator()(const T &a, const T &b) { return a < b; }
    };

    template <typename T, typename L>
    void quick_sort_recursive(T *sort_items, int d, int h, L less_than_fn)
    {
        for (;;) {
            int i = h;
            int j = d;
            __declspec(align(16)) T str;
            str = sort_items[(d + h) >> 1];
            do {
                while (less_than_fn(sort_items[j], str)) { j++; }
                while (less_than_fn(str, sort_items[i])) { i--; }
                if (i >= j) {
                    if (i != j) {
                        T sort_pivot;
                        sort_pivot = sort_items[i];
                        sort_items[i] = sort_items[j];
                        sort_items[j] = sort_pivot;
                    }
                    i--;
                    j++;
                }
            } while (j <= i);
            if (d < i) {
                quick_sort_recursive(sort_items, d, i, less_than_fn);
            }
            if (j < h) {
                d = j;
            } else {
                break;
            }
        }
    }

    template <>
    void quick_sort_recursive(unsigned int *sort_items, int d, int h, less<unsigned int> less_than_fn);

    template <typename T, typename L>
    __forceinline void quick_sort(T *sort_items, int elem_count, L less_than_fn)
    {
        if (elem_count > 1) {
            int d = 0;
            int h = elem_count - 1;
            quick_sort_recursive(sort_items, d, h, less_than_fn);
        }
    }

    template <typename T>
    __forceinline void quick_sort(T *sort_items, int elem_count)
    {
        quick_sort(sort_items, elem_count, less<T>());
    }
}

#endif
