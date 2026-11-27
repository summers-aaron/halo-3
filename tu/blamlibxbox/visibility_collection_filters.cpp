// blamlibxbox/visibility_collection_filters.obj: from the records 0x57f340
// @flags /TP /O2 /Oc
// @address 0x57f340 ?postprocess_filter_unlight_non_visible_items@c_visibility_collection@@SAXXZ
#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"

#include "visibility/visibility_collection.h"

void c_visibility_collection::postprocess_filter_unlight_non_visible_items()
{
    s_visible_items &items = c_visible_items::get_global_items();
    long root_start = c_visible_items::get_root_objects_starting_index();
    long root_count = items.hierarchies.get_count();
    long camera_root_start = 0;
    long camera_root_count = c_visible_items::get_camera_root_objects_count();
    for (long root_index = root_start; root_index < root_count; root_index++)
    {
        bool found = false;
        bool camera_flag = false;
        s_object_hierarchy &root = items.hierarchies[root_index];
        if (root.object_count > 0)
        {
            s_object_visibility &visibility = items.objects[root.first_object];
            for (long camera_index = camera_root_start; camera_index < camera_root_count && found == 0; camera_index++)
            {
                s_object_hierarchy &camera_root = items.hierarchies[camera_index];
                if (camera_root.object_count > 0)
                {
                    for (long object_index = camera_root.first_object;
                        object_index < camera_root.first_object + camera_root.object_count && found == 0; object_index++)
                    {
                        s_object_visibility &camera_visibility = items.objects[object_index];
                        found |= camera_visibility.render_setup.object_index - visibility.render_setup.object_index == 0;
                        camera_flag = (camera_root.flags & 1) != 0;
                    }
                }
            }
        }
        if (found == 0)
            root.flags &= ~1;
        else if (camera_flag)
            root.flags |= 1;
        else
            root.flags &= ~1;
    }
    long camera_instance_start = 0;
    long camera_instance_count = c_visible_items::get_camera_instances_count();
    long instance_start = c_visible_items::get_instances_starting_index();
    long instance_count = items.instances.get_count();
    for (long instance_index = instance_start; instance_index < instance_count; instance_index++)
    {
        bool found = false;
        s_visible_instances &instance = items.instances[instance_index];
        for (long camera_index = camera_instance_start; camera_index < camera_instance_count && found == 0; camera_index++)
        {
            s_visible_instances &camera_instance = items.instances[camera_index];
            found |= camera_instance.instance_index - instance.instance_index == 0;
        }
        if (found == 0)
            instance.flags &= ~1;
    }
}
