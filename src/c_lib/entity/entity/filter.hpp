/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <entity/entity/entity.hpp>
#include <entity/entity/entity_list.hpp>

namespace Entities
{

const int ENTITY_FILTER_SIZE = 256;
// reads an instance of EntityList, applies filters and stores the pointers
class EntityListFilter
{
    public:
        Entity** entities;   // filtered entities
        float* distances;
        int count;          // count of filtered entities
        int max;

    int within_sphere(const EntityList* list, EntityType type,
                      const Vec3& position, float radius);
    int within_sphere(const EntityList* list, const EntityType* types, int n_types,
                      const Vec3& position, float radius);

    void init();

    ~EntityListFilter();
    EntityListFilter() :
        entities(NULL), distances(NULL), count(0), max(ENTITY_FILTER_SIZE)
    {}
};



} // Entities
