#ifndef CSERIES_LONG_SORT_H
#define CSERIES_LONG_SORT_H

// The released long specialization has no object parameter.
template <class T>
class templated_sort
{
public:
    typedef bool (*t_compare)(T, T, const void *);
    static void qsort(T *base, unsigned long count, bool (*compare)(T, T, const void *), const void *context);
};

template <> void templated_sort<long>::qsort(long *base, unsigned long count, bool (*compare)(long, long, const void *), const void *context);
template <> void templated_sort<short>::qsort(short *base, unsigned long count, bool (*compare)(short, short, const void *), const void *context);

#endif
