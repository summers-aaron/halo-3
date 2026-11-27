#ifndef BINK_BINK_PLAYBACK_REQUESTS_H
#define BINK_BINK_PLAYBACK_REQUESTS_H

extern "C" void bink_playback_play_movie(const char *path, long index, unsigned long flags);
extern "C" void bink_playback_queue_file_request(const char *remote_name, const char *path,
    void (*progress)(long current, long total), void (*completion)(bool success, void *context), void *context);

#endif // BINK_BINK_PLAYBACK_REQUESTS_H
