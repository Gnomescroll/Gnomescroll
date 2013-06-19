#include "object_list.hpp"

#include <entity/constants.hpp>
#include <entity/object/object.hpp>
#include <entity/components/health.hpp>
#include <entity/object/object_data_list.hpp>

namespace Entities
{

EntityID EntityList::get_free_id(EntityType type)
{
    if (this->objects[type] == NULL) return NULL_ENTITY;
    for (int i=0; i<this->maximums[type]; i++)
        if (!this->used[type][i])
            return EntityID(i);
    return NULL_ENTITY;
}

void EntityList::set_object_id(Entity* object)
{
    EntityID id = this->get_free_id(object->type);
    IF_ASSERT(id == NULL_ENTITY)
    {
        printf("WARNING: no free entity ids for type %d\n", object->type);
        return;
    }
    this->set_object_id(object, id);
}

void EntityList::set_object_id(Entity* object, EntityID id)
{
    EntityType type = object->type;
    IF_ASSERT(this->used[type][id] != 0) return;
    IF_ASSERT(!this->is_valid_id(id, type)) return;

    // swap from staging slot
    this->staging_objects[type] = this->objects[type][id];
    this->staging_objects[type]->id = NULL_ENTITY;
    this->objects[type][id] = object;
    object->id = id;
    this->used[type][id] = 1;
    this->indices[type] += 1;
}

inline int EntityList::count(EntityType type)
{
    IF_ASSERT(!isValid(type)) return 0;
    return this->indices[type];
}

inline int EntityList::max(EntityType type)
{
    IF_ASSERT(!isValid(type)) return 0;
    return this->maximums[type];
}

inline bool EntityList::empty(EntityType type)
{
    return (this->count(type) == 0);
}

inline bool EntityList::full(EntityType type)
{
    return (this->count(type) >= this->max(type));
}

inline bool EntityList::in_use(EntityType type, EntityID id)
{
    IF_ASSERT(!isValid(type)) return true;
    IF_ASSERT(this->used == NULL) return true;
    IF_ASSERT(this->used[type] == NULL) return true;
    IF_ASSERT(this->maximums == NULL) return true;
    IF_ASSERT(!this->is_valid_id(id, type)) return true;
    return (this->used[type][id] == 1);
}

inline bool EntityList::is_valid_id(EntityID id, EntityType type)
{
    return (id >= 0 && id < this->max(type));
}

void EntityList::destroy(EntityType type, EntityID id)
{
    if (this->used[type] == NULL) return;
    GS_ASSERT(this->used[type][id]);
    this->used[type][id] = 0;
    this->indices[type] -= 1;
}

Entity* EntityList::get(EntityType type, EntityID id)
{
    IF_ASSERT(!isValid(type)) return NULL;
    IF_ASSERT(!this->is_valid_id(id, type)) return NULL;

    if (this->objects[type] == NULL) return NULL;
    if (this->used[type] == NULL || this->used[type][id] == 0) return NULL;
    return this->objects[type][id];
}

Entity* EntityList::create(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    if (this->full(type)) return NULL;
    return this->staging_objects[type];
}

// preemptively check against used ids
Entity* EntityList::create(EntityType type, EntityID id)
{
    IF_ASSERT(!isValid(type)) return NULL;
    IF_ASSERT(!this->is_valid_id(id, type)) return NULL;
    IF_ASSERT(this->used[type] == NULL || this->used[type][id] == 1) return NULL;
    return this->staging_objects[type];
}

Entity** EntityList::get_objects(EntityType type)
{
    GS_ASSERT(type < MAX_ENTITY_TYPES);
    GS_ASSERT(type >= 0);
    return this->objects[type];
}

char* EntityList::get_used(EntityType type)
{
    GS_ASSERT(type < MAX_ENTITY_TYPES);
    GS_ASSERT(type >= 0);
    return this->used[type];
}

void EntityList::destroy_all()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
        if (this->objects[i] != NULL)
            for (int j=0; j<this->maximums[j]; j++)
                if (this->used[j])
                    this->destroy(EntityType(i), EntityID(j));
}

