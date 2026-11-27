#ifndef INCLUDE_PHYS_FLAG_PROXY_PAIR_COLLECTOR_H
#define INCLUDE_PHYS_FLAG_PROXY_PAIR_COLLECTOR_H

// hkFlagCdBodyPairCollector, defined in tu/hkcollide/hkFlagCdBodyPairCollector.cpp.

#include "phys/phys_collectors.h"

class phys_flag_proxy_pair_collector : public phys_proxy_pair_collector {
public:
    __forceinline phys_flag_proxy_pair_collector() { reset(); }
    __forceinline phys_bool has_hit() const { return get_early_out(); }
protected:
    virtual void add_proxy_pair(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b);
};

#endif // HAVOK_HK_FLAG_CD_BODY_PAIR_COLLECTOR_H
