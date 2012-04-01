#include "list.hpp"

#include <c_lib/agent/net_agent.hpp>
#include <c_lib/items/items.hpp>
#include <net_lib/net.hpp>

void GameObject_list::tick()
{
    if (this->num == 0) return;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->tick();
        if (this->a[i]->state()->ttl >= this->a[i]->state()->ttl_max
          || this->a[i]->state()->health <= 0)
            this->destroy(this->a[i]->state()->id);
    }
}

void GameObject_list::draw()
{
    if (this->num == 0) return;
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw();
}

void GameObject_list::update()
{
    if (this->num == 0) return;
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->update();
}

void GameObject_list::send_to_client(Object_types type, int client_id)
{   // TODO: use sublists to do this efficiently
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->state()->type != type) continue;
        this->a[i]->sendToClientCreate(client_id);
    }
}

// TODO: restructure the list creation to adapt based on type
ObjectPolicyInterface* GameObject_list::create(float x, float y, float z, float mx, float my, float mz, Object_types type)
{
    if (this->full(type)) return NULL;
    int id = this->get_free_id();
    if (id < 0) return NULL;
    this->num++;
    this->id_c = id+1;

    //ItemDrops::PickupObject* obj = new ItemDrops::PickupObject(id, x,y,z, mx,my,mz);

    float texture_scale, mass, damp;
    int texture_index, ttl_max;

    // TODO: REPLACE WITH SOMETHING LIKE ObjectType/Data.
    //ObjectData* data = get_object_metadata(type);
    //ObjectPolicyInterface* obj = create_object(type);
    //obj->load_data(data);
    //obj->born();
    // data passed to a copy method
    // 

    ObjectPolicyInterface* obj;
    
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            obj = new ItemDrops::PickupObject(id, x,y,z, mx,my,mz);
            texture_index = ItemDrops::GRENADE_REFILL_TEXTURE_ID;
            texture_scale = ItemDrops::GRENADE_REFILL_TEXTURE_SCALE;
            mass = ItemDrops::GRENADE_REFILL_MASS;
            ttl_max = ItemDrops::GRENADE_REFILL_TTL;
            damp = ItemDrops::GRENADE_REFILL_DAMP;
            obj->state()->texture_index = texture_index;
            obj->state()->texture_scale = texture_scale;
            obj->state()->mass = mass;
            obj->state()->ttl_max = ttl_max;
            obj->state()->damp = damp;
            break;
            
        case OBJ_TYPE_LASER_REFILL:
            obj = new ItemDrops::PickupObject(id, x,y,z, mx,my,mz);
            texture_index = ItemDrops::LASER_REFILL_TEXTURE_ID;
            texture_scale = ItemDrops::LASER_REFILL_TEXTURE_SCALE;
            mass = ItemDrops::LASER_REFILL_MASS;
            ttl_max = ItemDrops::LASER_REFILL_TTL;
            damp = ItemDrops::LASER_REFILL_DAMP;
            obj->state()->texture_index = texture_index;
            obj->state()->texture_scale = texture_scale;
            obj->state()->mass = mass;
            obj->state()->ttl_max = ttl_max;
            obj->state()->damp = damp;
            break;
            
        case OBJ_TYPE_TURRET:
            obj = new Turret(id, x,y,z,mx,my,mz);
            break;
        case OBJ_TYPE_SPAWNER:
            obj = new Spawner(id, x,y,z,mx,my,mz);
            break;
            
        default:
            printf("WARNING: %s create() -- unhandled object type %d\n", name(), type);
            return NULL;
    };

    obj->state()->type = type;
    this->a[id] = obj;
    this->occupancy[type] += 1;
    
    return obj;
}

ObjectPolicyInterface* GameObject_list::create(int id, float x, float y, float z, float mx, float my, float mz, Object_types type)
{
    if (this->a[id] != NULL) return NULL;
    if (this->full(type)) return NULL;
    this->num++;

    float texture_scale, mass, damp;
    int texture_index, ttl_max;

    ObjectPolicyInterface* obj;
    
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            obj = new ItemDrops::PickupObject(id, x,y,z, mx,my,mz);
            texture_index = ItemDrops::GRENADE_REFILL_TEXTURE_ID;
            texture_scale = ItemDrops::GRENADE_REFILL_TEXTURE_SCALE;
            mass = ItemDrops::GRENADE_REFILL_MASS;
            ttl_max = ItemDrops::GRENADE_REFILL_TTL;
            damp = ItemDrops::GRENADE_REFILL_DAMP;
            obj->state()->texture_index = texture_index;
            obj->state()->texture_scale = texture_scale;
            obj->state()->mass = mass;
            obj->state()->ttl_max = ttl_max;
            obj->state()->damp = damp;
            break;
            
        case OBJ_TYPE_LASER_REFILL:
            obj = new ItemDrops::PickupObject(id, x,y,z, mx,my,mz);
            texture_index = ItemDrops::LASER_REFILL_TEXTURE_ID;
            texture_scale = ItemDrops::LASER_REFILL_TEXTURE_SCALE;
            mass = ItemDrops::LASER_REFILL_MASS;
            ttl_max = ItemDrops::LASER_REFILL_TTL;
            damp = ItemDrops::LASER_REFILL_DAMP;
            obj->state()->texture_index = texture_index;
            obj->state()->texture_scale = texture_scale;
            obj->state()->mass = mass;
            obj->state()->ttl_max = ttl_max;
            obj->state()->damp = damp;
            break;
            
        case OBJ_TYPE_TURRET:
            obj = new Turret(id, x,y,z,mx,my,mz);
            break;
        case OBJ_TYPE_SPAWNER:
            obj = new Spawner(id, x,y,z,mx,my,mz);
            break;

        default:
            printf("WARNING: %s create() -- unhandled object type %d\n", name(), type);
            return NULL;
    };
    
    obj->state()->type = type;
    this->a[id] = obj;
    this->occupancy[type] += 1;

    return obj;
}

