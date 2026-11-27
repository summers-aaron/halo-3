// blamlibxbox/facial_animation_channel.obj: from the records 0xe5028 0x15b4a0 0x2fd580 0x2fdb00 0x2fdc10 0x709018 0x70c118 0x70c120
// @compiler xdk6130
#include "math/random_math_range.h"
#if defined(_XBOX) && !defined(__clang__)
#include "math/real_constant_pool_zero.h"
#endif
#include "tag_files/tag_groups.h"
#include "animations/animation_data.h"
// @address 0x7c888c g_short_fixed_8_scale_initialized
// @address 0x7c8890 g_short_fixed_8_scale
#include "math/short_fixed_storage.h"
#include "cseries/cseries_wrapped.h"
#include "sound/sound_cache_file_gestalt_globals.h"
#include "sound/sound_cache_file_gestalt.h"
#include <string.h>
// @flags /TP /O2
#include "memory/static_arrays.h"
#include "animations/mixing_board/channels/facial_animation_channel.h"
#include "sound/sound_cache_file_definitions.h"
#include <stdlib.h>
#include "cseries/crt_xdk.h"
#include "memory/tls_globals.h"
#include "math/matrix_math_constants.h"
// @address 0x15b4a0 ?facial_animation_in_progress@c_facial_animation_channel@@QBA_NXZ

#include "animations/mixing_board/channels/facial_animation_channel_types.h"
#include "animations/animation_search_path_iterator.h"

#include "math/short_fixed.h"
#include "cseries/cseries.h"

// mixing_board_slider.cpp owns the out-of-line specialization and its function-local statics.
extern template float t_short_fixed<8>::export_real() const;




struct s_lipsync_phoneme {
    long index;
    signed char value;
    signed char next_value;
};

#include "animations/animation_codec_flags.h"

struct s_facial_animation_lipsync_track
{
    long track;
    float scale;
};
// Brow intensity and emotion identifiers share the animation lookup table.
struct s_facial_animation_brow_table
{
    float brow_minimum;
    long emotion_string_ids[9];
};

struct s_facial_animation_emotion_table
{
    long frames[9];
    float epsilon;
};


bool c_facial_animation_channel::facial_animation_in_progress() const
{
    // match: 0x15b4a0: valid_bit materializes both state flags before branching.
    bool emotion_in_progress = m_state_flags.valid_bit(_facial_animation_state_emotion_bit) && m_state_flags.test(_facial_animation_state_emotion_bit);
    bool gesture_in_progress = m_state_flags.valid_bit(_facial_animation_state_gesture_bit) && m_state_flags.test(_facial_animation_state_gesture_bit);
    return emotion_in_progress || gesture_in_progress || lipsync_in_progress() || m_sound_index != -1;
}


// @address 0x2251c g_real_constant_pool_zero

// @address 0x2fdb00 ?apply_gesture_orientations@c_facial_animation_channel@@IAAXPBUrender_model_definition@@MJJPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@Vc_animation_id@@PBKM@ZJ@Z
void c_facial_animation_channel::apply_gesture_orientations(const render_model_definition *model, float weight, long positive_frame,
    long negative_frame, const c_static_flags<255> *mask, long count, real_orientation *orientations,
    void (*callback)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float), long callback_arg)
{
    if (valid()) {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x2fdb00: 0.0001f and 0.0f load off one 0x8202251c GPR base (literals: each load takes its own lis)
        if (weight > g_real_constant_pool_zero[2300])
#else
        if (weight > 0.0001f)
#endif
            internal_apply_weighted_node_orientations(
                m_animation_id, (float)positive_frame, model, weight, mask, count, orientations, 0, callback_arg, false);
        else if (weight < rasterizer_states_mode_7_depth_bias)
            internal_apply_weighted_node_orientations(m_animation_id, (float)negative_frame, model,
#if defined(_XBOX) && !defined(__clang__)
                (-weight > g_real_constant_pool_zero[0] ? -weight : g_real_constant_pool_zero[0]),
#else
                (-weight > 0.0f ? -weight : 0.0f),
#endif
                mask, count, orientations, 0, callback_arg, false);
    }
}


struct cache_file_tag_instance;

static __forceinline void facial_animation_channel_blend_phoneme(float *phoneme_weights, const s_lipsync_phoneme *phoneme, float t)
{
    if (phoneme->index >= 0 && (unsigned long)phoneme->index < 34) {
        float a = (float)phoneme->value * (1.0f / 127.0f);
        float b = (float)phoneme->next_value * (1.0f / 127.0f);
        phoneme_weights[phoneme->index] = ((a * 2.0f - b * 2.0f) * t + (b * 3.0f - a * 3.0f)) * t * t + a;
    }
}

