// interfacelibxbox/user_interface_matchmaking.obj: from the records 0x3b1f10 0x494a08 0x494b00 0x494ce0 0x494f08 0x495068 0x495168 0x495220 0x4953b0 0x4954f8 0x4955b0
// @flags /TP /O2
#include "memory/static_arrays.h"
#include "game/game_types.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "game/multiplayer_game_hopper.h"
#include "interface/user_interface_matchmaking.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/online_url.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/session/network_session_parameters.h"
#include "interface/utf8_types.h"
#include "interface/hopper_category_id_types.h"
#include "networking/tools/network_blf.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/logic/storage/hopper_load_status_types.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/session/network_session.h"
#include "networking/logic/life_cycle/life_cycle_globals_access.h"
#include "networking/logic/network_life_cycle_session_helpers.h"
#include "interface/hopper_information_types.h"
#include "interface/user_interface_global_strings.h"
#include "networking/session/network_session_parameters_generic_set.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "networking/logic/storage/network_http_buffer_url_api.h"
#include "networking/tools/blf_authentication_types.h"

struct utf8;

// @address 0xf11308 g_network_life_cycle_globals

struct s_matchmaking_wchar_string
{
  unsigned short text[1024];
  s_matchmaking_wchar_string() { text[0] = 0; }
};

static __forceinline const e_matchmaking_search_preference *user_interface_matchmaking_search_preference_get_value(
    const c_generic_network_session_parameter<e_matchmaking_search_preference> *parameter)
{
  const e_matchmaking_search_preference *result = 0;
  if (parameter->m_flags & 1)
    result = &parameter->m_data;
  return result;
}

e_hopper_load_status user_interface_matchmaking_hopper_catalog_load_status(void)
{
  long result = 0;
  c_network_session *session;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    const c_network_session_parameters::s_network_session_parameters_internal *membership = &session->get_session_parameters()->m_parameters;
    if (membership->matchmaking_hopper_list.get_allowed())
      result = membership->matchmaking_hopper_list.get()->hopper_load_status;
  }
  return (e_hopper_load_status)result;
}


// @address 0x494a08 user_interface_matchmaking_get_category_ids
// match: 0x494a08: pointer __restrict qualifier prevents alias reload
long user_interface_matchmaking_get_category_ids(long *__restrict category_ids)
{
    long count = 0;
    c_network_session *session;
    if (network_life_cycle_in_squad_session(&session) && session->established()) {
        const c_network_session_parameters::s_network_session_parameters_internal *parameters = &session->get_session_parameters()->m_parameters;
        if ((parameters->matchmaking_hopper_list.get_allowed()) != 0) {
            const s_network_session_matchmaking_hopper_list *list = parameters->matchmaking_hopper_list.get();
            if (list) {
                for (long i = 0; i < 12; i++) {
                    if ((long)list->category_populated_mask & (1 << i)) category_ids[count++] = i;
                }
            }
        }
    }
    return count;
}

// @address 0x494b00 user_interface_matchmaking_get_category_name
bool user_interface_matchmaking_get_category_name(long category_identifier, unsigned short *category_name)
{
  bool result = false;
  long string_identifier = -1;
  switch (category_identifier) {
  case 0: string_identifier = 0x10427; break;
  case 1: string_identifier = 0x10429; break;
  case 2: string_identifier = 0x1042b; break;
  case 3: string_identifier = 0x1042d; break;
  case 4: string_identifier = 0x1042f; break;
  }
  if (string_identifier != -1) {
    s_matchmaking_wchar_string string;
    if (user_interface_global_string_get(string_identifier, string.text)) {
      ustrnzcpy((wchar_t *)category_name, (const wchar_t *)string.text, 32);
      result = true;
    }
  } else {
    c_network_session *session;
    if (network_life_cycle_in_squad_session(&session) && session->established()) {
      const c_network_session_parameters::s_network_session_parameters_internal *membership = &session->get_session_parameters()->m_parameters;
      if (membership->matchmaking_hopper_list.get_allowed()) {
        const s_network_session_matchmaking_hopper_list *list = membership->matchmaking_hopper_list.get();
        for (long i = 0; i < list->category_count; i++) {
          if (list->categories[i].identifier == category_identifier) {
            ::utf8_string_to_wchar_string((utf8 const *)list->categories[i].name, (wchar_t *)(category_name), 32, (long *)(0));
            result = true;
            break;
          }
        }
      }
    }
  }
  return result;
}

