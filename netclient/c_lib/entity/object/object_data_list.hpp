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
        
    public:

        
        void init();

    ~ObjectDataList();

    ObjectDataList()
    : component_types(NULL)
    {}
};

} // Objects
