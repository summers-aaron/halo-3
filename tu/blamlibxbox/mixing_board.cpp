// blamlibxbox/mixing_board.obj: from the records 0xedd58 0x2fd128 0x2fd4f8
// @flags /TP /O2 /Oc

#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "cseries/pointer_utilities.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "memory/datum_index.h"
#include "animations/animation_id.h"
#include "animations/animation_definitions.h"
#include "animations/animation_search_path_iterator.h"
#include "animations/mixing_board/mixing_board.h"
#include "animations/mixing_board/mixing_board_slider_types.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "animations/mixing_board/channels/auxilary_channel.h"
#include "animations/mixing_board/channels/aiming_channel.h"
#include "animations/mixing_board/channels/facial_animation_channel_types.h"

// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0xeddc0 ?get_blend_screen_bounds@c_model_animation_graph@@QBAPBUanimation_aiming_screen_bounds@@Vc_animation_id@@@Z

// Shipped-image .rdata words at 0x2aaf4: the nine facial-emotion string ids 0x3d2..0x3da.
// Hosted here: named .rdata lands only from single-@rdata-block TUs and
// animation_manager.cpp already owns one (0x331f4).
// @rdata 0x8202aaf4..0x8202ab18
// @address 0x2aaf4 k_facial_emotion_string_ids
const long k_facial_emotion_string_ids[9] = { 0x3d2, 0x3d3, 0x3d4, 0x3d5, 0x3d6, 0x3d7, 0x3d8, 0x3d9, 0x3da };
// @rdata-end

// @address 0xedd58 mixing_board_get_blend_screen_bounds
extern "C" const animation_aiming_screen_bounds *mixing_board_get_blend_screen_bounds(const c_channel_base *channel)
{
    const animation_aiming_screen_bounds *result = 0;
    bool valid = channel->valid();
    if (valid)
    {
        const c_model_animation_graph *graph = (const c_model_animation_graph *)
            global_tag_instances[datum_index_to_absolute_index(channel->m_graph_index)].definition;
        result = graph->get_blend_screen_bounds(channel->m_animation_id);
    }
    return result;
}

// @address 0x2fd128 ?reset_mixing_board@@YAXPAVc_animation_manager@@JPBVc_mixing_board_definition@@@Z
#if defined(__clang__)
void reset_mixing_board(c_animation_manager *manager, unsigned char *starting_address, const c_mixing_board_definition *definition)
#else
void reset_mixing_board(c_animation_manager *manager, long starting_address, const c_mixing_board_definition *definition)
#endif
{
    long total_size = definition->get_total_size();
    if (total_size > 0)
    {
#if defined(__clang__)
        unsigned char *base = starting_address;
#else
        unsigned char *base = static_cast<unsigned char *>(pointer_from_address(starting_address));
#endif
        {
            c_mixing_board_slider *slider;
            c_mixing_board_component_iterator<c_mixing_board_slider> iterator;
            iterator.setup(base, definition, 0);
            while (iterator.next(&slider))
                slider->initialize();
        }
        {
            c_animation_channel *channel;
            c_mixing_board_component_iterator<c_animation_channel> iterator;
            iterator.setup(base, definition, 1);
            while (iterator.next(&channel))
                channel->initialize();
        }
        {
            c_auxilary_channel *channel;
            c_mixing_board_component_iterator<c_auxilary_channel> iterator;
            iterator.setup(base, definition, 2);
            while (iterator.next(&channel))
                channel->initialize();
        }
        {
            c_aiming_channel *channel;
            c_mixing_board_component_iterator<c_aiming_channel> iterator;
            iterator.setup(base, definition, 3);
            while (iterator.next(&channel))
                channel->initialize();
        }
        {
            c_facial_animation_channel *channel;
            c_mixing_board_component_iterator<c_facial_animation_channel> iterator;
            iterator.setup(base, definition, 4);
            while (iterator.next(&channel))
            {
                channel->initialize_base();
                channel->initialize();
            }
        }
        if (definition && definition->setup_channels)
            definition->setup_channels(manager);
    }
}

// @address 0x2fd4f8 ?initialize@c_facial_animation_channel@@QAAXXZ
void c_facial_animation_channel::initialize()
{
    clear();
    memset(m_emotions, 0, sizeof(m_emotions));
    m_sound_index = -1;
    m_lipsync_index = -1;
    m_frame = 0.0f;
    m_gesture_frame = 0;
    m_gesture_duration = 0;
    m_lipsync_frames_remaining = 0;
    m_state_flags.clear();
    m_settings_flags.clear();
    m_animation_ids[0].clear();
    m_animation_ids[1].clear();
    m_blend = 0.0f;
    m_scale = 1.0f;
}
