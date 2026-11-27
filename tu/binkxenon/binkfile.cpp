// binkxenon/binkfile.obj: from the records 0x717d88 0x718200 0x718228 0x7182b8 0x718588
// @flags /TP /O2 /Oc

#include "bink/bink_file.h"
#include "cseries/crt_xdk.h"

// @address 0x717d88 dosimulate
extern "C" void dosimulate(s_bink_file *file, unsigned int bytes, int elapsed)
{
    int duration = (unsigned __int64)bytes * 1000 / file->simulated_rate;
    int start = RADTimerRead();
    file->simulated_delay = duration - start + file->simulated_delay + elapsed;
    while (file->simulated_delay > 0) {
        int now;
        do {
            now = RADTimerRead();
        } while (now - start < file->simulated_delay);
        file->simulated_delay = start - now + file->simulated_delay;
        start = now;
    }
}

// @address 0x718200 bink_file_compute_allocation_size
extern "C" unsigned int bink_file_compute_allocation_size(s_bink_file *file, unsigned int buffer_size)
{
    unsigned int allocation_size = ((buffer_size + 0x1FFFF) & ~0x1FFFFU) + 0x1000;
    if (allocation_size < 0x41000)
        allocation_size = 0x41000;
    return allocation_size;
}

// @address 0x718228 BinkFileSetInfo
extern "C" void BinkFileSetInfo(s_bink_file *file, unsigned int buffer, unsigned int size,
                               unsigned int simulate, unsigned int rate)
{
    if (file->try_suspend_callback)
        file->try_suspend_callback(file);
    size &= ~0x1FFFFU;
    unsigned int aligned_buffer = (buffer + 0xFFF) & ~0xFFFU;
    file->buffer_begin = aligned_buffer;
    file->read_buffer = aligned_buffer;
    file->buffer_position = aligned_buffer;
    file->buffer_end = aligned_buffer + size;
    file->buffer_size = size;
    file->read_size = size;
    file->buffer_used = 0;
    file->simulate = simulate;
    file->simulated_rate = rate;
    if (file->resume_callback)
        file->resume_callback(file);
}

// @address 0x7182b8 BinkFileClose
void BinkFileClose(s_bink_file *file)
{
    if (file->try_suspend_callback)
        file->try_suspend_callback(file);
    if (file->shared == 0)
        CloseHandle(file->handle);
    if (file->resume_callback)
        file->resume_callback(file);
}

// @address 0x718588 BinkFileBGControl
extern "C" unsigned int BinkFileBGControl(s_bink_file *file, unsigned int control)
{
    if (control & 1) {
        if (file->bg_state == 0)
            file->bg_state = 1;
        if (control & 0x80000000) {
            if (file->try_suspend_callback)
                file->try_suspend_callback(file);
            if (file->resume_callback)
                file->resume_callback(file);
        }
    } else if (control & 2) {
        if (file->bg_state == 1)
            file->bg_state = 0;
        if (control & 0x80000000)
            BinkFileIdle(file);
    }
    return file->bg_state;
}
