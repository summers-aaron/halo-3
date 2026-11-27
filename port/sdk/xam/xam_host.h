// port/sdk/xam: what the XAM services need from the host.
#ifndef PORT_XAM_HOST_H
#define PORT_XAM_HOST_H

// The host user's language and region as two lowercase (ISO 639) and two uppercase (ISO 3166) letters, each left
// empty when the host does not say.
void port_host_language(char language[3], char region[3]);

#endif
