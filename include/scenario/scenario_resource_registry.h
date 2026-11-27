#ifndef SCENARIO_SCENARIO_RESOURCE_REGISTRY_H
#define SCENARIO_SCENARIO_RESOURCE_REGISTRY_H

class c_scenario_resource_registry
{
public:
    __forceinline bool is_tag_available(long tag_index) const
    {
        return (m_available_tags[tag_index >> 5] & (1 << (tag_index & 31))) != 0;
    }
private:
    // 628 words: the 0x9d0-byte registry scenario.cpp builds on the stack for the designer zone set switch.
    unsigned long m_available_tags[628];
};

#endif // SCENARIO_SCENARIO_RESOURCE_REGISTRY_H
