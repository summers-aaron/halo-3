// objectslibxbox/objects_profile.obj: from the records 0xe2220
// @flags /TP /O2 /Oc

#include "animations/animation_data.h"

// @address 0xe2220 ?valid@c_animation_data@@QBA_NXZ
bool c_animation_data::valid() const
{
    return m_data && m_sizes && m_frame_count > 0 && total_size() > 0;
}
