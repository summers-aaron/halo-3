#ifndef INCLUDE_PHYS_BROADPHASE_H
#define INCLUDE_PHYS_BROADPHASE_H

#include "phys/phys_dynamics.h"

#include "phys/phys_broadphase_lock.h"

class phys_broadphase_handle;
class phys_broadphase_handle_pair;
class phys_aabb;
class phys_primitive_cast_callback;
class phys_broadphase_cast_collector;

typedef char phys_broadphase_aabb_cache;

class phys_broadphase: public phys_ref_object
{
	public:

		phys_broadphase();
		~phys_broadphase();

		PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BROADPHASE);

		enum broadphase_kind
		{
			BROADPHASE_HANDLE_16,
			BROADPHASE_HANDLE_32
		};

		inline broadphase_kind get_type_id() const { return BROADPHASE_HANDLE_16; }

		inline void add_broadphase_object( phys_broadphase_handle* object, const phys_aabb_u32& aabb,
			phys_array<phys_broadphase_handle_pair>& batch_pairs_out ) {}

		virtual void add_broadphase_object( phys_broadphase_handle* object, const phys_aabb& aabb,
			phys_array<phys_broadphase_handle_pair>& batch_pairs_out  ) = 0;

		virtual void add_object_batch(
			phys_array<phys_broadphase_handle *> &add_object_list, phys_array<phys_aabb> &add_aabb_list,
				phys_array<phys_broadphase_handle_pair> &new_pairs) = 0;

		virtual void remove_broadphase_object( phys_broadphase_handle* object, phys_array<phys_broadphase_handle_pair>& batch_pairs_out ) = 0;

		virtual void remove_broadphase_object_batch( phys_array<phys_broadphase_handle*>& remove_handle_list,
			phys_array<phys_broadphase_handle_pair>& removed_pairs_out ) = 0;

		virtual int get_object_count() = 0;

		virtual void update_aabbs(phys_broadphase_handle *objects[], const phys_aabb *aabb_list, int sweep_object_count,
			phys_array<phys_broadphase_handle_pair> &sweep_added_pairs, phys_array<phys_broadphase_handle_pair> &erased_sweep_pairs) = 0;

		inline void update_aabbs_u32(phys_broadphase_handle *objects[], const phys_aabb_u32 *aabb_list, int sweep_object_count,
			phys_array<phys_broadphase_handle_pair> &sweep_added_pairs, phys_array<phys_broadphase_handle_pair> &erased_sweep_pairs)
		{
		}

		virtual void defragment_sweep() = 0;

		virtual void get_all_aabbs( phys_array<phys_aabb>& all_aabbs ) = 0;

		virtual void get_handle_aabb(const phys_broadphase_handle* object, phys_aabb& aabb) const = 0;

		virtual void get_extents(phys_vec4& world_min_out, phys_vec4& world_max_out) const = 0;

		virtual void query_single_aabb( const phys_aabb& aabb, phys_array<phys_broadphase_handle_pair>& node_pairs_out) const = 0;

		virtual void requery_single_object( const phys_broadphase_handle* object, phys_array<phys_broadphase_handle_pair>& node_pairs_out) const = 0;

		virtual void shift_all_objects(const phys_vec4 &shift_distance, phys_vec4 &effective_shift_out,
			phys_array<phys_broadphase_handle *> &border_entering_objects) = 0;

		virtual void shift_broadphase(const phys_vec4 &shift_distance, phys_vec4 &effective_shift_out,
			phys_array<phys_broadphase_handle *> &border_entering_objects) = 0;

		inline void get_offset_low_high_32bit(phys_vec4& offset_low_32bit, phys_vec4& offset_high_32bit) const {}

		struct phys_ray_cast_input
		{
			PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_BROADPHASE, phys_broadphase::phys_ray_cast_input );

			phys_vec4		m_ray_from;

			int       m_cast_count;

			const phys_vec4* m_to_base;

			int		  m_to_stride;

			const phys_broadphase_aabb_cache* m_aabb_cache_info;

			phys_ray_cast_input(): m_cast_count(1), m_to_stride( int(sizeof(phys_vec4))),m_aabb_cache_info(0){}
		};

		virtual	void trace_ray( const phys_ray_cast_input& input, phys_broadphase_cast_collector* cast_collector_base, int collector_stride) const = 0;

		virtual int get_aabb_cache_size() const = 0;

		virtual void compute_bounds_cache( const phys_aabb& aabb, phys_broadphase_aabb_cache* bounds_cache_out) const = 0;

		virtual void compute_bounds_cache( const phys_array<class phys_collider*>& overlapping_colliders,
			phys_broadphase_aabb_cache* bounds_cache_out) const = 0;

		struct phys_aabb_cast_input
		{
			PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_BROADPHASE, phys_broadphase::phys_aabb_cast_input );

			phys_vec4 m_ray_from;

			phys_vec4 m_ray_to;

			phys_vec4 m_half_extents;

			const phys_broadphase_aabb_cache* m_aabb_cache_info;

			phys_aabb_cast_input(): m_aabb_cache_info(0){}
		};

		virtual void cast_bounds( const phys_aabb_cast_input& input, phys_broadphase_cast_collector& collector ) const = 0;

		inline void set_32bit_offset_and_scale(const phys_vec4& sweep_offset_low, const phys_vec4& sweep_offset_high, const phys_vec4& scale) {}

		void enable_multithreading( int critical_section_spin);

		inline phys_multi_thread_lock& get_multithread_lock();
		inline const phys_multi_thread_lock& get_multithread_lock() const;

		__forceinline void mark_for_read() const;

		__forceinline void mark_for_write();

		__forceinline void unmark_for_read() const;

		__forceinline void unmark_for_write();

		__forceinline void lock();

		__forceinline void unlock();

		typedef phys_broadphase* (PHYS_CALL *broadphase_create_fn)( const phys_vec4& world_min, const phys_vec4& world_max, int sweep_marker_count );

		static broadphase_create_fn m_default_create_fn;

	protected:

		// Acquires m_criticalSection.
		friend void broadphase_try_lock(phys_broadphase *phase);

		void lock_broadphase_impl();
		void unlock_impl();

	protected:

		mutable phys_multi_thread_lock m_mt_lock;

		class phys_critical_section *m_display_lock;
};

