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
        ObjectState* state = this->a[i]->state();
        if (state->type != type) continue;
        object_create_StoC_model msg;
        this->a[i]->create_message(&msg);
        ((PacketInterface*)&msg)->sendToClient(client_id);
    }
}

// TODO: restructure the list creation to adapt based on type
ObjectPolicyInterface* GameObject_list::create(float x, float y, float z, float mx, float my, float mz, Object_types type)
{
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
            
        default:
            printf("WARNING: %s create() -- unhandled object type %d\n", name(), type);
            return NULL;
    };
    
    obj->state()->type = type;

    this->a[id] = obj;

    return obj;
}

ObjectPolicyInterface* GameObject_list::create(int id, float x, float y, float z, float mx, float my, float mz, Object_types type)
{
    if (this->a[id] != NULL) return NULL;
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
            
        default:
            printf("WARNING: %s create() -- unhandled object type %d\n", name(), type);
            return NULL;
    };
    
    obj->state()->type = type;

    this->a[id] = obj;

    return obj;
}

void GameObject_list::destroy(int id)
{
    ObjectPolicyInterface* obj = this->a[id];
    if (obj == NULL) return;
    obj->die();
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
