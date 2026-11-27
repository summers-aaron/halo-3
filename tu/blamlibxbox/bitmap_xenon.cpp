// blamlibxbox/bitmap_xenon.obj: from the records 0x15fd00
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "bitmaps/bitmap_data.h"
#include "tag_files/tag_groups.h"
#include "bitmaps/bitmap_xenon_cache_builder.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "bitmaps/bitmap_group.h"
#include "bitmaps/bitmap_group_definitions.h"
// @address 0x15fd00 bitmap_group_2d_get_real_pixel

static __forceinline const bitmap_data *bitmap_xenon_get_bitmap(long group_index,short index){
 const bitmap_group *group=TAG_GET('bitm', bitmap_group, group_index);
 const bitmap_data *result=0;
 if(group && index>=0 && index<group->bitmaps.count) result=TAG_BLOCK_GET_ELEMENT(&group->bitmaps, index, const bitmap_data);
 return result;
}
static __forceinline bool bitmap_xenon_get_hardware_format(long group_index,long index,c_rasterizer_texture_ref *texture,const bitmap_data **bitmap){
 const bitmap_group *group=TAG_GET('bitm', bitmap_group, group_index);
 bool success=false;
 if (TEST_BIT(group->flags, bitmap_group::_using_interop_resource_bit))
     success = ((bool)::bitmap_group_get_xenon_hardware_format(
         group_index, index, texture, bitmap, (__int64 *)(false)));
 else
 {
     const bitmap_data *data = bitmap_xenon_get_bitmap(group_index, (short)index);
     if (data)
     {
         *texture = c_rasterizer_texture_ref(data->texture_ref);
         *bitmap = data;
         success = true;
     }
 }
 return success;
}
extern "C" real_argb_color *bitmap_group_2d_get_real_pixel(
    // match: 0x15fd00: pixel copy into *result loads all words before storing; without __restrict c2 interleaves each load with its store
    real_argb_color *__restrict result, long group_index, long bitmap_index, const real_point2d *point, long mip, long layer, bool linear)
{
 c_rasterizer_texture_ref hardware_format;
 const bitmap_data *bitmap;
 bool success=bitmap_xenon_get_hardware_format(group_index,bitmap_index,&hardware_format,&bitmap);
 void *texture=hardware_format.get_d3d_texture();
 if(texture){*result=bitmap_2d_xenon_get_real_pixel(bitmap,texture,point,mip,layer,linear);}
 else {result->red=0.5f;result->green=0.5f;result->blue=0.5f;result->alpha=1.0f;}
 return result;
}
