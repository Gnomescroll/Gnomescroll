#include "filter.hpp"

namespace Objects
{

int ObjectListFilter::within_sphere(ObjectList* list, const ObjectType type, Vec3 position, float radius)
{
    if (list->empty(type)) return 0;

    Object** objects = list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = list->max(type);
    GS_ASSERT(max >= 0);
    
    const float radius_squared = radius*radius;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;

    Object* object;

    using Components::PhysicsComponent;
    PhysicsComponent* physics;
    
    for (int i=0; i<max; i++)
    {
        if (!used[i]) continue;
        object = objects[i];

        physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) continue;
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
int ObjectListFilter::within_sphere(ObjectList* list, const ObjectType* types, const int n_types, Vec3 position, float radius)
{
    const float radius_squared = radius*radius;
    ObjectType type;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;

    int max;
    char* used;
    Object** objects;
    Object* object;

    using Components::PhysicsComponent;
    PhysicsComponent* physics;

    for (int j=0; j<n_types; j++)
    {
        if (ct >= this->max) break;

        type = types[j];
        if (list->empty(type)) continue;
        objects = list->get_objects(type);
        GS_ASSERT(objects != NULL);
        used = list->get_used(type);
        GS_ASSERT(used != NULL);
        max = list->max(type);
        GS_ASSERT(max > 0);

        for (int i=0; i<max; i++)
        {
            if (!used[i]) continue;
            object = objects[i];
            
            physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            GS_ASSERT(physics != NULL);
            if (physics == NULL) continue;
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

void ObjectListFilter::init()
{
    this->objects = (Object**)malloc(this->max * sizeof(Object*));
    this->distances = (float*)malloc(this->max * sizeof(float));
}

ObjectListFilter::~ObjectListFilter()
{
    if (this->objects != NULL) free(this->objects);
    if (this->distances != NULL) free(this->distances);
}



} // Objects
