// blamlibxbox/hs_compile.obj: from the records 0x2b96a0
// @flags /TP /O2 /Oc
#include "memory/data_types.h"
// @address 0x2b96a0 ?hs_compile_postprocess_fast@@YAXXZ
#include <stdlib.h>
#include "hs/hs_types.h"
#include "hs/hs_compile.h"
#include "memory/data.h"
#include "scenario/scenario.h"
#include "hs/hs.h"
#include "scenario/scenario_tag.h"

static __forceinline bool hs_compile_test_node_flag(unsigned char value, long bit) { return (value & (1u << bit)) != 0; }

void hs_compile_postprocess_fast(void)
{
    const char *script_string_data = (const char *)global_scenario->script_string_data.address;
    s_data_array *syntax_data = g_hs_syntax_data;
    long node_index = data_next_index(g_hs_syntax_data, NONE);
    if (node_index != NONE) {
        long next_index;
        do {
            hs_syntax_node *node = datum_get<hs_syntax_node>(syntax_data, node_index);
            if (hs_compile_test_node_flag(node->flags, 0) && node->type == _hs_type_string &&
                !hs_compile_test_node_flag(datum_get<hs_syntax_node>(syntax_data, node_index)->flags, 2))
            {
                node->string_value = script_string_data + node->source_offset;
            }
            long absolute_index = data_next_absolute_index(syntax_data, (node_index + 1) & 0xffff);
            next_index = NONE;
            if (absolute_index != NONE) {
                const s_datum_header *header = (const s_datum_header *)(syntax_data->data + syntax_data->size * absolute_index);
                next_index = _rotl((unsigned short)header->identifier, 16) | absolute_index;
            }
            node_index = next_index;
        } while (next_index != NONE);
    }
}
