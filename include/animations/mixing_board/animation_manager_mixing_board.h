#ifndef ANIMATIONS_MIXING_BOARD_ANIMATION_MANAGER_MIXING_BOARD_H
#define ANIMATIONS_MIXING_BOARD_ANIMATION_MANAGER_MIXING_BOARD_H

#include "animations/animation_manager.h"
#include "animations/mixing_board/mixing_board_definition.h"

__forceinline c_mixing_board_slider *c_animation_manager::get_mixing_board_slider(long slider_index) const
{
    const c_mixing_board_definition *definition = 0;
    if (m_mixing_board_type <= 2)
        definition = &g_mixing_board_definition_list[m_mixing_board_type];
    long slider_offset = definition->get_component_type_offset(0);
    return static_cast<c_mixing_board_slider *>(static_cast<void *>(
        get_mixing_board_buffer() + slider_offset + c_mixing_board_definition::get_component_size(0) * slider_index));
}

__forceinline c_animation_channel *c_animation_manager::get_mixing_board_animation_channel(long channel_index) const
{
    const c_mixing_board_definition *definition = 0;
    if (m_mixing_board_type <= 2)
        definition = &g_mixing_board_definition_list[m_mixing_board_type];
    long channel_offset = definition->get_component_type_offset(1);
    return static_cast<c_animation_channel *>(static_cast<void *>(
        get_mixing_board_buffer() + channel_offset + c_mixing_board_definition::get_component_size(1) * channel_index));
}

#endif
