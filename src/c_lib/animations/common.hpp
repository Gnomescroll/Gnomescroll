#pragma once

#if DC_SERVER
# error Don't include this file in the server
#endif

#include <common/compat_gl.h>
#include <physics/vec3.hpp>
#include <common/color.hpp>
#include <voxel/common.hpp>

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

    VertexElementList1() :
        vlist(NULL), vlist_index(0), vlist_max(1024), VBO(0), vertex_number(0)
    {
        this->vlist = (vertexElement1*)malloc(vlist_max*sizeof(struct vertexElement1));
    }

    ~VertexElementList1()
    {
        free(this->vlist);
    }

    ALWAYS_INLINE void push_vertex(struct Vec3 pos, float tx, float ty)
     {
        vlist[vlist_index].pos = pos;
        vlist[vlist_index].tx = tx;
        vlist[vlist_index].ty = ty;

        vlist_index++;

        if (vlist_index >= vlist_max)
        {
            vlist_max *= 2;
            vlist = (vertexElement1*)realloc(vlist, vlist_max*sizeof(struct vertexElement1));
        }
     }

    //upload data to card for drawing
    void buffer()
    {
        if (this->VBO == 0) glGenBuffers(1, &this->VBO);

        if (vlist_index != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferData(GL_ARRAY_BUFFER, this->vlist_index*stride, NULL, GL_DYNAMIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, this->vlist_index*stride, vlist, GL_DYNAMIC_DRAW);
        }
        else
        {
            if (vertex_number > 0)
            {
                glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
                glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
            }
        }

        this->vertex_number = vlist_index;
        this->vlist_index = 0;
    }

};

template <class VertexElement>
class VertexElementList
{
    private:

    void add_element()
    {
        this->vlist_index++;
        if (this->vlist_index >= this->vlist_max)
        {
            this->vlist_max *= 2;
            this->vlist = (VertexElement*)realloc(this->vlist, this->vlist_max*sizeof(VertexElement));
        }
    }

    public:
        const static unsigned int stride = sizeof(VertexElement);

        VertexElement* vlist;
        int vlist_index;
        int vlist_max;

        GLuint VBO;
        unsigned int vertex_number;

    VertexElementList() :
        vlist_index(0), vlist_max(1024),
        VBO(0), vertex_number(0)
    {
        this->vlist = (VertexElement*)malloc(this->vlist_max*sizeof(VertexElement));
    }

    ~VertexElementList()
    {
        free(this->vlist);
    }

    ALWAYS_INLINE
    void push_vertex(struct Vec3 position, struct Vec3 normal, float tx, float ty)
    {
        this->vlist[this->vlist_index].position = position;
        this->vlist[this->vlist_index].normal   = normal;
        this->vlist[this->vlist_index].tx       = tx;
        this->vlist[this->vlist_index].ty       = ty;
        this->add_element();
    }

    ALWAYS_INLINE
    void push_vertex(struct Vec3 position, float tx, float ty)
    {
        this->vlist[this->vlist_index].position = position;
        this->vlist[this->vlist_index].tx       = tx;
        this->vlist[this->vlist_index].ty       = ty;
        this->add_element();
    }

    ALWAYS_INLINE
    void push_vertex(struct Vec3 position, struct Vec3 normal, Color color)
    {
        this->vlist[this->vlist_index].position = position;
        this->vlist[this->vlist_index].normal   = normal;
        this->vlist[this->vlist_index].color    = color;
        this->add_element();
    }

    ALWAYS_INLINE
    void push_vertex(struct Vec3 position, Color color, const char normal[3])
    {
        this->vlist[this->vlist_index].position    = position;
        this->vlist[this->vlist_index].color       = color;
        for (int i=0; i<3; i++)
            this->vlist[this->vlist_index].normal[i] = normal[i];
        this->add_element();
    }

    void buffer_static()
    {
        if (this->VBO == 0) glGenBuffers(1, &this->VBO);

        IF_ASSERT(this->VBO == 0) return;
        IF_ASSERT(this->vlist_index == 0) return;

        this->vertex_number = this->vlist_index;

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vlist_index*this->stride, this->vlist, GL_STATIC_DRAW);
    }

    void buffer()
    {   // upload data to card for drawing
        if (this->VBO == 0) glGenBuffers(1, &this->VBO);
        GS_ASSERT(this->VBO != 0);
        if (this->VBO != 0)
        {
            if (this->vlist_index != 0)
            {
                glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
                glBufferData(GL_ARRAY_BUFFER, this->vlist_index*this->stride, NULL, GL_DYNAMIC_DRAW);   // clears data or something
                glBufferData(GL_ARRAY_BUFFER, this->vlist_index*this->stride, this->vlist, GL_DYNAMIC_DRAW);
            }
            else
            {
                if (this->vertex_number > 0)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
                    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
                }
            }
        }

        this->vertex_number = this->vlist_index;
        this->vlist_index = 0;
    }

    void clear()
    {
        this->vlist_index = 0;
        this->vertex_number = 0;
    }
};

struct VertexElementTexture
{
    struct Vec3 position;
    float tx,ty;
};

struct VertexElementTextureNormal
{
    struct Vec3 position;
    struct Vec3 normal;
    float tx,ty;
};

struct VertexElementColorNormal
{
    struct Vec3 position;
    struct Vec3 normal;
    Color color;
};

struct VertexElementColorNormalByte
{                             // offset
    struct Vec3 position;    // 0
    Color color;              // 12

    // 32bit alignment; not all values are used
    char normal[4];         // 16
};

typedef class VertexElementList<struct VertexElementTexture> VertexElementListTexture;
typedef class VertexElementList<struct VertexElementTextureNormal> VertexElementListTextureNormal;
typedef class VertexElementList<struct VertexElementColorNormal> VertexElementListColor;
typedef class VertexElementList<struct VertexElementColorNormalByte> VertexElementListColorByte;

}   // Animations
