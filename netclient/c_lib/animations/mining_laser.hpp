#pragma once

#include <c_lib/physics/vec3.hpp>

#include <c_lib/animations/common.hpp>




namespace Animations 
{

VertexElementList1* mining_laser_vlist = NULL;

unsigned int mining_laser_vbo;

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
    insect_mob_shader.set_debug(true);

    insect_mob_shader.load_shader( "mining shader",
        "./media/shaders/weapon/mining_laser.vsh",
        "./media/shaders/weapon/mining_laser.fsh" );
    

    mining_laser_TexCoord = insect_mob_shader.get_attribute("InTexCoord");
}


class MiningLaser
{
    public:

    
    MiningLaser()
    {

    }


    void prep()
    {


    }

    void tick()
    {

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
/*
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
*/
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