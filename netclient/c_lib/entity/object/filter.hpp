#pragma once

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/object_list.hpp>

namespace Objects
{

const int OBJECT_FILTER_SIZE = 256;
// reads an instance of ObjectList, applies filters and stores the pointers
class ObjectListFilter
{
    public:
        Object** objects;   // filtered objects
        float* distances;
        int count;          // count of filtered objects
        int max;

        int within_sphere(ObjectList* list, const ObjectType type, Vec3 position, float radius);
        int within_sphere(ObjectList* list, const ObjectType* types, const int n_types, Vec3 position, float radius);

        void init();
        
    ~ObjectListFilter();
    ObjectListFilter()
    : objects(NULL), distances(NULL), count(0), max(OBJECT_FILTER_SIZE)
    {}
};

    

} // Objects
