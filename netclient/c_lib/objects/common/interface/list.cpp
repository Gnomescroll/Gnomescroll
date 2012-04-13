#include "list.hpp"

#include <c_lib/agent/net_agent.hpp>
#include <c_lib/items/items.hpp>
#include <net_lib/net.hpp>
#include <c_lib/items/pickup.hpp>
#include <c_lib/items/inventory/inventory.hpp>
#include <c_lib/monsters/monsters.hpp>


//Inventory* create_inventory_of_type(InventoryTypes type, int id)
//{
    //Inventory* inventory = NULL;
    //switch (type)
    //{
        //case INVENTORY_TYPE_AGENT:
            //inventory = new AgentInventory(id);
            //break;
            
        ////case INVENTORY_TYPE_TMAP:
            ////inventory = new TMapInventory(id);
            ////break;
        ////case INVENTORY_TYPE_TITEM:
            ////inventory = new TItemInventory(id);
            ////break;

        //default: break;
    //}
    //return inventory;
//}

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

        case OBJ_TYPE_MEAT:
        //case OBJ_TYPE_MALACHITE:
        //case OBJ_TYPE_RUBY:
        //case OBJ_TYPE_TURQUOISE:
        //case OBJ_TYPE_SILVER:
        //case OBJ_TYPE_AMETHYST:
        //case OBJ_TYPE_JADE:
        //case OBJ_TYPE_ONYX:
        //case OBJ_TYPE_GRENADE_REFILL:
        //case OBJ_TYPE_LASER_REFILL:
        case OBJ_TYPE_GEMSTONE:
        case OBJ_TYPE_REFILL:
            obj = new ItemDrops::PickupObjectSprite(type, id);  // TODO
            break;
        case OBJ_TYPE_DIRT:
        case OBJ_TYPE_STONE:
            obj = new ItemDrops::PickupObjectMinivox(type, id);
            break;

        case OBJ_TYPE_INVENTORY:
            //obj = create_inventory_of_type((InventoryTypes)subtype, id);
            obj = new Inventory(id);
            break;

        case OBJ_TYPE_SLIME:
            obj = new Monsters::Slime(id);
            break;
        case OBJ_TYPE_MONSTER_BOX:
            obj = new Monsters::Box(id);
            break;
        case OBJ_TYPE_MONSTER_SPAWNER:
            obj = new Monsters::MonsterSpawner(id);
            break;
            
        default: break;
    }
    return obj;
}


void GameObject_list::tick()
{
    ObjectPolicyInterface* obj;
    ObjectState* state;
    for (int type=0; type<this->max_objects; type++)
    {
        if (this->occupancy[type] <= 0) continue;
        int max = this->get_object_max((Object_types)type);
        for (int i=0; i<max; i++)
        {
            obj = this->objects[type][i];
            if (obj == NULL) continue;
            state = obj->state();
            obj->tick();
            if (state->ttl >= state->ttl_max
              || obj->is_dead())
                this->destroy(state->type, state->id);
        }
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

bool GameObject_list::point_occupied_by_type(Object_types type, int x, int y, int z)
{   // TODO -- should be on spatial list
    if (this->occupancy[type] == 0) return false;
    int max = this->get_object_max(type);
    ObjectPolicyInterface *obj;
    ObjectState* state;

    for (int i=0; i<max; i++)
    {
        obj = this->objects[type][i];
        if (obj == NULL) continue;
        state = obj->state();
        if (state->type != type) continue;
        Vec3 p = obj->get_position();
        int px = (int)p.x;
        int py = (int)p.y;
        if (px != x || py != y) continue;
        int pz = (int)p.z;
        int height = (int)ceil(obj->get_height());
        for (int j=0; j<height; j++)
            if (pz + j == z)
                return true;
    }
    return false;
}

int GameObject_list::all_objects_within_sphere(float x, float y, float z, float radius)
{   // MAJOR TODO -- MAKE ITS OWN SUBSCRIPTION LIST
    const float radius_squared = radius*radius;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;
    int closest = -1;
    Vec3 p;

    for (int type=0; type<MAX_OBJECT_TYPE; type++)
    {
        if (ct >= this->max_filtered) break;
        if (this->objects[type] == NULL) continue;
        if (this->occupancy[type] == 0) continue;
        int max = this->max_occupancy[type];
        for (int i=0; i<max; i++)
        {
            if (ct >= this->max_filtered) break;
            if (this->objects[type][i] == NULL) continue;
            p = this->objects[type][i]->get_position();
            dist = distancef_squared(x,y,z, p.x, p.y, p.z);
            if (dist < radius_squared)
            {   // object is in sphere
                this->filtered_objects[ct] = this->objects[type][i];
                this->filtered_object_distances[ct] = dist;
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest = ct;
                }
                ct++;            
            }
        }
    }
    
    this->n_filtered = ct;
    return closest;
}

int GameObject_list::objects_within_sphere(const Object_types type, float x, float y, float z, float radius)
{
    if (this->objects[type] == NULL) return 0;
    if (this->occupancy[type] == 0) return 0;

    const float radius_squared = radius*radius;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;
    int closest = -1;
    Vec3 p;

    int max = this->max_occupancy[(int)type];
    for (int i=0; i<max; i++)
    {
        if (this->objects[type][i] == NULL) continue;
        p = this->objects[type][i]->get_position();
        dist = distancef_squared(x,y,z, p.x, p.y, p.z);
        if (dist < radius_squared)
        {   // object is in sphere
            this->filtered_objects[ct] = this->objects[type][i];
            this->filtered_object_distances[ct] = dist;
            if (dist < min_dist)
            {
                min_dist = dist;
                closest = ct;
            }
            ct++;
            if (ct >= this->max_filtered) break;
        }
    }
    
    this->n_filtered = ct;
    return closest;
}

// array of types to filter
int GameObject_list::objects_within_sphere(const Object_types* types, const int n_types, float x, float y, float z, float radius)
{
    const float radius_squared = radius*radius;
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;
    int closest = -1;
    Vec3 p;

    for (int j=0; j<n_types; j++)
    {
        if (ct >= this->max_filtered) break;
        Object_types type = types[j];
        if (this->objects[type] == NULL) continue;
        if (this->occupancy[type] == 0) continue;
        int max = this->max_occupancy[(int)type];
        for (int i=0; i<max; i++)
        {
            if (ct >= this->max_filtered) break;
            if (this->objects[type][i] == NULL) continue;
            p = this->objects[type][i]->get_position();
            dist = distancef_squared(x,y,z, p.x, p.y, p.z);
            if (dist < radius_squared)
            {   // object is in sphere
                this->filtered_objects[ct] = this->objects[type][i];
                this->filtered_object_distances[ct] = dist;
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest = ct;
                }
                ct++;
            }
        }
    }
    
    this->n_filtered = ct;
    return closest;
}

