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
    if (this->ct == 0) return;
    ObjectState* state;
    for (int i=0; i<this->max; i++)
    {
        if (this->a[i] == NULL) continue;
        state = this->a[i]->state();
        this->a[i]->tick();
        if (state->ttl >= state->ttl_max
          || state->health <= 0)
            this->destroy(state->type, state->id);
    }
}

void GameObject_list::draw()
{
    if (this->ct == 0) return;
    for (int i=0; i<this->max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw();
}

void GameObject_list::update()
{
    if (this->ct == 0) return;
    for (int i=0; i<this->max; i++)
        if (this->a[i] != NULL)
            this->a[i]->update();
}

void GameObject_list::send_to_client(Object_types type, int client_id)
{   // TODO: use sublists to do this efficiently
    for (int i=0; i<this->max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->state()->type != type) continue;
        this->a[i]->sendToClientCreate(client_id);
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
    ObjectPolicyInterface* obj = this->a[id];
    if (obj == NULL) return;
    obj->die();
    this->occupancy[type] -= 1;
    delete obj;
    this->a[id] = NULL;
    this->ct--;
}

void GameObject_list::transfer_ownership(int owner, int new_owner)
{
    #if DC_SERVER
    if (owner == new_owner)
    {
        printf("WARNING -- %s_list::transfer_ownership -- owner == new_owner %d\n", this->name(), owner);
        return;
    }
    for (int i=0; i<this->max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->state()->get_owner() != owner) continue;
        this->a[i]->state()->set_owner(new_owner);
        alter_item_ownership_StoC msg;
        msg.owner = new_owner;
        msg.id = this->a[i]->state()->id;
        msg.type = this->a[i]->state()->type;
        msg.broadcast();
    }
    #endif
}

bool GameObject_list::point_occupied_by_type(Object_types type, int x, int y, int z)
{
    for (int i=0; i<this->max; i++)
    {
        ObjectPolicyInterface *obj = this->a[i];
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
{
    const float radius_squared = radius*radius;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;
    int closest = -1;
    int i;
    Vec3 p;
    for (i=0; i<this->max; i++)
    {
        if (this->a[i] == NULL) continue;
        p = this->a[i]->state()->get_position();
        dist = distancef_squared(x,y,z, p.x, p.y, p.z);
        if (dist < radius_squared)
        {
            // agent in sphere
            this->filtered_objects[ct] = this->a[i];
            this->filtered_object_distances[ct] = dist;
            if (dist < min_dist)
            {
                min_dist = dist;
                closest = ct;
            }
            ct++;            
        }
    }
    this->n_filtered = ct;
    return closest;
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
    if (this->ct >= this->max) return NULL;
    if (this->full(type)) return NULL;
    int i;
    int id;
    for (i=0; i<this->max; i++)
    {   // find available id
        id = (i + this->id_start) % this->max;
        if (this->a[id] == NULL) break;
    }
    if (i >= this->max) return NULL;    // no slots found (went through all ids without breaking)
    ObjectPolicyInterface* obj = create_object_of_type(type, id);
    if (obj == NULL) return NULL;
    a[id] = obj;
    this->ct++;
    this->id_start = id+1;
    this->occupancy[type] += 1;
    obj->state()->type = type;

    //TODO
    initialize_object_metadata(type, obj);
    //TODO
    
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
    if (this->ct >= this->max) return NULL;
    if (this->full(type)) return NULL;
    ObjectPolicyInterface* obj = NULL;
    if (this->a[id] == NULL)
    {   // available, create
        obj = create_object_of_type(type, id);
        if (obj == NULL) return NULL;

        this->a[id] = obj;
        this->ct++;
        this->occupancy[type] += 1;
        obj->state()->type = type;

        //TODO
        initialize_object_metadata(type, obj);
        //TODO
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
    if (id < 0 || id >= this->max)
    {
        printf("WARNING: %s_list::get() -- id %d out of range\n", name(), id);
        return NULL;
    }
    if (this->a[id] == NULL) return NULL;
    return this->a[id];
}