phys_broadphase* PHYS_CALL phys_axis_sweep16_create_broadphase( const phys_vec4& world_min, const phys_vec4& world_max, int sweep_marker_count );
phys_broadphase* PHYS_CALL phys_axis_sweep32_create_broadphase( const phys_vec4& world_min, const phys_vec4& world_max, int sweep_marker_count );

#define PHYS_ENABLE_32BIT_BROADPHASE phys_broadphase::m_default_create_fn = phys_axis_sweep32_create_broadphase


inline phys_multi_thread_lock& phys_broadphase::get_multithread_lock()
{
	return m_mt_lock;
}

inline const phys_multi_thread_lock& phys_broadphase::get_multithread_lock() const
{

	phys_broadphase* mutable_self = const_cast<phys_broadphase*>(this);
	return mutable_self->get_multithread_lock();
}

inline void phys_broadphase::mark_for_read( ) const
{
	m_mt_lock.mark_for_read();
}

inline void phys_broadphase::mark_for_write( )
{
	m_mt_lock.mark_for_write();
}

inline void phys_broadphase::unmark_for_read( ) const
{
	m_mt_lock.unmark_for_read();
}

inline void phys_broadphase::unmark_for_write()
{
	m_mt_lock.unmark_for_write();
}

inline void phys_broadphase::lock()
{
	if ( m_display_lock )
	{
		broadphase_try_lock(this);
	}
	else
	{
		mark_for_write();
	}
}

inline void phys_broadphase::unlock()
{
	if ( m_display_lock )
	{
		unlock_impl();
	}
	else
	{
		unmark_for_write();
	}
}

#endif // HAVOK_HK_BROAD_PHASE_H