// @address 0x494ce0 user_interface_matchmaking_get_category_description
bool user_interface_matchmaking_get_category_description(long category_identifier, unsigned short *description)
{
  bool result = false;
  long string_identifier = -1;
  switch (category_identifier) {
  case 0: string_identifier = 0x10428; break;
  case 1: string_identifier = 0x1042a; break;
  case 2: string_identifier = 0x1042c; break;
  case 3: string_identifier = 0x1042e; break;
  case 4: string_identifier = 0x10430; break;
  }
  if (string_identifier != -1) {
    s_matchmaking_wchar_string string;
    if (user_interface_global_string_get(string_identifier, string.text)) {
      ustrnzcpy((wchar_t *)description, (const wchar_t *)string.text, 256);
      result = true;
    }
  } else {
    c_network_session *session;
    if (network_life_cycle_in_squad_session(&session) && session->established()) {
      const c_network_session_parameters::s_network_session_parameters_internal *membership = &session->get_session_parameters()->m_parameters;
      if (membership->matchmaking_hopper_list.get_allowed()) {
        const s_network_session_matchmaking_hopper_list *list = membership->matchmaking_hopper_list.get();
        for (long i = 0; i < list->category_count; i++) {
          if (list->categories[i].identifier == category_identifier) {
            const utf8 *text = multiplayer_game_hopper_get_description(list->categories[i].identifier);
            if (text)
              ::utf8_string_to_wchar_string((utf8 const *)(const unsigned char *)text, (wchar_t *)(description), 256, (long *)(0));
            else
              ustrnzcpy_inline((wchar_t *)description, L" ", 256);
            result = true;
            break;
          }
        }
      }
    }
  }
  return result;
}

// @address 0x494f08 user_interface_matchmaking_get_category_value
int user_interface_matchmaking_get_category_value(long category)
{
    int result = -1;
    switch (category) {
    case 0: result = 0; break;
    case 1: result = 1; break;
    case 2: result = 2; break;
    case 3: result = 3; break;
    case 4: result = 4; break;
    default: {
        c_network_session *session;
        if (network_life_cycle_in_squad_session(&session) && session->established()) {
            const c_network_session_parameters::s_network_session_parameters_internal *membership = &session->get_session_parameters()->m_parameters;
            if (membership->matchmaking_hopper_list.get_allowed()) {
                const s_network_session_matchmaking_hopper_list *list = membership->matchmaking_hopper_list.get();
                long count = list->category_count;
                for (long i = 0; i < count; i++) {
                    if (list->categories[i].identifier == category) {
                        result = list->categories[i].hopper_count;
                        break;
                    }
                }
            }
        }
        break;
    }
    }
    return result;
}

long user_interface_matchmaking_get_hopper_ids(e_hopper_category_id category, unsigned short *const ids)
{
    long count = 0;
    c_network_session *session;
    if (network_life_cycle_in_squad_session(&session) && session->established()) {
        const c_network_session_parameters::s_network_session_parameters_internal *membership = &session->get_session_parameters()->m_parameters;
        if (membership->matchmaking_hopper_list.get_allowed()) {
            const s_network_session_matchmaking_hopper_list *list = membership->matchmaking_hopper_list.get();
            long hopper_count = list->hopper_count;
            for (long i = 0; i < hopper_count; i++) {
                // Hopper categories are compared as signed short identifiers.
                if ((short)list->hoppers[i].category_identifier == category)
                    ids[count++] = list->hoppers[i].identifier;
            }
        }
    }
    return count;
}

e_matchmaking_search_preference user_interface_matchmaking_squad_get_search_preference(void)
{
    c_network_session *session = 0;
    long result = 0;
    if (network_life_cycle_in_squad_session(&session) && session->established()) {
        const c_network_session_parameters::s_network_session_parameters_internal *membership = &session->get_session_parameters()->m_parameters;
        if (membership->matchmaking_search_preference.m_flags & 1)
            result = *user_interface_matchmaking_search_preference_get_value(&membership->matchmaking_search_preference);
    }
    return (e_matchmaking_search_preference)result;
}

// @address 0x3b1f10 user_interface_matchmaking_squad_set_search_preference
bool user_interface_matchmaking_squad_set_search_preference(long value)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    c_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    result = parameter_request_change(&session->get_session_parameters()->m_parameters.matchmaking_search_preference, &value);
  }
  return result;
}

// @address 0x495220 ?user_interface_matchmaking_get_hopper_information@@YA_NGPAUs_hopper_information@@@Z
bool user_interface_matchmaking_get_hopper_information(unsigned short id, s_hopper_information *out)
{
  bool result = false;
  c_network_session *session;
  if (id != 0xffff && network_life_cycle_in_squad_session(&session) && session->established()) {
    const c_network_session_parameters::s_network_session_parameters_internal *membership = &session->get_session_parameters()->m_parameters;
    if (membership->matchmaking_hopper_list.get_allowed()) {
      const s_network_session_matchmaking_hopper_list *value = membership->matchmaking_hopper_list.get();
      for (long i = 0; i < value->hopper_count; i++) {
        const s_network_session_matchmaking_hopper_description *entry = &value->hoppers[i];
        if (entry->identifier == id) {
          utf8_string_to_wchar_string((const utf8 *)value->hoppers[i].name, (wchar_t *)out, 32, 0);
          result = true;
          out->id = entry->identifier;
          // The hopper information value holds the sign-extended category identifier.
          out->value = (short)entry->category_identifier;
          out->kind = entry->rich_presence_context_id;
          out->number = entry->image_index;
          out->visible = entry->is_visible;
          out->enabled = entry->is_playable;
          out->ranked = entry->is_ranked;
          out->b3 = entry->experience_penalty_enabled;
          break;
        }
      }
    }
  }
  return result;
}

