#include "object_list.hpp"

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/components/health.hpp>

namespace Objects
{

void ObjectList::init()
{
    this->component_types = (ComponentType*)calloc(MAX_OBJECT_TYPES * sizeof(ComponentType));
}

ObjectList::~ObjectList()
{
    if (this->component_types != NULL)
    {
        for (int i=0; i<MAX_OBJECT_TYPES; i++)
            if (this->component_types[i] != NULL)
                free(this->component_types[i];

        free(component_types);
    }
}

} // Objects
