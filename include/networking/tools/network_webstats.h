#ifndef NETWORKING_TOOLS_NETWORK_WEBSTATS_H
#define NETWORKING_TOOLS_NETWORK_WEBSTATS_H

// Functions defined in tu/networkinglibxbox/network_webstats.cpp.

void upload_failed(bool immediate);
bool update_lsp(void);
void network_webstats_update_upload_state(void);
extern "C" void network_webstats_initialize(void);
extern "C" void network_webstats_dispose(void);
void do_uploading_work(void);
void upload_complete(void);
bool upload_step_is_taking_too_long(void);

enum e_online_lsp_service_type;

bool network_webstats_submit_buffer(e_online_lsp_service_type service_type, const void *buffer, long buffer_size,
    const char *content_type, long timeout);

// Data defined in tu/networkinglibxbox/network_webstats.cpp that other units use.

struct s_webstats_globals;

extern s_webstats_globals g_webstats_globals;

#endif // NETWORKING_TOOLS_NETWORK_WEBSTATS_H
