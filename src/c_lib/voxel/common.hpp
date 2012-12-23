#pragma once

namespace Voxels
{

union Voxel 
{
    struct
    {
        unsigned char r,g,b,a;
    };
    unsigned int color;
};

struct VoxelNormal
{
    union
    {
        char normal[4]; //16
        unsigned int n;
    };
};

struct VoxAOElement
{
    union
    {
        struct
        {
            unsigned char ao[4]; //32
        };
        unsigned int AO;
    };
};

struct VoxelTex
{
    union
    {
        unsigned char tex[4]; //32
        unsigned int t;
    };
};

class VoxelVertex
{
    public:
    
    float x,y,z;

    // these all must be 32bit aligned due to some GPU model bugs
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
    union
    {
        char ao[4]; //20
        unsigned int AO;
    };
    union
    {
        char tex[4]; //24
        unsigned int t;
    };
};

#if DC_CLIENT
class VoxelVertexList
{
    public:
    VoxelVertex* vertex_list;   //number of vertices

    int vnum;   //number of vertices
    int voff;   //offset of vertices

    VoxelVertexList() :
        vertex_list(NULL), vnum(0), voff(0)
    {}

    ~VoxelVertexList()
    {
        if(vertex_list != NULL)
            delete[] vertex_list;
    }
};
#endif

//fast rond up to next power of two
unsigned next_pow2(unsigned x)
{
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

}   // Voxels
