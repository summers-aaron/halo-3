#ifndef SAVED_GAMES_STORAGE_DEVICE_XDK_H
#define SAVED_GAMES_STORAGE_DEVICE_XDK_H

struct s_storage_device_data;

extern "C" unsigned long XShowDeviceSelectorUI(unsigned long, unsigned long, unsigned long, unsigned __int64, unsigned long *, void *);
extern "C" unsigned long XContentGetDeviceData(unsigned long device_id, s_storage_device_data *data);

#endif // SAVED_GAMES_STORAGE_DEVICE_XDK_H
