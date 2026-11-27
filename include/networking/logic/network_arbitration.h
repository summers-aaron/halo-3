#ifndef NETWORKING_LOGIC_NETWORK_ARBITRATION_H
#define NETWORKING_LOGIC_NETWORK_ARBITRATION_H

// Functions defined in tu/networkinglibxbox/network_arbitration.cpp that other units call.

bool network_arbitration_initiate(long index,unsigned __int64 nonce);

// Data defined in tu/networkinglibxbox/network_arbitration.cpp that other units use.

struct s_network_arbitration_globals;

extern s_network_arbitration_globals network_arbitration_globals;

#endif // NETWORKING_LOGIC_NETWORK_ARBITRATION_H
