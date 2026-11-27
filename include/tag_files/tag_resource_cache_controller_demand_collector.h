#ifndef TAG_FILES_TAG_RESOURCE_CACHE_CONTROLLER_DEMAND_COLLECTOR_H
#define TAG_FILES_TAG_RESOURCE_CACHE_CONTROLLER_DEMAND_COLLECTOR_H

#include <string.h>

struct s_tag_resource;
struct s_tag_resource_cache_external_access_state;
struct s_tag_resource_cache_external_access_published_table;

// The two external-access tables a demand pass reads: the published table and the one being built.
struct s_tag_resource_cache_external_access_tables
{
    s_tag_resource_cache_external_access_published_table *original_table;
    s_tag_resource_cache_external_access_state *table;
};

// The interface a demand pass hands to each demand callback.
class c_tag_resource_demand_collector
{
public:
    virtual long get_slot_from_lookup(long first, long second, long third) = 0;
    virtual unsigned char test_active_bit(long index) = 0;
    virtual void set_active_slot(long index) = 0;
    virtual void demand_resource(const s_tag_resource *tag_resource) = 0;
    virtual void set_key_bits(int key) = 0;
    virtual void update_active_slot() = 0;
    virtual void demand_required_resource(const s_tag_resource *tag_resource) = 0;
};

// Collects the resources one demand pass asks for. Methods are defined in
// tu/blamlibxbox/tag_resource_cache_external_access.cpp; the vftable is placed from main.cpp, which builds it.
class c_tag_resource_cache_controller_demand_collector : public c_tag_resource_demand_collector
{
public:
    __forceinline c_tag_resource_cache_controller_demand_collector(s_tag_resource_cache_external_access_tables *tables)
        : m_tables(tables), m_prediction_count(0), m_unique_prediction_count(0), m_active_index(-1)
    {
        memset(m_demanded_resources, 0, sizeof(m_demanded_resources));
        memset(m_required_resources, 0, sizeof(m_required_resources));
        memset(m_active_bits, 0, sizeof(m_active_bits));
    }

    virtual long get_slot_from_lookup(long first, long second, long third);
    virtual unsigned char test_active_bit(long index);
    virtual void set_active_slot(long index);
    virtual void demand_resource(const s_tag_resource *tag_resource);
    virtual void set_key_bits(int key);
    virtual void update_active_slot();
    virtual void demand_required_resource(const s_tag_resource *tag_resource);

    s_tag_resource_cache_external_access_tables *m_tables;
    long m_prediction_count;
    long m_unique_prediction_count;
    unsigned long m_demanded_resources[2048];
    unsigned long m_required_resources[2048];
    unsigned long m_active_bits[2048];
    long m_active_index;

private:
    void predict_resource(long resource_handle, long lod);
};

#endif // TAG_FILES_TAG_RESOURCE_CACHE_CONTROLLER_DEMAND_COLLECTOR_H
