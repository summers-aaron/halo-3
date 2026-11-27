#ifndef SAVED_GAMES_TEXT_STREAM_TYPES_H
#define SAVED_GAMES_TEXT_STREAM_TYPES_H

#include "memory/data_types.h"

struct s_text_stream {
    char *begin;
    char *next;
    long size;
};

#endif // SAVED_GAMES_TEXT_STREAM_TYPES_H
