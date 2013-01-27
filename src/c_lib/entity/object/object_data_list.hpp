#pragma once

namespace Entities
{

//forward decl
class Entity;

/* Management */

class EntityDataList
{
    private:
        // keep track of component type buffers for
        ComponentInterfaceType** interface_types;
        ComponentType** component_types;
        int* expected_component_sizes;
        int* component_sizes;

    public:

        void set_components(EntityType type, int n_components);
        void attach_component(EntityType type, ComponentType component);
        inline int get_component_count(EntityType type);
        inline int get_component_slot(EntityType type, ComponentType component);
        inline int get_component_interface_slot(EntityType type, ComponentInterfaceType interface);

        void init();
        void sanity_check();

    ~EntityDataList();

    EntityDataList()
    : interface_types(NULL), component_types(NULL),
    expected_component_sizes(NULL), component_sizes(NULL)
    {}
};

} // Entities
