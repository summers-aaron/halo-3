// networkinglibxbox/online_service_record.obj: from the records 0x70c4d8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x70c4d8 online_service_record_dispose_downloader

#include "networking/online/online_service_record.h"
#include "networking/network_configuration_globals.h"

// Other compilers only: the Xbox link takes this object's constant-initialized .data words and its initializer from the image.
#if !defined(_XBOX) || defined(__clang__)
c_online_service_record_manager g_online_service_record_manager;
#endif

extern "C" void online_service_record_dispose_downloader(void)
{
    g_network_configuration_state.download.m_downloader.c_http_buffer_downloader::~c_http_buffer_downloader();
}
