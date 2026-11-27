# cache_file_tag_resource_runtime.cpp hand vtables (k_tag_resource_runtime_interface0..8_vftable). Interface 6 is the
# physical-memory resize_listener (thiscall: initialize(buffer), resize(buffer, buffer), dispose(buffer)); slot sizes for the others from
# the interface classes: 0 c_tag_resource_runtime_listener, 1 c_tag_resource_runtime_active_set, 3 c_tag_resource_page_range_allocator
# (set_window(window, buffer *) (F57), resize(buffer *, min, requested, flags), allocate(buffer *, size)), 4 c_tag_resource_cache_file_reader.
datum k_tag_resource_runtime_interface0_vftable 0xcc8 t12 t12
datum k_tag_resource_runtime_interface1_vftable 0x3bc00 t0 t8 t8 t8
datum k_tag_resource_runtime_interface2_vftable 0x3bc10 t4
datum k_tag_resource_runtime_interface3_vftable 0x3bc14 t12 t16 t8 p p p p
datum k_tag_resource_runtime_interface4_vftable 0xdec t0 t0 t4 t12
datum k_tag_resource_runtime_interface5_vftable 0x998 t12
datum k_tag_resource_runtime_interface6_vftable 0x3bc30 t8 t16 t8
datum k_tag_resource_runtime_interface7_vftable 0x3bc3c p
datum k_tag_resource_runtime_interface8_vftable 0x3bc40 p3
# main_game.cpp: the two resource vtables (s_cache_file_tag_resource_vtable: identifier, register, unregister)
datum retail_745d78 0x745d78 w4 p p
datum retail_35954 0x35954 w4 p p
datum g_cache_file_tag_resource_vtable_list 0x745d90 p p
# codec handler table: { persistent identifier key (4 words), handler } x 2
datum k_tag_resource_handler_table 0x35944 p p p p
# the secondary paging codec context (a c_tag_resource_paging_secondary_task subclass not in source): vtable
# is_receiver_null(), process(), process_and_handle(long), reset_receiver(), initialize_context(void *)
name 0x2afa48 ?process_and_handle@c_tag_resource_paging_secondary_task@@UAEPAUs_saved_film_stream_compressor@@_K@Z
name 0x141ca0 ?process@c_tag_resource_paging_secondary_task@@UAEPAUs_saved_film_stream_compressor@@XZ
datum retail_vt_fe8 0xfe8 t0 t0 t8 t12 t4
datum g_tag_resource_paging_secondary_context 0x793a88 p a9759
datum k_tag_resource_datum_handler_vftable 0x1b88 p16
# c_tag_resource_page_allocator (get_page_location, get_page_size, get_page_hash, get_page_position, get_request_priority)
datum k_tag_resource_window_allocator_vftable 0x111c t8 t4 t8 t4 t0
# deflate code-length order (19 bytes + pad)
datum k_deflate_code_length_order 0x2a3d4 b20
# Interface 0 is the runtime listener: register / unregister_resource of the manager (no // @address in source;
# identified from the retail listing: m_resource_vtables at +0x2ac58, slot +16 / +20, tag_try_and_get_unsafe 0x87af0).
name 0x87a40 ?register_resource@c_cache_file_tag_resource_runtime_manager_storage@@UAE_NJJPAX@Z
name 0x1b26e8 ?unregister_resource@c_cache_file_tag_resource_runtime_manager_storage@@UAEXJJPAX@Z
name 0x1b2790 ?is_resource_required@c_cache_file_tag_resource_runtime_manager_storage@@UBE_NJJ@Z
name 0x1aefc8 ?allocate@c_cache_file_tag_resource_runtime_manager_storage@@UAEXPAV?$c_basic_buffer@X@@K@Z

# GAME-CHANGES F62: word data moved into fill/rdata_pool.spec (retail_real_pool), named here for pointers into it
name 0x32958 retail_32958
name 0x32948 retail_32948
