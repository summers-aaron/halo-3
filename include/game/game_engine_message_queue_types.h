#ifndef GAME_GAME_ENGINE_MESSAGE_QUEUE_TYPES_H
#define GAME_GAME_ENGINE_MESSAGE_QUEUE_TYPES_H

#include <stddef.h>

struct s_pending_entry {
  s_pending_entry *next;
  short value;
  signed char bit;
  signed char type;
  long m_slist_reserved[2]; // sixteen-byte SLIST entries, as in t_event_queue::s_entry
};
struct s_list_globals {
  s_pending_entry entries[128];
#ifdef HALO3_64BIT
  s_pending_entry *pending;
  long pad804;
  s_pending_entry *free;
  long pad80c;
#else
  union { struct { s_pending_entry *pending; long pad804; }; __int64 pending_head; };
  union { struct { s_pending_entry *free; long pad80c; }; __int64 free_head; };
#endif
  bool initialized;
  long limit;
};

typedef char game_engine_message_entry_size[(sizeof(void *) != 4 || ((sizeof(s_pending_entry) == 0x10))) ? 1 : -1];
typedef char game_engine_message_queue_pending_offset[(sizeof(void *) != 4 || ((offsetof(s_list_globals, pending) == 0x800))) ? 1 : -1];
typedef char game_engine_message_queue_limit_offset[(sizeof(void *) != 4 || ((offsetof(s_list_globals, limit) == 0x814))) ? 1 : -1];
typedef char game_engine_message_queue_size[(sizeof(void *) != 4 || ((sizeof(s_list_globals) == 0x818))) ? 1 : -1];

#endif
