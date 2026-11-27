// networkinglibxbox/network_arbitration.obj: from the records 0x402e88
// @flags /TP /O2 /Oc
// @address 0x402e88 ?network_arbitration_initiate@@YA_NJ_K@Z

#include "cseries/async_xoverlapped.h"
#include "networking/logic/network_arbitration.h"
#include "networking/session/network_managed_session.h"
#include "networking/logic/network_arbitration_globals_types.h"
#include "networking/online/online_arbitration_xenon.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/online/online_arbitration_types.h"

// @address 0x8c4490 ?network_arbitration_globals@@3Us_network_arbitration_globals@@A
s_network_arbitration_globals network_arbitration_globals;

__forceinline bool online_session_get_handle(long index,void **out) {
 bool result=false;
 if(index!=-1) {
  s_online_managed_session *session=&online_session_manager_globals.sessions[index];
  if(session->actual.handle && session->actual.handle!=(void*)-1) { *out=session->actual.handle; result=true; }
 }
 return result;
}

__forceinline bool online_arbitration_initiate_registration(void *handle,unsigned __int64 nonce) {
 c_arbitration_overlapped_task *registration=g_online_arbitration_globals.registration;
 registration->m_session=handle;
 registration->m_session_nonce=nonce;
 return overlapped_task_start_internal(g_online_arbitration_globals.registration,"..\\networking\\online\\online_arbitration_xenon.cpp",122);
}

bool network_arbitration_initiate(long index,unsigned __int64 nonce) {
 bool success=false;
 network_arbitration_globals.status=3;
 network_arbitration_globals.index=index;
 void *handle;
 if(online_session_get_handle(index,&handle) && online_arbitration_initiate_registration(handle,nonce)) {network_arbitration_globals.status=1;success=true;}
 return success;
}
