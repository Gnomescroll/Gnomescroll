#pragma once

#if DC_CLIENT
#include <common/gl_assert.hpp>
#endif

#include <physics/verlet_particle.hpp>

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

#if DC_CLIENT
extern GLuint ItemSheetTexture;

void init_item_particle();
void teardown_item_particle();
#endif

class ItemParticle //: public VerletComponent
{
    public:
        VerletParticle::VerletParticle verlet;

        int id;
        int item_type;
        
        int target_agent; // when being picked up
        
        // render stuff
        #if DC_CLIENT
        // config
        bool is_voxel;
        Components::TexturedVoxelComponent voxel;
        int sprite_index;
        bool should_draw;
        void draw();
        #endif

        #if DC_SERVER
        int ttl;
        ItemID item_id;
        int broadcast_tick;

        int pickup_prevention;	// timer lock against auto pickup
        bool get_picked_up;
        
        bool can_be_picked_up()
        {
            return (this->pickup_prevention <= 0
				 && this->target_agent == NO_AGENT);
        }
        void lock_pickup()
        {
            this->pickup_prevention = ITEM_PICKUP_PREVENTION_DELAY;
        }
        #endif

        void picked_up(int agent_id);
		void pickup_cancelled();
		
        void tick();

        void set_state(float x, float y, float z, float mx, float my, float mz)
        {
            this->verlet.position = vec3_init(x,y,z);
            this->verlet.velocity = vec3_init(mx,my,mz);
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

#include <common/template/dynamic_object_list.hpp>

namespace ItemParticle
{

class ItemParticle_list: public DynamicObjectList<ItemParticle, ITEM_PARTICLE_MAX, ITEM_PARTICLE_HARD_MAX>
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

    //GS_ASSERT(ItemSheetTexture != 0);

    glEnable(GL_ALPHA_TEST);

    glAlphaFunc ( GL_GREATER, 0.5f);

    glBindTexture(GL_TEXTURE_2D, ItemSheetTexture);

    glBegin(GL_QUADS);

    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL && !this->a[i]->is_voxel)
            this->a[i]->draw();
    glEnd();

    glDisable(GL_ALPHA_TEST);

    // draw textured voxels
    glColor4ub(255,255,255,255);
    glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal); // block texture sheet
    glBegin(GL_QUADS);
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL && this->a[i]->is_voxel && this->a[i]->should_draw)
        {
            ItemParticle* p = this->a[i];
            p->voxel.delta_rotation(0.01f, 0.0f);
            p->voxel.draw(p->verlet.position);
        }
    glEnd();
    #endif
}

}
