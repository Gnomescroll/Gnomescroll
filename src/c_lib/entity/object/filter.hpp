#pragma once

#include <entity/object/object.hpp>
#include <entity/object/object_list.hpp>

namespace Entities
{

const int OBJECT_FILTER_SIZE = 256;
// reads an instance of EntityList, applies filters and stores the pointers
class EntityListFilter
{
    public:
        Entity** objects;   // filtered objects
        float* distances;
        int count;          // count of filtered objects
        int max;

        int within_sphere(EntityList* list, const EntityType type, Vec3 position, float radius);
        int within_sphere(EntityList* list, const EntityType* types, const int n_types, Vec3 position, float radius);

        void init();
        
    ~EntityListFilter();
    EntityListFilter()
    : objects(NULL), distances(NULL), count(0), max(OBJECT_FILTER_SIZE)
    {}
};

    

} // Entities
