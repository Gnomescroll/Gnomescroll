#include "grenades.hpp"

#include <c_lib/physics/verlet.hpp>

#include <net_lib/net.hpp>

namespace ItemDrops
{

class item_create_StoC: public FixedSizeNetPacketToClient<item_create_StoC>
{
    public:
        uint8_t type;
        uint8_t id;
        float x,y,z,mx,my,mz;   // TODO -- encode velocity in a single param -- its just random seed direction
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
        }
        inline void handle();
};

class item_destroy_StoC: public FixedSizeNetPacketToClient<item_destroy_StoC>
{
    public:
        uint8_t type;
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

#if DC_CLIENT
inline void item_create_StoC::handle()
{
    switch (type)
    {
        case OBJ_TYPE_GRENADE_DROP:
            ClientState::grenade_drops_list->create(id, x,y,z, mx,my,mz);
            break;
        default: return;
    }
}

inline void item_destroy_StoC::handle()
{
    switch (type)
    {
        case OBJ_TYPE_GRENADE_DROP:
            ClientState::grenade_drops_list->destroy(id);
            break;
        default: return;
    }
}
#endif

#if DC_SERVER
inline void item_create_StoC::handle() {}
inline void item_destroy_StoC::handle() {}
#endif



int PickupItem::nearest_agent_in_range(Vec3 p)
{
    int n = STATE::agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
    if (n > 0)
        return STATE::agent_list->filtered_objects[0]->id;
    return -1;
}

void Grenades::tick()
{
    Verlet::bounce(this->vp, GRENADES_DAMP);
    #if DC_SERVER
    int agent_id = this->nearest_agent_in_range(this->vp->p);
    if (agent_id >= 0 && STATE::agent_list->agent_pickup_item(agent_id, (Object_types)this->type))
    {   // was picked up, die
        this->schedule_death_broadcast();
        this->ttl = this->ttl_max;
    }
    #endif
    this->ttl++;
}

void Grenades::schedule_death_broadcast()
{
    this->broadcast_death = true;
}

void Grenades::die()
{
    #if DC_SERVER
    if (this->broadcast_death)
    {
        item_destroy_StoC msg;
        msg.type = this->type;
        msg.id = this->id;
        msg.broadcast();
    }
    #endif
}

void Grenades::born()
{
    #if DC_SERVER
    item_create_StoC msg;
    msg.type = this->type;
    msg.id = this->id;
    msg.x = this->vp->p.x;
    msg.y = this->vp->p.y;
    msg.z = this->vp->p.z;
    Vec3 m = this->vp->get_momentum();
    msg.mx = m.x;
    msg.my = m.y;
    msg.mz = m.z;
    msg.broadcast();
    #endif
}

void Grenades::init()
{
    this->type = OBJ_TYPE_GRENADE_DROP;
    this->texture_index = GRENADES_TEXTURE_ID;
    this->scale = GRENADES_TEXTURE_SCALE;
    this->ttl_max = GRENADES_TTL;
    this->born();
}

Grenades::Grenades(int id)
:
Particle(id, 0,0,0,0,0,0, GRENADES_MASS),
BillboardSprite(),
PickupItem(ITEM_PICKUP_RADIUS),
broadcast_death(false)
{
    this->init();
}

Grenades::Grenades(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z,mx,my,mz, GRENADES_MASS),
BillboardSprite(),
PickupItem(ITEM_PICKUP_RADIUS),
broadcast_death(false)
{
    this->init();
}


void GrenadeDrops_list::tick()
{
    if (num==0) return;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->tick();
        if (this->a[i]->ttl >= this->a[i]->ttl_max)
        {
            this->a[i]->die();
            this->destroy(this->a[i]->id);
        }
    }
}

void GrenadeDrops_list::draw()
{
    if (num==0) return;
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw_from_bottom(this->a[i]->vp->p);
}

}   // ItemDrops
