// networkinglibxbox/transport_address.obj: from the records 0x3dd1c0 0x3dd230
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x3dd1c0 ?transport_address_valid@@YA_NPBUtransport_address@@@Z
// @address 0x3dd230 ?transport_address_equivalent@@YA_NPBUtransport_address@@0_N@Z
#include <string.h>
#include "networking/transport/transport_address.h"

// 0x3dd1c0
bool transport_address_valid(const transport_address *address)
{
  bool result = false;
  if (address) {
    short type = address->address_length;
    if (type == -1 || type == 4) {
      result = address->address.ipv4_address != 0;
    } else if (type == 16) {
      for (int i = 0; i < 8; i++) {
        if (address->address.ipv6_address[i] != 0) {
          result = true;
          break;
        }
      }
    }
  }
  return result;
}

// 0x3dd230
bool transport_address_equivalent(const transport_address *a, const transport_address *b, bool include_port)
{
  short length_a = a->address_length;
  short length_b = b->address_length;
  short length = (length_a > length_b) ? length_b : length_a;
  if (length_a > 0 && length_a == length_b && memcmp(a, b, length) == 0)
    return true;
  return false;
}
