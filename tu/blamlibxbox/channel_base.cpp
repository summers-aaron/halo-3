// blamlibxbox/channel_base.obj: from the records 0xed968 0x2fbba0 0x2fbc10 0x2fbcb0 0x2fbd90 0x2fbe70
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "animations/animation_id.h"
#include "animations/mixing_board/channels/channel_base.h"
// @address 0x2fbba0 ?prepare_node_mask_flags@c_channel_base@@QBAPBV?$c_static_flags@$0PP@@@PBUrender_model_definition@@J@Z
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "animations/animation_definitions.h"

// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
#include "tag_files/tag_groups.h"

#include "animations/animation_search_path_iterator.h"


#include "animations/animation_compute_orientations.h"
#include "animations/animation_data.h"
#include "animations/animation_codec_types.h"

// @address 0x79d838 ?g_codec_descriptions@@3PAVc_codec_description@@A

// @address 0xed968 ?setup_channel_base@c_channel_base@@IAA_NPBVc_base_channel_settings@@@Z
bool c_channel_base::setup_channel_base(const c_base_channel_settings *settings) {
    clear();
    if (settings && settings->m_graph_index != -1 && settings->m_graph_index != 0 && settings->m_animation_id.valid()) {
        m_graph_index = settings->m_graph_index;
        m_animation_id = settings->m_animation_id;
        m_playback_flags = settings->m_playback_flags;
        m_definition_index = settings->m_definition_index;
        m_location = settings->m_location;
        if (m_definition_index == -1) {
            const c_model_animation *animation = get_animation();
            if (animation)
                m_definition_index = animation->name;
        }
    }
    return valid();
}

// match: 0x2fbd90: without it internal_apply_weighted_node_orientations inlines this mask lookup instead of bl 0x2fbba0
#pragma auto_inline(off)
const c_static_flags<255> *c_channel_base::prepare_node_mask_flags(const render_model_definition *render_model, long node_count) const
{
    const c_static_flags<255> *result = 0;
    unsigned short flags = m_playback_flags.get_unsafe();
    unsigned char primary_mask = flags >> 11;
    if ((primary_mask & 1) != 0) {
        result = &TAG_GET(0, c_model_animation_graph, m_graph_index)->node_masks[0];
    } else {
        unsigned char secondary_mask = (flags >> 10) & 63;
        if ((secondary_mask & 1) != 0) {
            result = &TAG_GET(0, c_model_animation_graph, m_graph_index)->node_masks[1];
        }
    }
    return result;
}
// match: 0x2fbd90: without it internal_apply_weighted_node_orientations inlines this mask lookup instead of bl 0x2fbba0
#pragma auto_inline(on)

// @address 0x2fbc10 ?animation_allows_interpolation_out@c_channel_base@@QBA_NXZ
bool c_channel_base::animation_allows_interpolation_out() const {
    bool result = true;
    if (valid()) {
        const c_model_animation *animation = get_animation();
        result = (animation->animation_flags & 2) == 0;
    }
    return result;
}

static __forceinline float channel_base_convert_animation_frame_value_to_time_signature(float frame_value, float rate)
{
    rate = rate > 0.0001f ? rate : 0.0001f;
    return frame_value / rate;
}

// @address 0x2fbe70 ?get_authored_duration@c_channel_base@@QBAMXZ
float c_channel_base::get_authored_duration() const {
    float result = 0.0f;
    if (valid()) {
        const c_model_animation *animation = get_animation();
        result = channel_base_convert_animation_frame_value_to_time_signature((float)animation->frame_count, get_authored_rate());
    }
    return result;
}

// @address 0x2fbcb0 ?get_keyframe_translation@c_channel_base@@QBAXMGFPATreal_point3d@@@Z
void c_channel_base::get_keyframe_translation(float p1, unsigned short p2, short p3, real_point3d *p4) const {
    c_animation_data animation_data;
    if (animation_definitions_try_and_get_data(this, &animation_data)) {
        const s_animation_codec_header *codec_header =
            (const s_animation_codec_header *)(animation_data.m_data + animation_data.m_sizes->m_default_data_size);
        real_orientation orientation;
        g_codec_descriptions[codec_header->codec_type].decompress_translation(
            codec_header, (short)p2, 0, 0, 0.0f, 0.0f, &orientation);
        *p4 = orientation.translation;
    } else {
        p4->x = 0.0f;
        p4->y = 0.0f;
        p4->z = 0.0f;
    }
}

// @address 0x2fbd90 ?internal_apply_weighted_node_orientations@c_channel_base@@IAAXVc_animation_id@@MPBUrender_model_definition@@MPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@0PBKM@ZJ_N@Z
void c_channel_base::internal_apply_weighted_node_orientations(c_animation_id animation_id, float frame,
    const render_model_definition *render_model, float weight, const c_static_flags<255> *static_flags, long node_count,
    real_orientation *orientations, void (*callback)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float),
    long callback_arg, bool force)
{
    c_static_flags_no_init<255> node_flags;
    bool computation_default = force || (bool)((m_playback_flags.get_unsafe() >> 6) & 1);
    bool computation_forced = !computation_default;
    bool computation_flags = !computation_forced;
    // match: 0x2fbd90: pointer __restrict qualifier prevents alias reload
    const c_static_flags<255> *__restrict masks = prepare_node_mask_flags(render_model, node_count);
    const c_static_flags_no_init<255> *use = combine_node_flag_masks(
        (const c_static_flags_no_init<255> *)masks, (const c_static_flags_no_init<255> *)static_flags, &node_flags) ? &node_flags : 0;
    if (node_count != 0)
        animation_compute_node_orientations(m_graph_index, animation_id, node_count, frame, weight, use, orientations, computation_flags,
            false, 0.0f, 0.0f);
}
