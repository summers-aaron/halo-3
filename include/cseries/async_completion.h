#ifndef CSERIES_ASYNC_COMPLETION_H
#define CSERIES_ASYNC_COMPLETION_H

class c_synchronized_long;

extern "C" bool async_block_until_done(c_synchronized_long *done, bool (*yield_function)(c_synchronized_long *),
    bool idle, bool with_idle, long file, long line);

#endif // CSERIES_ASYNC_COMPLETION_H
