#include "model_loader.hpp"

#include <t_mob/assimp_includes.hpp>
#include <physics/mat4.hpp>

namespace t_mob
{

ModelLoader::~ModelLoader()
{
    if (this->pScene != NULL) aiReleaseImport(this->pScene);
    delete[] this->nl;
    if (this->_nl != NULL)
        for (int i=0; i<this->_nlm; i++)
            free(this->_nl[i].name);
    delete[] this->_nl;
    delete[] this->_ml;
    delete[] this->bnl;
}

void ModelLoader::init(const aiScene* pScene)
{
    this->pScene = pScene;
    this->nli = 0;
    this->count_nodes(this->pScene->mRootNode); //count the nodes with meshes
    this->nlm = this->nli;

    this->nl = new aiNode*[this->nlm];
    for (int i=0; i<this->nlm; i++) nl[i] = NULL;

    this->nli = 0;
    this->set_node_parents(this->pScene->mRootNode);

    this->initialize_meshes1();
    this->init_bone_list();
    this->init_node_list();
    this->init_bone_list2();
    this->initialize_meshes2();
}

void ModelLoader::count_nodes(aiNode* pNode)
{
    GS_ASSERT(pNode->mNumMeshes < 2); //assume only one mesh per node for now
    for (size_t i=0; i < pNode->mNumMeshes; i++) this->nli++;
    for (size_t i=0; i < pNode->mNumChildren; i++)
        count_nodes(pNode->mChildren[i]);
}

void ModelLoader::set_node_parents(aiNode* pNode)
{   //only includes nodes that have meshes?
    if (pNode->mNumMeshes != 0)
    {
        GS_ASSERT(this->nli < this->nlm);
        this->nl[this->nli] = pNode;
        GS_ASSERT(pNode->mNumMeshes == 0 || pNode->mNumMeshes == 1);
        this->nli++;
    }
    for (size_t i=0; i < pNode->mNumChildren; i++)
        this->set_node_parents(pNode->mChildren[i]);
}

void ModelLoader::initialize_meshes1()
{
    int mesh_count = 0;
    for (int i=0; i<this->nlm; i++)
    {
        aiNode* node = this->nl[i];
        GS_ASSERT(node->mNumMeshes == 0 || node->mNumMeshes == 1);
        if (node->mNumMeshes == 1) mesh_count++;
    }

    this->_mlm = mesh_count;
    this->_ml = new struct _Mesh[this->_mlm];

    mesh_count = 0;
    for (int i=0; i<this->nlm; i++)
    {
        aiNode* node = this->nl[i];
        if (node->mNumMeshes == 1)
        {
            int mesh_index = node->mMeshes[0]; //grab the first mesh
            this->_ml[mesh_count].mesh = this->pScene->mMeshes[mesh_index];
            this->_ml[mesh_count].node = node;
            mesh_count++;
        }
    }
    //printf("init_mesh: mesh_count= %d node_count= %d \n", mesh_count, nlm);

    for (int i=0; i<this->_mlm; i++)
    {   //set vertex base array
        aiMesh* mesh = this->_ml[i].mesh;
        this->_ml[i].bvln = mesh->mNumVertices;
        this->_ml[i].bvl = new struct _Vertex[mesh->mNumVertices];
        for (unsigned int j=0; j<mesh->mNumVertices; j++)
        {
            aiVector3D pos = mesh->mVertices[j];
            aiVector3D tex = mesh->mTextureCoords[0][j];
            struct _Vertex v;
            v.v.x = pos.x;
            v.v.y = pos.y;
            v.v.z = pos.z;
            v.ux =  tex.x;
            v.uy =  1.0f - tex.y;
            this->_ml[i].bvl[j] = v;
        }
    }

    for (int i=0; i<this->_mlm; i++)
    {   // Vertex count
        aiMesh* mesh = this->_ml[i].mesh;
        this->_ml[i].vln = 3 * mesh->mNumFaces;
    }

    for (int i=0; i<this->_mlm; i++)
    {   // vertex index array
        aiMesh* mesh = this->_ml[i].mesh;
        GS_ASSERT(mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);
        GS_ASSERT(mesh->mTextureCoords[0] != NULL);
        this->_ml[i].viam = 3 * mesh->mNumFaces;
        this->_ml[i].via = new int[3 * mesh->mNumFaces];

        for (unsigned int j=0; j<mesh->mNumFaces; j++)
            for (int k=0; k<3; k++)
                this->_ml[i].via[3*j + k] = mesh->mFaces[j].mIndices[k];

        for (unsigned int j=0; j < 3*mesh->mNumFaces; j++)
            GS_ASSERT(this->_ml[i].via[j] < this->_ml[i].viam);

        for (unsigned int j=0; j<mesh->mNumFaces; j++)
        {
            GS_ASSERT(mesh->mFaces[j].mNumIndices == 3);
            GS_ASSERT(mesh->mNumUVComponents[0] == 2);
        }
    }

    for (int i=0; i<this->_mlm; i++)
    {   // vertex weights
        int index = 0;
        aiMesh* mesh = this->_ml[i].mesh;
        for (unsigned int j=0; j<mesh->mNumBones; j++)
        {
            aiBone* bone = mesh->mBones[j];
            index += bone->mNumWeights;
        }
        this->_ml[i].vwlm = index;
        this->_ml[i].vwl = new _VertexWeight[index];
    }
}

void ModelLoader::initialize_meshes2()
{   //needs bone list done first
    for (int i=0; i<this->_mlm; i++)
    {
        int index = 0;
        aiMesh* mesh = this->_ml[i].mesh;
        for (unsigned int j=0; j<mesh->mNumBones; j++)
        {
            aiBone* bone = mesh->mBones[j];
            for (unsigned int k=0; k<bone->mNumWeights; k++)
            {
                this->_ml[i].vwl[index].bone_index = get_bone_index(bone);       //index of bone matrix
                this->_ml[i].vwl[index].vertex_index = bone->mWeights[k].mVertexId;
                this->_ml[i].vwl[index].weight = bone->mWeights[k].mWeight;
                index++;
            }
        }
    }
}

bool ModelLoader::bone_in_list(aiBone* bone, int bone_count)
{
    for (int i=0; i<bone_count; i++)
        if (strcmp(this->bnl[i].name, bone->mName.data) == 0)
            return true;
    return false;
}

int ModelLoader::get_bone_index(aiBone* bone)
{
    for (int i=0; i<this->bnlm;i++)
        if (strcmp(this->bnl[i].name, bone->mName.data) == 0)
            return i;
    GS_ASSERT(false);
    return 0;
}

void ModelLoader::init_bone_list()
{   //count bones
    int bone_count = 0;
    int _bone_count = 0;

    //count number of times bone appears
    for (int i=0; i<this->_mlm; i++)
    {
        aiMesh* mesh = this->_ml[i].mesh;
        for (unsigned int j=0; j<mesh->mNumBones; j++)
        {
            bool new_bone = true;
            for (int _i=0; _i<=i; _i++)
            {
                for (unsigned int _j=0; _j<this->_ml[_i].mesh->mNumBones; _j++)
                {
                    if (_i == i && _j == j) break;
                    if (strcmp(this->_ml[i].mesh->mBones[j]->mName.data,
                               this->_ml[_i].mesh->mBones[_j]->mName.data) == 0)
                    {
                        new_bone = false;
                        break;
                    }
                }
            }
            if (new_bone) bone_count++;
            _bone_count++;
        }
    }

    //printf("init_bone_list: bone_count= %d _bone_count= %d \n", bone_count, _bone_count);
    //set bone list
    this->bnlm = bone_count;
    this->bnl = new BoneNode[this->bnlm];

    for (int i=0; i<this->bnlm; i++)
    {
        this->bnl[i].name = NULL;
        this->bnl[i].parent_node = NULL;
    }

    //populate bone list
    bone_count = 0;
    for (int i=0; i<this->_mlm; i++)
    {
        aiMesh* mesh = this->_ml[i].mesh;
        for (unsigned int j=0; j<mesh->mNumBones; j++)
        {
            aiBone* bone = mesh->mBones[j];
            if (bone_in_list(bone, bone_count)) continue;
            this->bnl[bone_count].name = copy_string(bone->mName.data);
            this->bnl[bone_count].mOffsetMatrix = _convert_matrix(bone->mOffsetMatrix);
            this->bnl[bone_count].parent_node =  find_node_by_name(bone->mName.data);
            this->bnl[bone_count].parent_index = -1;
            bone_count++;
        }
    }
    GS_ASSERT(bone_count == this->bnlm);
}

void ModelLoader::init_bone_list2()
{   //set bone parents
    for (int i=0; i<this->bnlm; i++)
        this->bnl[i].parent_index = node_index_from_list(this->bnl[i].parent_node);
}

bool ModelLoader::node_in_list(struct aiNode* node, int node_count)
{
    for (int i=0; i<node_count; i++)
    {
        GS_ASSERT(this->_nl[i].name != NULL);
        if (strcmp(node->mName.data, this->_nl[i].name) == 0)
            return true;
    }
    return false;
}

int ModelLoader::node_index_from_list(struct aiNode* node)
{
    IF_ASSERT(node == NULL) return 0;
    for (int i=0; i<this->_nlm; i++)
        if (strcmp(node->mName.data, this->_nl[i].name) == 0)
            return i;
    GS_ASSERT(false);
    //printf("_nlm= %d node_name= %s \n", this->_nlm, node->mName.data);
    return 0;
}

void ModelLoader::_set_node_index(int* index, Node* node)
{
    //printf("node_set_index: node_node= %s node_index= %d \n", node->name, *index);
    node->index = *index;
    (*index)++;
    for (int i=0; i<node->cn; i++)
        this->_set_node_index(index, node->c[i]);
}

void ModelLoader::init_node_list()
{   //populate node list
    GS_ASSERT(this->_nl == NULL);
    this->_nl = new struct Node[this->nlm];
    this->_nlm = nlm;

    int node_count = 0;

    for (int i=0; i<this->bnlm; i++)
    {   // start with the mesh-to-bone matrix
        aiNode* tempNode = this->bnl[i].parent_node;
        GS_ASSERT(tempNode != NULL)
        while (tempNode)
        {   //assume that if a node is, then all its parents are in?
            if (node_in_list(tempNode, node_count)) break;
            //insert
            this->_nl[node_count].name            = copy_string(tempNode->mName.data);
            this->_nl[node_count].mTransformation = _convert_matrix(tempNode->mTransformation);
            this->_nl[node_count].node            = tempNode;
            this->_nl[node_count].p               = NULL;
            this->_nl[node_count].c               = NULL;
            this->_nl[node_count].cn              = 0;
            this->_nl[node_count].index           = -1;
            node_count++;
            if (strcmp(tempNode->mName.data, "Armature") == 0) break;
            IF_ASSERT(tempNode == NULL) break;
            tempNode = tempNode->mParent;
        }
    }
    this->_nlm = node_count;  //some overhang on array

    //printf("init_node_list: node_count= %d nlm= %d _nlm= %d \n", node_count, this->nlm, this->_nlm);
    for (int i=0; i<this->_nlm; i++)
    {   //set parent node
        aiNode* node = this->_nl[i].node;
        if (strcmp(node->mName.data, "Armature") == 0)   //parent should be null for armature which is top level
        {
            this->_nl[i].p = NULL;
            continue;
        }
        int index = this->node_index_from_list(node->mParent);
        this->_nl[i].p = &this->_nl[index];
    }

    for (int i=0; i<this->_nlm; i++)
    {   //set children
        struct Node* node = &this->_nl[i];
        int child_count = 0;
        for (int j=0; j<this->_nlm; j++)
            if (this->_nl[j].p == node)
                child_count++;
        node->c = new Node*[child_count];
        node->cn = child_count;
        child_count = 0;
        for (int j=0; j<this->_nlm; j++)
            if (this->_nl[j].p == node)
                node->c[child_count++] = &this->_nl[j];
    }

    //for (int i=0; i<this->_nlm; i++)
        //printf("Node %02d name: %s children_number= %d \n", i, this->_nl[i].name, this->_nl[i].cn);

    struct Node* root_node = NULL;
    for (int i=0; i<this->_nlm; i++)
    {   //set index
        if (root_node != NULL)
        {
            GS_ASSERT(this->_nl[i].p != NULL);
        }
        if (this->_nl[i].p == NULL)
            root_node = &this->_nl[i];
    }
    int index = 0;
    this->_set_node_index(&index, root_node);

    for (int i=0; i<this->_nlm; i++)
    {   //make sure parent stuff is setup
        if (this->_nl[i].index == 0)
        {
            GS_ASSERT(this->_nl[i].p == NULL);
            continue;
        }
        else
        {
            IF_ASSERT(this->_nl[i].index <= this->_nl[i].p->index ||
                this->_nl[i].index == -1 ||
                this->_nl[i].index >= this->_nlm)
            {
                printf("ERROR: i=%d index= %d node_name= %s parent_index= %d "
                       "parent_node_name= %s \n", i, this->_nl[i].index,
                       this->_nl[i].name, this->_nl[i].p->index,
                       this->_nl[i].p->name);
            }
        }
    }

    for (int i=0; i<this->_nlm; i++)
    {   // sort
        if (this->_nl[i].index != i)
        {
            struct Node ntmp;
            bool found = false;
            for (int j=0; j<this->_nlm; j++)
            {
                if (this->_nl[j].index == i)
                {
                    ntmp = this->_nl[i];
                    this->_nl[i] = this->_nl[j];
                    this->_nl[j] = ntmp;
                    found = true;
                    break;
                }
            }
            GS_ASSERT(found);
        }
    }
    for (int i=0; i<this->_nlm; i++)
        GS_ASSERT(this->_nl[i].index == i);

    for (int i=0; i<this->_nlm; i++)
    {   //reset parent node
        aiNode* node = this->_nl[i].node;
        if (strcmp(node->mName.data, "Armature") == 0)
        {   //parent should be null for armature which is top level
            this->_nl[i].p = NULL;
            continue;
        }
        int index = this->node_index_from_list(node->mParent);
        this->_nl[i].p = &this->_nl[index];
    }

    for (int i=0; i<this->_nlm; i++)
    {   //reset children
        struct Node* node = &this->_nl[i];
        int child_count = 0;
        for (int j=0; j<this->_nlm; j++)
            if (this->_nl[j].p == node)
                child_count++;

        node->c = new Node*[child_count];
        node->cn = child_count;

        child_count = 0;
        for (int j=0; j<this->_nlm; j++)
            if (this->_nl[j].p == node)
                node->c[child_count++] = &this->_nl[j];
    }
}

aiNode* ModelLoader::find_node_recursively_by_name(aiNode* pNode, char* node_name)
{
    if (strcmp(node_name, pNode->mName.data) == 0) return pNode;
    for (size_t i=0; i < pNode->mNumChildren; i++)
    {
        aiNode* tmp = this->find_node_recursively_by_name(pNode->mChildren[i],
                                                          node_name);
        if (tmp != NULL) return tmp;
    }
    return NULL;
}

aiNode* ModelLoader::find_node_by_name(char* node_name)
{
    aiNode* node = this->find_node_recursively_by_name(pScene->mRootNode,
                                                       node_name);
    GS_ASSERT(node != NULL);
    return node;
}

struct Mat4 ModelLoader::get_anim_matrix(int frame_time, aiNodeAnim** node_channels, int node_channel_max, aiNode* node)
{   //get matrix for a node, if not is not animated, just returns the scene matrix
    /* Another complication is how to use interpolation. It is possible
    to create a transformation matrix from the rotation, location and
    scaling, but interpolating rotation using transformation matrices
    are not good. If you interpolate an object moved in an arch defined
    by two keys 90 degrees rotated to each other, the result will be
    a straight line (at best). It actually looks quite fun, but is not
    good. Therefore, you usually should do the interpolation on the
    quaternions first, then create the transformation matrix. */
    for (int i=0; i<node_channel_max; i++)
    {
        aiNodeAnim* anim = node_channels[i];
        //printf("node channel= %s \n", anim->mNodeName.data);

        if (strcmp(anim->mNodeName.data, node->mName.data) == 0)
        {
            GS_ASSERT(anim->mNumPositionKeys == anim->mNumRotationKeys);
            int tmax = anim->mNumPositionKeys;

            aiVectorKey pos =  anim->mPositionKeys[frame_time % tmax];
            aiQuatKey rot = anim->mRotationKeys[frame_time % tmax];
            GS_ASSERT(pos.mTime == rot.mTime);

            //CONVERT TO MATRIX
            return quantenion_to_rotation_matrix(rot.mValue, pos.mValue);
        }
    }
    //if cannot find, then node does not have an animation and use this
    return _convert_matrix(node->mTransformation);
}

}   // t_mob