// @address 0x2fd580 facial_animation_channel_compute_phoneme_weights
extern "C" void facial_animation_channel_compute_phoneme_weights(const c_facial_animation_channel *channel, float *phoneme_weights)
{
    bool success = false;
    if (channel->m_sound_index != -1 && channel->m_lipsync_index != -1) {
        XMemSet(phoneme_weights, 0, 34 * sizeof(float));
        const s_cache_file_sound_definition *sound =
            (const s_cache_file_sound_definition *)tag_get('snd!', channel->m_sound_index);
        const s_sound_extra_info *info =
            &g_sound_cache_file_gestalt.data->extra_infos[sound->extra_info_index]
                .encoded_dialogue[0].facial_animation[channel->m_lipsync_index];
        const s_lipsync_data *lipsync;
        long keyframe_count;
        if (info && info->facial_animation_count == 1 && (lipsync = info->lipsync) != 0 && (keyframe_count = lipsync->keyframe_count) > 0) {
            long last_keyframe = keyframe_count - 1;
            short time = (short)((channel->m_frame + 0.1f) * 1000.0f);
            for (long keyframe_index = 0; keyframe_index < last_keyframe; keyframe_index++) {
                const s_lipsync_keyframe *keyframe = &lipsync->keyframes[keyframe_index];
                const s_lipsync_keyframe *next_keyframe = &lipsync->keyframes[keyframe_index + 1];
                if (keyframe->time <= time && time <= next_keyframe->time) {
                    s_lipsync_phoneme phonemes[6];
                    phonemes[0].index = keyframe->phoneme_index[0];
                    phonemes[0].value = keyframe->phoneme_value[0];
                    phonemes[0].next_value = 0;
                    phonemes[1].index = keyframe->phoneme_index[1];
                    phonemes[1].value = keyframe->phoneme_value[1];
                    phonemes[1].next_value = 0;
                    phonemes[2].index = keyframe->phoneme_index[2];
                    phonemes[2].value = keyframe->phoneme_value[2];
                    phonemes[2].next_value = 0;
                    phonemes[3].index = next_keyframe->phoneme_index[0];
                    phonemes[3].value = 0;
                    phonemes[3].next_value = next_keyframe->phoneme_value[0];
                    phonemes[4].index = next_keyframe->phoneme_index[1];
                    phonemes[4].value = 0;
                    phonemes[4].next_value = next_keyframe->phoneme_value[1];
                    phonemes[5].index = next_keyframe->phoneme_index[2];
                    phonemes[5].value = 0;
                    phonemes[5].next_value = next_keyframe->phoneme_value[2];
                    for (long i = 0; i < 3; i++) {
                        for (long j = 3; j < 6; j++) {
                            if (phonemes[i].index == phonemes[j].index) {
                                phonemes[i].next_value = phonemes[j].next_value;
                                phonemes[j].index = -1;
                            }
                        }
                    }
                    float t;
                    long duration = next_keyframe->time - keyframe->time;
                    long elapsed = time - keyframe->time;
                    if (duration > 1) {
                        t = PIN((float)elapsed / (float)duration, 0.0f, 1.0f);
                    } else {
                        t = 1.0f;
                    }
                    facial_animation_channel_blend_phoneme(phoneme_weights, &phonemes[0], t);
                    facial_animation_channel_blend_phoneme(phoneme_weights, &phonemes[1], t);
                    facial_animation_channel_blend_phoneme(phoneme_weights, &phonemes[2], t);
                    facial_animation_channel_blend_phoneme(phoneme_weights, &phonemes[3], t);
                    facial_animation_channel_blend_phoneme(phoneme_weights, &phonemes[4], t);
                    facial_animation_channel_blend_phoneme(phoneme_weights, &phonemes[5], t);
                    break;
                }
            }
            success = true;
        }
    }
    if (!success)
        memset(phoneme_weights, 0, 34 * sizeof(float));
}



// @rdata 0x8202aaf0..0x8202abe0
static const s_facial_animation_brow_table k_facial_animation_brow_table = {
    -0.2f,
    { 978, 979, 980, 981, 982, 983, 984, 985, 986 }
};


static const s_facial_animation_lipsync_track k_facial_animation_lipsync_tracks[20] = {
    {0, 1.0f},
    {1, 1.0f},
    {2, 1.0f},
    {3, 1.0f},
    {4, 1.0f},
    {5, 1.0f},
    {6, 1.0f},
    {7, 1.0f},
    {8, 1.0f},
    {9, 1.0f},
    {10, 1.0f},
    {11, 1.0f},
    {12, 1.0f},
    {13, 1.0f},
    {14, 1.0f},
    {15, 1.0f},
    {-1, 0.0f},
    {16, 1.0f},
    {16, -1.0f},
    {17, 1.0f},
};

