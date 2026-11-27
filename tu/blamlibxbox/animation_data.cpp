// blamlibxbox/animation_data.obj: from the records 0xe22a8 0x2fd070
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "animations/animation_data.h"
#include "math/real_math_constants.h"
// @address 0x3aa90 k_real_math_origin3d


// @address 0xe22a8 ?get_pill_offset_for_frame@c_animation_data@@QBAXJPATreal_point3d@@PATreal_vector3d@@@Z
// match: 0xe22a8: __restrict on position_delta lets the first origin word load ahead of the delta store.
void c_animation_data::get_pill_offset_for_frame(long frame_index,real_point3d *position,real_vector3d *__restrict position_delta) const
{
 int frame_mode;
 const c_animation_data_sizes *frame_sizes;
 const float *frame;
 short frame_index_short;
 position_delta->i=0.0f;
 *position=k_real_math_origin3d;
 frame_mode=m_movement_data_type;
 if(frame_mode!=1) {
  if(frame_mode!=2) {
   if(frame_mode!=3) return;
   frame_sizes=m_sizes;
   frame =
       (const float *)((((((((short)frame_index) << 4) + frame_sizes->m_default_data_size) + frame_sizes->m_animated_node_flags_data_size) +
                            frame_sizes->m_compressed_data_size) +
                           frame_sizes->m_static_node_flags_data_size) +
           m_data);
   position->x=frame[0];
   position->y=frame[1];
   position->z=frame[2];
   position_delta->i=frame[3];
   return;
  }
  frame_sizes=m_sizes;
  frame_index_short=(short)frame_index;
  frame = (const float *)((((((frame_index_short + (frame_index_short << 1)) << 2) + frame_sizes->m_default_data_size) +
                               frame_sizes->m_animated_node_flags_data_size) +
                              frame_sizes->m_compressed_data_size + frame_sizes->m_static_node_flags_data_size) +
      m_data);
  position->x=frame[0];
  position->y=frame[1];
  position_delta->i=frame[2];
  return;
 }
 frame_sizes=m_sizes;
 frame =
     (const float *)((((((((short)frame_index) << 3) + frame_sizes->m_default_data_size) + frame_sizes->m_animated_node_flags_data_size) +
                          frame_sizes->m_compressed_data_size) +
                         frame_sizes->m_static_node_flags_data_size) +
         m_data);
 position->x=frame[0];
 position->y=frame[1];
}

// @address 0x2fd070 animation_data_merge_flags
void animation_data_merge_flags(const c_animation_data *buffer, unsigned char *destination)
{
    if (buffer->m_sizes->m_animated_node_flags_data_size != 0) {
        const unsigned char *source = buffer->m_data + buffer->m_sizes->get_animated_node_flags_data_offset();
        int count = buffer->m_sizes->m_animated_node_flags_data_size / 3;
        for (int i = 0; i < count; ++i) {
            destination[i] |= source[i];
            destination[i] |= source[count + i];
            destination[i] |= source[2 * count + i];
        }
    }
}
