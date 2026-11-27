// blamlibxbox/aiming_channel.obj: from the records 0xdbb78 0xdbca0 0x290098
// @flags /TP /O2 /Oc

#include "tag_files/tag_types.h"
// @address 0x2fd4f8 ?initialize@c_facial_animation_channel@@QAAXXZ
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "animations/animation_id.h"
#include "animations/animation_search_path_iterator.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/channels/channel_base.h"
#include "animations/mixing_board/channels/aiming_channel.h"
#include "animations/mixing_board/channels/facial_animation_channel_types.h"
#include "math/short_fixed.h"
#include "animations/animation_manager.h"
#include "tag_files/tag_groups.h"
#include "memory/datum_index.h"

const float k_aiming_channel_epsilon = 0.0001f;

// @address 0x79d44c ?k_aiming_interpolation_stop_delta@@3MA
float k_aiming_interpolation_stop_delta = 0.08726646f;
// @address 0x79d450 ?k_aiming_interpolation_start_delta@@3MA
float k_aiming_interpolation_start_delta = 1.3089969f;

// @address 0xdbb78 ?set_yaw@c_aiming_channel@@QAAXM@Z
void c_aiming_channel::set_yaw(float yaw)
{
    float pinned = (yaw > m_yaw_min ? yaw : m_yaw_min) < m_yaw_max
        ? (yaw > m_yaw_min ? yaw : m_yaw_min)
        : m_yaw_max;
    if (fabs(m_yaw_max - m_yaw_min - 6.2831855f) < k_aiming_channel_epsilon) {
        m_target_yaw = pinned;
        m_yaw = pinned;
    } else {
        bool interpolating;
        float candidate;
        float best_error = fabs(m_yaw - pinned);
        candidate = yaw - 6.2831855f;
        if (candidate >= m_yaw_min && candidate <= m_yaw_max) {
            float error = (m_yaw - candidate) * (m_yaw - candidate);
            if (error < best_error) {
                pinned = candidate;
                best_error = error;
            }
        }
        candidate = yaw + 6.2831855f;
        if (candidate >= m_yaw_min && candidate <= m_yaw_max) {
            float error = (m_yaw - candidate) * (m_yaw - candidate);
            if (error < best_error) {
                pinned = candidate;
                best_error = error;
            }
        }
        interpolating = fabs(m_target_yaw - m_yaw) >= k_aiming_channel_epsilon;
        m_target_yaw = pinned;
        if (interpolating) {
            if (best_error < k_aiming_interpolation_stop_delta) {
                m_yaw = pinned;
            }
        } else if (best_error < k_aiming_interpolation_start_delta) {
            m_yaw = pinned;
        }
    }
}

// @address 0xdbca0 ?set_pitch@c_aiming_channel@@QAAXM@Z
void c_aiming_channel::set_pitch(float pitch)
{
    float pinned = (pitch > m_pitch_min ? pitch : m_pitch_min) < m_pitch_max
        ? (pitch > m_pitch_min ? pitch : m_pitch_min)
        : m_pitch_max;
    if (fabs(m_pitch_max - m_pitch_min - 6.2831855f) < k_aiming_channel_epsilon) {
        m_target_pitch = pinned;
        m_pitch = pinned;
    } else {
        bool interpolating;
        float candidate;
        float best_error = fabs(m_pitch - pinned);
        candidate = pitch - 6.2831855f;
        if (candidate >= m_pitch_min && candidate <= m_pitch_max) {
            float error = (m_pitch - candidate) * (m_pitch - candidate);
            if (error < best_error) {
                pinned = candidate;
                best_error = error;
            }
        }
        candidate = pitch + 6.2831855f;
        if (candidate >= m_pitch_min && candidate <= m_pitch_max) {
            float error = (m_pitch - candidate) * (m_pitch - candidate);
            if (error < best_error) {
                pinned = candidate;
                best_error = error;
            }
        }
        interpolating = fabs(m_target_pitch - m_pitch) >= k_aiming_channel_epsilon;
        m_target_pitch = pinned;
        if (interpolating) {
            if (best_error < k_aiming_interpolation_stop_delta) {
                m_pitch = pinned;
            }
        } else if (best_error < k_aiming_interpolation_start_delta) {
            m_pitch = pinned;
        }
    }
}

static __forceinline c_animation_id aiming_channel_find_facial_animation_overlay(const c_animation_manager *manager)
{
    const c_model_animation_graph *graph = (const c_model_animation_graph *)
        global_tag_instances[datum_index_to_absolute_index(manager->m_graph_tag_index)].definition;
    return graph->find_animation(0x3d1);
}

// @address 0x290098 ?reset_facial_animation_channels@c_animation_manager@@QAAXXZ
void c_animation_manager::reset_facial_animation_channels()
{
    long board_index = this->m_mixing_board_type;
    if ((unsigned long)board_index > 2)
        return;
    c_mixing_board_definition *board = &g_mixing_board_definition_list[board_index];
    if (board == 0 || board->component_types[4].count <= 0)
        return;
    c_animation_manager *mixing_board = this + 1;
    c_facial_animation_channel *component = (c_facial_animation_channel *)
        ((char *)mixing_board + board->component_types[4].offset);
    if (component == 0)
        return;

    c_base_channel_settings settings;
    settings.m_definition_index = 0x3d0;
    settings.m_graph_index = this->m_graph_tag_index;
    settings.m_animation_id = aiming_channel_find_facial_animation_overlay(this);
    settings.m_playback_flags.clear();
    settings.m_location = _animation_location_none;

    component->initialize();
    if (settings.m_graph_index != -1 && settings.m_animation_id.valid()) {
        const c_model_animation_graph *definition = (const c_model_animation_graph *)
            global_tag_instances[datum_index_to_absolute_index(this->m_graph_tag_index)].definition;
        c_animation_id animation_1 = definition->find_animation(0x4a2);
        c_animation_id animation_0 = definition->find_animation(0x3d0);

        component->initialize_inline();
        if (component->call_setup_channel_base(&settings)) {
            component->set_animations(animation_0, animation_1);
        }
    }
}
