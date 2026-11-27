// networkinglibxbox/network_http_request_cache.obj: from the records 0x407d58
// @flags /TP /O2 /Oc
// @address 0x407d58 ?network_http_request_cache_add_file_to_cache@@YAXPBVc_network_http_request_description@@PBXJPAUs_network_http_request_hash@@@Z
#include <xtl.h>
#include "cseries/crt_xdk.h"
#include <string.h>
#include "networking/logic/storage/network_http_request_cache.h"
#include "memory/static_arrays.h"

#include "networking/network_time_types.h"
#include "networking/logic/storage/network_http_request_hash.h"
s_network_request_cache_globals g_network_request_cache_globals;
// @address 0x8c447c g_network_time_globals
class c_network_http_request_description;

void network_http_request_cache_add_file_to_cache(
    const c_network_http_request_description *description, const void *data, long size, s_network_http_request_hash *hash)
{
  const char *name = (const char *)description;
  s_network_request_cache_entry *slot = 0;
  s_network_request_cache_entry *entry = g_network_request_cache_globals.entries;
  unsigned long offset = 0;
  do {
    if (entry->name.is_empty()) {
      slot = entry;
    } else if (strncmp(entry->name, name, 256) == 0) {
      slot = entry;
      break;
    }
    offset += sizeof(s_network_request_cache_entry);
    entry++;
  } while (offset < sizeof(g_network_request_cache_globals.entries));
  if (!slot)
    slot = &g_network_request_cache_globals.entries[g_network_request_cache_globals.first_entry_time & 31];
  slot->name.set(name);
  int hashStateStorage[24];
  XHashMemoryBegin((XHASH_STATE *)hashStateStorage);
  XHashMemoryUpdate((XHASH_STATE *)hashStateStorage, data, (unsigned long)size);
  XHashMemoryEnd((XHASH_STATE *)hashStateStorage, slot->digest);
  if (g_network_request_cache_globals.first_entry_time == 0) {
    int now = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    g_network_request_cache_globals.first_entry_time = now;
  }
}
