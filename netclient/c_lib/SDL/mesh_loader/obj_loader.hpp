#pragma once


//#include <stdlib.h>
//#include <stdio.h>
//#include "obj.h"

#include <SDL/shader_loader.hpp>

namespace obj_load
{


struct ObjVertex
{
   float x, y, z;
};

typedef ObjVertex ObjNormal;

struct ObjTexCoord
{
   float u, v;
};

struct ObjTriangle
{
   int Vertex[3];
   int Normal[3];
   int TexCoord[3];
};

struct ObjModel
{
   int nVertex, nNormal, nTexCoord, nTriangle;

   ObjVertex* VertexArray;
   ObjNormal* NormalArray;
   ObjTexCoord* TexCoordArray;
   ObjTriangle* TriangleArray;
};

struct Vertex
{
	float x,y,z;
	float u,v;
	float n[3];
};

ObjModel* ObjLoadModel(char*, size_t);
size_t    ObjLoadFile(char*, char**);


ObjModel* m_model = NULL;

struct Vertex* v_array = NULL;
int v_num = 0;

void load_model()
{
   char* memory = NULL;
   size_t bytes = ObjLoadFile((char*) "media/mesh/mob1.obj", &memory);

   ObjModel* model = ObjLoadModel(memory, bytes);

   m_model = model;

   printf("Object Model: nTriangle= %d \n", model->nTriangle);
   printf("Object Model: nVertex= %d \n", model->nVertex);
   printf("Object Model: nTexCoord= %d \n", model->nTexCoord);
   printf("Object Model: nNormal= %d \n", model->nNormal);
 
 	v_array = new Vertex[model->nTriangle];
 	v_num = model->nTriangle;

 	for(int i=0; i<model->nTriangle; i++)
 	{

 		for(int j=0; j<3; j++)
 		{
	 		struct Vertex v;

	 		int iv = model->TriangleArray[i].Vertex[j];
	 		int itx = model->TriangleArray[i].TexCoord[j];

	 		printf("triangle: %i,  iv= %i itx= %i \n", i, iv, itx);
	 		v.x = model->VertexArray[iv].x;
	 		v.y = model->VertexArray[iv].y;
	 		v.z = model->VertexArray[iv].z;
	 		v.u = model->TexCoordArray[itx].u;
	 		v.v = model->TexCoordArray[itx].v;
	 		////v.n[0] = 
	 		//v.n[1] = 
	 		//v.n[2] =

	 		v_array[i] = v;
 		}
 	}
}

class SHADER monster_shader;
unsigned int monster_texture;

unsigned int monster_TexCoord;
unsigned int monster_InPosition;

unsigned int monster_vbo;

void init_shader()
{
    monster_shader.set_debug(false);

    monster_shader.load_shader( "insect mob shader",
        "./media/shaders/mob/monster_mob.vsh",
        "./media/shaders/mob/monster_mob.fsh" );
   
    monster_TexCoord = monster_shader.get_attribute("InTexCoord");

    monster_InPosition = monster_shader.get_uniform("InPosition");
}

void init_texture()
{
    SDL_Surface* s = create_surface_from_file((char*) "./media/mesh/mob1_dm.png");

    if(s == NULL)
    {
        printf("init_insect_mob: texture load error\n");
        GS_ABORT();
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &monster_texture );
    glBindTexture( GL_TEXTURE_2D, monster_texture );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    GLuint internalFormat = GL_RGBA; //GL_RGBA;
    GLuint format = GL_RGBA;
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);
}

void init_draw_model()
{
	init_shader();
	init_texture();
}


void draw_model(float x, float y, float z)
{

    const static unsigned int stride = sizeof(struct vertexElement2);

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, monster_texture );

    glBindBuffer(GL_ARRAY_BUFFER, monster_vbo);
    
    glUseProgramObjectARB(monster_shader.shader);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(insect_mob_TexCoord);

    glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);
    glVertexAttribPointer(monster_TexCoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)12);

    glUniform3f(monster_InPosition, x,y,z);

    glDrawArrays(GL_TRIANGLES,0, v_num);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(monster_TexCoord);
    glUseProgramObjectARB(0);
}

void free_model(struct ObjModel* model)
{
   free(model->NormalArray);
   free(model->TexCoordArray);
   free(model->TriangleArray);
   free(model->VertexArray);
   free(model);
}


}