// online_session_search_xenon.obj: from the records 0x3d2728 0x3d2738 0x3d27e0 0x3d27f0 0x3d2828 0x3d28f0 0x3d2988 0x3d29f8 0x3d2a68 0x3d2bb8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <string.h>
#include <stddef.h>
#include "cseries/cseries.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "networking/online/online_session_search_xenon.h"
#include "networking/online/online_session_search_types.h"
#include "networking/online/overlapped_allocation_marker_types.h"

#include "networking/online/online_search_parameters.h"


#include "networking/online/online_session_results.h"
#include "networking/online/online_session_results_types.h"

#include "cseries/async_xoverlapped_allocation.h"
#include "networking/online/online_session_search_tasks.h"

struct s_online_search_query_internal
{
    unsigned long query_id;
    long unknown;
    long context_count;
    s_online_context contexts[6];
    long property_count;
    XUSER_PROPERTY properties[20];
};
s_online_session_search_globals online_session_search_globals;

// @address 0x73a2ec k_online_session_search_query_id_map
unsigned long k_online_session_search_query_id_map[] =
{
    0x00000000,
    0x00000001,
    0x82046f74,
    0x82046f50,
    0x82046f30,
    0x82046f0c,
    0x82046ee4,
};

const char *c_session_search_overlapped_task::get_context_string() const
{
    return "session-search";
}
unsigned long c_session_search_overlapped_task::start(void *overlapped)
{
    m_results_size = sizeof(m_results_buffer);
    unsigned long result = XSessionSearchEx(m_procedure, m_user_index, 50, 1, (unsigned short)m_property_count,
        (unsigned short)m_context_count, m_properties, (XUSER_CONTEXT *)m_contexts,
        &m_results_size, (XSESSION_SEARCHRESULT_HEADER *)m_results_buffer, (XOVERLAPPED *)overlapped);
    return result;
}
const char *c_session_search_by_id_overlapped_task::get_context_string() const
{
    return "session-by-id-search";
}
unsigned long c_session_search_by_id_overlapped_task::start(void *overlapped)
{
    m_results_size = sizeof(m_results_buffer);
    unsigned long result = XSessionSearchByID(m_session_id, m_user_index, &m_results_size,
        (XSESSION_SEARCHRESULT_HEADER *)m_results_buffer, (XOVERLAPPED *)overlapped);
    return result;
}
void online_session_search_initialize()
{
    memset(&online_session_search_globals, 0, sizeof(online_session_search_globals));
    online_session_search_globals.search_task = new (0, 0, _overlapped_allocation_marker) c_session_search_overlapped_task(0, 0);
    online_session_search_globals.search_by_id_task = new (0, 0, _overlapped_allocation_marker) c_session_search_by_id_overlapped_task(0, 0);
    online_session_search_globals.initialized = true;
}

__forceinline void c_session_search_overlapped_task::initialize(const s_online_search_query_internal *parameters)
{
    memcpy(&m_procedure, parameters, sizeof(*parameters));
    m_results_size = 0;
}
bool online_session_search_initiate(const s_online_session_search_parameters *parameters)
{
    bool result = false;
    s_online_search_query_internal internal_parameters;
    internal_parameters.query_id = 0;
    memset(&internal_parameters.unknown, 0, sizeof(internal_parameters) - sizeof(internal_parameters.query_id));
    if (online_session_search_build_internal_search_parameters(parameters, &internal_parameters))
    {
        online_session_search_globals.search_task->initialize(&internal_parameters);
        result = overlapped_task_start_internal(online_session_search_globals.search_task, "..\\networking\\online\\online_session_search_xenon.cpp", 187);
    }
    return result;
}
e_online_session_search_status online_session_search_get_status()
{
    e_overlapped_task_state task_state = online_session_search_globals.search_task->get_task_state();
    e_online_session_search_status result = _online_session_search_status_none;
    switch (task_state)
    {
    case _overlapped_task_state_starting:
    case _overlapped_task_state_pending:
    case _overlapped_task_state_completing: result = _online_session_search_status_pending; break;
    case _overlapped_task_state_success: result = _online_session_search_status_success; break;
    case _overlapped_task_state_failure: result = _online_session_search_status_failure; break;
    case _overlapped_task_state_none: result = _online_session_search_status_none; break;
    default: result = _online_session_search_status_none; break;
    }
    return result;
}
e_online_session_search_status online_session_search_by_id_get_status()
{
    e_overlapped_task_state task_state = online_session_search_globals.search_by_id_task->get_task_state();
    e_online_session_search_status result = _online_session_search_status_none;
    switch (task_state)
    {
    case _overlapped_task_state_starting:
    case _overlapped_task_state_pending:
    case _overlapped_task_state_completing: result = _online_session_search_status_pending; break;
    case _overlapped_task_state_success: result = _online_session_search_status_success; break;
    case _overlapped_task_state_failure: result = _online_session_search_status_failure; break;
    case _overlapped_task_state_none: result = _online_session_search_status_none; break;
    default: result = _online_session_search_status_none; break;
    }
    return result;
}


