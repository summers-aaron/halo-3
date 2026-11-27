#ifndef VISIBILITY_VISIBILITY_COLLECTION_H
#define VISIBILITY_VISIBILITY_COLLECTION_H

#include "math/real_math_types.h"
#include "visibility/visibility_projection_types.h"
#include "objects/object_reference_types.h"
#include "visibility/visibility_collection_types.h"
#include "visibility/visible_items.h"

// the default instance fade distances, in projected pixels
extern long instance_default_fade_start_pixels;
extern long instance_default_fade_end_pixels;

struct visibility_projection;
struct structure_instanced_geometry_instance;
struct s_visibility_input;
struct s_visibility_region;
struct s_visibility_region_build;
struct s_object_cluster_payload;
struct s_visible_clusters;
struct s_visible_instances;
enum e_collection_shape
{
    _collection_shape_0,
    _collection_shape_1,
    _collection_shape_2,
};
class c_visibility_collection
{
public:
    explicit c_visibility_collection(s_visibility_input *input) : m_region((s_visibility_region_build *)input) {}
    ~c_visibility_collection() {}
    s_visibility_input *get_input() { return (s_visibility_input *)m_region; }
    __forceinline const s_visibility_region *get_region() const { return (const s_visibility_region *)m_region; }
    e_collection_type get_collection_type();
    static unsigned long *get_part_bitvector_space(long bit_count);
    static void postprocess_filter_unlight_non_visible_items();
    static __forceinline void render_objects_generate_visible_subparts() { generate_objects_visible_subparts(); }
    static bool postprocess_filter_isolate_shadowing_object(long object_index, bool isolate);
    c_visibility_collection *compute(e_collection_type collection_type, long flags, const visibility_projection *projections,
        long projection_count, const real_point3d *point, float separation, long unused, s_cluster_reference cluster_reference,
        long option0, long option1);
protected:
    static float compute_instance_alpha(structure_instanced_geometry_instance *geometry, long bsp_index);
    e_collection_shape prepare_collection_for_build(long flags, e_collection_type collection_type, const visibility_projection *projections,
        long projection_count, s_cluster_reference cluster_reference, long unused, const real_point3d *point, float sphere_radius,
        long option0, long option1);
    void collect_structure_elements_from_regions();
    void collect_objects_and_lights_from_regions();
    void setup_sphere_region();
    s_visibility_region *get_sphere_region() { return (s_visibility_region *)m_region; }
    bool sphere_is_visible(const real_point3d *center, float radius, short cluster_index, bool unused, bool *fully_contained, bool *lit, bool *shadow_casting);
    void object_try_and_add_to_collection(long object_index, long cluster_index, const s_object_cluster_payload *payload);
    void region_cluster_bitvector_add_index(short region_index, short cluster_index);
    bool object_is_visible(long object_index, long cluster_index, bool *lit, bool *shadow, bool *visible, bool *fully_contained,
        const s_object_cluster_payload *payload);
    static void generate_objects_visible_subparts();
    void light_try_and_add_to_collection(long p1, long obj, bool B);
    void generate_structure_visible_subparts();
    void generate_cluster_visible_parts(s_visible_clusters *visible);
    void generate_instance_visible_parts(s_visible_instances *visible);
private:
    s_visibility_region_build *m_region;
};

struct s_visibility_input
{
    union
    {
        char storage[200304];
        struct
        {
            short projection_count;
            char projection_header[14];
            visibility_projection projection;
            char region_storage[0x2fc44 - 0x240];
            long option0;
            long option1;
            char region_options[0x2fc6c - 0x2fc4c];
            long region_index;
            char region_tail[200304 - 0x2fc70];
        };
    };
};

typedef char visibility_input_projection_offset[(offsetof(s_visibility_input, projection) == 0x10) ? 1 : -1];
typedef char visibility_input_size[(sizeof(s_visibility_input) == 200304) ? 1 : -1];

struct s_visibility_globals
{
    s_visibility_input input;
    s_visibility_input *collection_input;
    char collection_storage[12];
    bool initialized;
};

void visibility_collection_initialize();
void visibility_collection_dispose();

#endif // VISIBILITY_VISIBILITY_COLLECTION_H
