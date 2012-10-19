#include "object_list.hpp"

#include <entity/constants.hpp>
#include <entity/object/object.hpp>
#include <entity/components/health.hpp>
#include <entity/object/object_data_list.hpp>

namespace Objects
{

int ObjectList::get_free_id(ObjectType type)
{
    if (this->objects[type] == NULL) return -1;
    for (int i=0; i<this->maximums[type]; i++)
        if (this->used[type][i] == 0) return i;
    return -1;
}

void ObjectList::set_object_id(Object* object)
{
    int id = this->get_free_id(object->type);
    if (id < 0) { printf("WARNING: no free ids\n"); return; }
    this->set_object_id(object, id);
}

void ObjectList::set_object_id(Object* object, int id)
{
    ObjectType type = object->type;
    GS_ASSERT(this->used[type][id] == 0);
    GS_ASSERT(id >= 0);
    GS_ASSERT(id < this->max(type));
    if (this->used[type][id] != 0) return;
    if (id < 0 || id >= this->max(type)) return;

    // swap from staging slot
    this->staging_objects[type] = this->objects[type][id];
    this->staging_objects[type]->id = -1;
    this->objects[type][id] = object;
    object->id = id;
    this->used[type][id] = 1;
    this->indices[type] += 1;
}

inline int ObjectList::count(ObjectType type)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return 0;
    return this->indices[type];
}

inline int ObjectList::max(ObjectType type)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return 0;
    return this->maximums[type];
}

inline bool ObjectList::empty(ObjectType type)
{
    return (this->count(type) == 0);
}

inline bool ObjectList::full(ObjectType type)
{
    return (this->count(type) >= this->max(type));
}

inline bool ObjectList::in_use(ObjectType type, int id)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    GS_ASSERT(this->used != NULL);
    GS_ASSERT(this->used[type] != NULL);
    GS_ASSERT(this->maximums != NULL);
    GS_ASSERT(id >= 0 && id < this->maximums[type]);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return true;
    if (this->used == NULL) return true;
    if (this->used[type] == NULL) return true;
    if (this->maximums == NULL) return true;
    if (id < 0 || id >= this->maximums[type]) return true;
    return (this->used[type][id] == 1);
}

void ObjectList::destroy(ObjectType type, int id)
{
    if (this->used[type] == NULL) return;
    GS_ASSERT(this->used[type][id]);
    this->used[type][id] = 0;
    this->indices[type] -= 1;
}

Object* ObjectList::get(ObjectType type, int id)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return NULL;
    GS_ASSERT(id >= 0 && id < this->maximums[type]);
    if (id < 0 || id >= this->maximums[type]) return NULL;
    
    if (this->objects[type] == NULL) return NULL;
    if (this->used[type] == NULL || this->used[type][id] == 0) return NULL;
    return this->objects[type][id];
}

Object* ObjectList::create(ObjectType type)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return NULL;
    if (this->full(type)) return NULL;
    return this->staging_objects[type];
}

// preemptively check against used ids
Object* ObjectList::create(ObjectType type, int id)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return NULL;
    GS_ASSERT(id >= 0 && id < this->maximums[type]);
    if (id < 0 || id >= this->maximums[type]) return NULL;
    GS_ASSERT(this->used[type] != NULL);
    GS_ASSERT(this->used[type][id] == 0);
    if (this->used[type] == NULL || this->used[type][id] == 1) return NULL;
    return this->staging_objects[type];
}

Object** ObjectList::get_objects(ObjectType type)
{
    GS_ASSERT(type < MAX_OBJECT_TYPES);
    GS_ASSERT(type >= 0);
    return this->objects[type];
}

char* ObjectList::get_used(ObjectType type)
{
    GS_ASSERT(type < MAX_OBJECT_TYPES);
    GS_ASSERT(type >= 0);
    return this->used[type];
}

