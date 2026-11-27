#ifndef INCLUDE_PHYS_UNION_FIND_H
#define INCLUDE_PHYS_UNION_FIND_H
#include "phys/phys_base.h"
#include "phys/phys_local_buffer.h"
class phys_union_find
{
	public:

		typedef phys_fixed_array<int>	union_find_int_array;

	public:

		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_BASE, phys_union_find);

		phys_union_find( union_find_int_array& parent_indices, int map_element_count );

		~phys_union_find(){}

		void add_union_edge( int i1, int i2 );

		void assign_groups( phys_array<int>& elements_per_group );

		__forceinline phys_bool is_one_group() { return m_node_parents[0] == -m_union_node_count; }

		void reindex_groups( const phys_fixed_array<int>& reindex_groups, int new_group_count, phys_array<int>& elements_per_group );

		int move_biggest_group_first( phys_array<int>& elements_per_group );

	private:

		__forceinline int find_root_of_node(int i);
		__forceinline void union_roots(int r1, int r2);

		void collapse_tree();

	public:
			
		union_find_int_array&	m_node_parents;
		int m_union_node_count;
};

__forceinline int phys_union_find::find_root_of_node(int i)
{
	
	int root = i;
	while ( 1 )
	{
		if ( m_node_parents[root] < 0 )
		{
			break;
		}
		root = m_node_parents[root];
	}

	while(m_node_parents[i]>=0) 
	{
		int j = m_node_parents[i];
		m_node_parents[i] = root;
		i = j;
	}
	return i;
}

__forceinline void phys_union_find::union_roots(int r1, int r2) 
{
	int n1 = m_node_parents[r1];
	int n2 = m_node_parents[r2];

	if(r1 < r2)
	{
		m_node_parents[r1] += n2;
		m_node_parents[r2] = r1;
	}
	else
	{
		m_node_parents[r2] += n1;
		m_node_parents[r1] = r2;
	}
}

 // @address 0x7006c0 ?add_union_edge@phys_union_find@@QAAXHH@Z
#endif // INCLUDE_HAVOK_HK_UNION_FIND_H
