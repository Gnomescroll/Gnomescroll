#pragma once

#if DC_CLIENT
#include <c_lib/common/gl_assert.hpp>
#include <c_lib/t_item/client/texture.hpp>
#endif

#if DC_SERVER
    //#include <c_lib/t_item/net/StoC.hpp>
#endif

#include <c_lib/physics/verlet_particle.hpp>

namespace Particles
{

#ifdef DC_CLIENT
extern SDL_Surface* ItemSheetSurface;
extern unsigned int ItemSheetTexture;

void init_item_particle();
void teardown_item_particle();
#endif

const int ITEM_PARTICLE_TTL = 300; // 10 seconds
const float ITEM_PARTICLE_DAMPENING = 0.5;

class ItemParticle //: public VerletComponent
{
    public:
        VerletParticle::VerletParticle verlet;

        int id;
        int item_id;
        int sprite_index;

        int ttl;

    void die();
        
    void tick()
    {
        //this->verlet_bounce(this->damp);
        verlet.bounce_box(0.20);
        this->ttl--;
    }

    void draw();

    explicit ItemParticle(int id)
    : id(id), ttl(ITEM_PARTICLE_TTL)
    {
        verlet.dampening = ITEM_PARTICLE_DAMPENING;
    }

    void init(float x, float y, float z, float mx, float my, float mz)
    {
        this->sprite_index = (rand() % 16);

        verlet.position = vec3_init(x,y,z);
        verlet.velocity = vec3_init(mx,my,mz);
    }
};

void ItemParticle::draw()
{
#if DC_CLIENT
    const float scale = 0.25;
    const float h = 0.35;

    Vec3 up = vec3_init(
        model_view_matrix[0]*scale,
        model_view_matrix[4]*scale,
        model_view_matrix[8]*scale
    );
    Vec3 right = vec3_init(
        model_view_matrix[1]*scale,
        model_view_matrix[5]*scale,
        model_view_matrix[9]*scale
    );

    const int texture_index = sprite_index;

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(texture_index%8)* (1.0/8.0);
    tx_max = tx_min + (1.0/8.0);
    ty_min = (float)(texture_index/8)* (1.0/8.0);
    ty_max = ty_min + (1.0/8.0);

    Vec3 position = verlet.position;
    Vec3 p = vec3_sub(position, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z+h);
#endif
}

}

/*
    List Definition
*/

#include <c_lib/common/template/object_list.hpp>

namespace Particles
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
