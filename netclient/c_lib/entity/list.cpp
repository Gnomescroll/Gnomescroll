#include "list.hpp"

#include <c_lib/agent/net_agent.hpp>
#include <c_lib/items/items.hpp>
#include <net_lib/net.hpp>

// TODO -- find better way to do this
void initialize_object_metadata(Object_types type, ObjectPolicyInterface* obj)
{
    ObjectState* state = obj->state();
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            state->texture_index = ItemDrops::GRENADE_REFILL_TEXTURE_ID;
            state->texture_scale = ItemDrops::GRENADE_REFILL_TEXTURE_SCALE;
            state->mass = ItemDrops::GRENADE_REFILL_MASS;
            state->ttl_max = ItemDrops::GRENADE_REFILL_TTL;
            state->damp = ItemDrops::GRENADE_REFILL_DAMP;
            break;

        case OBJ_TYPE_LASER_REFILL:
            state->texture_index = ItemDrops::LASER_REFILL_TEXTURE_ID;
            state->texture_scale = ItemDrops::LASER_REFILL_TEXTURE_SCALE;
            state->mass = ItemDrops::LASER_REFILL_MASS;
            state->ttl_max = ItemDrops::LASER_REFILL_TTL;
            state->damp = ItemDrops::LASER_REFILL_DAMP;
            break;

        default: return;
    }
}

void GameObject_list::tick()
{
    ObjectPolicyInterface* obj;
    ObjectState* state;
    for (int type=0; type<this->max_objects; type++)
    {
        if (this->occupancy[type] == 0) continue;
        int max = this->get_object_max((Object_types)type);
        for (int i=0; i<max; i++)
        {
            obj = this->objects[type][i];
            if (obj == NULL) continue;
            state = obj->state();
            obj->tick();
            if (state->ttl >= state->ttl_max
              || state->health <= 0)
                this->destroy(state->type, state->id);
        }
    }
}

void GameObject_list::draw()
{
    for (int type=0; type<this->max_objects; type++)
    {
        if (this->occupancy[type] == 0) continue;
        int max = this->get_object_max((Object_types)type);
        for (int i=0; i<max; i++)
            if (this->objects[type][i] != NULL)
                this->objects[type][i]->draw();
    }
}

void GameObject_list::update()
{
    for (int type=0; type<this->max_objects; type++)
    {
        if (this->occupancy[type] == 0) continue;
        int max = this->get_object_max((Object_types)type);
        for (int i=0; i<max; i++)
            if (this->objects[type][i] != NULL)
                this->objects[type][i]->update();
    }
}

void GameObject_list::send_to_client(Object_types type, int client_id)
{
    if (this->occupancy[type] == 0) return;
    int max = this->get_object_max(type);
    ObjectPolicyInterface* obj;
    for (int i=0; i<max; i++)
    {
        obj = this->objects[type][i];
        if (obj == NULL) continue;
        if (obj->state()->type != type) continue;
        obj->sendToClientCreate(client_id);
    }
}

bool GameObject_list::full(Object_types type)
{
    if (this->occupancy[type] >= this->max_occupancy[type])
        return true;
    return false;
}

void GameObject_list::destroy(Object_types type, int id)
{
    ObjectPolicyInterface* obj = this->objects[type][id];
    if (obj == NULL) return;
    if (obj->state()->type != type)
    {
        printf("WARNING: GameObject_list::destroy() -- type provided does not object type. id: %d, type: %d\n", id, type);
        return;
    }
    obj->die();
    this->occupancy[type] -= 1;
    delete obj;
    this->objects[type][id] = NULL;
}

void GameObject_list::transfer_ownership(int owner, int new_owner)
{   // TODO -- MAKE OWNERSHIP LIST
    #if DC_SERVER
    if (owner == new_owner)
    {
        printf("WARNING -- %s_list::transfer_ownership -- owner == new_owner %d\n", this->name(), owner);
        return;
    }
    ObjectState* state;
    for (int type=0; type<this->max_objects; type++)
    {
        if (this->occupancy[type] == 0) continue;
        int max = this->get_object_max((Object_types)type);
        for (int i=0; i<max; i++)
        {
            if (this->objects[type][i] == NULL) continue;
            state = this->objects[type][i]->state();
            if (state->get_owner() != owner) continue;
            state->set_owner(new_owner);
            alter_item_ownership_StoC msg;
            msg.owner = new_owner;
            msg.id = state->id;
            msg.type = state->type;
            msg.broadcast();
        }
    }
    #endif
}

