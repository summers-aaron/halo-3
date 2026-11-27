#ifndef INTERFACE_LIST_WIDGET_DEFINITION_H
#define INTERFACE_LIST_WIDGET_DEFINITION_H

#include "interface/core_widget_definition.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

struct s_list_item_widget_block
{
    s_core_widget_definition core;
    long item_label;
};

struct s_list_widget_definition
{
    s_core_widget_definition core;
    long datasource;
    s_tag_reference skin;
    long rows;
    s_tag_block items;
    s_tag_reference previous;
    s_tag_reference next;

    __forceinline s_list_widget_definition()
        : datasource(0), skin(), rows(0), items(), previous(), next()
    {
        skin.index = -1;
        items.count = 0;
        items.address = 0;
        previous.index = -1;
        next.index = -1;
    }
};

struct s_list_widget_block
{
    s_gui_tag_reference list_widget_template;
    s_list_widget_definition definition;

    s_list_widget_block();
};

typedef char list_item_widget_block_size[(sizeof(s_list_item_widget_block) == 0x30) ? 1 : -1];
typedef char list_item_widget_label_offset[(offsetof(s_list_item_widget_block, item_label) == 0x2c) ? 1 : -1];
typedef char list_widget_definition_size[(sizeof(s_list_widget_definition) == 0x70) ? 1 : -1];
typedef char list_widget_datasource_offset[(offsetof(s_list_widget_definition, datasource) == 0x2c) ? 1 : -1];
typedef char list_widget_skin_offset[(offsetof(s_list_widget_definition, skin) == 0x30) ? 1 : -1];
typedef char list_widget_rows_offset[(offsetof(s_list_widget_definition, rows) == 0x40) ? 1 : -1];
typedef char list_widget_items_offset[(offsetof(s_list_widget_definition, items) == 0x44) ? 1 : -1];
typedef char list_widget_previous_offset[(offsetof(s_list_widget_definition, previous) == 0x50) ? 1 : -1];
typedef char list_widget_next_offset[(offsetof(s_list_widget_definition, next) == 0x60) ? 1 : -1];
typedef char list_widget_block_size[(sizeof(s_list_widget_block) == 0x80) ? 1 : -1];
typedef char list_widget_block_definition_offset[(offsetof(s_list_widget_block, definition) == 0x10) ? 1 : -1];

#endif // INTERFACE_LIST_WIDGET_DEFINITION_H
