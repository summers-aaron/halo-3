// networkinglibxbox/network_web_events.obj: from the records 0x445178 0x445278
// @flags /TP /O2 /Oc
// @address 0x445178 network_web_event_report_local_cheater
// @address 0x445278 network_web_event_report_remote_cheater
#include <string.h>
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "networking/online/online_xenon.h"
#include "networking/online/online_lsp_service_type.h"
#include "networking/tools/network_web_events.h"
#include "networking/tools/network_webstats.h"
#include "networking/transport/transport_security.h"
#include "networking/logic/cheating_player_types.h"
#include "networking/logic/local_player_types.h"

#pragma pack(push,4)
struct cheating_event
{
    unsigned __int64 machine;
    long count;
    s_network_web_event_local_player players[4];
    cheating_player cheating;
};
struct remote_cheating_event
{
    unsigned __int64 machine;
    unsigned __int64 remote_machine;
    long count;
    s_network_web_event_local_player players[4];
    cheating_player cheating;
};
#pragma pack(pop)
struct web_event { unsigned long start; unsigned short version,flags; long build; unsigned long type; char payload[512]; unsigned long end; };

static __forceinline void network_web_event_build(web_event *event,unsigned long type,const void *payload,long length) {
 memset(event,0,sizeof(*event));
 event->start=0x626e7765;
 event->version=2;
 event->flags=0;
 event->build=11855;
 event->end=0x656e7765;
 event->type=type;
 memcpy(event->payload,payload,length);
}

extern "C" bool network_web_event_report_local_cheater(const cheating_player *cheating,
    const s_network_web_event_local_player *players, long count) {
 bool success=false;
 if(::online_local_xuid_is_online_enabled()) {
  cheating_event payload={0};
  payload.machine=transport_secure_address_get_local_machine_id();
  memcpy(&payload.cheating,cheating,sizeof(payload.cheating));
  payload.count=count;
  if(count>0) memcpy(payload.players,players,count*sizeof(s_network_web_event_local_player));
  web_event event;
  network_web_event_build(&event,0x6c636865,&payload,sizeof(payload));
  success=network_webstats_submit_buffer(online_lsp_service_webstats,&event,sizeof(event),"application/x-halo3event",-1);
 }
 return success;
}

extern "C" bool network_web_event_report_remote_cheater(unsigned __int64 remote_machine_id, const cheating_player *cheating,
    const s_network_web_event_local_player *players, long count)
{
    bool success = false;
    if (::online_local_xuid_is_online_enabled())
    {
        remote_cheating_event payload = {0};
        payload.machine = transport_secure_address_get_local_machine_id();
        payload.remote_machine = remote_machine_id;
        memcpy(&payload.cheating, cheating, sizeof(payload.cheating));
        payload.count = count;
        if (count > 0)
            memcpy(payload.players, players, count * sizeof(s_network_web_event_local_player));
        web_event event;
        network_web_event_build(&event, 0x72636865, &payload, sizeof(payload));
        success = network_webstats_submit_buffer(online_lsp_service_webstats, &event, sizeof(event), "application/x-halo3event", -1);
    }
    return success;
}
