#ifndef SAVED_GAMES_BUFFERED_FILE_WRITER_TYPES_H
#define SAVED_GAMES_BUFFERED_FILE_WRITER_TYPES_H

struct s_file_reference;

struct s_buffered_file_writer {
    s_file_reference *file;
    char *buffer;
    long buffer_size;
    long buffer_position;
};

#endif // SAVED_GAMES_BUFFERED_FILE_WRITER_TYPES_H
