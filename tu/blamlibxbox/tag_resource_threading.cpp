// blamlibxbox/tag_resource_threading.obj: from the records 0x88df8
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include <xtl.h>
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "multithreading/read_write_lock.h"

#include "rasterizer/rasterizer.h"
#include "multithreading/thread_tls.h"
#include "multithreading/synchronized_value.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "cache/geometry_storage.h"

// @address 0x88df8 ?unlock_access_for_current_thread_internal@c_tag_resource_thread_access@@AAAPAUs_resource_access_cookie@@_N@Z
s_resource_access_cookie *c_tag_resource_thread_access::unlock_access_for_current_thread_internal(bool allocate_event)
{
 long thread_index=get_current_thread_index();
 c_tag_resource_cache_file_access_cache *access_cache=m_per_thread_access_cache[thread_index];
 void *result=access_cache;
 m_per_thread_acquired_access_cache[thread_index]=0;
 m_per_thread_access_cache[thread_index]=0;
 m_resource_thread_access_lock->release_access_cache_access(access_cache,allocate_event);
 m_access_lock.read_unlock();
 return (s_resource_access_cookie *)result;
}
