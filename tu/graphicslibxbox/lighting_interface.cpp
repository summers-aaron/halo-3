// graphicslibxbox/lighting_interface.obj: from the records 0x1799d0 0x179f40 0x17a270 0x45c1f0 0x45c340
// @flags /TP /O2 /Oc
#include <xtl.h>
#include <xboxmath.h>
#include "rasterizer/rasterizer.h"
#include <math.h>
#include <string.h>
#include "math/real_math.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include "objects/objects.h"
#include "geometry/geometry_sampling.h"
#include "geometry/geometry_sampling_types.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "math/spherical_harmonics.h"
#include "units/vehicle_type_component.h"
#include "models/render_model_definitions.h"
#include "render/lighting_interface.h"
#include "visibility/visibility_collection.h"

extern "C" {
// @address 0x8c483c g_lighting_debug_mode
long g_lighting_debug_mode;
}

struct s_debug_sh_probe
{
    float red[36];
    float green[36];
    float blue[36];
};

// @address 0x730e88 g_lighting_debug_sh_probes
static s_debug_sh_probe g_lighting_debug_sh_probes[5] = {
    {
        {
            10.054f, -5.078f, -4.346f, 3.762f, -1.428f, 3.344f, -2.053f, -7.145f, 2.725f, 
            0.526f, -0.269f, -0.699f, 3.947f, 2.369f, -1.951f, 1.04f, 2.14f, -2.681f, 
            -0.56f, -1.879f, -4.494f, 3.65f, -1.644f, -4.217f, 2.874f, -0.787f, -0.302f, 
            5.176f, -0.545f, 2.635f, -0.185f, -3.433f, -0.548f, 2.154f, -3.474f, 1.607f
        },
        {
            5.571f, -4.507f, -2.328f, 0.717f, -0.656f, 2.859f, -1.244f, -2.731f, -0.704f, 
            0.772f, 0.337f, -0.997f, 2.016f, 1.397f, 1.79f, -1.209f, 2.087f, -2.657f, 
            -0.627f, -1.515f, -2.696f, 1.209f, -2.362f, -1.028f, 1.079f, -0.243f, -0.496f, 
            3.942f, 0.048f, 2.708f, 0.509f, -1.225f, -1.172f, -0.045f, -1.131f, 0.528f
        },
        {
            6.904f, -7.781f, -3.446f, -0.12f, -1.575f, 6.082f, -1.93f, -1.763f, -3.915f, 
            2.019f, 1.522f, -2.087f, 2.788f, 1.281f, 6.165f, -3.458f, 3.514f, -4.71f, 
            -1.62f, -1.825f, -3.667f, 0.86f, -4.974f, 0.403f, 1.637f, -1.133f, -0.193f, 
            5.508f, 0.722f, 5.667f, 1.941f, -1.042f, -0.376f, -1.849f, -1.997f, 1.278f
        }
    },
    {
        {
            3.629f, -1.761f, -0.246f, -0.349f, 0.201f, 0.469f, -0.9f, -0.045f, -0.821f, 
            0.367f, -0.013f, 0.333f, -0.624f, 0.087f, 1.336f, -0.171f, 0.303f, -1.574f, 
            0.047f, 0.634f, 0.111f, 0.326f, -0.693f, -0.411f, 0.608f, -1.078f, 0.629f, 
            0.999f, -0.203f, 0.098f, 0.756f, 0.005f, -0.437f, 0.101f, -1.22f, 0.173f
        },
        {
            2.629f, -1.437f, -0.1f, -0.225f, 0.146f, 0.254f, -0.762f, -0.034f, -0.322f, 
            -0.054f, -0.002f, 0.229f, -0.71f, 0.026f, 1.034f, -0.074f, 0.159f, -1.23f, 
            0.06f, 0.675f, 0.241f, 0.244f, -0.653f, -0.259f, 0.424f, -0.983f, 0.407f, 
            0.845f, -0.172f, -0.065f, 0.722f, 0.016f, -0.393f, 0.051f, -0.796f, 0.119f
        },
        {
            2.33f, -1.26f, -0.01f, -0.102f, 0.045f, 0.118f, -0.741f, 0.004f, 0.034f, 
            -0.341f, -0.03f, 0.204f, -0.816f, -0.056f, 0.92f, -0.041f, 0.058f, -1.076f, 
            0.108f, 0.717f, 0.343f, 0.16f, -0.692f, -0.203f, 0.403f, -1.1f, 0.295f, 
            0.813f, -0.1f, -0.151f, 0.745f, 0.056f, -0.447f, 0.022f, -0.497f, 0.077f
        }
    },
    {
        {
            0.1172485f, 0.1326904f, 0.03424072f, 0.1257324f, 0.1781006f, 0.05447388f, -0.1148071f, 0.05038452f
        },
        {
            0.07800293f, 0.08837891f, 0.02267456f, 0.08203125f, 0.1162109f, 0.03631592f, -0.07623291f, 0.03302002f
        },
        {
            0.03164673f, 0.03659058f, 0.009346008f, 0.02996826f, 0.04232788f, 0.01521301f, -0.03086853f, 0.01217651f
        }
    },
    {
        {
            0.01649475f, 0.02270508f, 0.005523682f, -0.003595352f, -0.005626678f, 0.0102005f, -0.01600647f, -0.001280785f
        },
        {
            0.01182556f, 0.01625061f, 0.003940582f, -0.002706528f, 0.007263184f, 0.03631592f, -0.01148224f, -0.0009717941f
        },
        {
            0.006847382f, 0.009391785f, 0.002271652f, -0.001643181f, -0.002573013f, 0.004180908f, -0.006660461f, -0.0005927086f
        }
    },
    {
        {
            0.14355469f, 0.16125488f, 0.061431885f, 0.1373291f, 0.19604492f, 0.08795166f, -0.11938477f, 0.057922363f
        },
        {
            0.094055176f, 0.10766602f, 0.03967285f, 0.089538574f, 0.12683105f, 0.058532715f, -0.07904053f, 0.03717041f
        },
        {
            0.039855957f, 0.04663086f, 0.01777649f, 0.033691406f, 0.047546387f, 0.026824951f, -0.03213501f, 0.01398468f
        }
    }
};

