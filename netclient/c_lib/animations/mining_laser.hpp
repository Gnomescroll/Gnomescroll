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
    SDL_Surface* s = create_surface_from_file((char*) "./media/sprites/mining_laser2.png");

    if(s == NULL)
    {
        printf("init_insect_mob: texture load error\n");
        abort();
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &mining_laser_texture );
    glBindTexture( GL_TEXTURE_2D, mining_laser_texture  );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
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
    
    mining_laser_TexCoord = mining_laser_shader.get_attribute("InTexCoord");
}


const int MINING_LASER_TTL = 120; // 10 seconds
const float MINING_LASER_DAMPENING = 0.20;

class MiningLaser
{
    public:

    public:
        VerletParticle verlet;

        int id;
        int type;

        int ttl;
        //int ttl_max;

        float damp;

    void init(float x, float y, float z, float mx, float my, float mz)
    {

    	ttl = MINING_LASER_TTL;

    	type = 0+rand()%4;
    	verlet.dampening = MINING_LASER_DAMPENING;

        verlet.position = vec3_init(x,y,z);
        verlet.velocity = vec3_init(mx,my,mz);
    }

    void tick()
    {
        //verlet.bounce_box(0.125);
        
        
        if( verlet.bool_no_gravity() ) ttl = 0;
        
        //verlet.bounce_box_no_gravity(0.125)
    	//verlet.radial(rx, ry);

        this->ttl--;
    }

    void prep(Vec3 cam)
    {
	    const float scale = 0.25;
	    const float _h = scale / 2;

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

	    int texture_index = this->type;
	    //int texture_index = 4;

	    float tx_min, tx_max, ty_min, ty_max;
	    tx_min = (float)(texture_index%4)* (1.0/4.0);
	    tx_max = tx_min + (1.0/4.0);
	    ty_min = (float)(texture_index/4)* (1.0/4.0);
	    ty_max = ty_min + (1.0/4.0);

	    Vec3 position = verlet.position;
	    position.z += _h;

	    Vec3 p = vec3_sub(position, vec3_add(right, up));
		mining_laser_vlist->push_vertex(p, tx_min,ty_max);

	    p = vec3_add(position, vec3_sub(up, right));
	    mining_laser_vlist->push_vertex(p, tx_max,ty_max);

	    p = vec3_add(position, vec3_add(up, right));
	    mining_laser_vlist->push_vertex(p, tx_max,ty_min);

	    p = vec3_add(position, vec3_sub(right, up));
	    mining_laser_vlist->push_vertex(p, tx_min,ty_min);

    }

};

}

#include <c_lib/template/object_list.hpp>

namespace Animations 
{

const int MINING_LASER_MAX = 1024;

class MiningLaserEffect_list: public Simple_object_list<MiningLaser, MINING_LASER_MAX>
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
	//mining_laser_vlist->reset_index();

	Vec3 cam = vec3_init(current_camera->x, current_camera->y, current_camera->z);
    for (int i=0; i<this->num; i++)
    {
        this->a[i].prep(cam);
    }

    mining_laser_vlist->buffer(); //upload data to GPU and reset list
}


void MiningLaserEffect_list::draw()
{

	if(mining_laser_vlist->vertex_number == 0) return;
	const static unsigned int stride = sizeof(struct vertexElement1);

	//printf("%i \n", mining_laser_vlist->vlist_index);

    //glBindBuffer(GL_ARRAY_BUFFER, mining_laser_vbo);
    //glBufferData(GL_ARRAY_BUFFER, mining_laser_vlist->vlist_index*stride, NULL, GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, mining_laser_vlist->vlist_index*stride, mining_laser_vlist->vlist, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mining_laser_vlist->VBO);

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //AlphaFunc(GL_NOTEQUAL, 0.0);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, mining_laser_texture );


    glUseProgramObjectARB(mining_laser_shader.shader);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(mining_laser_TexCoord);

    glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);
    glVertexAttribPointer(mining_laser_TexCoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)12);

    glDrawArrays(GL_QUADS,0, mining_laser_vlist->vertex_number);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(mining_laser_TexCoord);
    glUseProgramObjectARB(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void MiningLaserEffect_list::tick()
{
    for (int i=0; i<this->num; i++)
    {
        a[i].tick();

	    if (a[i].ttl <= 0 )
	    {
	    	this->destroy(a[i].id);
	        //printf("destroy \n");
	    }

    }
}

}