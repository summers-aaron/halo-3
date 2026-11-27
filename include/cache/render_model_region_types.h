#ifndef CACHE_RENDER_MODEL_REGION_TYPES_H
#define CACHE_RENDER_MODEL_REGION_TYPES_H

#include "tag_files/tag_types.h"

struct s_render_model_permutation;

struct s_render_model_region {
    long name;
    long permutation_count;
    TAG_POINTER(s_render_model_permutation) permutations;
    TAG_POINTER(void) permutations_definition;
};

struct s_render_model_permutation
{
    long name;
    short start_index;
    unsigned short count;
    unsigned long instance_mask[2];
};

typedef char render_model_permutation_size[(sizeof(s_render_model_permutation) == 16) ? 1 : -1];
#endif // CACHE_RENDER_MODEL_REGION_TYPES_H
