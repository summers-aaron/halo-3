// blamlibxbox/watch_window.obj: from the records 0x243780 0x243788 0x2437c8
// @flags /TP /O2 /Oc
#include <malloc.h>
#include <string.h>
#include "cseries/cseries.h"
#include "interface/watch_window.h"

struct s_watch_object_results
{
    const char *description;
    void *object;
};

static char watch_window_description[512];
static s_watch_object_results watch_window_object_results;

// @address 0x2437c8 ?clear_watch_window_globals@@YAKXZ
unsigned long clear_watch_window_globals(void)
{
    csstrnzcpy(watch_window_description, "failed to get object information", sizeof(watch_window_description));
    memset(&watch_window_object_results, 0, sizeof(watch_window_object_results));
    return NONE;
}

// the only caller (watch_window_initialize) passes NONE
s_watch_object_results &watch_object(long object_index)
{
    clear_watch_window_globals();
    object_index = NONE;
    watch_window_object_results.description = watch_window_description;
    if (object_index != NONE) {
        char *buffer = (char *)_alloca(sizeof(watch_window_description));
        csstrnzcpy(buffer, watch_window_description, sizeof(watch_window_description));
    }
    return watch_window_object_results;
}

void watch_window_initialize(void)
{
    watch_object(NONE);
}

// @address 0x5bec98 ?watch_window_dispose@@YAXXZ
void watch_window_dispose(void)
{
}
