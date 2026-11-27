#ifndef SIMULATION_SIMULATION_CONNECTION_STATISTICS_TYPES_H
#define SIMULATION_SIMULATION_CONNECTION_STATISTICS_TYPES_H

struct s_simulation_connection_statistics {
    unsigned long : 32;
    unsigned long : 32;
    unsigned short metrics[4];
};

typedef char simulation_connection_statistics_size[(sizeof(s_simulation_connection_statistics) == 0x10) ? 1 : -1];

#endif // SIMULATION_SIMULATION_CONNECTION_STATISTICS_TYPES_H
