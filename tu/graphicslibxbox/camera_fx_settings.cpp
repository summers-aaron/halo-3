// graphicslibxbox/camera_fx_settings.obj: from the records 0x16ae48 0x16b4b8 0x16b908 0x16ba68 0x16bc40 0x44d620 0x44d630 0x44d678 0x44d6d0 0x44d778 0x44d7d8 0x44d958 0x44dab8
// @address 0x11df6e0 camera_fx_globals
// @flags /TP /O2 /Oc
#include "render/camera_fx_globals_types.h"
__declspec(thread) s_camera_fx_globals *camera_fx_globals;
#include "cseries/cseries.h"
#include "render/camera_fx_settings.h"
#include "render/camera_fx_values.h"
#include "render/screen_postprocess.h"
#include "camera/camera_fx_settings.h"
#include "camera/cinematic_lightmap_shadow.h"
// @address 0x44d7d8 ?animate_global_exposure_modifier@@YAXPAUs_global_modifier@@M@Z
// @address 0x44dab8 ?get_max_historic_exposure@c_exposure@@QAAMM@Z
#include <float.h>
#include "memory/tls_globals.h"
#include <math.h>
#include "game/game_time_types.h"
#include "math/real_math_types.h"
#include "cache/restricted_memory_regions.h"
#include "cache/cache_files.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_surface_globals.h"
#include "rasterizer/xenon/rasterizer_xenon_d3d.h"

#ifdef HALO3_64BIT
#include "cseries/crt_xdk.h"
extern "C" unsigned int port_vk_frame_rate();
#endif

// Minimum of the last N samples of a 60-entry ring buffer.
#include "render/camera_fx_globals.h"


#include "render/camera_fx_settings_types.h"


// @address 0x8c4839 g_cinematic_lightmap_shadow_enabled
unsigned char g_cinematic_lightmap_shadow_enabled;

// @address 0x73a170 ?g_HDR_target_stops@c_camera_fx_values@@2MA
float c_camera_fx_values::g_HDR_target_stops = 7.0f;

#ifndef MIN
#endif
// 0x16ae48
struct s_camera_fx_step_result {
    float value;
};

static __forceinline void camera_fx_settings_unlock_surface(c_rasterizer::e_surface surface)
{
    c_render_surface *render_surface = g_rasterizer_surface_groups[surface].get_specialized_surface(surface);
    D3DTexture_UnlockRect(render_surface->get_d3d_texture(), 0);
}
static __forceinline void camera_fx_settings_update_exposure_buffer(c_exposure &exposure, float target)
{
    exposure.samples[exposure.next_index] = target;
    if (++exposure.next_index >= 60)
        exposure.next_index = 0;
}

// @address 0x16b4b8 ?calculate_new_value@s_real_exposure_parameter@c_camera_fx_settings@@QBAXAAVc_exposure@@KMMMM@Z
void c_camera_fx_settings::s_real_exposure_parameter::calculate_new_value(c_exposure &exposure, unsigned long flags,
    float target, float adjustment, float minimum, float maximum) const
{
    if (flags & 4) {
        float adjusted = exposure.previous + adjustment;
        char *pixels = 0;
        long pitch = 0;
        float measured = 0.0f;
        if (exposure.surface != -1) {
            c_rasterizer::e_surface surface = (c_rasterizer::e_surface)(exposure.surface + 42);
            c_rasterizer::lock_surface_as_texture(surface, 16, &pixels, &pitch);
            measured = -half_to_real(*(unsigned short *)pixels);
            camera_fx_settings_unlock_surface(surface);
        }
        target = adjusted + measured;
    }
    exposure.previous = exposure.current;
    target = MIN(MAX(target, minimum), maximum);
    s_camera_fx_globals *globals = camera_fx_globals;
    if ((unsigned char)((~globals->flags >> 1) & 1))
        camera_fx_settings_update_exposure_buffer(exposure, target);
    bool instantly = (unsigned char)((~globals->flags >> 1) & 1) && (globals->flags & 1);
    if (instantly) {
        exposure.current = (float)(0.1 * exposure.current + 0.9 * target);
    } else {
        float current = exposure.current;
        if ((current < target && current < exposure.get_min_historic_exposure(history_seconds)) ||
            (current > target && current > exposure.get_max_historic_exposure(history_seconds))) {
            float blend = interpolation;
            float delta = fabsf(current - target) * 0.05f;
            if (blend > delta)
                blend = delta;
            if (!c_screen_postprocess::x_editable_settings.exposure_adapt_instantly && (unsigned char)((~(camera_fx_globals)->flags >> 1) & 1))
                exposure.current = (1.0f - blend) * current + blend * target;
        }
#ifndef HALO3_64BIT
        else {
            exposure.timer = 0.0f;
        }
#endif
    }
}