#include "visibility/visible_items_types.h"

// @address 0x1799d0 ?calculate_and_set_ravi_constants@c_lighting_interface@@SAXPBM00M@Z
void c_lighting_interface::calculate_and_set_ravi_constants(const float *red,const float *green,const float *blue,float scale) {
 if (g_lighting_debug_mode == _lighting_debug_disabled) {
  float local_red[9];
  float local_green[9];
  float local_blue[9];
  memcpy(local_red, red, sizeof(float) * 9);
  memcpy(local_green, green, sizeof(float) * 9);
  memcpy(local_blue, blue, sizeof(float) * 9);
  calculate_and_set_ravi_constants_internal(local_red, local_green, local_blue, scale);
 } else {
  float sh_red[16];
  float sh_green[16];
  float sh_blue[16];
  real_vector3d direction;
  switch (g_lighting_debug_mode) {
  case _lighting_debug_positive_x:
   set_real_vector3d(&direction, 1.0f, 0.0f, 0.0f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_negative_x:
   set_real_vector3d(&direction, -1.0f, 0.0f, 0.0f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_positive_y:
   set_real_vector3d(&direction, 0.0f, 1.0f, 0.0f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_negative_y:
   set_real_vector3d(&direction, 0.0f, -1.0f, 0.0f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_positive_z:
   set_real_vector3d(&direction, 0.0f, 0.0f, 1.0f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_negative_z:
   set_real_vector3d(&direction, 0.0f, 0.0f, -1.0f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_positive_xy:
   set_real_vector3d(&direction, 0.707f, 0.707f, 0.0f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_negative_xy:
   set_real_vector3d(&direction, -0.707f, -0.707f, 0.0f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_positive_xz:
   set_real_vector3d(&direction, 0.707f, 0.0f, 0.707f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_negative_xz:
   set_real_vector3d(&direction, -0.707f, 0.0f, -0.707f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_positive_yz:
   set_real_vector3d(&direction, 0.0f, 0.707f, 0.707f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_negative_yz:
   set_real_vector3d(&direction, 0.0f, -0.707f, -0.707f);
   sh_eval_directional_light(3, &direction, 1.0f, 1.0f, 1.0f, sh_red, sh_green, sh_blue);
   calculate_and_set_ravi_constants_internal(sh_red, sh_green, sh_blue, scale);
   break;
  case _lighting_debug_probe_0:
   calculate_and_set_ravi_constants_internal(g_lighting_debug_sh_probes[0].red, g_lighting_debug_sh_probes[0].green, g_lighting_debug_sh_probes[0].blue, scale);
   break;
  case _lighting_debug_probe_1:
   calculate_and_set_ravi_constants_internal(g_lighting_debug_sh_probes[1].red, g_lighting_debug_sh_probes[1].green, g_lighting_debug_sh_probes[1].blue, scale);
   break;
  case _lighting_debug_probe_2:
   calculate_and_set_ravi_constants_internal(g_lighting_debug_sh_probes[2].red, g_lighting_debug_sh_probes[2].green, g_lighting_debug_sh_probes[2].blue, scale);
   break;
  case _lighting_debug_probe_3:
   calculate_and_set_ravi_constants_internal(g_lighting_debug_sh_probes[3].red, g_lighting_debug_sh_probes[3].green, g_lighting_debug_sh_probes[3].blue, scale);
   break;
  case _lighting_debug_probe_4:
   calculate_and_set_ravi_constants_internal(g_lighting_debug_sh_probes[4].red, g_lighting_debug_sh_probes[4].green, g_lighting_debug_sh_probes[4].blue, scale);
   break;
  }
 }
}

// @address 0x119aff4 ?sqrt_pi@?1??calculate_and_set_ravi_constants_internal@c_lighting_interface@@CAXPBM00M@Z@4MA
// @address 0x119aff8 ?$S1@?1??calculate_and_set_ravi_constants_internal@c_lighting_interface@@CAXPBM00M@Z@4IA
// @address 0x179f40 ?calculate_and_set_ravi_constants_internal@c_lighting_interface@@CAXPBM00M@Z
void c_lighting_interface::calculate_and_set_ravi_constants_internal(const float *red,const float *green,const float *blue,float scale) {
 float root3=sqrt(3.0);
 static float sqrt_pi = sqrt(k_pi);
 XMFLOAT4 constants[10];
 constants[0]=XMFLOAT4(red[0],green[0],blue[0],0.0f);
 constants[1]=XMFLOAT4(red[3],red[1],-red[2],0.0f);
 constants[2]=XMFLOAT4(green[3],green[1],-green[2],0.0f);
 constants[3]=XMFLOAT4(blue[3],blue[1],-blue[2],0.0f);
 constants[4]=XMFLOAT4(-red[4],red[5],red[7],0.0f);
 constants[5]=XMFLOAT4(-green[4],green[5],green[7],0.0f);
 constants[6]=XMFLOAT4(-blue[4],blue[5],blue[7],0.0f);
 constants[7]=XMFLOAT4(-red[8],red[8],-red[6]*root3,red[6]*root3);
 constants[8]=XMFLOAT4(-green[8],green[8],-green[6]*root3,green[6]*root3);
 constants[9]=XMFLOAT4(-blue[8],blue[8],-blue[6]*root3,blue[6]*root3);
 for (long i = 0; i < 10; i++)
 {
     constants[i].x = scale * constants[i].x;
     constants[i].y = scale * constants[i].y;
     constants[i].z = constants[i].z * scale;
     constants[i].w = constants[i].w * scale;
 }
 c_rasterizer::get_device()->SetPixelShaderConstantF(1, (const float *)constants, 10);
 c_rasterizer::get_device()->SetVertexShaderConstantF(240, (const float *)constants, 10);
}

// @address 0x17a270 ?set_object_dominant_light_direction_and_intensity@c_lighting_interface@@SAXPBTreal_vector3d@@PBTreal_rgb_color@@@Z
void c_lighting_interface::set_object_dominant_light_direction_and_intensity(const real_vector3d *direction,const real_rgb_color *intensity) {
 real_vector4d constant;
 constant.x=direction->i; constant.y=direction->j; constant.z=direction->k; constant.w=1.0f;
 c_rasterizer::get_device()->SetPixelShaderConstantF(11,(const float *)&constant,1);
 constant.x=intensity->red; constant.y=intensity->green; constant.z=intensity->blue; constant.w=1.0f;
 c_rasterizer::get_device()->SetPixelShaderConstantF(13,(const float *)&constant,1);
}

// @address 0x45c340 lighting_interface_get_visible_sky_lighting
extern "C" unsigned char lighting_interface_get_visible_sky_lighting(long *dst0, long *dst1, long *dst2, real_vector3d *dir, real_rgb_color *intensity)
{
    bool result = false;
    if (c_visible_items::m_items.skies.count != 0) {
        long sky_index = c_visible_items::m_items.skies.entries[0];
        if (sky_index != -1) {
            void *obj = object_try_and_get_and_verify_type(sky_index, -1);
            if (obj != 0) {
                long object_definition_index = ((const object_datum *)obj)->definition_index;
                if (object_definition_index != -1) {
                    const void *object_definition = tag_get('obje', object_definition_index);
                    long model_index = ((const s_object_definition *)object_definition)->model.index;
                    if (model_index != -1) {
                        const void *model = tag_get('hlmt', model_index);
                        long render_model_index = ((const s_model_definition *)model)->render_model_index;
                        if (render_model_index != -1) {
                            const render_model_definition *def3 =
                                (const render_model_definition *)tag_get('mode', render_model_index);
                            memcpy(dst0, def3->ravi_red, 9 * sizeof(long));
                            memcpy(dst1, def3->ravi_green, 9 * sizeof(long));
                            memcpy(dst2, def3->ravi_blue, 9 * sizeof(long));
                            float out_value;
                            calculate_dominant_light_from_lightprobe(def3->ravi_red, def3->ravi_green, def3->ravi_blue, dir, intensity, &out_value);
                            result = true;
                        }
                    }
                }
            }
        }
    }
    return result;
}
static __forceinline const real_vector3d *lighting_interface_get_default_direction()
{
    return global_down3d;
}
// @address 0x45c1f0 ?setup_default_lighting@c_lighting_interface@@SAXXZ
void c_lighting_interface::setup_default_lighting()
{
    real_vector3d light_direction;
    real_rgb_color light_intensity;
    float light_scale[2];
    bool lighting_set = false;

    if (c_visible_items::m_items.skies.count != 0)
    {
        long sky_index = c_visible_items::m_items.skies.entries[0];
        if (sky_index != -1)
        {
            long definition_index = OBJECT_GET(sky_index, object_datum, _object_mask_all)->definition_index;
            if (definition_index != -1)
            {
                const s_object_definition *object_definition =
                    (const s_object_definition *)tag_get('obje', definition_index);
                long address = object_definition->model.index;
                if (address != -1)
                {
                    const s_model_definition *model_definition =
                        (const s_model_definition *)tag_get('hlmt', address);
                    long group0 = model_definition->render_model_index;
                    if (group0 != -1)
                    {
                        const render_model_definition *definition =
                            (const render_model_definition *)tag_get('mode', group0);
                        calculate_and_set_ravi_constants(definition->ravi_red, definition->ravi_green, definition->ravi_blue, 1.0f);
                        calculate_dominant_light_from_lightprobe(definition->ravi_red, definition->ravi_green, definition->ravi_blue,
                            &light_direction, &light_intensity, light_scale);
                        set_object_dominant_light_direction_and_intensity(&light_direction, &light_intensity);
                        lighting_set = true;
                    }
                }
            }
        }
    }
    if (!lighting_set)
    {
        calculate_and_set_ravi_constants(c_geometry_sampler::m_default_lightprobe_sample.red, c_geometry_sampler::m_default_lightprobe_sample.green,
            c_geometry_sampler::m_default_lightprobe_sample.blue, 1.0f);
        const real_argb_color *white = objects_get_default_argb_white();
        const real_vector3d *down = lighting_interface_get_default_direction();
        set_object_dominant_light_direction_and_intensity(down, &white->rgb);
    }
}
