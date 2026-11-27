#ifndef HS_HS_H
#define HS_HS_H

// Functions defined in tu/blamlibxbox/hs.cpp.

#include <stddef.h>
#include "tag_files/tag_types.h"

struct s_hs_script_parameter
{
    char name[32];
    short type;
    short : 16;
};

struct s_hs_script_definition
{
    char name[32];
    unsigned short type;
    short return_type;
    long root_expression;
    s_tag_block parameters;
};

typedef char hs_script_parameter_size[(sizeof(s_hs_script_parameter) == 36) ? 1 : -1];
typedef char hs_script_definition_size[(sizeof(s_hs_script_definition) == 52) ? 1 : -1];
typedef char hs_script_root_offset[(offsetof(s_hs_script_definition, root_expression) == 36) ? 1 : -1];
typedef char hs_script_parameters_offset[(offsetof(s_hs_script_definition, parameters) == 40) ? 1 : -1];

struct s_hs_global_definition
{
    char name[32];
    unsigned short type;
    short : 16;
    long root_expression;
};

// External globals are supplied by hs_globals_external.cpp.
struct hs_global_external
{
    int type;
    void *value;
};

typedef char hs_global_definition_size[(sizeof(s_hs_global_definition) == 40) ? 1 : -1];
typedef char hs_global_definition_type_offset[(offsetof(s_hs_global_definition, type) == 32) ? 1 : -1];
typedef char hs_global_definition_root_offset[(offsetof(s_hs_global_definition, root_expression) == 36) ? 1 : -1];

struct hs_syntax_node
{
    unsigned short identifier;
    short constant_type;
    unsigned short type;
    unsigned short flags;
    long next_node_index;
    long source_offset;
    union {
        long value;
        short short_value;
        TAG_POINTER(const char) string_value;
    };
    unsigned short line_number;
};

typedef char hs_syntax_node_size[(sizeof(void *) != 4 || sizeof(hs_syntax_node) == 24) ? 1 : -1];
typedef char hs_syntax_node_type_offset[(offsetof(hs_syntax_node, type) == 4) ? 1 : -1];
typedef char hs_syntax_node_flags_offset[(offsetof(hs_syntax_node, flags) == 6) ? 1 : -1];
typedef char hs_syntax_node_source_offset[(offsetof(hs_syntax_node, source_offset) == 12) ? 1 : -1];
typedef char hs_syntax_node_value_offset[(offsetof(hs_syntax_node, value) == 16) ? 1 : -1];

struct s_data_array;
extern s_data_array *g_hs_syntax_data;

short hs_global_get_type(short global_index);
void hs_node_gc(void);
void hs_initialize(void);
void hs_initialize_for_new_map();
void hs_initialize_for_new_map(bool, bool);
void hs_dispose_from_old_map();
void hs_dispose();

// The external globals table (hs_globals_external.cpp), indexed by the low 15 bits of a global index whose top bit
// is set.
extern hs_global_external *hs_external_globals[];

// Script time runs at a fixed rate, independent of the game tick rate (March hs.cpp).
enum { k_hs_ticks_per_second = 30 };

static __forceinline float hs_ticks_to_seconds(long ticks)
{
    return (float)ticks / (float)k_hs_ticks_per_second;
}

static __forceinline long hs_seconds_to_ticks(float seconds)
{
    return (long)(seconds * (float)k_hs_ticks_per_second + 0.5f);
}

#endif // HS_HS_H
