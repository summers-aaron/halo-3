#ifndef RENDER_CAMERA_FX_VALUES_H
#define RENDER_CAMERA_FX_VALUES_H

#include "render/camera_fx_settings_types.h"
#include "cseries/cseries_types.h"
#include "tag_files/tag_types.h"

extern "C" void *_blkmov(void *dst, const void *src, unsigned long size);

class c_exposure {
public:
  long index0;
  long surface;
  float previous;
  float current;
#ifdef HALO3_64BIT
  unsigned long last_update_interval;
#else
  float timer;
#endif
  float samples[60];
  long next_index;
public:
  void set_exposure(float exposure)
  {
#ifdef HALO3_64BIT
      this->last_update_interval = ~0ul;
#else
      this->timer = 0.0f;
#endif
      this->current = exposure;
      float *history = this->samples;
      history[0] = exposure;
      _blkmov(history + 1, history, 236);
  }

  float get_min_historic_exposure(float seconds);
  float get_max_historic_exposure(float seconds);
};

struct structure_camera_fx_palette_entry {
    string_id name;
    s_tag_reference camera_fx;
    unsigned char flags;
    float forced_exposure;
    float forced_auto_exposure_screen_brightness;
    float minimum;
    float maximum;
    float bloom_threshold;
    float bloom_brightness;
};

typedef char structure_camera_fx_palette_entry_flags_offset[(offsetof(structure_camera_fx_palette_entry, flags) == 20) ? 1 : -1];
typedef char structure_camera_fx_palette_entry_size[(sizeof(structure_camera_fx_palette_entry) == 0x30) ? 1 : -1];

class c_camera_fx_values : public c_exposure {
public:
    float exposure_boost;
    float exposure_sensitivity, white_point, bloom_intensity, bloom_threshold, bloom_brightness;
    float parameter144, parameter160, parameter176;
    unsigned short parameter192;
    unsigned short reserved302;
    real_rgb_color color_balance, color_contrast, color_saturation;
    float exposure_offset, exposure_scale, exposure_result;

    static float g_HDR_target_stops;

    void set(const c_camera_fx_settings *settings);

    const c_camera_fx_settings::s_real_exposure_parameter *update(
        const c_camera_fx_settings *base, const structure_camera_fx_palette_entry *palette,
        const c_camera_fx_settings *scenario_settings, const c_camera_fx_settings *game_settings, float value);
};

#endif // RENDER_CAMERA_FX_VALUES_H
