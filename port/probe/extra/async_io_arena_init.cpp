// probe only: the dynamic initializer of g_asynchronous_io_arena (retail 0x82707d10, not in source: the source
// initializes the arena statically with completion_port = INVALID_HANDLE_VALUE):
//   arena.outstanding_io_requests = 0; arena.valid_files = 0;
//   arena.completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1); atexit(arena destructor)
#include "cache/cache_file_bulk_reader_types.h"
extern "C" c_asynchronous_io_arena g_asynchronous_io_arena;
extern "C" void *CreateIoCompletionPort(void *file, void *existing, unsigned long key, unsigned long threads);
static void __attribute__((constructor)) probe_async_io_arena_initialize()
{
    g_asynchronous_io_arena.outstanding_io_requests = 0;
    g_asynchronous_io_arena.completion_port = CreateIoCompletionPort((void *)-1, 0, 0, 1);
}
