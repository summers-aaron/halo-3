// hkbase/hkUnionFind.obj: from the records 0x700680 0x7006c0 0x700800 0x700878 0x700940 0x700b10
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x7006c0 ?add_union_edge@phys_union_find@@QAAXHH@Z
// @address 0x700680 ??0phys_union_find@@QAA@AAV?$phys_fixed_array@H@@H@Z
// @address 0x700800 ?collapse_tree@phys_union_find@@AAAXXZ
// @address 0x700878 ?assign_groups@phys_union_find@@QAAXAAV?$phys_array@H@@@Z
// @address 0x700940 ?move_biggest_group_first@phys_union_find@@QAAHAAV?$phys_array@H@@@Z
// @address 0x700b10 ?reindex_groups@phys_union_find@@QAAXABV?$phys_fixed_array@H@@HAAV?$phys_array@H@@@Z
// @address 0x42d298 ?havok_override_thread_memory_stack_allocations@@YA_NXZ
#define PHYS_CONFIG_SIMD 1
#include "physics/havok_memory.h"

#include "phys/phys_base.h"
#include "phys/phys_local_buffer.h"

#include "phys/phys_union_find.h"
phys_union_find::phys_union_find( union_find_int_array& parent_indices, int uf_node_cnt )
:	m_node_parents(parent_indices)
{
	m_union_node_count = uf_node_cnt;
	for(int i = 0; i < uf_node_cnt; ++i)
	{
		m_node_parents[i] = -1;
	}
}

void phys_union_find::add_union_edge( int i1, int i2 )
{
	if ( i1 > i2 )
	{
		int h = i1; i1 = i2; i2 = h;
	}
	const int r1 = find_root_of_node(i1);
	const int r2 = find_root_of_node(i2);
	if(r1!=r2)
	{
		union_roots(r1, r2);
	}
}

void phys_union_find::collapse_tree()
{

	int* c = m_node_parents.begin();
	int* e = c + m_union_node_count;

	for( ; c!=e; ++c)
	{
		if(*c>=0)
		{
			while( m_node_parents[*c]>=0 )
			{
				*c = m_node_parents[*c];
			}
		}
	}
}

void phys_union_find::assign_groups( phys_array<int>& elements_per_group )
{
	collapse_tree();

	int union_group_count = 0;

	for (int i = 0; i < m_union_node_count; ++i)
	{
		int parent = m_node_parents[i];
		if ( parent < 0)
		{

			elements_per_group.array_push_back( -parent );
			m_node_parents[i] = union_group_count++;
		}
		else
		{
			int group = m_node_parents[parent];
			m_node_parents[i] = group;
		}
	}
}

int phys_union_find::move_biggest_group_first( phys_array<int>& elements_per_group )
{

	int biggest_group_size = elements_per_group[0];
	int biggest_group_idx = 0;
	int uf_group_cnt = elements_per_group.getSize();
	{
		for (int i = 1; i < uf_group_cnt;i++)
		{
			if ( elements_per_group[i] > biggest_group_size)
			{
				biggest_group_size = elements_per_group[i];
				biggest_group_idx = i;
			}
		}
	}

	if ( biggest_group_idx == 0)
	{
		return 0;
	}

	phys_local_buffer<int> group_remap(uf_group_cnt);
	{
		for (int i = 0; i < uf_group_cnt;i++)
		{
			group_remap[i] = i;
		}
		group_remap[0] = biggest_group_idx;
		group_remap[biggest_group_idx] = 0;
		int h = elements_per_group[ biggest_group_idx ];
		elements_per_group[ biggest_group_idx ] = elements_per_group[0];
		elements_per_group[0] = h;
	}

	for (int i = 0; i < m_union_node_count;i++)
	{
		int old_group = m_node_parents[i];
		int merged_group_id = group_remap[old_group];
		m_node_parents[i] = merged_group_id;
	}
	return biggest_group_idx;
}

void phys_union_find::reindex_groups( const phys_fixed_array<int>& group_remap, int new_group_count, phys_array<int>& elements_per_group )
{
	for (int i = 0; i < m_union_node_count;i++)
	{
		int old_group = m_node_parents[i];
		int merged_group_id = group_remap[old_group];
		m_node_parents[i] = merged_group_id;
	}

	phys_local_buffer<int> new_group_sizes( new_group_count );
	{ for (int i = 0; i < new_group_count; i++){	 new_group_sizes[i] = 0;	} }

	{
		for (int i = 0; i < elements_per_group.getSize(); i++)
		{
			int merged_group_id = group_remap[i];
			new_group_sizes[merged_group_id] += elements_per_group[i];
		}
	}

	elements_per_group.set_array_size( new_group_count );
	{ for (int i = 0; i < new_group_count; i++){	 elements_per_group[i] = new_group_sizes[i];	} }

}