static const s_facial_animation_emotion_table k_facial_animation_emotion_table = {
    { 4, 24, 44, 64, 84, 104, 124, 144, 164 },
    0.0001f
};
// @rdata-end


// match: 0x2fdc10: the disabled noise branch preserves loop unrolling and value numbering.
static __forceinline float facial_animation_channel_noise(float value, float speed, float scale, bool enabled)
{
    if (enabled) {
        float a = speed * 0.7f + scale * 1.1f;
        float b = speed * 1.1f + scale * 0.9f;
        float c = speed * 0.9f + scale * 0.7f;
        float d = a * b + b * c + c * a;
        float e = a * c - b * d + d * a;
        float f = e * a + d * b + c * c;
        float g = f * e - d * c + b * a;
        float h = g * f + e * d - c * b;
        float i = h * g + f * e - d * c;
        float j = i * h + g * f - e * d;
        float k = j * i + h * g - f * e;
        float l = k * j + i * h - g * f;
        value += (a + b + c + d + e + f + g + h + i + j + k + l) * scale;
    }
    return value;
}

// @address 0x2fdc10 ?apply_node_orientations@c_facial_animation_channel@@IAAXPBUrender_model_definition@@MPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@Vc_animation_id@@PBKM@ZJ@Z
void c_facial_animation_channel::apply_node_orientations(
    const render_model_definition *render_model, float weight,
    const c_static_flags<255> *static_flags, long node_count,
    real_orientation *orientations,
    void (*callback)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float),
    long user_data)
{
    if (valid()) {
        bool secondary_animation = ((m_state_flags.m_storage >> 3) & 1) != 0;
        c_animation_id animation_id = m_animation_ids[0];
        if (secondary_animation)
            animation_id = m_animation_ids[1];
        if (animation_id.valid()) {
            float sum0 = 0.0f;
            float sum1 = 0.0f;
            float sum2 = 0.0f;
            float sum3 = 0.0f;
            float sum4 = 0.0f;
            float sum5 = 0.0f;
            float sum6 = 0.0f;
            float sum7 = 0.0f;
            bool in_progress = facial_animation_in_progress();
            if (in_progress) {
                if (!secondary_animation
                    && (m_state_flags.m_storage & (1 << _facial_animation_state_emotion_bit)) == 0
                    && (m_state_flags.m_storage & (1 << _facial_animation_state_gesture_bit)) == 0
                    && m_sound_index == -1
                    && lipsync_in_progress()) {
                    float lipsync_weight = m_lipsync_frames_remaining.export_real();
                    float lipsync_frame = 3.0f;
                    internal_apply_weighted_node_orientations(get_animation_id(), lipsync_frame, render_model, lipsync_weight, static_flags,
                        node_count, orientations, 0, user_data, false);
                } else {
                    float phoneme_weights[34];
                    c_static_array<float, 9> track_weights;
                    flags256 node_mask;
                    facial_animation_channel_compute_phoneme_weights(this, phoneme_weights);
                    float total_weight = 0.0f;
                    for (long i = 0; i < 9; i++) {
                        track_weights[i] = m_emotions[i].m_value.unpack();
                        if (i != 0)
                            track_weights[i] = MAX(track_weights[i], phoneme_weights[i + 25]);
                        total_weight += track_weights[i];
                    }
                    if (total_weight <= 1.0f) {
                        track_weights[0] = MIN(MAX(track_weights[0] + (1.0f - total_weight), 0.0f), 1.0f);
                    } else {
                        // match: 0x2fdc10: bind the reciprocal once for all emotion tracks.
                        float track_weight;
                        const float &inv_total_weight = (1.0f / total_weight);
                        for (long i = 0; i < 9; i++) {
                            track_weight = track_weights[i];
                            track_weights[i] = track_weight * inv_total_weight;
                        }
                    }
                    float blend = m_blend;
                    if ((m_state_flags.m_storage & (1 << _facial_animation_state_gesture_bit)) != 0)
                        blend = 1.0f;
                    internal_apply_weighted_node_orientations(
                        animation_id, 0.0f, render_model, blend, static_flags, node_count, orientations, 0, user_data, false);
                    memset(&node_mask, 0, sizeof(node_mask));
                    const c_model_animation_graph *graph =
                        (const c_model_animation_graph *)tag_get('jmad', m_graph_index);
                    if (graph) {
                        c_animation_data animation_data;
                        if (graph->get_animation_data(animation_id, &animation_data) && animation_data.valid())
                            animation_data_merge_flags(&animation_data, (unsigned char *)&node_mask);
                    }
                    if (static_flags)
                        node_mask.and_with((const flags256 *)static_flags);
                    for (long i = 0; i < 9; i++) {
                        if (track_weights[i] > 0.0f) {
                            internal_apply_weighted_node_orientations(get_animation_id(), (float)k_facial_animation_emotion_table.frames[i],
                                render_model, track_weights[i] * blend, (const c_static_flags<255> *)&node_mask, node_count, orientations,
                                0, user_data, false);
                        }
                    }
                    for (long track = 1; track < 20; track++) {
                        long track_index = k_facial_animation_lipsync_tracks[track].track;
                        float track_scale = k_facial_animation_lipsync_tracks[track].scale;
                        if (track_index != -1) {
                            float track_weight = phoneme_weights[track_index] * track_scale;
                            if (track_index == 17) {
                                track_weight = MAX(track_weight, m_lipsync_frames_remaining.export_real());
                            } else {
                                track_weight = m_blend * track_weight;
                            }
                            if (track_weight > 0.0001f) {
                                for (long i = 0; i < 9; i++) {
                                    if (track_weights[i] > 0.0f) {
                                        internal_apply_weighted_node_orientations(get_animation_id(),
                                            (float)(k_facial_animation_emotion_table.frames[i] + track), render_model,
                                            track_weights[i] * track_weight, (const c_static_flags<255> *)&node_mask, node_count,
                                            orientations, 0, user_data, false);
                                    }
                                }
                            }
                        }
                    }
                    sum0 = phoneme_weights[18] + phoneme_weights[21];
                    sum2 = phoneme_weights[20] + phoneme_weights[23];
                    sum1 = phoneme_weights[19] + phoneme_weights[22];
                    sum3 = phoneme_weights[24] - sum0;
                    sum4 = phoneme_weights[25] - sum2;
                }
            }
            sum0 = facial_animation_channel_noise(sum0, 1.0f, 2.0f, false);
            float gesture0 = PIN(m_scale * sum0, -1.0f, 1.0f);
            float gesture1 = PIN(m_scale * sum1, -1.0f, 1.0f);
            float gesture2 = PIN(m_scale * sum2, -1.0f, 1.0f);
            float gesture3 = PIN(m_scale * sum3, -1.0f, 1.0f);
            float gesture4 = PIN(m_scale * sum4, -1.0f, 1.0f);
            float gesture5 = PIN(m_scale * sum5, k_facial_animation_brow_table.brow_minimum, 1.0f);
            float gesture6 = PIN(m_scale * sum6, k_facial_animation_brow_table.brow_minimum, 1.0f);
            float gesture7 = PIN(m_scale * sum7, 0.0f, 1.0f);
            apply_gesture_orientations(render_model, gesture0, 185, 186, static_flags, node_count, orientations, 0, user_data);
            apply_gesture_orientations(render_model, gesture1, 187, 188, static_flags, node_count, orientations, 0, user_data);
            apply_gesture_orientations(render_model, gesture2, 189, 190, static_flags, node_count, orientations, 0, user_data);
            apply_gesture_orientations(render_model, gesture3, 191, 192, static_flags, node_count, orientations, 0, user_data);
            apply_gesture_orientations(render_model, gesture4, 193, 194, static_flags, node_count, orientations, 0, user_data);
            apply_gesture_orientations(render_model, gesture5, 195, 196, static_flags, node_count, orientations, 0, user_data);
            apply_gesture_orientations(render_model, gesture6, 197, 198, static_flags, node_count, orientations, 0, user_data);
            internal_apply_weighted_node_orientations(get_animation_id(), 4.0f, render_model,
                (float)m_emotions[0].m_value.m_value * gesture7 * (1.0f / 127.0f), static_flags, node_count, orientations, 0, user_data,
                false);
        }
    }
}


