#ifndef NETWORKING_ONLINE_ONLINE_ARBITRATION_XENON_H
#define NETWORKING_ONLINE_ONLINE_ARBITRATION_XENON_H

// Functions defined in tu/networkinglibxbox/online_arbitration_xenon.cpp that other units call.

struct s_online_arbitration_result;
extern "C" unsigned char online_arbitration_registration_get_result(s_online_arbitration_result *result);

// Data defined in tu/networkinglibxbox/online_arbitration_xenon.cpp that other units use.

struct s_online_arbitration_globals;

extern s_online_arbitration_globals g_online_arbitration_globals;

#endif // NETWORKING_ONLINE_ONLINE_ARBITRATION_XENON_H
