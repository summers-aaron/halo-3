// blamlibxbox/havok_contact_points.obj: from the records 0xe7880 0xe7990
// @flags /TP /O2
// @address 0xe7880 ?next@c_havok_contact_point_iterator@@QAAXXZ
// @address 0xe7990 ?valid@c_havok_contact_point_iterator@@QAA_NXZ
#include "objects/object_header.h"
#include "memory/data.h"
#include "cseries/crt_xdk.h"

#define TEST_BIT16(flags, bit) (_rotl((flags), 32 - (bit)) & 1)
#include "physics/havok_contact_points.h"
#include "physics/havok_contact_point_reference.h"
#include "physics/havok_contact_point_iterator.h"
#include "physics/havok_contact_point.h"
#include "physics/havok_component.h"
#include "physics/havok_component_class.h"

// @address 0x33794 ?k_invalid_contact_point_reference@@3Us_havok_contact_point_reference@@B
// @rdata 0x82033794..0x82033798
const s_havok_contact_point_reference k_invalid_contact_point_reference = { -1, -1 };
// @rdata-end

// Advance through each rigid body, skipping contact points with flag bit 1 unless unfiltered.
void c_havok_contact_point_iterator::next()
{
  object_datum *object = object_get(object_index);
  bool again;
  do {
    again = false;
    contact_point_index++;
    if (object->havok_component_index != -1) {
      c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
      if (rigid_body_index >= 0 && rigid_body_index < component->m_rigid_bodies.m_size) {
        if (contact_point_index >= 0 && contact_point_index < component->m_rigid_bodies.m_data[rigid_body_index].m_contact_points.m_size) {
          if (unfiltered)
            return;
          bool skip = TEST_BIT16(component->get_contact_point(rigid_body_index, contact_point_index)->m_life_cycle_flags, 1);
          if (!skip)
            return;
          again = true;
        } else {
          contact_point_index = -1;
          rigid_body_index++;
          again = true;
        }
      }
    }
  } while (again);
}

// Check both indices against the object's Havok component and rigid body.
bool c_havok_contact_point_iterator::valid()
{
  bool result = false;
  object_datum *object = object_get(object_index);
  long havok_component_index = object->havok_component_index;
  if (havok_component_index != -1) {
    c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, havok_component_index);
    if (rigid_body_index >= 0 && rigid_body_index < component->m_rigid_bodies.m_size &&
        contact_point_index >= 0 && contact_point_index < component->m_rigid_bodies.m_data[rigid_body_index].m_contact_points.m_size)
      result = true;
  }
  return result;
}
