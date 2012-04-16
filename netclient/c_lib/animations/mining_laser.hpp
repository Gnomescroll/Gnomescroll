#pragma once

#ifdef DC_CLIENT
#include <c_lib/common/gl_assert.hpp>
#include <c_lib/t_item/client/texture.hpp>
#endif

#include <c_lib/physics/vec3.hpp>
#include <c_lib/animations/common.hpp>

#include <c_lib/physics/verlet_particle.hpp>

namespace Animations 
{

using VerletParticle::VerletParticle;

const int FREE_ITEM_TTL = 300; // 10 seconds
const float FREE_ITEM_DAMPENING = 0.5;

VertexElementList1* mining_laser_vlist = NULL;

unsigned int mining_laser_vbo;
class SHADER mining_laser_shader;

int mining_laser_surface;
unsigned int mining_laser_texture;

unsigned int mining_laser_TexCoord;


void init_mining_laser_texture();
void init_mining_laser_shader();

void init_mining_laser()
{
    mining_laser_vlist = new VertexElementList1;

    glGenBuffers(1, &mining_laser_vbo);

    init_mining_laser_texture();
    init_mining_laser_shader();
}

void teardown_mining_laser()
{
	delete mining_laser_vlist;


}
void init_mining_laser_texture()
{
    SDL_Surface* s = create_surface_from_file((char*) "./media/sprites/mining_laser.png");

    if(s == NULL)
    {
        printf("init_insect_mob: texture load error\n");
        abort();
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &mining_laser_texture );
    glBindTexture( GL_TEXTURE_2D, mining_laser_texture  );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    GLuint internalFormat = GL_RGBA; //GL_RGBA;
    GLuint format = GL_RGBA;
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);
}

void init_mining_laser_shader()
{
    mining_laser_shader.set_debug(true);

    mining_laser_shader.load_shader( "mining shader",
        "./media/shaders/weapon/mining_laser.vsh",
        "./media/shaders/weapon/mining_laser.fsh" );
    
    mining_laser_TexCoord = insect_mob_shader.get_attribute("InTexCoord");
}


class MiningLaser
{
    public:

    public:
        VerletParticle verlet;

        int id;
        int type;

        int ttl;
        int ttl_max;

        float damp;

    MiningLaser() {}
    MiningLaser(int _id) { id = _id; }

    void init(float x, float y, float z, float mx, float my, float mz)
    {
        verlet.position = vec3_init(x,y,z);
        verlet.velocity = vec3_init(mx,my,mz);
    }

    void tick()
    {
        //this->verlet_bounce(this->damp);
        verlet.bounce_box(0.20);
        this->ttl++;
    }

    void prep()
    {
	#ifdef DC_CLIENT
	    const float scale = 0.25;

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

	    int texture_index = rand() % 16;

	    float tx_min, tx_max, ty_min, ty_max;
	    tx_min = (float)(texture_index%4)* (1.0/4.0);
	    tx_max = tx_min + (1.0/4.0);
	    ty_min = (float)(texture_index/4)* (1.0/4.0);
	    ty_max = ty_min + (1.0/4.0);

	    Vec3 position = verlet.position;

	    Vec3 p = vec3_sub(position, vec3_add(right, up));
		mining_laser_vlist->push_vertex(p, tx_min,ty_max);

	    p = vec3_add(position, vec3_sub(up, right));
	    mining_laser_vlist->push_vertex(p, tx_max,ty_max);

	    p = vec3_add(position, vec3_add(up, right));
	    mining_laser_vlist->push_vertex(p, tx_max,ty_min);

	    p = vec3_add(position, vec3_sub(right, up));
	    mining_laser_vlist->push_vertex(p, tx_min,ty_min);

	#endif
    }

};

}

#include <c_lib/template/object_list.hpp>

namespace Animations 
{

const int MINING_LASER_MAX = 1024;

class MiningLaserEffect_list: public Object_list<MiningLaser, MINING_LASER_MAX>
{
    private:
        const char* name() { return "MiningLaserEffect_list"; }

    public:
        MiningLaserEffect_list() { print();}

        void prep();
        void draw();
        void tick();

};


void MiningLaserEffect_list::prep()
{
#if DC_CLIENT
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
        {
            this->a[i]->prep();
        }
#endif
}


void MiningLaserEffect_list::draw()
{
#if DC_CLIENT

    glBindBuffer(GL_ARRAY_BUFFER, mining_laser_vbo);
    glBufferData(GL_ARRAY_BUFFER, mining_laser_vlist->vlist_index*sizeof(struct vertexElement2), NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, mining_laser_vlist->vlist_index*sizeof(struct vertexElement2), mining_laser_vlist->vlist, GL_DYNAMIC_DRAW);


    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, mining_laser_texture );


    glUseProgramObjectARB(mining_laser_shader.shader);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(mining_laser_TexCoord);

    glVertexPointer(3, GL_FLOAT, sizeof(struct vertexElement2), (GLvoid*)0);
    glVertexAttribPointer(mining_laser_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertexElement2), (GLvoid*)12);

    glDrawArrays(GL_TRIANGLES,0, mining_laser_vlist->vlist_index);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(mining_laser_TexCoord);
    glUseProgramObjectARB(0);

#endif
}

void MiningLaserEffect_list::tick()
{
    MiningLaser* m;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] != NULL)
        {
        	m = this->a[i];
        	m->tick();
        }
    }
}

}