template<class T>
static __forceinline const T *camera_fx_settings_select(const c_camera_fx_settings *base, const c_camera_fx_settings *tag,
    const c_camera_fx_settings *c, const c_camera_fx_settings *d, T c_camera_fx_settings::*member)
{
    const T *result = &(base->*member);
    if (result->flags & 1) {
        if (tag)
            result = &(tag->*member);
        if (result->flags & 1) {
            if (c)
                result = &(c->*member);
            if (result->flags & 1) {
                if (d)
                    result = &(d->*member);
            }
        }
    }
    return result;
}
static __forceinline void camera_fx_settings_blend_color(real_rgb_color *current, const c_camera_fx_settings::s_color_parameter *target)
{
    current->red = (float)(current->red * (1.0 - 0.05f) + target->red * 0.05f);
    current->green = (float)(current->green * (1.0 - 0.05f) + target->green * 0.05f);
    current->blue = (float)(current->blue * (1.0 - 0.05f) + target->blue * 0.05f);
}
static __forceinline s_camera_fx_step_result camera_fx_settings_step(
    const c_camera_fx_settings::s_real_parameter *parameter, float current_value, float target_value)
{
    s_camera_fx_step_result result;
    result.value = parameter->calculate_new_value(current_value, target_value);
    return result;
}
static __forceinline void camera_fx_settings_update_real(const c_camera_fx_settings::s_real_parameter *parameter, float *current, float target)
{
    *current = camera_fx_settings_step(parameter, *current, target).value;
}
static __forceinline float camera_fx_settings_apply_global_exposure(const s_camera_fx_globals *globals, float current, float offset, float scale)
{
    float blended = (1.0f - globals->current_b) * current + globals->current_a * globals->current_b;
    return (blended + globals->exposure_offset - offset) * scale + offset;
}
// @address 0x16ae48 ?update@c_camera_fx_values@@QAAPBUs_real_exposure_parameter@c_camera_fx_settings@@PBV3@PBUstructure_camera_fx_palette_entry@@00M@Z
const c_camera_fx_settings::s_real_exposure_parameter *c_camera_fx_values::update(
    const c_camera_fx_settings *base, const structure_camera_fx_palette_entry *palette, const c_camera_fx_settings *c,
    const c_camera_fx_settings *d, float value)
{
    c_camera_fx_values *self = this;
    self->exposure_boost = value;
    const c_camera_fx_settings *tag = 0;
    if (palette && palette->camera_fx.index != -1)
        tag = (const c_camera_fx_settings *)tag_get(0, palette->camera_fx.index);
    const c_camera_fx_settings::s_real_exposure_parameter *exposure = camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::exposure);
    float target = exposure->value;
    unsigned long flags = exposure->flags;
    float adjustment = exposure->adjustment;
    float minimum = exposure->minimum;
    float maximum = exposure->maximum;
    if (palette && exposure != &base->exposure) {
        unsigned char palette_flags = palette->flags;
        if (palette_flags & 1) {
            target = palette->forced_exposure;
            flags &= ~4;
        }
        if (palette_flags & 2) {
            adjustment = palette->forced_exposure;
            flags |= 4;
        }
        if (palette_flags & 4) {
            minimum = palette->minimum;
            maximum = palette->maximum;
        }
    }
