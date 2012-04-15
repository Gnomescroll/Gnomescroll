#include "mining_laser.hpp"



#include <c_lib/physics/vec3.hpp>

#include <c_lib/animations/common.hpp>

namespace Animations 
{

/*
struct vertexElement1
{
    struct Vec3 pos;

    float tx,ty;
    struct Vec3 n;
};
*/

struct vertexElement1* mining_laser_vlist;
int mining_laser_vlist_index = 0;

unsigned int mining_laser_vbo;

int mining_laser_surface;
unsigned int mining_laser_texture;

//unsigned int insect_mob_Vertex;
unsigned int mining_laser_TexCoord;

static inline void push_vertex(struct Vec3 pos, float tx, float ty)
{
    vertexElement1* v = &mining_laser_vlist[insect_mob_vlist_index];

    v->pos = pos;
    v->tx = tx;
    v->ty = ty;

    insect_mob_vlist_index++;
}

void init_mining_laser_texture();
void init_mining_laser_shader();

void init_mining_laser()
{
    mining_laser_vlist = new vertexElement1[1024];
    mining_laser_vlist_index = 0;

    glGenBuffers(1, &mining_laser_vbo);

    init_insect_mob_texture();
    init_insect_mob_shader();
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
    glGenTextures( 1, &insect_mob_texture );
    glBindTexture( GL_TEXTURE_2D, insect_mob_texture  );

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

class Mining_laser_list: public Object_list<InsectMob, MINING_LASER_MAX>
{
    private:
        const char* name() { return "Mining_laser_list"; }

        bool needs_update;
    public:
        Mining_laser_list() { print();}

        void prep();
        void draw();
        void tick();

};


void Mining_laser_list::prep()
{
#if DC_CLIENT
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->prep();
#endif
}


void Mining_laser_list::draw()
{
#if DC_CLIENT

    glBindBuffer(GL_ARRAY_BUFFER, insect_mob_vbo);
    glBufferData(GL_ARRAY_BUFFER, insect_mob_vlist_index*sizeof(struct vertexElement1), NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, insect_mob_vlist_index*sizeof(struct vertexElement1), mining_laser_vlist, GL_DYNAMIC_DRAW);


    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, mining_laser_texture );


    glUseProgramObjectARB(insect_mob_shader[0]);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(mining_laser_TexCoord);

    glVertexPointer(3, GL_FLOAT, sizeof(struct vertexElement1), (GLvoid*)0);
    glVertexAttribPointer(mining_laser_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertexElement1), (GLvoid*)12);

    glDrawArrays(GL_TRIANGLES,0, mining_laser_vlist_index);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(mining_laser_TexCoord);
    glUseProgramObjectARB(0);

#endif
}

void Mining_laser_list::tick()
{
    needs_update = true;
    InsectMob* m;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        m = this->a[i];
        m->tick();
    }
}



}