bool GameObject_list::point_occupied_by_type(Object_types type, int x, int y, int z)
{
    if (this->occupancy[type] == 0) return false;
    int max = this->get_object_max(type);
    for (int i=0; i<max; i++)
    {
        ObjectPolicyInterface *obj = this->objects[type][i];
        if (obj == NULL) continue;
        ObjectState* state = obj->state();
        if (state->type != type) continue;
        Vec3 p = state->get_position();
        if ((int)p.x == x && (int)p.y == y)
            for (int j=0; j<(int)ceil(state->height); j++)
                if ((int)p.z+j == z)
                    return true;
    }
    return false;
}

int GameObject_list::objects_within_sphere(float x, float y, float z, float radius)
{   // MAJOR TODO -- MAKE ITS OWN SUBSCRIPTION LIST
    this->n_filtered = 0;
    return 0;
    
    //const float radius_squared = radius*radius;
    //int ct = 0;
    //float dist;
    //float min_dist = 10000000.0f;
    //int closest = -1;
    //int i;
    //Vec3 p;
    //for (i=0; i<this->max; i++)
    //{
        //if (this->a[i] == NULL) continue;
        //p = this->a[i]->state()->get_position();
        //dist = distancef_squared(x,y,z, p.x, p.y, p.z);
        //if (dist < radius_squared)
        //{
            //// agent in sphere
            //this->filtered_objects[ct] = this->a[i];
            //this->filtered_object_distances[ct] = dist;
            //if (dist < min_dist)
            //{
                //min_dist = dist;
                //closest = ct;
            //}
            //ct++;            
        //}
    //}
    //this->n_filtered = ct;
    //return closest;
}

/* Creation API */

// Note: not attached to GameObject_list
ObjectPolicyInterface* create_object_of_type(Object_types type, int id)
{
    ObjectPolicyInterface* obj = NULL;
    switch (type)
    {
        case OBJ_TYPE_TURRET:
            obj = new Turret(id);
            break;
        case OBJ_TYPE_SPAWNER:
            obj = new Spawner(id);
            break;
        case OBJ_TYPE_GRENADE_REFILL:
        case OBJ_TYPE_LASER_REFILL:
            obj = new ItemDrops::PickupObject(id);
            break;
        default: return NULL;
    }
    return obj;
}


ObjectPolicyInterface* GameObject_list::create(Object_types type)
{
    if (this->full(type)) return NULL;
    int i;
    int id;
    int max = this->get_object_max(type);
    int id_start = this->get_index_start(type);
    for (i=0; i<max; i++)
    {   // find available id
        id = (i + id_start) % max;
        if (this->objects[type][id] == NULL) break;
    }
    if (i >= max) return NULL;    // no slots found (went through all ids without breaking)
    ObjectPolicyInterface* obj = create_object_of_type(type, id);
    if (obj == NULL) return NULL;
    this->objects[type][id] = obj;
    this->index_start[type] = id+1;
    this->occupancy[type] += 1;
    obj->state()->type = type;
    initialize_object_metadata(type, obj);
    
    return obj;
}

ObjectPolicyInterface* GameObject_list::create(Object_types type, float x, float y, float z)
{
    ObjectPolicyInterface* obj = this->create(type);
    if (obj != NULL)
        obj->state()->set_position(x,y,z);
    return obj;
}

ObjectPolicyInterface* GameObject_list::create(Object_types type, float x, float y, float z, float mx, float my, float mz)
{
    ObjectPolicyInterface* obj = this->create(type, x,y,z);
    if (obj != NULL)
        obj->state()->set_momentum(mx,my,mz);
    return obj;
}

ObjectPolicyInterface* GameObject_list::create(Object_types type, int id)
{    
    ObjectPolicyInterface* obj = NULL;
    if (this->objects[type][id] == NULL)
    {   // available, create
        obj = create_object_of_type(type, id);
        if (obj == NULL) return NULL;

        this->objects[type][id] = obj;
        this->occupancy[type] += 1;
        obj->state()->type = type;
        initialize_object_metadata(type, obj);
    }
    else
        printf("%s_list: Cannot create object from id: id is in use: %d\n", name(), id);
    return obj;
}

ObjectPolicyInterface* GameObject_list::create(Object_types type, int id, float x, float y, float z)
{
    ObjectPolicyInterface* obj = this->create(type, id);
    if (obj != NULL)
        obj->state()->set_position(x,y,z);
    return obj;
}

ObjectPolicyInterface* GameObject_list::create(Object_types type, int id, float x, float y, float z, float mx, float my, float mz)
{
    ObjectPolicyInterface* obj = this->create(type, id, x,y,z);
    if (obj != NULL)
        obj->state()->set_momentum(mx,my,mz);
    return obj;
}

/* Getter */
ObjectPolicyInterface* GameObject_list::get(Object_types type, int id)
{
    int max = this->get_object_max(type);
    if (id < 0 || id >= max)
    {
        printf("WARNING: %s_list::get() -- id %d out of range for type %d\n", name(), id, type);
        return NULL;
    }
    return this->objects[type][id];
}

