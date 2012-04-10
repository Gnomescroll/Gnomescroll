#pragma once

#ifdef DC_CLIENT
#include <c_lib/common/gl_assert.hpp>
#include <c_lib/t_item/sprite.hpp>
#endif

#include <c_lib/physics/verlet.hpp>

namespace t_item
{

const int FREE_ITEM_TTL = 300; // 10 seconds
const float FREE_ITEM_DAMPENING = 0.5;

typedef enum
{
    type_NULL = 255,
} T_ItemTypes;

class Free_item
{
    private:

    public:

        int id;
        int type;

        int ttl;
        int ttl_max;

        class VerletParticle vp; //physics state
        float mass;
        float damp;

    void die();
        
    void tick()
    {
        Verlet::bounce(&vp, this->damp);
        this->ttl++;
    }

    void draw();

    void set_state(float x, float y, float z, float mx, float my, float mz)
    {
        vp.set_state(x,y,z,mx,my,mz);
    }

    Free_item(int id, float x, float y, float z, float mx, float my, float mz)
    :
    id(id), type(type_NULL),
    ttl(0), ttl_max(FREE_ITEM_TTL),
    mass(1.0f), damp(FREE_ITEM_DAMPENING)
    {
    	vp.set_state(x,y,z, mx,my,mz);
        vp.set_mass(mass);

        type = rand() % 16;
    }

    Free_item(int id)
    :
    id(id), type(type_NULL),
    ttl(0), ttl_max(FREE_ITEM_TTL), 
    mass(1.0f), damp(FREE_ITEM_DAMPENING)
    {
        type = rand() % 16;
    }
};

void Free_item::draw()
{
#ifdef DC_CLIENT
	const float scale = 0.5;
    const float h = 0.75;

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
/*
    Vec3 up = vec3_init(0.0, 0.0, 1.0);

    Vec3 look;

    look.x = current_camera->x - vp.p.x;
    look.y = current_camera->y - vp.p.y;
    look.z = 0.0;

    look = vec3_normalize(look);

    Vec3 right = vec3_cross(look, up);

    right = vec3_normalize(right);
*/
    /*
    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(this->texture_index%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(this->texture_index/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);
	*/

    //int texture_index = 5;
    //int texture_index = rand() % 16;

    int texture_index = this->type;

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(texture_index%8)* (1.0/8.0);
    tx_max = tx_min + (1.0/8.0);
    ty_min = (float)(texture_index/8)* (1.0/8.0);
    ty_max = ty_min + (1.0/8.0);


    Vec3 p = vec3_sub(vp.p, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(vp.p, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(vp.p, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(vp.p, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z+h);

#endif
}

}

/*
	List Definition
*/

#include <c_lib/template/object_list.hpp>

#ifdef DC_SERVER
#include <c_lib/state/server_state.hpp>
#endif

namespace t_item
{

const int FREE_ITEM_MAX = 1024;

class Free_item_list: public Object_list<Free_item, FREE_ITEM_MAX>
{
    private:
        const char* name() { return "Free_item"; }
    public:
        Free_item_list() { print(); }

        void draw();
        void tick();

        void check_item_pickups();
};

void Free_item_list::draw()
{
#ifdef DC_CLIENT
    glColor3ub(255,255,255);

    //glEnable(GL_TEXTURE_2D);
    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);

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

void Free_item_list::tick()
{
    Free_item* free_item;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        free_item = this->a[i];

        free_item->tick();
        if (free_item->ttl >= free_item->ttl_max)
        {
	        #ifdef DC_SERVER
	            free_item->die();
	            this->destroy(free_item->id);

            #endif
        }
    }
}


void Free_item_list::check_item_pickups()
{
/*
#ifdef DC_SERVER
    Free_item* free_item;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        free_item = this->a[i];


    }
#endif
*/
}


/*
    int n = STATE::agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
    if (n > 0)
    ServerState::agent_list->filtered_objects[0]->id;
*/


}