// @address 0x79d7e4 k_facial_animation_gesture_duration
// @address 0x73e648 k_facial_animation_gesture_table

__forceinline void c_facial_animation_value::pack(float value)
{
    m_value = (signed char)(int)PIN(value * 127.0f, -127.0f, 127.0f);
}

static __forceinline void facial_animation_channel_import_real(t_short_fixed<8> *fixed, float value)
{
    if ((g_short_fixed_8_scale_initialized & 1) == 0) {
        g_short_fixed_8_scale_initialized |= 1;
        g_short_fixed_8_scale = 256.0f;
    }
    fixed->m_value = (short)(g_short_fixed_8_scale * value);
}



// @rdata 0x82028bec
static const float k_facial_animation_slow_decay = 0.99f;
// @rdata 0x82022dbc
static const float k_facial_animation_minimum_scale = 0.1f;

// @address 0xe5028 ?update@c_facial_animation_channel@@IAA_NXZ
bool c_facial_animation_channel::update()
{
    game_time_globals_definition *game_time = tls_game_time_globals();
    t_short_fixed<8> frames;
    facial_animation_channel_import_real(&frames, game_time->tick_length);
    m_gesture_frame -= frames;
    if (m_gesture_frame < 0) {
        m_gesture_frame = random_range(tls_random_seed(), (short)(k_facial_animation_gesture_duration.m_value << 3),
            (short)(k_facial_animation_gesture_duration.m_value << 4));
        m_lipsync_frames_remaining = 0;
    } else {
        bool gesture_active = m_gesture_frame < k_facial_animation_gesture_duration;
        if (gesture_active) {
            float gesture_remaining = m_gesture_frame.export_real();
            float gesture_duration = k_facial_animation_gesture_duration.export_real();
            int gesture_index = PIN((int)(gesture_remaining / gesture_duration * 10.0f), 0,
                sizeof(k_facial_animation_gesture_table) / sizeof(k_facial_animation_gesture_table[0]) - 1);
            m_lipsync_frames_remaining = (short)(k_facial_animation_gesture_table[gesture_index] * 256.0f);
        }
    }
    float elapsed = game_time->tick_length;
    short elapsed_short = (short)(elapsed * 256.0f);
    bool any = false;
    for (unsigned long index = 0; index < 9; index++) {
        c_facial_animation_value *current = &m_emotions[index].m_value;
        short rate = m_emotions[index].m_rate.m_value;
        if (rate > elapsed_short) {
            float duration = (float)(short)m_emotions[index].m_rate.m_value * 0.00390625f;
            float cratio = PIN(elapsed / duration, 0.0f, 1.0f);
            float base = current->unpack_scaled();
            float v = (m_emotions[index].m_target.unpack_scaled() - base) * cratio + base;
            current->pack(v);
            m_emotions[index].m_rate = MAX(0, rate - elapsed_short);
        } else if (rate != 0) {
            m_emotions[index].m_rate.m_value = 0;
            *current = m_emotions[index].m_target;
        } else if (elapsed_short > 0 && (m_state_flags.m_storage & 8) == 0) {
            m_emotions[index].m_target.m_value = 0;
            m_emotions[index].m_rate.m_value = 0xA00;
        }
        bool active = current->active();
        if (active)
            any = true;
    }
    if (any)
        m_state_flags.m_storage |= 2;
    else
        m_state_flags.m_storage &= 253;
    unsigned char flags = m_state_flags.m_storage;
    if ((flags & 4) != 0) {
        float blend = PIN(m_blend, 0.0f, 1.0f);
        float inverse = 1.0f - blend;
        m_blend = blend;
        m_blend = PIN((inverse * 0.3f) + blend, 0.0f, 1.0f);
    } else {
        if ((flags & 8) != 0)
            m_blend = PIN(m_blend * k_facial_animation_slow_decay, 0.0f, 1.0f);
        else
            m_blend = PIN(m_blend * 0.8f, 0.0f, 1.0f);
        if (m_blend < 0.0001f) {
            m_frame = 0.0f;
            m_sound_index = -1;
            m_lipsync_index = -1;
        }
    }
    if ((flags & 8) != 0) {
        float clamped = PIN(m_scale, k_facial_animation_minimum_scale, 1.0f);
        float inverse = 1.0f - clamped;
        m_scale = clamped;
        m_scale = PIN((inverse * 0.3f) + clamped, k_facial_animation_minimum_scale, 1.0f);
    } else {
        float nonzero = MAX(m_scale, 0.0001f);
        m_scale = PIN((nonzero * 0.8f) + nonzero, k_facial_animation_minimum_scale, 1.0f);
    }
    return false;
}

// @address 0x709008 ??__Ek_facial_animation_gesture_duration@@YAXXZ
// @address 0x79d7e8 ?k_facial_animation_blink_duration@@3V?$t_short_fixed@$07@@B
// @address 0x709018 ??__Ek_facial_animation_blink_duration@@YAXXZ
extern "C" {
t_short_fixed<8> k_facial_animation_gesture_duration = 85;
float k_facial_animation_gesture_table[10] = { 0.0f, 0.02f, 0.1f, 0.4f, 1.0f, 0.99f, 0.97f, 0.8f, 0.5f, 0.0f };
}
const t_short_fixed<8> k_facial_animation_blink_duration = 25;
