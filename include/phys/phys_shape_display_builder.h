#ifndef INCLUDE_PHYS_SHAPE_DISPLAY_BUILDER_H
#define INCLUDE_PHYS_SHAPE_DISPLAY_BUILDER_H
#include "phys/phys_object.h"
#include "phys/phys_singleton.h"
#include "phys/phys_shape_type.h"
#include "phys/phys_scalar_types.h"
template <typename T> class phys_array;
class phys_transform;
 class phys_collider;
 class phys_display_geometry;
 class phys_shape;

 class phys_shape_display_builder: public phys_ref_object
 {
    public:

        PHYS_DECLARE_ALLOCATOR(PHYS_MEM_DEBUG_VIEWER);

        struct phys_shape_display_env
        {
            PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_DEBUG_VIEWER, phys_shape_display_builder::phys_shape_display_env );

            phys_shape_display_env();
            int m_sphere_phi_res;
            int m_sphere_theta_res;
        };

        phys_shape_display_builder(const phys_shape_display_env& collision_env);

        void build_display_geometries(		const phys_shape* shape, 
                                            phys_array<phys_display_geometry*>& display_geometries);

        void build_shape_display(				const phys_shape* shape, 
                                            const phys_transform& transform, 
                                            phys_array<phys_display_geometry*>& display_geometries);

        void reset_current_raw_geometry();

        static void PHYS_CALL add_object_to_debug_display( const phys_shape* shape, phys_transform& t, phys_ulong id );
    protected:

        phys_display_geometry* get_current_raw_geometry(phys_array<phys_display_geometry*>& display_geometries);

    protected:

        phys_shape_display_env m_display_env;
        phys_display_geometry* m_current_geometry;

 };

 class phys_user_shape_display_builder : public phys_singleton< phys_user_shape_display_builder >
 {
    
    public:

        phys_user_shape_display_builder() {}

        typedef void (PHYS_CALL *shape_display_builder_fn)(	const phys_shape* shape,
                                                        const phys_transform& transform,
                                                        phys_array<phys_display_geometry*>& display_geometries,
                                                        phys_shape_display_builder* builder );

        void register_user_shape_display( shape_display_builder_fn f, phys_shape_type type );

    public:

        struct user_shape_builder
        {
            PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_DEBUG_VIEWER, phys_user_shape_display_builder::user_shape_builder );

            shape_display_builder_fn f;
            phys_shape_type type;
        };
        phys_array< user_shape_builder > m_user_shape_builders;
 };

phys_ref_object *PHYS_CALL phys_user_shape_display_create();

#endif