#ifdef HALO3_64BIT
    // The adaptation coefficients and history samples assume 30 updates per second.
    // Use display time so exposure can still adapt while a paused film's camera moves.
    const unsigned long interval = (unsigned long)(GetTickCount() * 30ull / 1000ull);
    const bool advance_exposure = port_vk_frame_rate() == 30 || self->last_update_interval != interval;
    self->last_update_interval = interval;
    if (advance_exposure)
        exposure->calculate_new_value(*self, flags, target, adjustment, minimum, maximum);
    else
        // The resolve arrives two rendered frames later. Keep its exposure paired with the sample.
        self->previous = self->current;
#else
    exposure->calculate_new_value(*self, flags, target, adjustment, minimum, maximum);
#endif
    self->exposure_sensitivity = camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::exposure_sensitivity)->value;
    self->white_point = camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::white_point)->value;
    const c_camera_fx_settings::s_real_parameter *bloom_intensity = camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::bloom_intensity);
    self->bloom_intensity = bloom_intensity->calculate_new_value(self->bloom_intensity, bloom_intensity->value);
    const c_camera_fx_settings::s_real_parameter *bloom_threshold = camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::bloom_threshold);
    float threshold_target = bloom_threshold->value;
    if (palette && exposure != &base->exposure && (palette->flags & 8))
        threshold_target = palette->bloom_threshold;
    self->bloom_threshold = bloom_threshold->calculate_new_value(self->bloom_threshold, threshold_target);
    const c_camera_fx_settings::s_real_parameter *bloom_brightness = camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::bloom_brightness);
    float brightness_target = bloom_brightness->value;
    if (palette && exposure != &base->exposure && (palette->flags & 8))
        brightness_target = palette->bloom_brightness;
    self->bloom_brightness = bloom_brightness->calculate_new_value(self->bloom_brightness, brightness_target);
    camera_fx_settings_blend_color(&self->color_balance, camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::color_balance));
    camera_fx_settings_blend_color(&self->color_contrast, camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::color_contrast));
    camera_fx_settings_blend_color(&self->color_saturation, camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::color_saturation));
#ifdef HALO3_64BIT
    if (advance_exposure)
#endif
    {
        const c_camera_fx_settings::s_real_parameter *exposure_offset = camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::exposure_offset);
        self->exposure_offset = exposure_offset->calculate_new_value(self->exposure_offset, exposure_offset->value);
        const c_camera_fx_settings::s_real_parameter *exposure_scale = camera_fx_settings_select(base, tag, c, d, &c_camera_fx_settings::exposure_scale);
        camera_fx_settings_update_real(exposure_scale, &self->exposure_scale, exposure_scale->value);
    }
    s_camera_fx_globals *globals = camera_fx_globals;
    self->exposure_result = camera_fx_settings_apply_global_exposure(globals, self->current, self->exposure_offset, self->exposure_scale);
    return exposure;
}

// match: 0x15efc8: inline lets cl expand this into geometry_sampling_get_lightmap_probe and
// render_objects_decode_lightprobe; plain, both keep the calls and grow 8 bytes
#if defined(_XBOX) && !defined(__clang__)
inline float half_to_real(unsigned short half)
#else
float half_to_real(unsigned short half)
#endif
{
    int sign = (half >> 15) & 1;
    int exponent = (half >> 10) & 31;
    int mantissa = half & 0x3ff;
    union { int bits; float value; } result;
    result.bits = 0;
    if (exponent == 0) {
        if (mantissa == 0) {
            result.bits = sign << 31;
            return result.value;
        }
        while (!(mantissa & 0x400)) {
            mantissa <<= 1;
            exponent -= 1;
        }
        exponent += 1;
        mantissa &= ~0x400;
    } else if (exponent == 31) {
        if (mantissa == 0) {
            result.bits = (sign << 31) | 0x7f800000;
            return result.value;
        }
        result.bits = (sign << 31) | 0x7fc00000 | (mantissa & 1);
        return result.value;
    }
    exponent += 112;
    mantissa <<= 13;
    result.bits = (sign << 31) | (exponent << 23) | mantissa;
    return result.value;
}

