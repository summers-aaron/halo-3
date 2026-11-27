#ifndef SIMULATION_WATCHER_MACHINE_COLLECTION_TYPES_H
#define SIMULATION_WATCHER_MACHINE_COLLECTION_TYPES_H

struct s_watcher_machine_collection {
    unsigned long valid_mask;
    s_machine_identifier identifiers[16];
};

#endif // SIMULATION_WATCHER_MACHINE_COLLECTION_TYPES_H