int GameObject_list::filter_active_objects(const Object_types type)
{
    this->n_filtered = 0;
    int ct = 0;
    if (this->get_object_count(type) <= 0) return 0;
    int max = this->get_object_max(type);
    if (max <= 0) return 0;

    for (int i=0; i<max; i++)
    {
        if (this->objects[type][i] == NULL) continue;
        this->filtered_objects[ct++] = this->objects[type][i];
    }
    this->n_filtered = ct;
    return ct;
}

/* Creation API */

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
    }
    else
        printf("%s_list: Cannot create object type %d, id %d is in use\n", name(), type, id);
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

void init_gameobject_list_maximums(GameObject_list* list)
{
    if (list == NULL)
    {
        printf("WARNING: init_gameobject_list_maximums() -- list is NULL\n");
        return;
    }
    list->set_max_occupancy(OBJ_TYPE_SPAWNER, SPAWNER_MAX);
    list->set_max_occupancy(OBJ_TYPE_TURRET, TURRET_MAX);
    
    list->set_max_occupancy(OBJ_TYPE_INVENTORY, INVENTORY_MAX);
    
    //list->set_max_occupancy(OBJ_TYPE_GRENADE_REFILL, ItemDrops::GRENADE_REFILL_MAX);
    //list->set_max_occupancy(OBJ_TYPE_LASER_REFILL, ItemDrops::LASER_REFILL_MAX);
    list->set_max_occupancy(OBJ_TYPE_REFILL, ItemDrops::REFILL_MAX);
    list->set_max_occupancy(OBJ_TYPE_DIRT, ItemDrops::DIRT_MAX);
    list->set_max_occupancy(OBJ_TYPE_STONE, ItemDrops::STONE_MAX);
    list->set_max_occupancy(OBJ_TYPE_MEAT, ItemDrops::MEAT_MAX);
    
    //list->set_max_occupancy(OBJ_TYPE_MALACHITE, ItemDrops::GEMSTONE_MAX);
    //list->set_max_occupancy(OBJ_TYPE_RUBY, ItemDrops::GEMSTONE_MAX);
    //list->set_max_occupancy(OBJ_TYPE_TURQUOISE, ItemDrops::GEMSTONE_MAX);
    //list->set_max_occupancy(OBJ_TYPE_SILVER, ItemDrops::GEMSTONE_MAX);
    //list->set_max_occupancy(OBJ_TYPE_AMETHYST, ItemDrops::GEMSTONE_MAX);
    //list->set_max_occupancy(OBJ_TYPE_JADE, ItemDrops::GEMSTONE_MAX);
    //list->set_max_occupancy(OBJ_TYPE_ONYX, ItemDrops::GEMSTONE_MAX);
    list->set_max_occupancy(OBJ_TYPE_GEMSTONE, ItemDrops::GEMSTONE_MAX);

    // mobs
    list->set_max_occupancy(OBJ_TYPE_SLIME, Monsters::SLIME_MAX);
    list->set_max_occupancy(OBJ_TYPE_MONSTER_BOX, Monsters::BOX_MAX);
    list->set_max_occupancy(OBJ_TYPE_MONSTER_SPAWNER, Monsters::MONSTER_SPAWNER_MAX);

}
