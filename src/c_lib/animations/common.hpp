#pragma once

#include <common/compat_gl.h>

namespace Animations 
{

/*
    Textured Vertex element without normal
*/

struct vertexElement1
{
    struct Vec3 pos;
    float tx,ty;
};

class VertexElementList1
{
    public:

    const static unsigned int stride = sizeof(struct vertexElement1);

    struct vertexElement1* vlist;
    int vlist_index;
    int vlist_max;

    GLuint VBO;
    unsigned int vertex_number;

    VertexElementList1()
    {
        VBO = 0;
        vertex_number = 0;

        vlist_index = 0;
        vlist_max = 1024;
        vlist = (vertexElement1*) malloc(vlist_max*sizeof(struct vertexElement1));
    }

    ~VertexElementList1()
    {
        if (this->vlist != NULL) free(this->vlist);
    }

    __attribute__((always_inline))
    inline void push_vertex(struct Vec3 pos, float tx, float ty)
     {
        vlist[vlist_index].pos = pos;
        vlist[vlist_index].tx = tx;
        vlist[vlist_index].ty = ty;

        vlist_index++;

        if(vlist_index >= vlist_max)
        {
            vlist_max *= 2;
            vlist = (vertexElement1*) realloc(vlist, vlist_max*sizeof(struct vertexElement1));
        }
     }

    //upload data to card for drawing
    void buffer()
    {
        if(VBO == 0) glGenBuffers(1, &VBO);

        if(vlist_index != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vlist_index*stride, NULL, GL_DYNAMIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, vlist_index*stride, vlist, GL_DYNAMIC_DRAW);
        } 
        else
        {
            if(vertex_number > 0) 
            {
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
            }
        }

        vertex_number = vlist_index;
        vlist_index = 0;
    }

};

/*
    Textured vertex element with normal
*/

struct vertexElement2
{
    struct Vec3 pos;

    float tx,ty;
    struct Vec3 n;
};

class VertexElementList2
{
    public:

    const static unsigned int stride = sizeof(struct vertexElement2);

    struct vertexElement2* vlist;
    int vlist_index;
    int vlist_max;

    GLuint VBO;
    unsigned int vertex_number;

    VertexElementList2()
    {
        VBO = 0;
        vertex_number = 0;

        vlist_index = 0;
        vlist_max = 1024;
        vlist = (vertexElement2*) malloc(vlist_max*sizeof(struct vertexElement2));
    }

    ~VertexElementList2()
    {
        if (this->vlist != NULL) free(this->vlist);
    }

    __attribute__((always_inline))
    void push_vertex(struct Vec3 pos, float tx, float ty, struct Vec3 normal)
     {
        vlist[vlist_index].pos = pos;
        vlist[vlist_index].tx = tx;
        vlist[vlist_index].ty = ty;
        vlist[vlist_index].n = normal;

        vlist_index++;

        if(vlist_index >= vlist_max)
        {
            vlist_max *= 2;
            vlist = (vertexElement2*) realloc(vlist, vlist_max*sizeof(struct vertexElement2));
        }
     }
     
    __attribute__((always_inline))
    void push_vertex(struct Vec3 pos, float tx, float ty)
     {
        vlist[vlist_index].pos = pos;
        vlist[vlist_index].tx = tx;
        vlist[vlist_index].ty = ty;

        vlist_index++;

        if(vlist_index >= vlist_max)
        {
            vlist_max *= 2;
            vlist = (vertexElement2*) realloc(vlist, vlist_max*sizeof(struct vertexElement2));
        }
     }

    //upload data to card for drawing
    void buffer()
    {
        if(VBO == 0) glGenBuffers(1, &VBO);

        if(vlist_index != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vlist_index*stride, NULL, GL_DYNAMIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, vlist_index*stride, vlist, GL_DYNAMIC_DRAW);
        } 
        else
        {
            if(vertex_number > 0) 
            {
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
            }
        }

        vertex_number = vlist_index;
        vlist_index = 0;
    }

};

}   // Animations
