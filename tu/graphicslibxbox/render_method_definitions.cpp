// graphicslibxbox/render_method_definitions.obj: from the records 0x15f9b0 0x15fac0
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"

#include <math.h>
#include "render_methods/render_method_submit.h"
#include "render_methods/render_method_definitions.h"
#include "math/function_definitions.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "bitmaps/bitmap_group.h"
#include "cache/cache_file_globals.h"



#include "render_methods/render_method.h"
#include "cseries/cseries.h"
static unsigned long render_method_definitions_submit_group_tag(long tag_index)
{
    short salt = (short)(tag_index >> 16);
    long index = (short)(tag_index & 0xffff);
    const cache_file_tag_instance *instance = &global_tag_instances[index];
    const cache_file_tag_group *group = &g_cache_file_globals.tags_header->groups[instance->group_index];
    return group->group_tags[0];
}
bool c_render_method::get_is_decal(long index)
{
    bool result = index != -1 && render_method_definitions_submit_group_tag(index) == 'rmd ';
    return result;
}


// @address 0x15f9b0 ?sample_texture@s_render_method_postprocess_definition@@QBA_NW4e_runtime_queryable_property@1@Treal_point2d@@JPATreal_argb_color@@@Z
bool s_render_method_postprocess_definition::sample_texture(
    // match: 0x15f9b0: pointer __restrict qualifier prevents alias reload
    e_runtime_queryable_property property, real_point2d point, long sampler, real_argb_color *__restrict color) const
{
  bool result = false;
  long mipmap;
  long bitmap_index;
  {
    real_vector4d transform;
    if (query_texture_transform(property, &transform, &bitmap_index, &mipmap) && bitmap_index != -1) {
      point.x = transform.i * point.x + transform.k;
      point.y = transform.j * point.y + transform.l;
      bitmap_group *group = TAG_GET('bitm', bitmap_group, bitmap_index);
      long level = MIN(MAX(mipmap, 0), group->bitmaps.count - 1);
      *color = *bitmap_group_2d_get_real_pixel((real_argb_color *)&transform, bitmap_index, level, &point, 0, sampler, 0);
      result = true;
    }
  }
  return result;
}

// @address 0x15fac0 ?query_texture_transform@s_render_method_postprocess_definition@@QBA_NW4e_runtime_queryable_property@1@PATreal_vector4d@@PAJ2@Z
bool s_render_method_postprocess_definition::query_texture_transform(
    e_runtime_queryable_property property, real_vector4d *transform, long *bitmap_index, long *mipmap) const
{
    bool result = false;
    real_vector4d local = { 1.0f, 1.0f, 0.0f, 0.0f };
    short texture_index = query_constants[property];
    if (texture_index != -1)
    {
        const s_render_method_texture *t = (const s_render_method_texture *)tag_block_get_element_with_size(
            &textures, texture_index, sizeof(s_render_method_texture));
        if (bitmap_index != 0)
            *bitmap_index = t->bitmap.index;
        if (mipmap != 0)
            *mipmap = t->index;
        if (t->transform != 255)
        {
            local = *(const real_vector4d *)tag_block_get_element_with_size(&real_constants, t->transform, sizeof(real_vector4d));
            if ((t->functions >> 10) > 0)
            {
                for (long i = t->functions & 1023; i < (t->functions & 1023) + (t->functions >> 10); i++)
                {
                    const s_render_method_animated_parameter *p =
                        (const s_render_method_animated_parameter *)tag_block_get_element_with_size(
                            &animations, i, sizeof(s_render_method_animated_parameter));
                    float value = evaluate_function(-1, p);
                    switch (p->type)
                    {
                    case 2:
                        local.i = local.j = value;
                        break;
                    case 3:
                        local.i = value;
                        break;
                    case 4:
                        local.j = value;
                        break;
                    case 5:
                        local.k = value;
                        break;
                    case 6:
                        local.l = value;
                        break;
                    case 7:
                        if (mipmap != 0 && bitmap_index != 0)
                        {
                            const bitmap_group *group = TAG_GET('bitm', const bitmap_group, *bitmap_index);
                            *mipmap = (long)((float)floor((float)fmod(
                                (double)p->function.map_to_output_range_legacy(value),
                                (double)(float)group->bitmaps.count)) + 0.5f);
                        }
                        break;
                    }
                }
            }
        }
        if (transform != 0)
            *transform = local;
        result = true;
    }
    return result;
}
