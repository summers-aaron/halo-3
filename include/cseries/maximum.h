#ifndef CSERIES_MAXIMUM_H
#define CSERIES_MAXIMUM_H

template <typename t_type>
__forceinline const t_type &maximum(const t_type &first, const t_type &second)
{
    return first > second ? first : second;
}

#endif // CSERIES_MAXIMUM_H
