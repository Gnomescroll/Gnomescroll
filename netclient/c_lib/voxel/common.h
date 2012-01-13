#pragma once


union Voxel {
    struct
    {
        unsigned char r,g,b,a;
    };
    unsigned int color;
};

struct Voxel_normal
{
    union
    {
        char normal[4]; //16
        unsigned int n;
    };
};

struct Voxel_vertex
{
    float x,y,z;
    union
    {
        unsigned char rgba[4]; //12
        unsigned int color;
    };
    union
    {
        char normal[4]; //16
        unsigned int n;
    };
    //can compute normals from t
};

#ifdef DC_CLIENT
class Voxel_vertex_list
{
    public:
    Voxel_vertex* vertex_list;   //number of vertices
    int size;   //offset of vertices

    unsigned short vnum;   //number of vertices
    unsigned short voff;   //offset of vertices

    Voxel_vertex_list()
    :
    vertex_list(NULL),
    size(0), vnum(0), voff(0)
    {}

    ~Voxel_vertex_list()
    {
        if(vertex_list != NULL) free(vertex_list);
    }
};
#endif

/*
struct Voxel {
    unsigned char r,g,b,a;
};
*/

//fast rond up to next power of two
unsigned NextPow2( unsigned x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}


//round up to next power of 2 log 2
int pow2_1(int n)
{
    int x = 1;
    int i = 0;
    while(x < n) 
    {
        i++;
        x <<= 1;
    }
    return i;
} 

//round up to next power of two
int pow2_2(int n)
{
    int x = 1;
    //int i = 0;
    while(x < n) 
    {
        //i++;
        x <<= 1;
    }
    return x;
}