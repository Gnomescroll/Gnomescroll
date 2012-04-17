#include "object_list.hpp"

#include <c_lib/entity/object/object.hpp>

namespace Objects
{

int ObjectList::get_free_id(ObjectType type)
{
    if (this->objects[type] == NULL) return -1;
    for (int i=0; i<this->maximums[type]; i++)
        if (this->objects[type][i] == NULL) return i;
    return -1;
}

int ObjectList::count(ObjectType type)
{
    return this->indices[type];
}

int ObjectList::max(ObjectType type)
{
    return this->maximums[type];
}

bool ObjectList::full(ObjectType type)
{
    return (this->count(type) >= this->max(type));
}

void ObjectList::destroy(ObjectType type, int id)
{
    if (this->objects[type] == NULL) return;
    if (this->objects[type][id] == NULL) return;
    delete this->objects[type][id];
    this->objects[type][id] = NULL;
    this->indices[type] -= 1;
}

Object* ObjectList::create(ObjectType type)
{
    int id = this->get_free_id(type);
    return this->create(type, id);
}

Object* ObjectList::create(ObjectType type, int id)
{
    if (this->objects[type] == NULL) return NULL;
    if (this->objects[type][id] != NULL)
    {
        //printf("WARNING: ObjectList::create() -- object %d,%d in use\n", type, id);
        return NULL;
    }
    this->objects[type][id] = new Object(id);
    this->objects[type][id]->type = type;
    this->indices[type] += 1;
    return this->objects[type][id];
}

void ObjectList::set_object_max(ObjectType type, int max)
{
    this->maximums[type] = max;
    this->objects[type] = (Object**)calloc(max, sizeof(Object*));
}

void ObjectList::init()
{
    this->indices = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->maximums = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->objects = (Object***)calloc(MAX_OBJECT_TYPES, sizeof(Object**));
}

void ObjectList::tick()
{
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        if (this->objects[i] == NULL) continue;
        //if (this->maximums[i] > 0 && this->objects[i][0]->tick == NULL) continue;
        for (int j=0; j<this->maximums[i]; j++)
        {
            if (this->objects[i][j] == NULL) continue;
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
        if (this->objects[i] == NULL) continue;
        //if (this->maximums[i] > 0 && this->objects[i][0]->update == NULL) continue;
        for (int j=0; j<this->maximums[i]; j++)
        {
            if (this->objects[i][j] == NULL) continue;
            Object* obj = this->objects[i][j];
            if (obj->update != NULL)
                obj->update(obj);
        }
    }
}

ObjectList::~ObjectList()
{
    if (this->objects != NULL)
    {
        for (int i=0; i<MAX_OBJECT_TYPES; i++)
            if (this->objects[i] != NULL)
                for (int j=0; j<this->maximums[i]; j++)
                    delete this->objects[i][j];
                    
        free(this->objects);
    }
    
    if (this->indices != NULL) free(this->indices);
    if (this->maximums != NULL) free(this->maximums);
}

} // Objects
