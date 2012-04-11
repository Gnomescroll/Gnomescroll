#pragma once

#include <c_lib/physics/vec3.hpp>

namespace Animations 
{


struct vertexElement
{
	struct Vec3 pos;

	float tx,ty;
	struct Vec3 n;
};

struct vertexElement* insect_mob_vlist;
int insect_mob_vlist_index = 0;

unsigned int insect_mob_vbo;

int insect_mob_surface;
unsigned int insect_mob_texture;

unsigned int insect_mob_shader[1];
unsigned int insect_mob_vert_shader[1];
unsigned int insect_mob_frag_shader[1];

//unsigned int insect_mob_Vertex;
unsigned int insect_mob_TexCoord;

static inline void push_vertex(struct Vec3 pos, float tx, float ty)
{
	vertexElement* v = &insect_mob_vlist[insect_mob_vlist_index];

	v->pos = pos;
	v->tx = tx;
	v->ty = ty;

	insect_mob_vlist_index++;
}

void init_insect_mob_texture();
void init_insect_mob_shader();

void init_insect_mob()
{
	insect_mob_vlist = new vertexElement[4096];
	insect_mob_vlist_index = 0;

	glGenBuffers(1, &insect_mob_vbo);

	init_insect_mob_texture();
	init_insect_mob_shader();
}


void init_insect_mob_texture()
{
	SDL_Surface* s = create_surface_from_file((char*) "./media/sprites/AlienTex3.png");

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

void init_insect_mob_shader()
{
    const int index = 0;    //shader index
    const int DEBUG = 1;

    insect_mob_shader[index] = glCreateProgramObjectARB();
    insect_mob_vert_shader[index] = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    insect_mob_frag_shader[index] = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;

    if(DEBUG) printf("set_insect_mob_shader_0: \n");

    vs = textFileRead((char*) "./media/shaders/mob/insect_mob.vsh");
    fs = textFileRead((char*) "./media/shaders/mob/insect_mob.fsh");

    glShaderSourceARB(insect_mob_vert_shader[index], 1, (const GLcharARB**)&vs, NULL);
    glShaderSourceARB(insect_mob_frag_shader[index], 1, (const GLcharARB**)&fs, NULL);
    
    glCompileShaderARB(insect_mob_vert_shader[index]);
    if(DEBUG) printShaderInfoLog(insect_mob_vert_shader[index]);

    glCompileShaderARB(insect_mob_frag_shader[index]);
    if(DEBUG) printShaderInfoLog(insect_mob_frag_shader[index]);
    
    glAttachObjectARB(insect_mob_shader[index], insect_mob_vert_shader[index]);
    glAttachObjectARB(insect_mob_shader[index], insect_mob_frag_shader[index]);

    glLinkProgramARB(insect_mob_shader[index]);

    if(DEBUG) printProgramInfoLog(insect_mob_shader[index]);
    
    //uniforms

    //map_ChunkPosition = glGetUniformLocation(insect_mob_shader[index], "ChunkPosition");

    //attributes
    //insect_mob_Vertex = glGetAttribLocation(insect_mob_shader[index], "InVertex");
    insect_mob_TexCoord = glGetAttribLocation(insect_mob_shader[index], "InTexCoord");

}


class InsectMob
{
	public:

	static const int max_sides = 6;

	float x,y,z;

	int sides;

	float tw;	//torse width
	float th;	//torso height

	//float phase[max_sides];
	float speed[max_sides]; 
	
	InsectMob()
	{

		//for(int i=0; i<max_sides; i++ ) phase[i] = 3.1415*randf();
		for(int i=0; i<max_sides; i++ ) speed[i] = 1.0+ (0.4*randf() - 0.1);

		sides = 3 + (rand() % 4);
		tw = 1.0 + (randf()*.5 - 0.25);
		th = 1.0 + (randf()*.5 - 0.25);


	}

	inline void draw_legs()
	{

		static float t = 0.0;
		//printf("sf\n");

		t += 0.04;
		const float h = 0.8;
		const float li = 0.1;

		const float txh = 0.2; //texture slice height for claw


	    const float f1 = (1.0 /(float)(sides))*2*3.14159;
	    const float f2 = (1.0 / 3.0)*2*3.14159;

	    struct Vec3 v1,v2,v3;

	    for(int i=0; i<sides; i++)
	    {
			float _x = x+(tw-0.15)*sin(f1*i);
			float _y = y+(th-0.15)*cos(f1*i);
			float _z = z;

		    for(int j=0; j<3; j++)
		   	{
	    		
		   		v1 = vec3_init( _x + 0.3*sin(speed[i]*t), _y, _z-h );
		   		v2 = vec3_init( _x+li*sin(f2*j), _y+li*cos(f2*j), _z );
		   		v3 = vec3_init( _x+li*sin(f2*(j+1)), _y+li*cos(f2*(j+1)), _z );

		   		push_vertex(v1, 0.5, 0.5);
		   		push_vertex(v2, 0.0, 0.5+txh);
		   		push_vertex(v3, 0.0, 0.5-txh);
	    	}
		}


	}

	inline void draw_torso()
	{

	    const float z0 = 0;
	    const float z1 = 0.3;
	    const float z2 = -0.3;

	    const float f1 = (1 / (float)(sides))*2*3.14159;

	   	struct Vec3 v1,v2,v3;

	    for(int i=0; i<sides; i++)
	   	{

	   		v1 = vec3_init( x, y, z1+z );
	   		v2 = vec3_init( x+tw*sin(f1*i), y+th*cos(f1*i), z0+z );
	   		v3 = vec3_init( x+tw*sin(f1*(i+1)), y+th*cos(f1*(i+1)), z0+z );

	   		push_vertex(v1, 0.5, 0.5);
		   	push_vertex(v2, sin(f1*i)/2 + 0.5,  cos(f1*i)/2 + 0.5);
		   	push_vertex(v3, sin(f1*(i+1))/2 + 0.5,  cos(f1*(i+1))/2 + 0.5);

	   	}

	    for(int i=0; i<sides; i++)
	   	{

	   		v1 = vec3_init( x, y, z2+z );
	   		v2 = vec3_init( x+tw*sin(f1*i), y+th*cos(f1*i), z0+z );
	   		v3 = vec3_init( x+tw*sin(f1*(i+1)), y+th*cos(f1*(i+1)), z0+z );

	   		push_vertex(v1, 0.5, 0.5);
		   	push_vertex(v2, sin(f1*i)/2 + 0.5,  cos(f1*i)/2 + 0.5);
		   	push_vertex(v3, sin(f1*(i+1))/2 + 0.5,  cos(f1*(i+1))/2 + 0.5);
	   	}
	}
/* 
	Create vertex data, upload to card
*/

	void prep()
	{


	}

	void tick()
	{

	}
};

void set_insect_position(float x, float y, float z) {}

}

#include <c_lib/template/object_list.hpp>

namespace Animations 
{

const int INSECT_MOB_MAX = 1024;

class Insect_mob_list: public Object_list<InsectMob, INSECT_MOB_MAX>
{
    private:
        const char* name() { return "Insect_mob_list"; }

        bool needs_update;
    public:
        Insect_mob_list() { print(); needs_update = true;}

        void prep();
        void draw();
        void tick();

};


void Insect_mob_list::prep()
{
#ifdef DC_CLIENT
	if(	needs_update == false) return;
	insect_mob_vlist_index = 0;

    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->prep();

    needs_update = false;
#endif
}


void Insect_mob_list::draw()
{
#ifdef DC_CLIENT

	glBindBuffer(GL_ARRAY_BUFFER, insect_mob_vbo);
	glBufferData(GL_ARRAY_BUFFER, insect_mob_vlist_index*sizeof(struct vertexElement), NULL, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, insect_mob_vlist_index*sizeof(struct vertexElement), insect_mob_vlist, GL_DYNAMIC_DRAW);


    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, insect_mob_texture );


	glUseProgramObjectARB(insect_mob_shader[0]);

    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableVertexAttribArray(insect_mob_TexCoord);

	glVertexPointer(3, GL_FLOAT, sizeof(struct vertexElement), (GLvoid*)0);
	glVertexAttribPointer(insect_mob_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertexElement), (GLvoid*)12);

	glDrawArrays(GL_TRIANGLES,0, insect_mob_vlist_index);

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



}