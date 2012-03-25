#include "list.hpp"

#include <c_lib/items/pickup.hpp>
#include <c_lib/items/refills.hpp>

void GameObject_list::tick()
{
    if (this->num == 0) return;
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
        {
            this->a[i]->tick();
            if (this->a[i]->state()->ttl >= this->a[i]->state()->ttl_max)
            {
                this->a[i]->die();
                this->destroy(this->a[i]->state()->id);
            }
        }
}

void GameObject_list::draw()
{
    if (this->num == 0) return;
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw();
}

// TODO: restructure the list creation to adapt based on type
ObjectPolicyInterface* GameObject_list::create(float x, float y, float z, float mx, float my, float mz, Object_types type)
{
    int id = this->get_free_id();
    if (id < 0) return NULL;
    this->num++;
    this->id_c = id+1;

    ItemDrops::PickupObject* obj = new ItemDrops::PickupObject(id, x,y,z, mx,my,mz);
    this->a[id] = obj;

    float texture_scale, mass, damp;
    int texture_index, ttl_max;
    
    obj->state()->type = type;
    switch (type)
    {   // TODO: THIS WILL BE REPLACED BY ObjectType
        case OBJ_TYPE_GRENADE_REFILL:
            texture_index = ItemDrops::GRENADE_REFILL_TEXTURE_ID;
            texture_scale = ItemDrops::GRENADE_REFILL_TEXTURE_SCALE;
            mass = ItemDrops::GRENADE_REFILL_MASS;
            ttl_max = ItemDrops::GRENADE_REFILL_TTL;
            damp = ItemDrops::GRENADE_REFILL_DAMP;
            break;
        case OBJ_TYPE_LASER_REFILL:
            texture_index = ItemDrops::LASER_REFILL_TEXTURE_ID;
            texture_scale = ItemDrops::LASER_REFILL_TEXTURE_SCALE;
            mass = ItemDrops::LASER_REFILL_MASS;
            ttl_max = ItemDrops::LASER_REFILL_TTL;
            damp = ItemDrops::LASER_REFILL_DAMP;
            break;
        default:
            printf("WARNING: %s create() -- unhandled object type %d\n", name(), type);
            return NULL;
    };
    
    obj->state()->texture_index = texture_index;
    obj->state()->texture_scale = texture_scale;
    obj->state()->mass = mass;
    obj->state()->ttl_max = ttl_max;
    obj->state()->damp = damp;
    obj->born();

    return obj;
}

void GameObject_list::destroy(int id)
{
    ObjectPolicyInterface* obj = this->a[id];
    if (obj == NULL) return;
    obj->die();
    Object_list<ObjectPolicyInterface, GAME_OBJECTS_MAX>::destroy(id);
}
