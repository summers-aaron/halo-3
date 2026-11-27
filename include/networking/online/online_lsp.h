#ifndef NETWORKING_ONLINE_ONLINE_LSP_H
#define NETWORKING_ONLINE_ONLINE_LSP_H

// Data defined in tu/networkinglibxbox/online_lsp.cpp that other units use.

class c_online_lsp_manager;

extern c_online_lsp_manager g_online_lsp_manager;

struct s_lsp_raw_server;
template<long k_capacity> class c_static_string;
extern const c_static_string<4> g_server_descriptions[5];
extern const s_lsp_raw_server k_additional_raw_upload_address_list[1];

#endif // NETWORKING_ONLINE_ONLINE_LSP_H