float c_exposure::get_min_historic_exposure(float seconds)
{
  long count = (long)((float)tls_game_time_globals()->ticks_per_second * seconds);
  count = MIN(MAX(count, 1), 60);
  long index = next_index - count;
  if (index < 0)
    index += 60;
  float result = FLT_MAX;
  for (long i = 0; i < count; i++) {
    if (result > samples[index])
      result = samples[index];
    index = (index + 1) % 60;
  }
  return result;
}

inline float c_camera_fx_settings::s_real_parameter::calculate_new_value(float current_value, float target_value) const
{
    float new_value = (float)((1.0 - interpolation) * current_value + interpolation * target_value);
    if (max_change < 0.0f) return new_value;
    float maximum_step = max_change;
    if ((flags & 2) != 0) maximum_step = fabsf(current_value) * maximum_step;
    float difference = new_value - current_value;
    if (fabsf(difference) <= maximum_step) return new_value;
    float minimum_step = -maximum_step;
    float clamped_difference;
    if (difference > minimum_step) clamped_difference = difference;
    else clamped_difference = minimum_step;
    float limited_change;
    if (clamped_difference < maximum_step) {
        if (difference > minimum_step) limited_change = difference;
        else limited_change = minimum_step;
    } else {
        limited_change = maximum_step;
    }
    return limited_change + current_value;
}

// @address 0x44d620 camera_fx_set_globals_address
extern "C" void camera_fx_set_globals_address(void *value)
{
    camera_fx_globals = (s_camera_fx_globals *)value;
}

// @address 0x44d630 camera_fx_allocate_globals
extern "C" void camera_fx_allocate_globals(void)
{
    camera_fx_globals = (s_camera_fx_globals *)g_camera_fx_globals_allocator.reserve_memory("scripted exposure globals", 0, sizeof(s_camera_fx_globals), 0);
}

// @address 0x44d678 camera_fx_initialize_globals
extern "C" void camera_fx_initialize_globals(void)
{
    camera_fx_globals->flags = 0;
    camera_fx_globals->exposure_scale = 1.0f;
    camera_fx_globals->exposure_offset = 0.0f;
    camera_fx_globals->current_b = 0.0f;
    camera_fx_globals->current_a = 0.0f;
    camera_fx_globals->start_a = 0.0f;
    camera_fx_globals->target_a = 0.0f;
    camera_fx_globals->start_b = 0.0f;
    camera_fx_globals->target_b = 0.0f;
    camera_fx_globals->start_time = 0;
    camera_fx_globals->end_time = 0;
    camera_fx_globals->pulse_ticks = 0;
    camera_fx_globals->hold_ticks = 0;
}

// @address 0x44d6d0 camera_fx_animate_exposure
extern "C" void camera_fx_animate_exposure(float start_a, float target_a, float start_b, float target_b, float duration_seconds)
{
    camera_fx_globals->flags |= 4;
    camera_fx_globals->start_b = start_b;
    camera_fx_globals->target_b = target_b;
    camera_fx_globals->start_a = start_a;
    camera_fx_globals->target_a = target_a;
    camera_fx_globals->start_time = tls_game_time_globals()->time;
    camera_fx_globals->end_time = camera_fx_globals->start_time + (long)ceilf((float)tls_game_time_globals()->ticks_per_second * duration_seconds);
}

// @address 0x44d778 camera_fx_set_exposure
extern "C" void camera_fx_set_exposure(float scale)
{
    camera_fx_globals->exposure_offset = -scale;
    camera_fx_globals->exposure_scale = (float)pow(2.0, (double)scale);
}

