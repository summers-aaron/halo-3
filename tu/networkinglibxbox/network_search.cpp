// networkinglibxbox/network_search.obj: from the records 0x402b40 0x402bc0 0x402d88
// @flags /TP /O2
// @address 0x402b40 ?network_search_session@@YAPAUs_available_session@@J@Z
// @address 0x402bc0 network_search_begin
// @address 0x402d88 ?network_search_end@@YAXXZ
// @address 0x80608 GetTickCount
// @address 0x403278 ?release_storage@c_session_tracker@@QAAXXZ
#include "cseries/cseries_types.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/logic/logic_session_tracker.h"
#include "networking/logic/session_tracker_class.h"
#include <string.h>
#include <stdlib.h>
#include "networking/logic/network_search.h"
#include "networking/logic/network_recruiting_search.h"
#include "networking/session_tracker_types.h"
#include "networking/transport/transport_security.h"
#include "cseries/crt_xdk.h"
#include "shell/shell_types.h"
#include "networking/network_time_types.h"

#include "networking/logic/network_recruiting_search_internals.h"
#include "networking/logic/network_search_types.h"
#include "memory/data.h"
#include "networking/logic/network_broadcast_search_types.h"
#include "networking/logic/network_broadcast_search.h"
#include "networking/transport/transport_globals.h"
#if !defined(_XBOX) || defined(__clang__)
#include "networking/logic/network_search_globals.h"
#endif
// @address 0x80a2e4 g_network_search_globals
#if defined(_XBOX) && !defined(__clang__)
static s_available_squad_globals g_network_search_globals;
#else
// The port links the search globals by name (networking/logic/network_search_globals.h, C linkage): the game
// browser units read the squads this unit finds.
s_available_squad_globals g_network_search_globals;
#endif
typedef s_available_session session;



#include "networking/online/voice_interface_types.h"
#include "networking/online/online_url_name_globals.h"
#include "networking/transport/transport_qos_globals.h"
#include "networking/logic/network_bandwidth_globals.h"
#include "networking/online/online_lsp_xenon.h"

struct s_search_globals
{
    s_transport_globals transport;
    s_voice_interface_globals voice;
    __declspec(align(8)) s_online_url_name_globals url;
    s_transport_qos_globals qos;
    s_bandwidth_globals bandwidth;
    s_available_squad_globals squads;
    s_online_lsp_globals lsp;
    s_network_broadcast_search_globals search;
};
typedef char network_search_voice_offset[(sizeof(void *) != 4 || ((offsetof(s_search_globals, voice) == 0x88))) ? 1 : -1];
typedef char network_search_url_offset[(sizeof(void *) != 4 || ((offsetof(s_search_globals, url) == 0x448))) ? 1 : -1];
typedef char network_search_qos_offset[(sizeof(void *) != 4 || ((offsetof(s_search_globals, qos) == 0x4c8))) ? 1 : -1];
typedef char network_search_bandwidth_offset[(sizeof(void *) != 4 || ((offsetof(s_search_globals, bandwidth) == 0x4d0))) ? 1 : -1];
typedef char network_search_lsp_offset[(sizeof(void *) != 4 || ((offsetof(s_search_globals, lsp) == 0x510))) ? 1 : -1];
typedef char network_search_squads_offset[(sizeof(void *) != 4 || ((offsetof(s_search_globals, squads) == 0x4f4))) ? 1 : -1];
typedef char network_search_broadcast_offset[(sizeof(void *) != 4 || ((offsetof(s_search_globals, search) == 0x5a0))) ? 1 : -1];

#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"

bool network_search_initialize()
{
  memset(&g_network_search_globals, 0, sizeof(g_network_search_globals));
  return true;
}

struct s_network_recruiting_search_globals;
// The Xbox build lays transport, voice, url, qos, bandwidth, squads, lsp and broadcast search end to end from
// transport_startup_state, and this unit reaches the squads (+0x4f4) and broadcast search (+0x5a0) through that base.
// The PC link places each global on its own, so the port names transport_startup_state, g_network_search_globals and
// g_broadcast_search_globals directly.
#define network_search_globals (*(s_search_globals *)&transport_startup_state)

