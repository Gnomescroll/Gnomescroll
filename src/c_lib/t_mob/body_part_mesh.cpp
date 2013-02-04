#include "body_part_mesh.hpp"

namespace t_mob
{

void BodyPartMesh::load(struct _Mesh* mesh)
{   //assumes only one mesh per node
    GS_ASSERT(this->mesh_name == NULL);
    this->mesh_name = copy_string(mesh->node->mName.data);

    //base vertex list
    this->bvlm = mesh->bvln;
    this->bvl = new struct _Vertex[bvlm];
    this->tbvl = new struct _Vertex[bvlm];

    for (int i=0; i<this->bvlm; i++)
        this->bvl[i] = mesh->bvl[i];

    //vertex list
    this->tvln = mesh->vln;
    this->tvl = new struct _Vertex[this->tvln];

    //vertex list index array
    this->viam = mesh->viam;
    this->via = new int[viam];

    for (int i=0; i<this->viam; i++)
        this->via[i] = mesh->via[i];

    //vertex weight list
    this->vwlm = mesh->vwlm;
    this->vwl = new struct _VertexWeight[this->vwlm];

    for (int i=0; i<this->vwlm; i++)
        this->vwl[i] = mesh->vwl[i];
}

}   // t_mob
