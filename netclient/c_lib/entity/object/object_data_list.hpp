#pragma once

namespace Objects
{

//forward decl
class Object;

/* Management */

class ObjectDataList
{
    private:
        // keep track of component type buffers for 
        ComponentType** component_types;
        int* expected_component_sizes;
        int* component_sizes;
        
    public:

        void set_components(ObjectType type, int n_components);
        void attach_component(ObjectType type, ComponentType component);
        inline int get_component_count(ObjectType type);
        inline int get_component_slot(ObjectType type, ComponentType component);
        
        void init();
        void sanity_check();

    ~ObjectDataList();

    ObjectDataList()
    : component_types(NULL)
    {}
};

} // Objects
