#pragma once


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

    unsigned int VBO;
    unsigned int vertex_number;

    VertexElementList1()
    {
        VBO = 0;
        vertex_number = 0;

        vlist_index = 0;
        vlist_max = 1024;
        vlist = (vertexElement1*) malloc(1024*sizeof(struct vertexElement1));
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
            vlist = (vertexElement1*) realloc(vlist, vlist_max*sizeof(struct vertexElement1));
            printf("1 size= %i \n", vlist_max); 

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
        
            //printf("1 wtf 1\n");

        } 
        else
        {
            if(vertex_number > 0) 
            {
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
            }

            //printf("1 wtf 2\n");
        }


        vertex_number = vlist_index;

        vlist_index = 0;
    }

};

/*
    Textyred vertex element with normal
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

    unsigned int VBO;
    unsigned int vertex_number;

    VertexElementList2()
    {
        VBO = 0;
        vertex_number = 0;

        vlist_index = 0;
        vlist_max = 1024;
        vlist = (vertexElement2*) malloc(1024*sizeof(struct vertexElement2));
    }

    ~VertexElementList2()
    {
        free(vlist);
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
            printf("1 size= %i \n", vlist_max); 

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
        
            //printf("2 wtf 1\n");
        } 
        else
        {
            if(vertex_number > 0) 
            {
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
            }

            //printf("2 wtf 2\n");
        }


        vertex_number = vlist_index;

        vlist_index = 0;
    }

};

}   // Animations
