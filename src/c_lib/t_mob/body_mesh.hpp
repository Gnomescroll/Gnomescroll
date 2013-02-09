#pragma once

#include <physics/mat4.hpp>
#include <t_mob/body_part_mesh.hpp>

namespace t_mob
{

class BodyMesh
{
    public:
        class BodyPartMesh* ml; //body part mesh list
        int mlm;                 //body part mesh list max

        //nodes
        char** nnl;         //node name list
        int* npl;           //node parent list
        struct Mat4* node_mTransformation;    //node transform list, mTransformation
        int nm;             //node max

        //bones
        char** bnl;     //bone name list
        struct Mat4* bone_mOffsetMatrix;
        int* bpl;        //bone parent list; index of parent node
        int blm;        //bone list max

        struct Mat4* tbone_matrix;
        struct Mat4* tnode_matrix;

        GLuint texture;
        SDL_Surface* s;

    BodyMesh() :
        ml(NULL), mlm(0), nnl(NULL), npl(NULL),
        node_mTransformation(NULL), nm(0), bnl(NULL),
        bone_mOffsetMatrix(NULL), bpl(NULL), blm(0),
        tbone_matrix(NULL), tnode_matrix(NULL), texture(0), s(NULL)
    {
    }

    ~BodyMesh()
    {
        if (this->nnl != NULL) delete[] this->nnl;
        if (this->npl != NULL) delete[] this->npl;
        if (this->node_mTransformation != NULL) delete[] this->node_mTransformation;
        if (this->bnl != NULL) delete[] this->bnl;
        if (this->bone_mOffsetMatrix != NULL) delete[] this->bone_mOffsetMatrix;
        if (this->bpl != NULL) delete[] this->bpl;
        if (this->tbone_matrix != NULL) delete[] this->tbone_matrix;
        if (this->tnode_matrix != NULL) delete[] this->tnode_matrix;
        if (this->s != NULL) SDL_FreeSurface(this->s);
        if (this->ml != NULL) delete[] this->ml;
    }

    void load(class ModelLoader* ml);
    void draw_prep();


    void draw(float x, float y, float z, float theta, float phi);


    void draw(float x, float y, float z)
    {
        draw(x,y,z, 0.0, 0.0);
    }


    void init_texture();
};

}   // t_mob
