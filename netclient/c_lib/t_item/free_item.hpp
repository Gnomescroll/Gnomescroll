#pragma once


#include <c_lib/t_item/net.hpp>

namespace t_item
{

const int FREE_ITEM_TTL = 300; // 10 seconds

typedef enum
{
    ITEM_TYPE_NULL = 255,
} T_ItemTypes;

class Free_item
{
    private:
        void create_particle(float x, float y, float z, float mx, float my, float mz)
        {
            this->vp = new VerletParticle(x,y,z,mx,my,mz, this->mass);
        }

    public:

        int id;
        T_ItemTypes item_type;

        int ttl;
        int ttl_max;

        VerletParticle* vp;
        float mass;
        float damp;

    void die()
    {
        t_item_destroy_StoC msg;
        msg.id = this->id;
        msg.type = this->type;
        msg.broadcast();
    }
        
    void tick()
    {
        Verlet::bounce(this->vp, this->damp);
        this->ttl++;
    }

    void draw();

    void set_position(float x, float y, float z)
    {
        if (this->vp == NULL)
            this->create_particle(x,y,z, 0,0,0);
        this->vp->set_position(x,y,z);
    }

    void set_momentum(float mx, float my, float mz)
    {
        if (this->vp == NULL)
            return;
        this->vp->set_momentum(mx,my,mz);
    }

    void set_state(float x, float y, float z, float mx, float my, float mz)
    {
        this->set_position(x,y,z);
        this->set_momentum(mx,my,mz);
    }

    Free_item(int id, float x, float y, float z, float mx, float my, float mz)
    :
    id(id),
    item_type(ITEM_TYPE_NULL),
    ttl(0), ttl_max(FREE_ITEM_TTL),
    mass(1.0f), damp(1.0f)
    {
        this->create_particle(x,y,z,mx,my,mz, this->mass);
    }

    explicit Free_item(int id)
    :
    id(id), item_type(ITEM_TYPE_NULL),
    ttl(0), ttl_max(FREE_ITEM_TTL),
    vp(NULL), mass(1.0f), damp(1.0f)
    {
    }
};

void Free_item::draw()
{

}

}
