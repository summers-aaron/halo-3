#ifndef OBJECTS_CRATES_H
#define OBJECTS_CRATES_H

#include "objects/object_datum.h"

// Functions and types of tu/objectslibxbox/crates.cpp.

struct object_placement_data;
struct s_scenario_crate;

// A crate object: the base object, then the crate's own fields (c_object_iterator<crate_datum> and
// c_flags_no_init<e_crate_flag, unsigned long, 2> at 0x160 in the March 2007 build).
struct crate_datum
{
    object_datum object;
    unsigned long flags;
    long lifetime_ticks;                    // crate_new: -1, or the definition's lifetime in ticks
};

typedef char crate_datum_size[(sizeof(crate_datum) == 0x168) ? 1 : -1];

bool crate_new(long object_index, object_placement_data *placement_data, bool *out_of_memory);
void crate_place(long object_index, s_scenario_crate *scenario_crate);

#endif // OBJECTS_CRATES_H
