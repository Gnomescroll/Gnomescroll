#pragma once

namespace Components { class Component; }

namespace Entities
{

class Entity;

/* Management */

class EntityDataList
{
    private:
        static const size_t COMPONENTS_START_SIZE = 16;
        // keep track of component type buffers for
        ComponentInterfaceType** interface_types;
        ComponentType** component_types;
        Components::Component*** components;
        size_t* component_sizes;
        size_t* component_maxes;
        EntityType current_type;

    void grow_component_list(EntityType type);
    class Components::Component* add_component_to_entity(Entity* entity, ComponentType type);

    public:

    void begin_attaching_to(EntityType type);
    Components::Component* attach_component(EntityType type, ComponentType component);
    void load_properties(Entity* entity);
    inline Components::Component const* const* get_components(EntityType type) const;
    inline int get_component_count(EntityType type) const;
    inline int get_component_slot(EntityType type, ComponentType component) const;
    inline int get_component_interface_slot(EntityType type, ComponentInterfaceType interface) const;

    void init();

    ~EntityDataList();

    EntityDataList() :
        interface_types(NULL), component_types(NULL), components(NULL),
        component_sizes(NULL), component_maxes(NULL),
        current_type(NULL_ENTITY_TYPE)
    {}
};

} // Entities