static __forceinline unsigned long online_session_search_xenon_get_query_id(long id) { return k_online_session_search_query_id_map[id]; }
static __forceinline unsigned long online_session_search_xenon_get_context_id(long id) { return k_online_context_id_map[id]; }
static __forceinline unsigned long online_session_search_xenon_get_context_value(long id) { return k_online_context_value_map[id]; }
static __forceinline unsigned long online_session_search_xenon_get_property_id(long id) { return k_online_property_id_map[id]; }

// @address 0x3d2a68 online_session_search_build_internal_search_parameters
extern "C" bool online_session_search_build_internal_search_parameters(
    // match: 0x3d2a68: pointer __restrict qualifier prevents alias reload
    const s_online_session_search_parameters * parameters, s_online_search_query_internal *__restrict internal_parameters)
{
    bool success = true;
    internal_parameters->query_id = online_session_search_xenon_get_query_id(parameters->query_type);
    internal_parameters->unknown = parameters->controller_index;
    internal_parameters->context_count = parameters->context_count;
    for (long context_index = 0; context_index < parameters->context_count; context_index++)
    {
        internal_parameters->contexts[context_index].id = online_session_search_xenon_get_context_id(parameters->contexts[context_index].id);
        internal_parameters->contexts[context_index].value = online_session_search_xenon_get_context_value(parameters->contexts[context_index].value);
        bool id_valid = success && internal_parameters->contexts[context_index].id != -1;
        success = id_valid && internal_parameters->contexts[context_index].value != -1;
    }
    internal_parameters->property_count = parameters->property_count;
    for (long property_index = 0; property_index < parameters->property_count; property_index++)
    {
        internal_parameters->properties[property_index].dwPropertyId = online_session_search_xenon_get_property_id(parameters->properties[property_index].id);
        XUSER_DATA data;
        memcpy(&data, &parameters->properties[property_index].type, sizeof(data));
        internal_parameters->properties[property_index].value = data;
        success = success && internal_parameters->properties[property_index].dwPropertyId != -1;
    }
    return success;
}

static __forceinline long online_session_search_xenon_get_context_by_id(unsigned long id)
{
    long result = -1;
    for (long index = 0; index < 6u; ++index)
        if (id == k_online_context_id_map[index])
        {
            result = id;
            break;
        }
    return result;
}
static __forceinline long online_session_search_xenon_get_context_value_by_value(unsigned long value)
{
    long result = -1;
    for (long index = 0; index < 59u; ++index)
        if (value == k_online_context_value_map[index])
        {
            result = index;
            break;
        }
    return result;
}
static __forceinline long online_session_search_xenon_get_property_by_id(unsigned long id)
{
    long result = -1;
    for (long index = 0; index < 59u; ++index)
        if (id == k_online_property_id_map[index])
        {
            result = index;
            break;
        }
    return result;
}
static __forceinline wchar_t *online_session_search_xenon_ustrncpy(wchar_t *dest, const wchar_t *source, long count)
{
    for (long index = 0; index <= count; ++index)
    {
        if (!(dest[index] = *source))
            break;
        ++source;
    }
    return dest;
}
// @address 0x3d2bb8 online_session_build_search_result_from_results_buffer
extern "C" bool online_session_build_search_result_from_results_buffer(const void *buffer,s_online_search_results *out) {
 bool success=false;
 const XSESSION_SEARCHRESULT_HEADER *input=(const XSESSION_SEARCHRESULT_HEADER *)buffer;
 out->count=input->dwSearchResults;
 if(out->count<=50) {
  for(long index=0;index<input->dwSearchResults;++index) {
   const XSESSION_SEARCHRESULT *entry=&input->pResults[index];
   s_online_search_result *output=&out->entries[index];
   memcpy(&output->info,&entry->info,sizeof(XSESSION_INFO));
   output->public_open=entry->dwOpenPublicSlots;
   output->private_open=entry->dwOpenPrivateSlots;
   output->public_filled=entry->dwFilledPublicSlots;
   output->private_filled=entry->dwFilledPrivateSlots;
   output->context_count=MIN(MAX(entry->cContexts,0u),2u);
   for(long context_index=0;context_index<output->context_count;++context_index) {
    const XUSER_CONTEXT *src=&entry->pContexts[context_index];
    XUSER_CONTEXT *dst=&output->contexts[context_index];
    dst->dwContextId=online_session_search_xenon_get_context_by_id(src->dwContextId);
    dst->dwValue=online_session_search_xenon_get_context_value_by_value(src->dwValue);
   }
   output->property_count=MIN(MAX(entry->cProperties,0u),3u);
   for(long property_index=0;property_index<output->property_count;++property_index) {
    const XUSER_PROPERTY *src=&entry->pProperties[property_index];
    XUSER_PROPERTY *dst=&output->properties[property_index];
    long property_id=online_session_search_xenon_get_property_by_id(src->dwPropertyId);
    dst->dwPropertyId=property_id;
    dst->value=src->value;
    if(property_id==2) { online_session_search_xenon_ustrncpy(output->name,dst->value.string.pwszData,32); dst->value.string.pwszData=output->name; }
   }
  }
  success=true;
 }
 return success;
}
