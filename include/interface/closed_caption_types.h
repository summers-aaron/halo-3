#ifndef INTERFACE_CLOSED_CAPTION_TYPES_H
#define INTERFACE_CLOSED_CAPTION_TYPES_H

#include <stddef.h>

// March closed_caption_add_caption stores the caption name and creation time at
// the start of each entry. Five 12-byte entries follow the font and justification.
struct s_closed_caption_entry
{
    short flags;
    short start_y;
    long string_identifier;
    unsigned long creation_time;
};

struct s_closed_caption_globals
{
    unsigned long flags;
    long font;
    long justification;
    s_closed_caption_entry captions[5];
};

typedef char closed_caption_entry_size[(sizeof(s_closed_caption_entry) == 12) ? 1 : -1];
typedef char closed_caption_entry_time_offset[(offsetof(s_closed_caption_entry, creation_time) == 8) ? 1 : -1];
typedef char closed_caption_globals_size[(sizeof(s_closed_caption_globals) == 72) ? 1 : -1];
typedef char closed_caption_globals_entries_offset[(offsetof(s_closed_caption_globals, captions) == 12) ? 1 : -1];

#endif // INTERFACE_CLOSED_CAPTION_TYPES_H