void animate_global_exposure_modifier(s_global_modifier *modifier, float delta_time)
{
    if (modifier->flags & 4) {
        long start_time = modifier->start_time;
        long end_time = modifier->end_time;
        long duration = end_time - start_time;
        long time = tls_game_time_globals()->time;
        if (time < end_time && duration > 0) {
            float fraction = (float)(time - start_time) / (float)duration;
            float blend = (float)((3.0 - fraction * 2.0) * fraction * fraction);
            modifier->current_a = (float)((1.0 - blend) * modifier->start_a + modifier->target_a * blend);
            modifier->current_b = (float)((1.0 - blend) * modifier->start_b + modifier->target_b * blend);
        } else {
            modifier->current_a = modifier->target_a;
            modifier->current_b = modifier->target_b;
            modifier->flags &= ~4;
            modifier->hold_ticks = 3;
        }
    }
    if (modifier->current_b > 0.0f || (modifier->flags & 8) != 0 || modifier->hold_ticks > 0)
        modifier->flags |= 2;
    else
        modifier->flags &= ~2;
    if (modifier->hold_ticks > 0)
        modifier->hold_ticks--;
    if (modifier->pulse_ticks > 0) {
        modifier->pulse_ticks--;
        modifier->flags |= 1;
    } else {
        modifier->flags &= ~1;
    }
}

static __forceinline void camera_fx_settings_init_real(c_camera_fx_settings::s_real_parameter *p, float v, float m, float i, unsigned short flags = 1)
{
    p->flags = flags;
    p->value = v;
    p->max_change = m;
    p->interpolation = i;
}
static __forceinline void camera_fx_settings_init_exposure(c_camera_fx_settings::s_real_exposure_parameter *e)
{
    camera_fx_settings_init_real(e, 0.0f, 0.05f, 0.05f, 7);
    e->minimum = -6.0f;
    e->maximum = 6.0f;
    e->history_seconds = 0.5f;
}
static __forceinline void camera_fx_settings_init_instant(c_camera_fx_settings::s_real_instant_parameter *p, float v)
{
    p->flags = 1;
    p->value = v;
}

static __forceinline void camera_fx_settings_init_color(c_camera_fx_settings::s_color_parameter *p)
{
    p->flags = 1;
    p->red = 1.0f;
    p->green = 1.0f;
    p->blue = 1.0f;
}
// @address 0x44d958 camera_fx_settings_initialize
extern "C" void camera_fx_settings_initialize(c_camera_fx_settings *s, bool enabled)
{
    unsigned short flags = 1;
    s->exposure.set(0.0f, 0.05f, 0.05f, flags | 6);
    s->exposure.minimum = -6.0f;
    s->exposure.maximum = 6.0f;
    s->exposure.history_seconds = 0.5f;
    s->exposure_sensitivity.set(0.0f, flags);
    s->white_point.set(1.0f, flags);
    s->bloom_intensity.set(1.5f, 0.1f, 0.15f, flags);
    s->bloom_threshold.set(0.06f, 0.01f, 0.15f, flags);
    s->bloom_brightness.set(1.0f, 0.05f, 0.25f, flags);
    s->color_balance.set(1.0f, 1.0f, 1.0f, flags);
    s->color_contrast.set(1.0f, 1.0f, 1.0f, flags);
    s->color_saturation.set(1.0f, 1.0f, 1.0f, flags);
    s->parameter144.set(1.0f, 0.05f, 0.25f, flags);
    s->parameter160.set(250.0f, 2.0f, 0.25f, flags);
    s->parameter176.set(45.0f, 10.0f, 0.1f, flags);
    s->parameter192.set(3, flags);
    s->exposure_offset.set(0.0f, 0.05f, 0.2f, flags);
    s->exposure_scale.set(1.0f, 0.05f, 0.1f, flags);
}

float c_exposure::get_max_historic_exposure(float seconds)
{
  long count = (long)((float)tls_game_time_globals()->ticks_per_second * seconds);
  count = MIN(MAX(count, 1), 60);
  long index = next_index - count;
  if (index < 0)
    index += 60;
  float result = -FLT_MAX;
  for (long i = 0; i < count; i++) {
    if (!(result > samples[index]))
      result = samples[index];
    index = (index + 1) % 60;
  }
  return result;
}

camera_fx_settings_allocation_manager g_camera_fx_globals_allocator;


