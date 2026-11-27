// blamlibxbox/auxilary_channel.obj: from the records 0x2fcdc0 0x2fce78
// @compiler xdk6130
// @flags /TP /O2 /Oc

#include "cseries/cseries.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "animations/animation_id.h"
#include "animations/animation_search_path_iterator.h"
#include "animations/mixing_board/channels/channel_base.h"
#include "animations/mixing_board/channels/auxilary_channel.h"


static __forceinline const float &auxilary_channel_real_max(const float &a, const float &b)
{
    return a > b ? a : b;
}

static __forceinline const float &auxilary_channel_real_min(const float &a, const float &b)
{
    return a < b ? a : b;
}

// @address 0x2fcdc0 ?set_frame_position@c_auxilary_channel@@QAAXM@Z
void c_auxilary_channel::set_frame_position(float frame_position)
{
    const c_model_animation *animation = get_animation();
    m_frame_position = auxilary_channel_real_min(auxilary_channel_real_max(frame_position, 0.0f), animation->get_last_frame_index());
}

// @address 0x2fce78 ?get_playback_ratio@c_auxilary_channel@@QBAMXZ
float c_auxilary_channel::get_playback_ratio() const
{
    float result = 0.0f;
    if (valid()) {
        const c_model_animation *animation = get_animation();
        float frame_count = (float)animation->get_frame_count();
        if (frame_count > 0.0f) {
            result = m_frame_position / frame_count;
            result = (result > 0.0f ? result : 0.0f) < 1.0f ? (result > 0.0f ? result : 0.0f) : 1.0f;
        }
    }
    return result;
}
