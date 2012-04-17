#pragma once

#include <c_lib/entity/constants.hpp>

namespace Objects
{

/* Object */

class Object
{
    public:
        int n_components;
        Component** components;

        int id;
        ObjectType type;

        void add_component(Component* component)
        {
            // retrieve component of type from pool
            // call subscribe
            this->components[this->n_components++] = type;
        }
        
        // returns component of type if available
        // must cast to desired component manually
        void* get_component(ComponentType type)
        {
            for (int i=0; i<this->n_components; i++)
                if (this->components[i]->type == type)
                    return this->components[i];
            return NULL;
        }
        
        void* get_component_interface(ComponentInterfaceType interface)
        {
            for (int i=0; i<this->n_components; i++)
                if (this->components[i]->interface == interface)
                    return this->components[i];
            return NULL;
        }

        void init(int n_components)
        {
            this->components = (Component**)malloc(n_components * sizeof(Component*));
        }

    explicit Object(int id)
    : n_components(0), components(NULL), id(id), type(OBJECT_NONE)

    ~Object()
    {
        if (this->components != NULL) free(this->components);
    }
};


} // Objects
