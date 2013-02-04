#pragma once

namespace t_mob
{

class BodyPartMesh
{
    public:
        char* mesh_name;

        int bvlm;                   //base vertex list max
        struct _Vertex* bvl;        //base vertex list
        struct _Vertex* tbvl;

        struct _Vertex* tvl;        //vertex list
        int tvln;                   //vertex list max

        int* via;                   //vertex index array
        int viam;

        struct _VertexWeight* vwl;  //vertex weight list
        int vwlm;                   //vertex weight list

    BodyPartMesh() :
        mesh_name(NULL), bvlm(0), bvl(NULL), tbvl(NULL), tvl(NULL), tvln(0),
        via(NULL), viam(0), vwl(NULL), vwlm(0)
    {
    }

    ~BodyPartMesh()
    {
        if (this->mesh_name != NULL) free(this->mesh_name);
        if (this->bvl != NULL) delete[] this->bvl;
        if (this->tbvl != NULL) delete[] this->tbvl;
        if (this->tvl != NULL) delete[] this->tvl;
        if (this->vwl != NULL) delete[] this->vwl;
        if (this->via != NULL) delete[] this->via;
    }

    //assumes only one mesh per node
    void load(struct _Mesh* mesh);
};

}   // t_mob
