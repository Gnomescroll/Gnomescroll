#include "filter.hpp"

namespace Entities
{

int EntityListFilter::within_sphere(EntityList* list, const EntityType type, Vec3 position, float radius)
{
    IF_ASSERT(list == NULL) return 0;
    if (list->empty(type)) return 0;

    Entity** objects = list->get_objects(type);
    IF_ASSERT(objects == NULL) return 0;
    char* used = list->get_used(type);
    GS_ASSERT(used != NULL);
    IF_ASSERT(used == NULL) return 0;
    int max = list->max(type);
    GS_ASSERT(max >= 0);

    const float radius_squared = radius*radius;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;

    Entity* object;

    using Components::PhysicsComponent;
    PhysicsComponent* physics;

    for (int i=0; i<max; i++)
    {
        if (!used[i]) continue;
        object = objects[i];

        physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        IF_ASSERT(physics == NULL) continue;
        Vec3 p = quadrant_translate_position(position, physics->get_position());
        dist = vec3_distance_squared(position, p);

        if (dist < radius_squared)
        {   // object is in sphere
            this->objects[ct] = object;
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
int EntityListFilter::within_sphere(EntityList* list, const EntityType* types, const int n_types, Vec3 position, float radius)
{
    IF_ASSERT(list == NULL) return 0;

    const float radius_squared = radius*radius;
    EntityType type;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;

    int max;
    char* used;
    Entity** objects;
    Entity* object;

    using Components::PhysicsComponent;
    PhysicsComponent* physics;

    for (int j=0; j<n_types; j++)
    {
        if (ct >= this->max) break;

        type = types[j];
        if (list->empty(type)) continue;
        objects = list->get_objects(type);
        IF_ASSERT(objects == NULL) continue;
        used = list->get_used(type);
        IF_ASSERT(used == NULL) continue;
        max = list->max(type);
        GS_ASSERT(max > 0);

        for (int i=0; i<max; i++)
        {
            if (!used[i]) continue;
            object = objects[i];

            physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            IF_ASSERT(physics == NULL) continue;
            Vec3 p = quadrant_translate_position(position, physics->get_position());
            dist = vec3_distance_squared(position, p);

            if (dist < radius_squared)
            {   // object is in sphere
                this->objects[ct] = object;
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
    this->objects = (Entity**)malloc(this->max * sizeof(Entity*));
    this->distances = (float*)malloc(this->max * sizeof(float));
}

EntityListFilter::~EntityListFilter()
{
    free(this->objects);
    free(this->distances);
}

} // Entities