void EntityList::set_object_max(EntityType type, int max)
{
    GS_ASSERT(type < MAX_ENTITY_TYPES);
    GS_ASSERT(type >= 0);
    GS_ASSERT(this->maximums[type] == 0);
    GS_ASSERT(this->objects[type] == NULL);
    GS_ASSERT(this->used[type] == NULL);
    IF_ASSERT(max < 0) max = 0;
    this->maximums[type] = max;
    if (max > 0)
    {
        this->objects[type] = (Entity**)calloc(max, sizeof(Entity*));
        this->used[type] = (char*)calloc(max, sizeof(char));
    }
}

void EntityList::load_object_data(EntityDataList* data)
{   // preallocate component pointer buffers
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        size_t n_components = data->get_component_count(EntityType(i));
        for (int j=0; j<this->maximums[i]; j++)
        {
            this->objects[i][j] = new Entity(EntityID(j));
            this->objects[i][j]->init(n_components);
            this->objects[i][j]->type = EntityType(i);
        }
        this->staging_objects[i] = new Entity(NULL_ENTITY);
        this->staging_objects[i]->init(n_components);
        this->staging_objects[i]->type = EntityType(i);
    }
}

void EntityList::init()
{
    this->indices = (int*)calloc(MAX_ENTITY_TYPES, sizeof(int));
    this->maximums = (int*)calloc(MAX_ENTITY_TYPES, sizeof(int));
    this->used = (char**)calloc(MAX_ENTITY_TYPES, sizeof(char*));
    this->staging_objects = (Entity**)calloc(MAX_ENTITY_TYPES, sizeof(Entity*));
    this->objects = (Entity***)calloc(MAX_ENTITY_TYPES, sizeof(Entity**));
}

void EntityList::tick()
{
    char* used;
    int max;
    Entity** objects;
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        used = this->used[i];
        if (used == NULL) continue;
        max = this->maximums[i];
        objects = this->objects[i];
        for (int j=0; j<max; j++)
        {
            if (!used[j]) continue;
            Entity* obj = objects[j];
            if (obj->tick == NULL) break;   // none of these objects will have tick()
            obj->tick(obj);
        }
    }
}

void EntityList::update()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        if (this->used[i] == NULL) continue;
        for (int j=0; j<this->maximums[i]; j++)
        {
            if (!this->used[i][j]) continue;
            Entity* obj = this->objects[i][j];
            if (obj->update == NULL) break;
            obj->update(obj);
        }
    }
}

void EntityList::harvest()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        if (this->used[i] == NULL) continue;
        for (int j=0; j<this->maximums[i]; j++)
        {
            if (!this->used[i][j]) continue;
            Entity* obj = this->objects[i][j];
            auto health = GET_COMPONENT_INTERFACE(Health, obj);
            if (health != NULL && health->is_dead())
                Entities::destroy(obj);
        }
    }
}

void EntityList::send_to_client(EntityType type, ClientID client_id)
{
    GS_ASSERT(type < MAX_ENTITY_TYPES);
    GS_ASSERT(type >= 0);
    if (this->empty(type)) return;
    Entity** objects = this->get_objects(type);
    GS_ASSERT(objects != NULL);
    int max = this->max(type);
    GS_ASSERT(max > 0);

    for (int i=0; i<max; i++)
        if (this->used[type][i])
            objects[i]->sendToClientCreate(client_id);
}

EntityList::~EntityList()
{
    if (this->objects != NULL)
        for (int i=0; i<MAX_ENTITY_TYPES; i++)
        {
            if (this->maximums != NULL)
                for (int j=0; j<this->maximums[i]; j++)
                    delete this->objects[i][j];
            free(this->objects[i]);
        }
    free(this->objects);

    if (this->staging_objects != NULL)
        for (int i=0; i<MAX_ENTITY_TYPES; i++)
            delete this->staging_objects[i];
    free(this->staging_objects);

    free(this->indices);
    free(this->maximums);
    if (this->used != NULL)
        for (int i=0; i<MAX_ENTITY_TYPES; i++)
            free(this->used[i]);
    free(this->used);
}

} // Entities
