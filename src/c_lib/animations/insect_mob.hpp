#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <physics/vec3.hpp>
#include <animations/common.hpp>

namespace Animations 
{

static float insect_mob_t = 0.0f;

///struct vertexElement2* insect_mob_vlist = NULL;
//int insect_mob_vlist_index = 0;

//unsigned int insect_mob_vbo;

int insect_mob_surface;
unsigned int insect_mob_texture;

class SHADER insect_mob_shader;

//unsigned int insect_mob_Vertex;
unsigned int insect_mob_TexCoord;

VertexElementList2* insect_mob_vlist = NULL;

/*
static inline void im_push_vertex(struct Vec3 pos, float tx, float ty)
{
    vertexElement2* v = &insect_mob_vlist[insect_mob_vlist_index];

    v->pos = pos;
    v->tx = tx;
    v->ty = ty;

    insect_mob_vlist_index++;
}
*/

void init_insect_mob_texture();
void init_insect_mob_shader();

void init_insect_mob()
{
    //insect_mob_vlist = new vertexElement2[4096];
    //insect_mob_vlist_index = 0;

    insect_mob_vlist = new VertexElementList2;

    //glGenBuffers(1, &insect_mob_vbo);

    init_insect_mob_texture();
    init_insect_mob_shader();
}

void teardown_insect_mob()
{
    delete insect_mob_vlist;
}

void init_insect_mob_texture()
{
    SDL_Surface* s = create_surface_from_file((char*) "./media/sprites/AlienTex3.png");

    if(s == NULL)
    {
        printf("init_insect_mob: texture load error\n");
        GS_ABORT();
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &insect_mob_texture );
    glBindTexture( GL_TEXTURE_2D, insect_mob_texture  );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    GLenum internalFormat = 4; //GL_RGBA;
    GLenum format = GL_BGRA;
    if (s->format->Rmask == 0x000000ff)
        format = GL_RGBA;
   
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);

    SDL_FreeSurface(s);
}

void init_insect_mob_shader()
{
    insect_mob_shader.set_debug(false);

    insect_mob_shader.load_shader( "insect mob shader",
        "./media/shaders/mob/insect_mob.vsh",
        "./media/shaders/mob/insect_mob.fsh" );
    
    //uniforms
    //map_ChunkPosition = glGetUniformLocation(insect_mob_shader[index], "ChunkPosition");

    //attributes
    //insect_mob_Vertex = glGetAttribLocation(insect_mob_shader[index], "InVertex");
    //insect_mob_TexCoord = glGetAttribLocation(insect_mob_shader.shader, "InTexCoord");

    insect_mob_TexCoord = insect_mob_shader.get_attribute("InTexCoord");
}


class InsectMob
{
    public:

    static const int max_sides = 6;

    int id;

    float x,y,z;

    int sides;

    float tw;   //torse width
    float th;   //torso height

    //float phase[max_sides];
    float speed[max_sides]; 
    
    InsectMob() {}
    InsectMob(int _id) { id = _id; }

    void init(float _x, float _y, float _z)
    {
        for(int i=0; i<max_sides; i++ ) speed[i] = 1.0f + (0.4f*randf() - 0.1f);

        sides = 3 + (rand() % 4);
        //tw = 1.0 + (randf()*.5 - 0.25);
        //th = 1.0 + (randf()*.5 - 0.25);
        tw = 1.0f;
        th = 1.0f;

        x = _x;
        y = _y;
        z = _z;
    }

    inline void draw_legs()
    {
        float x = quadrant_translate_f(current_camera_position.x, this->x);
        float y = quadrant_translate_f(current_camera_position.y, this->y);
        float z = this->z;
        //static float t = 0.0;
        //printf("sf\n");

        //t += 0.04;

        const float t = insect_mob_t;

        const float h = 0.8f;
        const float li = 0.1f;

        const float txh = 0.2f; //texture slice height for claw


        const float f1 = (1.0f /(float)(sides))*2*3.14159f;
        const float f2 = (1.0f / 3.0f)*2*3.14159f;

        struct Vec3 v1,v2,v3;

        for(int i=0; i<sides; i++)
        {
            float _x = x+(tw-0.15f)*sinf(f1*i);
            float _y = y+(th-0.15f)*cosf(f1*i);
            float _z = z;

            for(int j=0; j<3; j++)
            {
                
                v1 = vec3_init( _x + 0.3f*sinf(speed[i]*t), _y, _z-h );
                v2 = vec3_init( _x+li*sinf(f2*j), _y+li*cosf(f2*j), _z );
                v3 = vec3_init( _x+li*sinf(f2*(j+1)), _y+li*cosf(f2*(j+1)), _z );

                insect_mob_vlist->push_vertex(v1, 0.5f, 0.5f);
                insect_mob_vlist->push_vertex(v2, 0.0f, 0.5f+txh);
                insect_mob_vlist->push_vertex(v3, 0.0f, 0.5f-txh);
            }
        }


    }

