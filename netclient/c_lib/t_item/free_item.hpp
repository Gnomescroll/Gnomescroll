#pragma once

namespace t_item
{

const int FREE_ITEM_TTL = 300; // 10 seconds

typedef enum
{
    type_NULL = 255,
} T_ItemTypes;

class Free_item
{
    private:

    public:

        int id;
        T_ItemTypes type;

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

    void set_position(float x, float y, float z)
    {
        vp.set_position(x,y,z);
    }

    void set_momentum(float mx, float my, float mz)
    {
        vp.set_momentum(mx,my,mz);
    }

    void set_state(float x, float y, float z, float mx, float my, float mz)
    {
        vp.set_position(x,y,z);
        vp.set_momentum(mx,my,mz);
    }

    Free_item(int id, float x, float y, float z, float mx, float my, float mz)
    :
    id(id), type(type_NULL),
    ttl(0), ttl_max(FREE_ITEM_TTL),
    mass(1.0f), damp(1.0f)
    {
    	vp.set_state(x,y,z, mx,my,mz);
        vp.set_mass(mass);
    }

    Free_item(int id)
    :
    id(id), type(type_NULL),
    ttl(0), ttl_max(FREE_ITEM_TTL), 
    mass(1.0f), damp(1.0f)
    {
    }
};

void Free_item::draw()
{
#ifdef DC_CLIENT
	const float scale = 0.5;

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
    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(this->texture_index%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(this->texture_index/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);
	*/

    int texture_index = 5;
    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(texture_index%8)* (1.0/8.0);
    tx_max = tx_min + (1.0/8.0);
    ty_min = (float)(texture_index/8)* (1.0/8.0);
    ty_max = ty_min + (1.0/8.0);

    Vec3 p = vec3_sub(vp.p, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(vp.p, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(vp.p, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(vp.p, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z);
#endif
}

}

/*
	List Definition
*/

#include <c_lib/template/object_list.hpp>

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
};

void Free_item_list::draw()
{
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw();
}

void Free_item_list::tick()
{
    Free_item* free_item;
    for (int i=0; i<this->n_max; i++)
    {
        free_item = this->a[i];
        if (free_item == NULL) continue;
        free_item->tick();
        if (free_item->ttl >= free_item->ttl_max)
        {
	        #ifdef DC_SERVER
	            free_item->die();
	        #endif
	            this->destroy(free_item->id);
        }
    }
}


}
