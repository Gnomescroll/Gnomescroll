#pragma once

#ifdef __MSVC__
extern "C"
{
#endif

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h> //defines for postprocessor
#include <assimp/config.h>

#ifdef __MSVC__
}
#endif

namespace t_mob
{

static struct Mat4 quantenion_to_rotation_matrix(aiQuaternion q, aiVector3D pos)
{
    float xx      = q.x * q.x;
    float xy      = q.x * q.y;
    float xz      = q.x * q.z;
    float xw      = q.x * q.w;
    float yy      = q.y * q.y;
    float yz      = q.y * q.z;
    float yw      = q.y * q.w;
    float zz      = q.z * q.z;
    float zw      = q.z * q.w;

    Mat4 m;
    m._f[0]  = 1 - 2 * (yy + zz);
    m._f[1]  =     2 * (xy - zw);
    m._f[2]  =     2 * (xz + yw);
    m._f[3]  = 0.0f;

    m._f[4]  =     2 * (xy + zw);
    m._f[5]  = 1 - 2 * (xx + zz);
    m._f[6]  =     2 * (yz - xw);
    m._f[7]  = 0.0f;

    m._f[8]  =     2 * (xz - yw);
    m._f[9]  =     2 * (yz + xw);
    m._f[10] = 1 - 2 * (xx + yy);
    m._f[11] = 0.0f;

    m._f[12] = 0.0f;
    m._f[13] = 0.0f;
    m._f[14] = 0.0f;

    m = mat4_transpose(m);

    m._f[12] = pos.x;
    m._f[13] = pos.y;
    m._f[14] = pos.z;

    m._f[15] = 1.0f;

    return m;
}

static struct Mat4 _convert_matrix(const aiMatrix4x4& in)
{
    Mat4 out;

    out.f[0][0] = in.a1;
    out.f[0][1] = in.a2;
    out.f[0][2] = in.a3;
    out.f[0][3] = in.a4;

    out.f[1][0] = in.b1;
    out.f[1][1] = in.b2;
    out.f[1][2] = in.b3;
    out.f[1][3] = in.b4;

    out.f[2][0] = in.c1;
    out.f[2][1] = in.c2;
    out.f[2][2] = in.c3;
    out.f[2][3] = in.c4;

    out.f[3][0] = in.d1;
    out.f[3][1] = in.d2;
    out.f[3][2] = in.d3;
    out.f[3][3] = in.d4;

    out = mat4_transpose(out);

    GS_ASSERT(out._f[0*4+3] == 0.0f && out._f[1*4+3] == 0.0f && out._f[2*4+3] == 0.0f && out._f[3*4+3] == 1.0f)

    return out;
}

char* copy_string(const char* xstr)
{
    char* nstr = (char*)malloc((strlen(xstr)+1)*sizeof(char));
    strcpy(nstr, xstr);
    return nstr;
}

struct _Vertex
{
    struct Vec3 v;
    float ux,uy;
};

struct _VertexWeight
{
    int bone_index; //index into bone matrix
    int vertex_index;       //mesh vertex index
    float weight;   //weight
};


struct _Mesh
{
    struct aiMesh* mesh;
    struct aiNode* node;

    int bvln;       //base vertex list max
    struct _Vertex* bvl; //base vertex list

    int vln;    //number of vertices

    int* via;   //vertex index array
    int viam;   //vertex index array max

    int vwlm;
    struct _VertexWeight* vwl;
};


}   // t_mob
