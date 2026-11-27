#ifndef INCLUDE_PHYS_CONSTRAINT_QUERY_OUT_H
#define INCLUDE_PHYS_CONSTRAINT_QUERY_OUT_H

#include "phys/phys_base_types.h"

class phys_schema;

class phys_constraint_query_out {
public:
    phys_padded<phys_schema *> m_jacobian_schemas;
};

typedef char phys_constraint_query_out_size[(sizeof(void *) != 4 || ((sizeof(phys_constraint_query_out) == 4) )) ? 1 : -1];

#endif
