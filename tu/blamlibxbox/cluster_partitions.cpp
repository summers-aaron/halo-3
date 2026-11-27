// blamlibxbox/cluster_partitions.obj: from the records 0x9c4f0 0xe33d0 0xe3af0 0xfc958 0x577698 0x577758
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
// @address 0x577758 ?cluster_partition_make_valid@@YAXPAUcluster_partition@@@Z
// @address 0x9c4f0 ?cluster_partition_get_first_datum_and_payload@@YAJPBUcluster_partition@@PAJUs_cluster_reference@@JPAPBX@Z
// @address 0xfc958 ?cluster_partition_update_payload@@YAXPAUcluster_partition@@JJJPBX@Z
// @address 0xe3af0 ?cluster_partition_reconnect@@YAXPAUcluster_partition@@JPAJPBTreal_point3d@@MPBUs_location@@PBUs_game_cluster_bit_vectors@@JPBXPA_N@Z
#include <ppcintrinsics.h>
#include <string.h>
#include "objects/reference_lists.h"
#include "structures/cluster_partitions.h"
#include "structures/structures.h"
#include "memory/data.h"
#include "cseries/cseries.h"

#include "memory/data.h"
#include "structures/cluster_partition_types.h"
#include "memory/data_types.h"
#include "memory/data_typed_access.h"
#include "memory/datum_index.h"

void cluster_partition_make_valid(cluster_partition *partition)
{
    memset(partition->cluster_heads, -1, sizeof(long) * 16 * 256);
    data_make_valid(partition->references);
    data_make_valid(partition->data);
}

long cluster_partition_get_first_datum_and_payload(
    const cluster_partition *partition, long *iterator, s_cluster_reference cluster, long payload_size, const void **payload)
{
    s_data_array *array = partition->data;
    long handle = partition->cluster_heads[cluster.bsp_index * 256 + cluster.cluster_index];
    *iterator = handle;
    if (handle != -1) {
        int datum_size = array->size;
        char *data = array->data;
        s_cluster_partition_element *element = (s_cluster_partition_element *)(data + (handle & 0xffff) * datum_size);
        long next = element->next;
        *payload = (char *)element + 12;
        if (next != -1)
            __dcbt(0, data + (next & 0xffff) * datum_size);
        {
            int datum_index = element->datum_index;
            *iterator = next;
            return datum_index;
        }
    }
    return -1;
}

static __forceinline s_cluster_reference cluster_partitions_cluster_reference_from_datum_index(long datum_index)
{
    union {
        short index;
        s_cluster_reference cluster;
    } reference;
    reference.index = (short)datum_index;
    return reference.cluster;
}
static __forceinline long *cluster_partitions_get_structure_heads(cluster_partition *partition, long bsp_index)
{
    return &partition->cluster_heads[bsp_index * 256];
}
static __forceinline long *cluster_partitions_get_cluster_head(cluster_partition *partition, s_cluster_reference cluster)
{
    long *heads = cluster_partitions_get_structure_heads(partition, cluster.bsp_index);
    return &heads[cluster.cluster_index];
}
void cluster_partition_update_payload(cluster_partition *partition, long datum_index, long first_reference, long payload_size, const void *payload)
{
    // match: 0xfc958: folds payload_size to li r5,20 in the update call (without it the arg passes through, frame 0x80 to 0x90)
    __assume(payload_size == 20);
    long reference_index = first_reference;
    s_cluster_reference cluster;
    while (reference_index != -1) {
        long absolute_index = datum_index_to_absolute_index(reference_index);
        void *element = datum_get_absolute(partition->references, absolute_index);
        const s_cluster_partition_reference *reference = (const s_cluster_partition_reference *)element;
        cluster = cluster_partitions_cluster_reference_from_datum_index(reference->cluster_datum);
        reference_list_update_payload(
            partition->data, cluster_partitions_get_cluster_head(partition, cluster), datum_index, payload_size, payload);
        reference_index = reference->next;
    }
}
// @address 0xe33d0 cluster_partition_disconnect
void cluster_partition_disconnect(cluster_partition *partition, long key, long *index)
{
    long datum = *index;
    if (datum != -1) {
        do {
            s_cluster_partition_reference *element =
                (s_cluster_partition_reference *)(partition->references->data + partition->references->size * (datum & 0xffff));
            unsigned short reference_value = (unsigned short)element->cluster_datum;
            long next = element->next;
            s_cluster_reference reference = cluster_partitions_cluster_reference_from_datum_index(reference_value);
            ::datum_delete(partition->references, datum);
            long reference_index = reference.bsp_index * 256 + reference.cluster_index;
            ::reference_list_remove(partition->data, (long *)(&partition->cluster_heads[reference_index]), key);
            datum = next;
        } while (datum != -1);
    }
    *index = -1;
}

// @address 0x577698 cluster_partition_new
extern "C" void cluster_partition_new(cluster_partition *partition, const char *name, long payload_size, c_allocation_base *allocator,
    c_allocation_base *data_allocator, c_allocation_base *reference_allocator)
{
    struct { char first[256]; char second[256]; } buf;
    allocator->allocate(0x4000, name);
    partition->data = reference_list_new(csnzprintf(buf.first, 256, "cluster %s", name), payload_size, 0x800, data_allocator);
    char *cluster_name = csnzprintf(buf.first, 256, "%s cluster", name);
    csnzprintf(buf.second, 256, "%s reference", cluster_name);
    partition->references = data_new(buf.second, 0x800, 0xc, 0, reference_allocator);
}

struct s_game_cluster_bit_vectors;

static __forceinline long cluster_partitions_cluster_reference_to_long(s_cluster_reference reference)
{
    return *(short *)&reference;
}
void cluster_partition_reconnect(cluster_partition *partition, long datum_index, long *first_cluster_reference,
    const real_point3d *position, float radius, const s_location *location, const s_game_cluster_bit_vectors *bits, long payload_size,
    const void *payload, bool *overflow)
{
    s_cluster_reference clusters[256];
    long total_count;
    long count;
    if (bits)
        count = structure_clusters_in_bitvector(bits, 256, clusters, &total_count);
    else
        count = structure_clusters_in_sphere(location->cluster_reference, position, radius, 256, clusters, &total_count);
    *overflow = total_count > count;
    for (long cluster_index = 0; cluster_index < count; ++cluster_index) {
        s_cluster_reference cluster = clusters[cluster_index];
        reference_list_add(partition->references, first_cluster_reference, cluster_partitions_cluster_reference_to_long(cluster), 0, 0);
        reference_list_add(partition->data, cluster_partitions_get_cluster_head(partition, cluster), datum_index, payload_size, payload);
    }
}