void GameObject_list::destroy(int id)
{
    ObjectPolicyInterface* obj = this->a[id];
    if (obj == NULL) return;
    obj->die();
    this->occupancy[obj->state()->type] -= 1;
    Object_list<ObjectPolicyInterface, GAME_OBJECTS_MAX>::destroy(id);
}


void GameObject_list::alter_owner(int owner, int new_owner)
{
    #if DC_SERVER
    if (owner == new_owner)
    {
        printf("WARNING -- %s_list::alter_owner -- owner == new_owner %d\n", this->name(), owner);
        return;
    }
    for (int i=0; i<this->n_max; i++)
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
    for (int i=0; i<this->n_max; i++)
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
    for (i=0; i<this->n_max; i++)
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

bool GameObject_list::full(Object_types type)
{
    if (this->occupancy[type] >= this->max_occupancy[type])
        return true;
    return false;
}


/* SHIT FROM SPAWNER INTERFACE */

/* MAJOR TODO -- FACTOR THIS OUT, OR LIST WILL BECOME BLOB */

bool GameObject_list::team_spawner_available(int team)
{
    int ct = 0;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->state()->type != OBJ_TYPE_SPAWNER) continue;
        if (this->a[i]->state()->get_team() == team) ct++;
    }
    return (ct < SPAWNERS_PER_TEAM);
}

int GameObject_list::get_random_spawner(int team)
{
    int spawners[SPAWNERS_PER_TEAM+1];
    int j=0;
    for (int i=0; i<this->n_max; i++)
    {   // filter down to team's spawners
        ObjectPolicyInterface *s = this->a[i];
        if (s == NULL) continue;
        if (s->state()->type != OBJ_TYPE_SPAWNER) continue;
        if (s->state()->get_team() == team) spawners[j++] = s->state()->team_index;
    }
    spawners[j++] = BASE_SPAWN_ID;
    return spawners[randrange(0,j-1)];
}

// when a player says "spawner 8" he may be on the other team
// we need to find the 8th spawner for his team
int GameObject_list::get_numbered_team_spawner(int team, int id)
{
    for (int i=0; i<this->n_max; i++)
    {
        ObjectPolicyInterface *s = this->a[i];
        if (s == NULL) continue;
        if (s->state()->type != OBJ_TYPE_SPAWNER) continue;
        if (s->state()->get_team() != team) continue;
        if ((int)s->state()->team_index == id)
        {
            return s->state()->id;
        }
    }
    return BASE_SPAWN_ID;
}

ObjectPolicyInterface* GameObject_list::get_by_team_index(int team, int team_index)
{
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->state()->type != OBJ_TYPE_SPAWNER) continue;
        if (this->a[i]->state()->get_team() != team) continue;
        if ((int)this->a[i]->state()->team_index == team_index)
            return this->a[i];
    }
    return NULL;
}

bool GameObject_list::spawner_exists(int team, int team_index)
{
    if (this->get_by_team_index(team, team_index) != NULL)
        return true;
    return false;
}

void GameObject_list::assign_team_index(ObjectPolicyInterface* spawner)
{   // pick an index for the spawner that is available, these are separate from
    // id because each team's set of spawners has its own indexing
    // and spawners may be destroyed; we dont want to renumber every time

    // get smallest available team index
    int taken[SPAWNER_MAX] = {0};
    for (int i=0; i<this->n_max; i++)
    {
        ObjectPolicyInterface* s = this->a[i];
        if (s == NULL) continue;
        if (s->state()->type != OBJ_TYPE_SPAWNER) continue;
        if (s->state()->get_team() != spawner->state()->get_team()) continue;
        if (s->state()->team_index != TEAM_INDEX_NONE && s->state()->team_index != 0)  // should never be 0
            taken[s->state()->team_index - 1] = 1;
    }
    for (int i=0; i<SPAWNER_MAX; i++)
        if (!taken[i])
        {
            spawner->state()->team_index = i+1;
            return;
        }
    printf("failed to get team index\n");
    spawner->state()->team_index = TEAM_INDEX_NONE;
}
