#ifndef SIMULATION_SIMULATION_QUEUE_TYPES_H
#define SIMULATION_SIMULATION_QUEUE_TYPES_H

#include <stddef.h>

// c_simulation_world::apply_simulation_queue dispatches these values.
enum e_simulation_queue_element_type
{
    _simulation_queue_element_type_none = 0,
    _simulation_queue_element_type_event = 1,
    _simulation_queue_element_type_entity_creation = 2,
    _simulation_queue_element_type_entity_update = 3,
    _simulation_queue_element_type_entity_deletion = 4,
    _simulation_queue_element_type_entity_promotion = 5,
    _simulation_queue_element_type_game_global_event = 6,
    _simulation_queue_element_type_game_simulation = 7,
    _simulation_queue_element_type_player_event = 8,
    _simulation_queue_element_type_player_update = 9,
    _simulation_queue_element_type_gamestates_delete = 10,
    _simulation_queue_element_type_editor_action = 11
};

// One allocation holds this header followed by its encoded data buffer.
struct s_simulation_queue_element
{
    long type;
    s_simulation_queue_element *next;
    long data_size;
    unsigned char *data;
};

typedef char simulation_queue_element_size[(sizeof(void *) != 4 || ((sizeof(s_simulation_queue_element) == 0x10) )) ? 1 : -1];
typedef char simulation_queue_element_data_size_offset[(sizeof(void *) != 4 || ((offsetof(s_simulation_queue_element, data_size) == 8) )) ? 1 : -1];
typedef char simulation_queue_element_data_offset[(sizeof(void *) != 4 || ((offsetof(s_simulation_queue_element, data) == 0xc) )) ? 1 : -1];

#endif // SIMULATION_SIMULATION_QUEUE_TYPES_H