void ObjectList::set_object_max(ObjectType type, int max)
{
    GS_ASSERT(type < MAX_OBJECT_TYPES);
    GS_ASSERT(type >= 0);
    GS_ASSERT(this->maximums[type] == 0);
    GS_ASSERT(this->objects[type] == NULL);
    GS_ASSERT(this->used[type] == NULL);
    GS_ASSERT(max >= 0);
    if (max < 0) max = 0;
    this->maximums[type] = max;
    if (max > 0)
    {
        this->objects[type] = (Object**)calloc(max, sizeof(Object*));
        this->used[type] = (char*)calloc(max, sizeof(char));
    }
}

void ObjectList::load_object_data(ObjectDataList* data)
{   // preallocate component pointer buffers
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        int n_components = data->get_component_count((ObjectType)i);
        for (int j=0; j<this->maximums[i]; j++)
        {
            this->objects[i][j] = new Object(j);
            this->objects[i][j]->init(n_components);
            this->objects[i][j]->type = (ObjectType)i;
        }
        this->staging_objects[i] = new Object(-1);
        this->staging_objects[i]->init(n_components);
        this->staging_objects[i]->type = (ObjectType)i;
    }
}

void ObjectList::init()
{
    this->indices = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->maximums = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->used = (char**)calloc(MAX_OBJECT_TYPES, sizeof(char*));
    this->staging_objects = (Object**)calloc(MAX_OBJECT_TYPES, sizeof(Object*));
    this->objects = (Object***)calloc(MAX_OBJECT_TYPES, sizeof(Object**));
}

void ObjectList::tick()
{
    char* used;
    int max;
    Object** objects;
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        used = this->used[i];
        if (used == NULL) continue;
        max = this->maximums[i];
        objects = this->objects[i];
        for (int j=0; j<max; j++)
        {
            if (!used[j]) continue;
            Object* obj = objects[j];
            if (obj->tick == NULL) break;   // none of these objects will have tick()
            obj->tick(obj);
        }
    }
}

void ObjectList::update()
{
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        if (this->used[i] == NULL) continue;
        for (int j=0; j<this->maximums[i]; j++)
        {
            if (!this->used[i][j]) continue;
            Object* obj = this->objects[i][j];
            if (obj->update == NULL) break;
            obj->update(obj);
        }
    }
}

void ObjectList::harvest()
{
    using Components::HealthComponent;
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        if (this->used[i] == NULL) continue;
        for (int j=0; j<this->maximums[i]; j++)
        {
            if (!this->used[i][j]) continue;
            Object* obj = this->objects[i][j];
            HealthComponent* health = (HealthComponent*)obj->get_component_interface(COMPONENT_INTERFACE_HEALTH);
            if (health != NULL && health->is_dead())
                Objects::destroy(obj);
        }
    }
}

void ObjectList::send_to_client(ObjectType type, ClientID client_id)
{
    GS_ASSERT(type < MAX_OBJECT_TYPES);
    GS_ASSERT(type >= 0);
    if (this->empty(type)) return;
    Object** objects = this->get_objects(type);
    GS_ASSERT(objects != NULL);
    int max = this->max(type);
    GS_ASSERT(max > 0);

    for (int i=0; i<max; i++)
        if (this->used[type][i])
            objects[i]->sendToClientCreate(client_id);
}

ObjectList::~ObjectList()
{
    if (this->objects != NULL)
    {
        for (int i=0; i<MAX_OBJECT_TYPES; i++)
        {
            // delete objects tracked in objects array
            if (this->used[i] != NULL)
                for (int j=0; j<this->maximums[i]; j++)
                    delete this->objects[i][j];

            // delete objects*
            if (this->objects[i] != NULL) free(this->objects[i]);
        }

        // delete objects**
        free(this->objects);
    }

    if (this->staging_objects != NULL)
    {
        for (int i=0; i<MAX_OBJECT_TYPES; i++)
            if (this->staging_objects[i] != NULL)
                delete this->staging_objects[i];
        free(this->staging_objects);
    }
    
    if (this->indices != NULL) free(this->indices);
    if (this->maximums != NULL) free(this->maximums);
    if (this->used != NULL)
    {
        for (int i=0; i<MAX_OBJECT_TYPES; i++)
            if (this->used[i] != NULL)
                free(this->used[i]);
        free(this->used);
    }
}

} // Objects