static __forceinline bool matchmaking_hopper_parameter_set(void *parameter, short value)
{
  return ((c_generic_network_session_parameter<short> *)parameter)->request_change(&value);
}

// @address 0x4953b0 ?user_interface_matchmaking_set_hopper@@YA_NF@Z
bool user_interface_matchmaking_set_hopper(short value)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    c_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    result = matchmaking_hopper_parameter_set(&session->get_session_parameters()->m_parameters.matchmaking_requested_hopper, value);
  }
  return result;
}

// @address 0x4955b0 ?user_interface_matchmaking_update_hopper_statistics_for_screen@@YAXPAV?$c_http_blf_simple_downloader@Us_dynamic_matchmaking_hopper_statistics@@@@PAGPAUs_matchmaking_single_hopper_statistics@@@Z
void user_interface_matchmaking_update_hopper_statistics_for_screen(
    c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics> *downloader,
    unsigned short *last_hopper,
    s_matchmaking_single_hopper_statistics *statistics)
{
  unsigned short hopper = network_squad_session_get_hopper();
  unsigned long statistics_updated = false;
  if (network_life_cycle_is_squad_host()) {
    s_network_http_buffer_url url;
    url.name[0] = 0;
    url.service_type = 0;
    url.cache_mode = 0;
    url.lifetime_seconds = 600;
    url.queue_type = 0;
    online_url_set_base_path((c_online_url_string *)&url);
    ((c_static_string<256> *)url.name)->append_print("dynamic_hopper_statistics.bin");
    url.cache_mode = 2;
    url.queue_type = 1;
    http_buffer_downloader_set_url(downloader->buffer_downloader(), &url);
    long downloaded_size = 0;
    const char *downloaded_data = 0;
    long download_status = downloader->get_data(&downloaded_data, &downloaded_size);
    if (download_status == 2 && downloaded_data != 0) {
      if (downloader->m_last_retry_count != downloader->get_retry_count())
        downloader->m_data = (const s_dynamic_matchmaking_hopper_statistics *)network_blf_find_chunk_in_file(
            downloaded_data, downloaded_size, 0x6d6d6873, 3, _blf_file_authentication_none,
            (unsigned long *)&downloader->m_data_size);
    } else {
      downloader->m_data = 0;
      downloader->m_data_size = 0;
    }
    const s_dynamic_matchmaking_hopper_statistics *hopper_statistics = downloader->m_data;
    if (hopper_statistics == 0 || hopper == 0xffff || *last_hopper == hopper)
      return;
    statistics->total_population = network_blf_payload_long(hopper_statistics->total_population);
    statistics->hopper_population = -1;
    long index = 0;
    const s_dynamic_matchmaking_hopper_statistics::s_hopper_population_entry *entry = hopper_statistics->hoppers;
    do {
      if (network_blf_payload_long(entry->hopper_id) == hopper) {
        statistics->hopper_population = network_blf_payload_long(hopper_statistics->hoppers[index].hopper_population);
        break;
      }
      index++;
      entry++;
    } while ((unsigned long)index < 32);
    bool set_result = false;
    c_network_session *squad_session = 0;
    bool in_squad_session = false;
    if (g_network_life_cycle_globals.initialized) {
      c_network_session *current = g_network_life_cycle_globals.manager.m_active_squad_session;
      if (current->current_local_state() != 0) {
        squad_session = current;
        in_squad_session = true;
      }
    }
    if (in_squad_session) {
      c_network_session_parameters::s_network_session_parameters_internal *membership = &squad_session->get_session_parameters()->m_parameters;
      s_matchmaking_single_hopper_statistics value = *statistics;
      if (value.hopper_population < 0 && value.hopper_population != -1)
        value.hopper_population = -1;
      if (value.total_population < 0 && value.total_population != -1)
        value.total_population = -1;
      set_result = membership->matchmaking_hopper_statistics.set(&value);
    }
    statistics_updated = set_result;
  } else {
    if (hopper == 0xffff || *last_hopper == hopper)
      return;
    long has_statistics = false;
    c_network_session *squad_session = 0;
    bool in_squad_session = false;
    if (g_network_life_cycle_globals.initialized) {
      c_network_session *current = g_network_life_cycle_globals.manager.m_active_squad_session;
      if (current->current_local_state() != 0) {
        squad_session = current;
        in_squad_session = true;
      }
    }
    if (in_squad_session) {
      c_network_session_parameters::s_network_session_parameters_internal *membership = &squad_session->get_session_parameters()->m_parameters;
      if (membership->matchmaking_hopper_statistics.get_allowed()) {
        const s_matchmaking_single_hopper_statistics *hopper_data = 0;
        const c_network_session_parameter_matchmaking_hopper_statistics *parameter =
            &membership->matchmaking_hopper_statistics;
        if (parameter->get_allowed())
          hopper_data = parameter->get();
        long total_population = hopper_data->total_population;
        has_statistics = true;
        statistics->total_population = total_population;
        statistics->hopper_population = hopper_data->hopper_population;
      }
    }
    statistics_updated = (unsigned char)has_statistics;
  }
  if (statistics_updated)
    *last_hopper = hopper;
}
