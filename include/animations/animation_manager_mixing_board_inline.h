#ifndef ANIMATIONS_ANIMATION_MANAGER_MIXING_BOARD_INLINE_H
#define ANIMATIONS_ANIMATION_MANAGER_MIXING_BOARD_INLINE_H

#include "animations/animation_manager.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/mixing_board_slider_types.h"

__forceinline c_mixing_board_slider *c_animation_manager::get_mixing_board_slider(long index) const
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
    return static_cast<c_mixing_board_slider *>(static_cast<void *>(
        get_mixing_board_buffer() + definition->component_types[0].offset + index * g_mixing_board_component_sizes[0]));
}

__forceinline c_animation_channel *c_animation_manager::get_mixing_board_animation_channel(long index) const
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
    long offset = definition->component_types[1].offset;
    long channel_offset = offset + index * g_mixing_board_component_sizes[1];
    return static_cast<c_animation_channel *>(static_cast<void *>(get_mixing_board_buffer() + channel_offset));
}

__forceinline c_auxilary_channel *c_animation_manager::get_mixing_board_auxilary_channel(long index) const
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
    long channel_offset = index * g_mixing_board_component_sizes[2];
    unsigned char *address = get_mixing_board_buffer();
    address += definition->component_types[2].offset;
    address += channel_offset;
    return static_cast<c_auxilary_channel *>(static_cast<void *>(address));
}

__forceinline c_aiming_channel *c_animation_manager::get_mixing_board_aiming_channel(long index) const
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
    return static_cast<c_aiming_channel *>(static_cast<void *>(
        get_mixing_board_buffer() + definition->component_types[3].offset + index * g_mixing_board_component_sizes[3]));
}

__forceinline c_facial_animation_channel *c_animation_manager::get_mixing_board_facial_animation_channel() const
{
    c_facial_animation_channel *channel = 0;
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
    if (definition && definition->get_component_type_count(4) > 0)
        channel = static_cast<c_facial_animation_channel *>(static_cast<void *>(get_mixing_board_buffer() +
            definition->get_component_type_offset(4)));
    return channel;
}

#include "animations/mixing_board/mixing_board.h"

__forceinline void c_animation_manager::reset_mixing_board()
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
    if (definition != 0)
#if defined(__clang__)
        ::reset_mixing_board(this, get_mixing_board_buffer(), definition);
#else
        ::reset_mixing_board(this, get_mixing_board_address(), definition);
#endif
}

#endif
