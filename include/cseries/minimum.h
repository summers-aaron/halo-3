#ifndef CSERIES_MINIMUM_H
#define CSERIES_MINIMUM_H

template <typename t_type>
__forceinline const t_type &minimum(const t_type &first, const t_type &second)
{
    return first < second ? first : second;
}

#endif // CSERIES_MINIMUM_H
