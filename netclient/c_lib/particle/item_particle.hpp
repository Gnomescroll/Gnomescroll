#pragma once

#if DC_CLIENT
#include <common/gl_assert.hpp>
#endif

#include <physics/verlet_particle.hpp>

namespace Particle
{

#ifdef DC_CLIENT
extern SDL_Surface* ItemSheetSurface;
extern unsigned int ItemSheetTexture;

void init_item_particle();
void teardown_item_particle();
#endif

const int ITEM_PARTICLE_TTL = 30 * 36; // 12 seconds
const float ITEM_PARTICLE_DAMPENING = 0.01;

const int PICKUP_PREVENTION_DELAY = 30 * 4; // 4 seconds

class ItemParticle //: public VerletComponent
{
    public:
        VerletParticle::VerletParticle verlet;

        int id;
        int item_type;
        #if DC_SERVER
        ItemID item_id;
        #endif
        
        // render stuff
        #if DC_CLIENT
        int sprite_index;
        void draw();
        #endif

        // pickup stuff
        #if DC_SERVER
        int pickup_prevention;
        bool was_picked_up;
        
        void picked_up(int agent_id);
        bool can_be_picked_up()
        {
            return (this->pickup_prevention <= 0);
        }
        void lock_pickup()
        {
            this->pickup_prevention = PICKUP_PREVENTION_DELAY;
        }
        #endif

        int ttl;
        void tick()
        {
            //this->verlet_bounce(this->damp);
            verlet.bounce_box(ITEM_PARTICLE_DAMPENING);
            this->ttl--;
            #if DC_SERVER
            this->pickup_prevention--;
            #endif
        }

        void die();

    explicit ItemParticle(int id);

    #if DC_CLIENT
    void init(int item_type, float x, float y, float z, float mx, float my, float mz);
    #endif
    #if DC_SERVER
    void init(ItemID item_id, int item_type, float x, float y, float z, float mx, float my, float mz);
    #endif
};

}

/*
    List Definition
*/

#include <common/template/object_list.hpp>

namespace Particle
{

const int ITEM_PARTICLE_MAX = 1024;

class ItemParticle_list: public Object_list<ItemParticle, ITEM_PARTICLE_MAX>
{
    private:
        const char* name() { return "ItemParticle"; }
    public:
        ItemParticle_list() { print_list((char*)this->name(), this); }

        void draw();
        void tick();

        void check_item_pickups();
};

void ItemParticle_list::draw()
{
#if DC_CLIENT
    glColor3ub(255,255,255);

    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

    //assert(ItemSheetTexture != 0);

    glEnable(GL_ALPHA_TEST);

    glAlphaFunc ( GL_GREATER, 0.5);

    glBindTexture(GL_TEXTURE_2D, ItemSheetTexture);

    glBegin(GL_QUADS);

    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw();

    glEnd();

    glDisable(GL_ALPHA_TEST);
#endif
}

void ItemParticle_list::tick()
{
    ItemParticle* ip;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        ip = this->a[i];

        ip->tick();
        if (ip->ttl <= 0)
        {
            #if DC_SERVER
            ip->die();
            this->destroy(ip->id);
            #endif
        }
    }
}

}
