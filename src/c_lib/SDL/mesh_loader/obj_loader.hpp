#pragma once

/*

Ray Triangle Intersection
float rayTriangleIntersection(lbVec3f& rayOrigin,lbVec3f& rayDir, lbVec3f& vertA,lbVec3f& vertB,lbVec3f& vertC)
{
    lbVec3f edge1=vertB-vertA;
    lbVec3f edge2=vertC-vertA;

    lbVec3f pvec=rayDir.crossed(edge2); //cross product
    float det=edge1.dot(pvec);  //dot
    if (det>-1.0e-9f && det<1.0e-9f)
    {
        return -1;
    }
    float invDet=1.0f/det;

    lbVec3f dst=rayOrigin-vertA;    //sub
    float u=dst.dot(pvec)*invDet;   //dot
    if (u<0.0f || u>1.0f)
    {
        return -1;
    }

    lbVec3f qvec=dst.crossed(edge1);    //cross
    float v=rayDir.dot(qvec)*invDet;    //dot
    if (v<0.0f || u+v>1.0f)
    {
        return -1;
    }

    return edge2.dot(qvec)*invDet;      //dot and scalar
}

//<Spliter> HaltingState: it returns a value between 0 and 1 if it hits and >=1 if it doesn't hit

<Spliter> HaltingState: yes, using that value and the start and end of the ray you can obtain the exact point at which it hit the triangle

{
    float time=rayTriangleIntersection(rayOrigin, rayDir   ,
                            _verts[_tris[i].ids[0]].fpos,
                            _verts[_tris[i].ids[1]].fpos,
                            _verts[_tris[i].ids[2]].fpos);
        if (time<minTime&&time>=0.0f)
            minTime=time;
}
*/

/*
    Smoothing Groups:
    http://www.martinreddy.net/gfx/3d/OBJ.spec

    Calculating Normals for Smoothing Groups:
    http://www.gamedev.net/topic/486058-normal-calculation-by-using-smoothing-groups-in-meshes/
*/


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
size_t ObjLoadFile(char*, char**);


ObjModel* m_model = NULL;

struct Vertex* v_array = NULL;
int v_num = 0;

void load_model()
{
    char* memory = NULL;
    //char* model_file = (char*) MEDIA_PATH "mesh/mob4.obj";
    //char* model_file = (char*) MEDIA_PATH "mesh/drone1.obj";
    char* model_file = (char*) MEDIA_PATH "mesh/cube_test.obj";

    size_t bytes = ObjLoadFile(model_file, &memory);

    if (memory == NULL)
    {
        printf("ERROR: %s -- failed to load file %s\n", __FUNCTION__, model_file);
        return;
    }

   ObjModel* model = ObjLoadModel(memory, bytes);

   m_model = model;

   printf("Object Model: nTriangle= %d \n", model->nTriangle);
   printf("Object Model: nVertex= %d \n", model->nVertex);
   printf("Object Model: nTexCoord= %d \n", model->nTexCoord);
   printf("Object Model: nNormal= %d \n", model->nNormal);

    v_num = model->nTriangle*3;
    v_array = new Vertex[v_num];
    memset(v_array, 0, v_num*sizeof(Vertex));

    for (int i=0; i<model->nTriangle; i++)
    {

        for (int j=0; j<3; j++)
        {
            struct Vertex v;

            int iv = model->TriangleArray[i].Vertex[j] -1;
            int itx = model->TriangleArray[i].TexCoord[j] -1;
            int in = model->TriangleArray[i].Normal[j] -1;

            if (iv >= model->nVertex)
            {
                printf("ERROR: iv= %i  nVertex= %i \n", iv, model->nVertex);
            }
            if (itx >= model->nTexCoord)
            {
                printf("ERROR: itx= %i  nTexCoord= %i \n", itx, model->nTexCoord);
            }
            if (in >= model->nNormal)
            {
                printf("ERROR: in= %i  nNormal= %i \n", in, model->nNormal);
            }
            //printf("triangle: %i,  iv= %i itx= %i \n", i, iv, itx);
            v.x = model->VertexArray[iv].x;
            v.y = model->VertexArray[iv].y;
            v.z = model->VertexArray[iv].z;

            v.u = model->TexCoordArray[itx].u;
            v.v = 1.0f - model->TexCoordArray[itx].v;

            v.n[0] = model->NormalArray[in].x;
            v.n[1] = model->NormalArray[in].y;
            v.n[2] = model->NormalArray[in].z;

            //printf("len= %f \n", v.n[0]*v.n[0] + v.n[1]*v.n[1] + v.n[2]*v.n[2]);

            //printf("normal= %f %f %f \n",  v.n[0],v.n[1],v.n[2]);

            v_array[3*i+j] = v;
        }
    }

    free(memory);
}

class Shader monster_shader;
GLuint monster_texture;
GLuint monster_TexCoord;
GLuint monster_InPosition;
GLuint monster_vbo;

void init_shader()
{
    monster_shader.set_debug(true);

    monster_shader.load_shader("monster mob shader",
        MEDIA_PATH "shaders/mob/monster_mob.vsh",
        MEDIA_PATH "shaders/mob/monster_mob.fsh");

    monster_TexCoord = monster_shader.get_attribute("InTexCoord");

    monster_InPosition = monster_shader.get_uniform("InPosition");
}

void init_texture()
{
    //SDL_Surface* s = create_surface_from_file((char*) MEDIA_PATH "mesh/drone0.png");
    SDL_Surface* s = create_surface_from_file((char*) MEDIA_PATH "mesh/red_flat.png");

    if (s == NULL)
    {
        printf("init_monster_mob: texture load error\n");
        GS_ABORT();
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &monster_texture);
    glBindTexture(GL_TEXTURE_2D, monster_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLenum internalFormat = 4; //GL_RGBA;
    GLenum format = GL_BGRA;
    if (s->format->Rmask == 0x000000ff)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels);
    glDisable(GL_TEXTURE_2D);

    SDL_FreeSurface(s);
}

void init_draw_model()
{
    load_model();
    init_shader();
    init_texture();

    const static unsigned int stride = sizeof(struct Vertex);
    glGenBuffers(1, &monster_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, monster_vbo);
    glBufferData(GL_ARRAY_BUFFER, v_num*stride, v_array, GL_STATIC_DRAW);

}


void draw_model(float x, float y, float z)
{
    x = ClientState::location_pointer.x;
    y = ClientState::location_pointer.y;
    z = ClientState::location_pointer.z;
    z+= 2.0f;

    const static unsigned int stride = sizeof(struct Vertex);

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindTexture(GL_TEXTURE_2D, monster_texture);

    glBindBuffer(GL_ARRAY_BUFFER, monster_vbo);

    glUseProgramObjectARB(monster_shader.shader);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(monster_TexCoord);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);
    glVertexAttribPointer(monster_TexCoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)12);
    glNormalPointer(GL_FLOAT, stride, (GLvoid*)20);

    glUniform4f(monster_InPosition, x,y,z,0.0f);

    glDrawArrays(GL_TRIANGLES,0, v_num);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(monster_TexCoord);
    glDisableClientState(GL_NORMAL_ARRAY);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
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

}   // obj_load
