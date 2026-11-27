#ifndef INCLUDE_PHYS_BROADPHASE_CAST_COLLECTOR_H
#define INCLUDE_PHYS_BROADPHASE_CAST_COLLECTOR_H

class phys_broadphase_handle;

// March 0x827fcbe8 has an empty virtual destructor.
class phys_broadphase_cast_collector {
public:
    virtual ~phys_broadphase_cast_collector() {}
    virtual float add_broadphase_handle(const phys_broadphase_handle *cast_broadphase_handle, int broadphase_cast_index) = 0;
};

#endif
