#ifndef INCLUDE_PHYS_DETERMINISM_UTIL_H
#define INCLUDE_PHYS_DETERMINISM_UTIL_H

class phys_determinism_util
{
public:
    template<typename TYPE> static __forceinline void check_multi_threaded(TYPE object)
    {
    }
    template<typename TYPE> static __forceinline void check_single_threaded(TYPE object)
    {
    }
};

#endif
