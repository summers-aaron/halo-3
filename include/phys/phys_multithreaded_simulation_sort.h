#ifndef INCLUDE_PHYS_MULTITHREADED_SIMULATION_SORT_H
#define INCLUDE_PHYS_MULTITHREADED_SIMULATION_SORT_H

#include "phys/phys_algorithm_sort.h"
#include "phys/phys_typed_broadphase_handle_pair.h"

class phys_island;

template void phys_algorithm::quick_sort_recursive<phys_typed_broadphase_pair,
    bool (*)(const phys_typed_broadphase_pair &, const phys_typed_broadphase_pair &)>(
    phys_typed_broadphase_pair *, int, int, bool (*)(const phys_typed_broadphase_pair &, const phys_typed_broadphase_pair &));

template <>
void phys_algorithm::quick_sort_recursive<phys_island *, phys_bool (*)(const phys_island *, const phys_island *)>(
    phys_island **, int, int, phys_bool (*)(const phys_island *, const phys_island *));

#endif
