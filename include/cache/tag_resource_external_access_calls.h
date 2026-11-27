#ifndef CACHE_TAG_RESOURCE_EXTERNAL_ACCESS_CALLS_H
#define CACHE_TAG_RESOURCE_EXTERNAL_ACCESS_CALLS_H

class c_tag_resource_page_table;
class c_tag_resource_controller_page_table_io_listener;
struct s_tag_resource_cache_external_access_block_stack;
struct s_tag_resource_paging_fiber;

extern "C" void tag_resource_cache_free_block(s_tag_resource_cache_external_access_block_stack *stack, long block_index, void *address);
extern "C" void tag_resource_paging_fiber_dispose(s_tag_resource_paging_fiber *context);
extern "C" void tag_resource_page_table_dispose_reserved_pages(c_tag_resource_page_table *table);
extern "C" void tag_resource_page_table_compact_pages(c_tag_resource_page_table *table);
// match: 0x21cf58: without noalias attempt_to_resize swaps its two size stack slots and grows its frame.
extern "C" __declspec(noalias) void tag_resource_page_table_compute_resize_sizes(c_tag_resource_page_table *, unsigned long *, unsigned long *);
// match: 0x21dd50: without noalias update_active_resources keeps the result byte in r3 and reloads it.
extern "C" __declspec(noalias) long tag_resource_cache_reconcile_published_resources(void *, void *, void *);
extern "C" void tag_resource_page_table_dispose_minimum_size(c_tag_resource_page_table *table, unsigned int minimum_size);
extern "C" void tag_resource_page_table_update_next_page(c_tag_resource_page_table *table, c_tag_resource_controller_page_table_io_listener *listener);

#endif // CACHE_TAG_RESOURCE_EXTERNAL_ACCESS_CALLS_H
