#ifndef SAVED_GAMES_CONTENT_CRITICAL_SECTION_TYPES_H
#define SAVED_GAMES_CONTENT_CRITICAL_SECTION_TYPES_H

struct s_content_critical_section {
    long unknown;
    long owner_thread;
    long reference_count;
    char native_lock[24];
};

#endif // SAVED_GAMES_CONTENT_CRITICAL_SECTION_TYPES_H