    inline void draw_torso()
    {

        float x = quadrant_translate_f(current_camera_position.x, this->x);
        float y = quadrant_translate_f(current_camera_position.y, this->y);
        float z = this->z;

        const float z0 = 0.0f;
        const float z1 = 0.3f;
        const float z2 = -0.3f;

        const float f1 = (1.0f / (float)(sides))*2*3.14159f;

        struct Vec3 v1,v2,v3;

        for(int i=0; i<sides; i++)
        {

            v1 = vec3_init( x, y, z1+z );
            v2 = vec3_init( x+tw*sinf(f1*i), y+th*cosf(f1*i), z0+z );
            v3 = vec3_init( x+tw*sinf(f1*(i+1)), y+th*cosf(f1*(i+1)), z0+z );

            insect_mob_vlist->push_vertex(v1, 0.5f, 0.5f);
            insect_mob_vlist->push_vertex(v2, sinf(f1*i)/2 + 0.5f,  cosf(f1*i)/2 + 0.5f);
            insect_mob_vlist->push_vertex(v3, sinf(f1*(i+1))/2 + 0.5f,  cosf(f1*(i+1))/2 + 0.5f);

        }

        for(int i=0; i<sides; i++)
        {
            v1 = vec3_init( x, y, z2+z );
            v2 = vec3_init( x+tw*sinf(f1*i), y+th*cosf(f1*i), z0+z );
            v3 = vec3_init( x+tw*sinf(f1*(i+1)), y+th*cosf(f1*(i+1)), z0+z );

            insect_mob_vlist->push_vertex(v1, 0.5f, 0.5f);
            insect_mob_vlist->push_vertex(v2, sinf(f1*i)/2 + 0.5f,  cosf(f1*i)/2 + 0.5f);
            insect_mob_vlist->push_vertex(v3, sinf(f1*(i+1))/2 + 0.5f,  cosf(f1*(i+1))/2 + 0.5f);
        }
    }
/* 
    Create vertex data, upload to card
*/

    void prep()
    {
        draw_legs();
        draw_torso();
    }

    void tick()
    {

    }
};

//void set_insect_position(float x, float y, float z) {}

}

#include <common/template/object_list.hpp>

namespace Animations 
{

const int INSECT_MOB_MAX = 1024;

class Insect_mob_list: public Object_list<InsectMob, INSECT_MOB_MAX>
{
    private:
        const char* name() { return "Insect_mob_list"; }

        bool needs_update;
    public:
        Insect_mob_list() { print_list((char*)this->name(), this); needs_update = true;}

        void prep();
        void draw();
        void tick();

};


void Insect_mob_list::prep()
{
    #if DC_CLIENT

    if( needs_update == false) return;
    insect_mob_t += 0.04f;

    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] != NULL)
        {
            this->a[i]->prep();
        }
    }
    needs_update = false;
    insect_mob_vlist->buffer();
    #endif
}


void Insect_mob_list::draw()
{
    #if DC_CLIENT

    if(insect_mob_vlist->vertex_number == 0) return;

    GS_ASSERT(insect_mob_vlist->VBO != 0);
    if (insect_mob_vlist->VBO == 0) return;

    const static unsigned int stride = sizeof(struct vertexElement2);

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, insect_mob_texture );

    glBindBuffer(GL_ARRAY_BUFFER, insect_mob_vlist->VBO);
    
    glUseProgramObjectARB(insect_mob_shader.shader);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(insect_mob_TexCoord);

    glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);
    glVertexAttribPointer(insect_mob_TexCoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)12);

    glDrawArrays(GL_TRIANGLES,0, insect_mob_vlist->vertex_number);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(insect_mob_TexCoord);
    glUseProgramObjectARB(0);

    #endif
}

void Insect_mob_list::tick()
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

}   // Animations