static __forceinline bool slot_active(const s_available_session *slot)
{
  return slot->active != 0 && 1;
}
static __forceinline bool slot_ready(const s_available_session *slot)
{
  return slot_active(slot) && slot->status.flags == 0;
}

static __forceinline bool network_search_physical_link_available()
{
#if defined(__clang__)
  bool result = transport_startup_state.initialized && transport_startup_state.started;
#else
  bool result = network_search_globals.transport.initialized && network_search_globals.transport.started;
#endif
  return result;
}

static __forceinline bool network_search_broadcast_search_begin(long controller,long maximum,session *storage)
{
#if defined(__clang__)
  s_network_broadcast_search_globals &g = g_broadcast_search_globals;
#else
  s_network_broadcast_search_globals &g = network_search_globals.search;
#endif
  if (!g.searching)
  {
    if (network_search_physical_link_available())
    {
      g.searching = true;
      g.changed = false;
      g.last_search_time = 0;
      g.search_nonce = transport_secure_nonce_generate();
      g.session_count = maximum;
      g.sessions = storage;
      memset(g.sessions, 0, sizeof(session) * g.session_count);
    }
  }
  return g.searching;
}

static __forceinline bool network_search_recruiting_search_begin(long controller,long flags,long maximum,session *storage)
{
  s_network_recruiting_search_globals &g = g_recruiting_search_globals;
  if (!g.enabled)
  {
    bool success = false;
    bool unused = false;
    g.matchmaking_quality.initialize();
    success = g.seeker.search_begin( (e_controller_index)controller, flags,
        unused, &g.matchmaking_quality);
    if (success)
    {
      g.enabled = true;
      g.dirty = false;
      g.count = maximum;
      g.entries = storage;
      memset(g.entries, 0, sizeof(session) * g.count);
    }
  }
  return g.enabled;
}

s_available_session *network_search_session(long index)
{
  s_available_session *result = 0;
  if (index >= 0 && index < g_network_search_globals.count) {
    s_available_session *slot = &g_network_search_globals.slots[index];
    if (slot_ready(slot))
      result = slot;
  }
  return result;
}

extern "C" bool network_search_begin(long controller)
{
  bool result = false;
#if defined(__clang__)
  s_available_squad_globals &g = g_network_search_globals;
#else
  s_available_squad_globals &g = network_search_globals.squads;
#endif
  if (!g.slots)
    g.slots = (session *)((c_allocation_base *)g.allocator)->allocate(sizeof(session) * g.maximum, "session_array");
  if (g.slots)
  {
    g.count = g.maximum;
    switch (g.active)
    {
    case 0: result = network_search_broadcast_search_begin(controller, g.count, g.slots); break;
    case 1: result = network_search_recruiting_search_begin(controller, g.flags, g.count, g.slots); break;
    case 2: result = true; break;
    // match: 0x402bc0: without it the switch gains a cmplwi 3/bge range check before the jump table
    default: __assume(0);
    }
  }
  return result;
}

// @address 0x8c447c g_network_time_globals

static __forceinline void network_search_recruiting_search_end()
{
  s_network_recruiting_search_globals &g = g_recruiting_search_globals;
  long start = g.seeker.start_time;
  long now = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
  g.matchmaking_quality.m_search_time = (now - start) / 1000;
  // match: 0x402d88: direct flag-byte access preserves the elapsed-time/flag-test schedule.
  if (g.seeker.flags.m_storage & 4)
    g.seeker.flags.m_storage &= ~4;
  ((c_session_tracker *)&session_tracker_globals)->release_storage();
  g.seeker.flags.m_storage &= ~1;
  // match: 0x402d88: spelled `= false`, the zero is hoisted into nonvolatile r28 and the frame grows 112->128B
  g.enabled = (bool)(_rotl(g.seeker.flags.m_storage & ~1u, 32) & 1);
}

void network_search_end()
{
  s_available_squad_globals &g = g_network_search_globals;
  switch (g.active)
  {
  case 0: network_broadcast_search_end(); break;
  case 1: network_search_recruiting_search_end(); break;
  }
  if (g.slots)
  {
    ((c_allocation_base *)g.allocator)->deallocate(g.slots);
    g.slots = 0;
    g.count = 0;
  }
}
