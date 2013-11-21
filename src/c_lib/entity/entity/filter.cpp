/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "filter.hpp"

namespace Entities
{

int EntityListFilter::within_sphere(const EntityList* list, EntityType type,
                                    const Vec3& position, float radius)
{
    IF_ASSERT(list == NULL) return 0;
    if (list->empty(type)) return 0;

    Entity* entities = list->get_entities(type);
    IF_ASSERT(entities == NULL) return 0;
    int max = list->max(type);
    int count = list->count(type);
    if (max <= 0) return 0;

    const float radius_squared = radius*radius;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;

    for (int i=0, j=0; i<max && j<count; i++)
    {
        if (entities[i].id == list->null_id)
            continue;
        j++;
        Entity* entity = &entities[i];

        auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
        IF_ASSERT(physics == NULL) continue;
        Vec3 p = quadrant_translate_position(position, physics->get_position());
        dist = vec3_distance_squared(position, p);

        if (dist < radius_squared)
        {   // entity is in sphere
            this->entities[ct] = entity;
            this->distances[ct] = dist;
            if (dist < min_dist) min_dist = dist;
            ct++;
            if (ct >= this->max) break;
        }
    }
    this->count = ct;
    return ct;
}

// array of types to filter
int EntityListFilter::within_sphere(const EntityList* list, const EntityType* types,
                                    int n_types, const Vec3& position, float radius)
{
    IF_ASSERT(list == NULL) return 0;

    const float radius_squared = radius*radius;
    int ct = 0;
    float min_dist = 10000000.0f;

    for (int j=0; j<n_types; j++)
    {
        if (ct >= this->max) break;

        EntityType type = types[j];
        if (list->empty(type)) continue;
        Entity* entities = list->get_entities(type);
        IF_ASSERT(entities == NULL) continue;
        int max = list->max(type);
        int count = list->count(type);
        for (int i=0, k=0; i<max && k<count; i++)
        {
            if (entities[i].id == list->null_id)
                continue;
            k++;
            Entity* entity = &entities[i];

            auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
            IF_ASSERT(physics == NULL) continue;
            Vec3 p = quadrant_translate_position(position, physics->get_position());
            float dist = vec3_distance_squared(position, p);

            if (dist < radius_squared)
            {   // entity is in sphere
                this->entities[ct] = entity;
                this->distances[ct] = dist;
                if (dist < min_dist) min_dist = dist;
                ct++;
                if (ct >= this->max) break;
            }
        }
    }
    this->count = ct;
    return ct;
}

void EntityListFilter::init()
{
    this->entities = (Entity**)malloc(this->max * sizeof(Entity*));
    this->distances = (float*)malloc(this->max * sizeof(float));
}

EntityListFilter::~EntityListFilter()
{
    free(this->entities);
    free(this->distances);
}

} // Entities
