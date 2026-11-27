#ifndef RENDER_RENDER_OBJECTS_STATIC_LIGHTING_SETUP_TYPES_H
#define RENDER_RENDER_OBJECTS_STATIC_LIGHTING_SETUP_TYPES_H

#include <stddef.h>

struct s_shader_extern_info;

struct s_render_objects_static_lighting_setup {
    long region_count;
    long definition_index;
    long skinning_memory_designator;
    unsigned char skinning_matrix_count;
    char region_permutations[16];
    char : 8;
    char region_zsort[32];
    unsigned short level_of_detail;
    short scenery_instance_index;
    short : 16;
    short : 16;
    short : 16;
    s_shader_extern_info *shader_extern_info;
    long object_index;
#ifdef HALO3_64BIT
    unsigned long *render_data;
#else
    long render_data;
#endif
    unsigned char flags;
};

typedef char render_setup_size[(sizeof(void *) != 4 || ((sizeof(s_render_objects_static_lighting_setup) == 0x58))) ? 1 : -1];
typedef char render_setup_object_index_offset[(sizeof(void *) != 4 || ((offsetof(s_render_objects_static_lighting_setup, object_index) == 0x4c))) ? 1 : -1];
typedef char render_setup_flags_offset[(sizeof(void *) != 4 || ((offsetof(s_render_objects_static_lighting_setup, flags) == 0x54))) ? 1 : -1];

#endif // RENDER_RENDER_OBJECTS_STATIC_LIGHTING_SETUP_TYPES_H
