#ifndef CSERIES_ALGORITHM_INTERNAL_H
#define CSERIES_ALGORITHM_INTERNAL_H

// Primary template for the algorithm specializations.
namespace std
{
template<typename Iterator> void _Median(Iterator first, Iterator middle, Iterator last);
template <typename Iterator, typename Difference, typename Value> void _Adjust_heap(Iterator first, Difference hole, Difference bottom, Value value);
template <typename Iterator, typename Difference> void _Sort(Iterator first, Iterator last, Difference ideal);
}

#endif // CSERIES_ALGORITHM_INTERNAL_H
