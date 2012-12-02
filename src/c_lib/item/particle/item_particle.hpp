#pragma once

#if DC_CLIENT
#include <common/gl_assert.hpp>
#include <common/draw/textured_voxel.hpp>
#endif

#include <physics/verlet.hpp>

/*
 *
    instead of getting picked up directly:
    assign target agent
    * the ttl should be reset to a fixed max lifespan here

    will need to do item splits in the check_item_pickups method
    
    the "picked_up" packet will be sent when the target is decided
    * client sets target
    
    both client and server will apply the same tick method
    and advance the item towards the player
    
    in the server, if close enough to the target, or ttl=0,
    die
    
    the client will play the sound for picking up when it
    receives a destroy() packet for an item that is 
    targeting the player's agent
* 
*/

namespace ItemParticle
{

class ItemParticle
{
    public:
        Verlet::VerletComponent verlet;

        ItemParticleID id;
        int item_type;
        
        AgentID target_agent; // when being picked up
        
        // render stuff
        #if DC_CLIENT
        // config
        bool is_voxel;
        Draw::TexturedVoxel voxel;
        int sprite_index;
        bool should_draw;
        void draw();
        #endif

        #if DC_SERVER
        int ttl;
        ItemID item_id;
        int broadcast_tick;

        int pickup_prevention;    // timer lock against auto pickup
        bool get_picked_up;
        
        bool can_be_picked_up()
        {
            return (this->pickup_prevention <= 0
                 && this->target_agent == NULL_AGENT);
        }
        void lock_pickup()
        {
            this->pickup_prevention = ITEM_PICKUP_PREVENTION_DELAY;
        }
        #endif

        void picked_up(AgentID agent_id);
        void pickup_cancelled();
        
        void tick();

        void set_state(float x, float y, float z, float mx, float my, float mz)
        {
            ASSERT_BOXED_POINTf(x);
            ASSERT_BOXED_POINTf(y);
            this->verlet.position = vec3_init(x,y,z);
            this->verlet.velocity = vec3_init(mx,my,mz);
            GS_ASSERT(vec3_is_valid(this->verlet.position));
            GS_ASSERT(vec3_is_valid(this->verlet.velocity));
        }

        void die();

    explicit ItemParticle(ItemParticleID id);

    #if DC_CLIENT
    void init(int item_type, float x, float y, float z, float mx, float my, float mz);
    #endif
    #if DC_SERVER
    void init(ItemID item_id, int item_type, float x, float y, float z, float mx, float my, float mz);
    #endif
};

}   // ItemParticle

/*
    List Definition
*/

#include <common/template/object_list.hpp>

namespace ItemParticle
{

class ItemParticle_list: public ObjectList<ItemParticle, ItemParticleID>
{
    private:
        const char* name() { return "ItemParticle"; }
    public:
        ItemParticle_list(unsigned int capacity) :
            ObjectList<ItemParticle, ItemParticleID>(capacity, NULL_PARTICLE)
        {
            this->print();
        }

        void draw();
        void tick();

        void check_item_pickups();
};

void ItemParticle_list::draw()
{
    #if DC_CLIENT
    using TextureSheetLoader::item_texture_sheet_loader;
    GS_ASSERT(item_texture_sheet_loader->texture != 0);
    if (item_texture_sheet_loader->texture == 0) return;

    glColor4ub(255,255,255,255);

    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

    glEnable(GL_ALPHA_TEST);

    glAlphaFunc(GL_GREATER, 0.5f);

    glBindTexture(GL_TEXTURE_2D, item_texture_sheet_loader->texture);

    glBegin(GL_QUADS);
    for (unsigned int i=0; i<this->max; i++)
        if (this->objects[i].id != this->null_id && !this->objects[i].is_voxel && this->objects[i].should_draw)
            this->objects[i].draw();
    glEnd();

    glDisable(GL_ALPHA_TEST);
    #endif
}

}   // ItemParticle
