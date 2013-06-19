#pragma once

namespace Entities
{

//forward decl
class Entity;

/* Management */

class EntityDataList
{
    private:
        static const size_t COMPONENTS_START_SIZE = 16;
        // keep track of component type buffers for
        ComponentInterfaceType** interface_types;
        ComponentType** component_types;
        size_t* component_sizes;
        size_t* component_maxes;
        EntityType current_type;

    void grow_component_list(EntityType type);

    public:

    void begin_attaching_to(EntityType type);
    void attach_component(EntityType type, ComponentType component);
    inline int get_component_count(EntityType type);
    inline int get_component_slot(EntityType type, ComponentType component);
    inline int get_component_interface_slot(EntityType type, ComponentInterfaceType interface);

    void init();

    ~EntityDataList();

    EntityDataList() :
        interface_types(NULL), component_types(NULL),
        component_sizes(NULL), current_type(NULL_ENTITY_TYPE)
    {}
};

} // Entities
