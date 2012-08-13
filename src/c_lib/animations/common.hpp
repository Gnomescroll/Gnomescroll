#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/compat_gl.h>
#include <physics/vec3.hpp>

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
     
    void compute_face_normals(unsigned int vertex_stride)
    {
        GS_ASSERT(vertex_stride == 3 || vertex_stride == 4);
        GS_ASSERT(this->vlist_index % vertex_stride == 0);
        for (int i=0; i<this->vlist_index; i+=vertex_stride)
        {
            struct Vec3 a = vec3_sub(this->vlist[i+1].pos, this->vlist[i].pos);
            normalize_vector(&a);
            struct Vec3 b = vec3_sub(this->vlist[i+vertex_stride-1].pos, this->vlist[i].pos);
            normalize_vector(&b);
            struct Vec3 n = vec3_cross(a,b);
            normalize_vector(&n);
            for (unsigned int j=0; j<vertex_stride; j++)
                this->vlist[i+j].n = n;
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
