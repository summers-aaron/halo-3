#ifndef GAME_MULTIPLAYER_GAME_HOPPER_GLOBALS_H
#define GAME_MULTIPLAYER_GAME_HOPPER_GLOBALS_H

#include "game/multiplayer_game_hopper.h"
#include "game/multiplayer_game_hopper_download_storage.h"

struct s_multiplayer_game_hopper_parsed_block {
  int download_id;
  void *data;
  unsigned int size;
};

template<long k_buffer_size>
struct s_http_download
{
  __forceinline s_http_download()
  {
    state = 0;
    result = 0;
    buffer_size = m_downloader.get_retry_count();
  }
  c_http_stored_buffer_downloader<k_buffer_size> m_downloader;
  union {
    s_multiplayer_game_hopper_parsed_block parsed;
    struct {
      long buffer_size;
      long state;
      long result;
    };
  };
};

struct s_multiplayer_game_hopper_global_storage_base
{
  __forceinline s_multiplayer_game_hopper_global_storage_base(s_multiplayer_game_hopper_global_storage *self)
  {
    // match: 0x57aa80: preserve the global address across member constructors.
    __assume(self == &g_multiplayer_game_hopper_globals);
  }
};

struct s_multiplayer_game_hopper_global_storage : s_multiplayer_game_hopper_global_storage_base
{
  const c_hopper_configuration *m_current_configuration;
  long m_unused;
  bool m_configuration_valid;
  bool m_configuration_failed;
  long m_configuration_download_id;
  s_http_download<21813> m_hopper_file;
  bool m_description_valid;
  bool m_description_failed;
  long m_description_download_id;
  s_http_download<19105> m_game_set_file;
  bool m_game_set_valid;
  bool m_game_set_failed;
  long m_game_set_download_id;
  s_http_download<17569> m_game_variant_file;
  bool m_game_variant_valid;
  bool m_game_variant_failed;
  s_http_download<2817> m_map_variant_file;
  bool m_map_variant_valid;
  bool m_map_variant_failed;
  s_http_download<59693> m_description_file;
  c_game_variant m_game_variant;
  __declspec(align(8)) c_map_variant m_map_variant;
  s_hopper_configuration_table m_configuration;
  s_hopper_description_table m_description;
  s_hopper_game_set_table m_game_set;
  s_multiplayer_game_hopper_global_storage();
  __forceinline ~s_multiplayer_game_hopper_global_storage() {}
};

__forceinline const c_hopper_configuration *multiplayer_game_hoppers_get_current_hopper_configuration()
{
  return g_multiplayer_game_hopper_globals.m_current_configuration;
}

#endif
