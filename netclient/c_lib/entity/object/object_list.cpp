#include "object_list.hpp"

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/components/health.hpp>
#include <c_lib/entity/object/object_data_list.hpp>

namespace Objects
{

int ObjectList::get_free_id(ObjectType type)
{
    if (this->objects[type] == NULL) return -1;
    for (int i=0; i<this->maximums[type]; i++)
        if (!this->used[type]) return i;
    return -1;
}

void ObjectList::set_object_id(Object* object, int id)
{
    ObjectType type = object->type;
    if (object->id >= 0)    // swap from current
        this->objects[type][object->id] = NULL;
    if (this->objects[type][id] != NULL) printf("WARNING -- Object_list::set_object_id -- putting object in occupied slot. Memory leak will occur\n");
    this->objects[type][id] = object;
    object->id = id;
}


int ObjectList::count(ObjectType type)
{
    return this->indices[type];
}

int ObjectList::max(ObjectType type)
{
    return this->maximums[type];
}

bool ObjectList::empty(ObjectType type)
{
    return (this->count(type) == 0);
}

bool ObjectList::full(ObjectType type)
{
    return (this->count(type) >= this->max(type));
}

void ObjectList::destroy(ObjectType type, int id)
{
    if (this->used[type] == NULL) return;
    if (!this->used[type][id]) return;
    //delete this->objects[type][id];
    this->used[type][id] = 0;
    this->indices[type] -= 1;
}

Object* ObjectList::get(ObjectType type, int id)
{
    if (this->objects[type] == NULL) return NULL;
    return this->objects[type][id];
}

Object* ObjectList::create(ObjectType type)
{
    int id = this->get_free_id(type);
    return this->create(type, id);
}

Object* ObjectList::create(ObjectType type, int id)
{
    if (this->used[type] == NULL) return NULL;
    if (this->used[type][id])
    {
        //printf("WARNING: ObjectList::create() -- object %d,%d in use\n", type, id);
        return NULL;
    }
    //this->objects[type][id] = new Object(id);
    //this->objects[type][id]->type = type;
    this->indices[type] += 1;
    this->used[type][id] = 1;
    return this->objects[type][id];
}

Object** ObjectList::get_objects(ObjectType type)
{
    assert(type < MAX_OBJECT_TYPES);
    assert(type >= 0);
    return this->objects[type];
}

char* ObjectList::get_used(ObjectType type)
{
    assert(type < MAX_OBJECT_TYPES);
    assert(type >= 0);
    return this->used[type];
}

void ObjectList::set_object_max(ObjectType type, int max)
{
    assert(type < MAX_OBJECT_TYPES);
    assert(type >= 0);
    this->maximums[type] = max;
    this->objects[type] = (Object**)calloc(max, sizeof(Object*));
    this->used[type] = (char*)calloc(max, sizeof(char));
}

void ObjectList::load_object_data(ObjectDataList* data)
{   // preallocate component pointer buffers
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
        for (int j=0; j<this->maximums[i]; j++)
        {
            this->objects[i][j] = new Object(j);
            this->objects[i][j]->init(data->get_component_count((ObjectType)i));
            this->objects[i][j]->type = (ObjectType)i;
        }
}

void ObjectList::init()
{
    this->indices = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->maximums = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->used = (char**)calloc(MAX_OBJECT_TYPES, sizeof(char*));
    this->objects = (Object***)calloc(MAX_OBJECT_TYPES, sizeof(Object**));
}

void ObjectList::tick()
{
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        if (this->used[i] == NULL) continue;
        //if (this->maximums[i] > 0 && this->objects[i][0]->tick == NULL) continue;
        for (int j=0; j<this->maximums[i]; j++)
        {
            if (!this->used[i][j]) continue;
            Object* obj = this->objects[i][j];
            if (obj->tick != NULL)
                obj->tick(obj);
        }
    }
}

void ObjectList::update()
{
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        if (this->used[i] == NULL) continue;
        //if (this->maximums[i] > 0 && this->objects[i][0]->update == NULL) continue;
        for (int j=0; j<this->maximums[i]; j++)
        {
            if (!this->used[i][j]) continue;
            Object* obj = this->objects[i][j];
            if (obj->update != NULL)
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
        //if (this->maximums[i] > 0 && this->objects[i][0]->update == NULL) continue;
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

void ObjectList::send_to_client(ObjectType type, int client_id)
{
    assert(type < MAX_OBJECT_TYPES);
    assert(type >= 0);
    if (this->empty(type)) return;
    Object** objects = this->get_objects(type);
    assert(objects != NULL);
    int max = this->max(type);
    assert(max > 0);

    for (int i=0; i<max; i++)
        if (this->used[type][i])
            objects[i]->sendToClientCreate(client_id);
}

ObjectList::~ObjectList()
{
    if (this->objects != NULL)
    {
        for (int i=0; i<MAX_OBJECT_TYPES; i++)
            if (this->used[i] != NULL)
                for (int j=0; j<this->maximums[i]; j++)
                    delete this->objects[i][j];
                    
        free(this->objects);
    }
    
    if (this->indices != NULL) free(this->indices);
    if (this->maximums != NULL) free(this->maximums);
    if (this->used != NULL) free(this->used);
}

} // Objects
