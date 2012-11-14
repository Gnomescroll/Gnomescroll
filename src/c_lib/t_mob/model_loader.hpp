#pragma once

#include <physics/mat4.hpp>

#ifdef __MINGW32__
# undef __cplusplus
#endif
extern "C"
{
    #include <assimp/cimport.h>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h> //defines for postprocessor
    #include <assimp/config.h>
}
#ifdef __MINGW32__
# define __cplusplus
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
    m._f[0]  = 1 - 2 * ( yy + zz );
    m._f[1]  =     2 * ( xy - zw );
    m._f[2]  =     2 * ( xz + yw );
    m._f[3]  = 0.0f;

    m._f[4]  =     2 * ( xy + zw );
    m._f[5]  = 1 - 2 * ( xx + zz );
    m._f[6]  =     2 * ( yz - xw );
    m._f[7]  = 0.0f;

    m._f[8]  =     2 * ( xz - yw );
    m._f[9]  =     2 * ( yz + xw );
    m._f[10] = 1 - 2 * ( xx + yy );
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

static struct Mat4 _ConvertMatrix(const aiMatrix4x4& in)
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

    char* copy_string(char* xstr)
    {
        char* nstr = new char[strlen(xstr)+1];
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


class ModelLoader
{
    public:

    ModelLoader()
    {
        pScene = NULL;
        nl = NULL;
        _ml = NULL;
        _nl = NULL;

        bnl = NULL;
    }
    
    ~ModelLoader()
    {
        //if (s != NULL) SDL_FreeSurface(s);
        if (pScene != NULL) aiReleaseImport(pScene);
        if (nl != NULL) delete[] nl;
    }

    aiScene* pScene;    //the scene

    aiNode** nl;    //node list
    int nlm;        //node list max

    int nli;        //node list index (just a counter variable)


    void init(aiScene* _pScene)
    {
        pScene = _pScene;

        nli = 0;
        count_nodes(pScene->mRootNode); //count the nodes with meshes
        nlm = nli;
        
        nl = new aiNode*[nlm];
        for(int i=0; i<nlm; i++) nl[i] = NULL;

        nli = 0;
        set_node_parents(pScene->mRootNode);

        initialize_meshes1();

        init_bone_list();
        init_node_list();
        init_bone_list2();

        initialize_meshes2();
    }

    void count_nodes(aiNode* pNode)
    {
        GS_ASSERT(pNode->mNumMeshes < 2); //assume only one mesh per node for now
        for(unsigned int i=0; i < pNode->mNumMeshes; i++)
            nli++;
        for(unsigned int i=0; i < pNode->mNumChildren; i++)
            count_nodes(pNode->mChildren[i]);
    }

    //only includes nodes that have meshes?
    void set_node_parents(aiNode* pNode)
    {
        if(pNode->mNumMeshes != 0)
        {
            GS_ASSERT(nli < nlm);
            nl[nli] = pNode;
            GS_ASSERT(pNode->mNumMeshes == 0 || pNode->mNumMeshes == 1);
            nli++;
        }
        for(unsigned int i=0; i < pNode->mNumChildren; i++)
        {
            set_node_parents(pNode->mChildren[i]);
        }
    }

/*
    struct _Mesh
    {
        aiMesh* mesh;

        int bvln;       //base vertex list max
        struct _Vertex* bvl; //base vertex list

        int vln;    //number of vertices

        int* via;   //vertex index array
        int viam;   //vertex index array max

        int vwlm;
        struct _VertexWeight* vwl;
    };
*/

    struct _Mesh* _ml;
    int _mlm;

    void initialize_meshes1()
    {

        int mesh_count = 0;
        
        for(int i=0; i<nlm; i++)
        {
            aiNode* node = nl[i];
            GS_ASSERT(node->mNumMeshes == 0 || node->mNumMeshes == 1); 
            if(node->mNumMeshes == 1)
                mesh_count++;
        }

        _mlm = mesh_count;
        _ml = new struct _Mesh[_mlm];
        
        //for(int i=0; i<_mlm; i++)
        //    _ml[i] = NULL;

        mesh_count = 0;
        for(int i=0; i<nlm; i++)
        {
            aiNode* node = nl[i];
            if(node->mNumMeshes == 1)
            {
                int mesh_index = node->mMeshes[0]; //grab the first mesh
                _ml[mesh_count].mesh = pScene->mMeshes[mesh_index];
                _ml[mesh_count].node = node;
                mesh_count++;
            }
        }
        printf("init_mesh: mesh_count= %i node_count= %i \n", mesh_count, nlm);

        //set vertex base array
        for(int i=0; i<_mlm; i++)
        {
            aiMesh* mesh = _ml[i].mesh;

            _ml[i].bvln = mesh->mNumVertices;
            _ml[i].bvl = new struct _Vertex[mesh->mNumVertices];

            for(unsigned int j=0; j<mesh->mNumVertices; j++)
            {
                aiVector3D pos = mesh->mVertices[j];
                aiVector3D tex = mesh->mTextureCoords[0][j];

                struct _Vertex v; 
                v.v.x = pos.x;
                v.v.y = pos.y;
                v.v.z = pos.z;

                v.ux =  tex.x;
                v.uy =  1.0-tex.y;

                //printf("x,y,z= %f %f %f tex: x,y= %f %f \n", pos.x, pos.y, pos.z, tex.x, tex.y);
                _ml[i].bvl[j] = v;

            }
        }

        // Vertex count
        
        for(int i=0; i<_mlm; i++)
        {
            aiMesh* mesh = _ml[i].mesh;
            _ml[i].vln = 3*mesh->mNumFaces;
        }

        // vertex index array

        for(int i=0; i<_mlm; i++)
        {
            aiMesh* mesh = _ml[i].mesh;

            GS_ASSERT(mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);
            GS_ASSERT(mesh->mTextureCoords[0] != NULL);

            _ml[i].viam = 3*mesh->mNumFaces;
            _ml[i].via = new int[3*mesh->mNumFaces];

            for(unsigned int j=0; j<mesh->mNumFaces; j++)
            {
                for(int k=0; k<3; k++)
                {
                    _ml[i].via[3*j+k] = mesh->mFaces[j].mIndices[k];
                }
            }

            for(unsigned int j=0; j<3*mesh->mNumFaces; j++)
                GS_ASSERT(_ml[i].via[j] < _ml[i].viam );


            for(unsigned int j=0; j<mesh->mNumFaces; j++)
            {
                GS_ASSERT(mesh->mFaces[j].mNumIndices == 3);
                GS_ASSERT(mesh->mNumUVComponents[0] == 2);
            }
        }

        // vertex weights

        for(int i=0; i<_mlm; i++)
        {
            int index = 0;
            aiMesh* mesh = _ml[i].mesh;
            for(unsigned int j=0; j<mesh->mNumBones; j++)
            {
                aiBone* bone = mesh->mBones[j];
                index += bone->mNumWeights;
            }
            _ml[i].vwlm = index;
            _ml[i].vwl = new _VertexWeight[index];
        }

    }

    //needs bone list done first
    void initialize_meshes2()
    {
        for(int i=0; i<_mlm; i++)
        {
            int index = 0;
            aiMesh* mesh = _ml[i].mesh;
            for(unsigned int j=0; j<mesh->mNumBones; j++)
            {
                aiBone* bone = mesh->mBones[j];

                for(unsigned int k=0; k<bone->mNumWeights; k++)
                {
                    _ml[i].vwl[index].bone_index =     get_bone_index(bone);       //index of bone matrix
                    _ml[i].vwl[index].vertex_index =   bone->mWeights[k].mVertexId;
                    _ml[i].vwl[index].weight =         bone->mWeights[k].mWeight;
                    index++;
                }
            }
        }

    }


    //int bam;        //bone array max
    //aiBone** ba;    //bone array

    //int* bal;       //bone array lookup: maps mesh to starting offset in ball
    //int* ball;      // ball[bal[mesh_num] + bone_num] is matrix index
    //int  ballm;

    //fstruct Mat4* bbma;      //bone base matrix array
    //struct Mat4* bma;       //bone matrix array

/*
    Stuff
*/

    struct Node
    {
        char* name;
        struct aiNode* node;
        struct Mat4 mTransformation;
        struct Node* p;     //parent
        struct Node** c;     //children
        int cn;             //children number
        int index;          //index for list
    };

    struct Node* _nl;
    int _nlm;

    struct BoneNode
    {
        char* name;
        struct Mat4 mOffsetMatrix;
        struct aiNode* parent_node;   //parent node
        int parent_index;
    };

    struct BoneNode* bnl;
    int bnlm;


    bool bone_in_list(aiBone* bone, int bone_count)
    {
        for(int i=0; i<bone_count;i++)
        {
            if( strcmp(bnl[i].name, bone->mName.data) ==0 )
                return true;
        }
        return false;
    }

    int get_bone_index(aiBone* bone)
    {
        for(int i=0; i<bnlm;i++)
        {
            if( strcmp(bnl[i].name, bone->mName.data) ==0 )
                return i;
        }

        GS_ASSERT(false);
        return 0;
    }

    void init_bone_list()
    {
        //count bones
        int bone_count = 0;
        int _bone_count = 0;

        //count number of times bone appears
        for(int i=0; i<_mlm; i++)
        {
            aiMesh* mesh = _ml[i].mesh;
            for(unsigned int j=0; j<mesh->mNumBones; j++)
            {
                //aiBone* bone = mesh->mBones[j];
                //printf("bone %d,%d name: %s \n", i,j, bone->mName.data);
                bool new_bone = true;
                for(int _i=0; _i<=i; _i++) 
                {
                    for(unsigned int _j=0; _j<_ml[_i].mesh->mNumBones; _j++) 
                    {
                        if(_i == i && _j == j) break;
                        if(strcmp(_ml[i].mesh->mBones[j]->mName.data, _ml[_i].mesh->mBones[_j]->mName.data) == 0)
                        {
                            new_bone = false;
                            break;
                        }
                    }
                }
                if(new_bone == true) bone_count++;
                _bone_count++;
            }
        }

        printf("init_bone_list: bone_count= %i _bone_count= %i \n", bone_count, _bone_count);
        //set bone list
        bnlm = bone_count;
        bnl = new BoneNode[bnlm];

        for(int i=0; i<bnlm; i++)
        {
            bnl[i].name = NULL;
            bnl[i].parent_node = NULL;
        }

        //populate bone list
        bone_count = 0;
        for(int i=0; i<_mlm; i++)
        {
            aiMesh* mesh = _ml[i].mesh;
            for(unsigned int j=0; j<mesh->mNumBones; j++)
            {
                aiBone* bone = mesh->mBones[j];

                if( bone_in_list(bone, bone_count) == true )
                    continue;
                
                bnl[bone_count].name = copy_string(bone->mName.data);
                bnl[bone_count].mOffsetMatrix = _ConvertMatrix(bone->mOffsetMatrix);
                bnl[bone_count].parent_node =  FindNodeByName(bone->mName.data);
                bnl[bone_count].parent_index = -1;

                bone_count++;    
            }
        }
        GS_ASSERT(bone_count == bnlm);
    }

    //set bone parents
    void init_bone_list2()
    {
        for(int i=0; i<bnlm; i++)
        {
            bnl[i].parent_index = node_index_from_list(bnl[i].parent_node);
        }
    }

    bool node_in_list(struct aiNode* node, int node_count)
    {
        for(int i=0; i<node_count; i++ )
        {
            GS_ASSERT(_nl[i].name != NULL);
            if(strcmp(node->mName.data, _nl[i].name) == 0)
                return true;
        }
        return false;
    }

    int node_index_from_list(struct aiNode* node)
    {
        GS_ASSERT(node != NULL);

        for(int i=0; i<_nlm; i++ )
        {
            if(strcmp(node->mName.data, _nl[i].name) == 0)
                return i;
        }

        GS_ASSERT(false);
        printf("_nlm= %d node_name= %s \n", _nlm, node->mName.data);

        return 0;
    }
/*
    struct Node
    {
        char* name;
        aiNode* node;
        struct Mat4 mTransformation;
        struct Node* p;     //parent
        struct Node* c;     //children
        int cn;             //children number
        int index;          //index in list
    };

    struct Node* _nl;
    int _nlm;
*/

    void _set_node_index(int* index, Node* node)
    {
        printf("node_set_index: node_node= %s node_index= %d \n", node->name, *index);
        node->index = *index;
        (*index)++;
        for(int i=0; i<node->cn; i++)
            _set_node_index(index, node->c[i] );
    }

/*
    set_node_parents(pScene->mRootNode);

    initialize_meshes1();

    init_bone_list();
    init_node_list();

    initialize_meshes2();
*/

    //populate node list
    void init_node_list()
    {
        _nl = new struct Node[nlm];
        _nlm = nlm;

        int node_count = 0;

        for(int i=0; i<bnlm; i++)
        {
            // start with the mesh-to-bone matrix 
            aiNode* tempNode = bnl[i].parent_node;
            GS_ASSERT(tempNode != NULL)

            while( tempNode )
            {
                //assume that if a node is, then all its parents are in?
                if(node_in_list(tempNode, node_count) == true)
                    break;
                //insert

                _nl[node_count].name            = copy_string(tempNode->mName.data);
                _nl[node_count].mTransformation = _ConvertMatrix(tempNode->mTransformation);
                _nl[node_count].node            = tempNode;
                _nl[node_count].p               = NULL;
                _nl[node_count].c               = NULL;
                _nl[node_count].cn              = 0;
                _nl[node_count].index           = -1;

                node_count++;

                if( strcmp(tempNode->mName.data, "Armature") == 0 )
                {
                    //_nl[node_count-1].p = NULL;
                    break;
                }

                if(tempNode == NULL)
                {
                    GS_ASSERT(false);
                    break;
                }

                tempNode = tempNode->mParent;
            }
        }

        _nlm = node_count;  //some overhang on array

        printf("init_node_list: node_count= %d nlm= %d _nlm= %d \n", node_count, nlm, _nlm);

        //set parent node
        for(int i=0; i<_nlm; i++)
        {
            aiNode* node = _nl[i].node;
            if(strcmp(node->mName.data, "Armature") == 0)   //parent should be null for armature which is top level
            {
                _nl[i].p = NULL;
                continue;
            }
            int index = node_index_from_list(node->mParent);
            _nl[i].p = &_nl[index];
        }

        //set children
        for(int i=0; i<_nlm; i++)
        {
            struct Node* node = &_nl[i];
            int child_count = 0;

            for(int j=0; j<_nlm; j++)
            {
                if( _nl[j].p == node)
                    child_count++;
            }

            node->c = new Node*[child_count];
            node->cn = child_count;

            child_count = 0;
            for(int j=0; j<_nlm; j++)
            {
                if( _nl[j].p == node)
                {
                    node->c[child_count] = &_nl[j];
                    child_count++;
                }
            }
        }


        for(int i=0; i<_nlm; i++)
        {
            printf("Node %02d name: %s children_number= %d \n", i, _nl[i].name, _nl[i].cn );
        }


        //set index
        struct Node* root_node = NULL;
        for(int i=0; i<_nlm; i++)
        {
            if(root_node != NULL) { GS_ASSERT(_nl[i].p != NULL) }
            if(_nl[i].p == NULL)
                root_node = &_nl[i];
        }
        int index = 0;
        _set_node_index(&index, root_node);

        //make sure parent stuff is setup
        for(int i=0; i<_nlm; i++)
        {
            if(_nl[i].index == 0)
            {
                GS_ASSERT( _nl[i].p == NULL);
                continue;
            }
            else
            {
                GS_ASSERT( _nl[i].index > _nl[i].p->index);
                GS_ASSERT(_nl[i].index != -1);
                GS_ASSERT(_nl[i].index < _nlm );
                if( _nl[i].index <= _nl[i].p->index || _nl[i].index == -1 || _nl[i].index >= _nlm)
                    printf("ERROR: i=%d index= %d node_name= %s parent_index= %d parent_node_name= %s \n", 
                        i, _nl[i].index,_nl[i].name,  _nl[i].p->index,_nl[i].p->name );
            }
        }

        //sort

        for(int i=0; i<_nlm; i++)
        {
            if(_nl[i].index != i)
            {
                struct Node ntmp;
                bool found = false;
                for(int j=0; j<_nlm; j++)
                {
                    if(_nl[j].index == i)
                    {
                        ntmp = _nl[i];
                        _nl[i] = _nl[j];
                        _nl[j] = ntmp;
                        found = true;
                        break;
                    }
                }
                GS_ASSERT(found == true);
            }
        }
        for(int i=0; i<_nlm; i++)
            GS_ASSERT(_nl[i].index == i);



        //reset parent node
        for(int i=0; i<_nlm; i++)
        {
            aiNode* node = _nl[i].node;
            if(strcmp(node->mName.data, "Armature") == 0)   //parent should be null for armature which is top level
            {
                _nl[i].p = NULL;
                continue;
            }
            int index = node_index_from_list(node->mParent);
            _nl[i].p = &_nl[index];
        }

        //reset children
        for(int i=0; i<_nlm; i++)
        {
            struct Node* node = &_nl[i];
            int child_count = 0;

            for(int j=0; j<_nlm; j++)
            {
                if( _nl[j].p == node)
                    child_count++;
            }

            node->c = new Node*[child_count];
            node->cn = child_count;

            child_count = 0;
            for(int j=0; j<_nlm; j++)
            {
                if( _nl[j].p == node)
                {
                    node->c[child_count] = &_nl[j];
                    child_count++;
                }
            }
        }

/*
        for(int i=0; i<_nlm; i++)
        {
            if(_nl[i].p != NULL)
            {
                printf("node %02d: name= %s parent index= %d  parent_name= %s \n", i, _nl[i].name, _nl[i].p->index, _nl[i].p->name);
            }
            else
            {
                printf("node %02d: name= %s  \n", i, _nl[i].name);
            }
        }
        printf("!!!\n");
*/

    }


    aiNode* FindNodeRecursivelyByName(aiNode* pNode, char* node_name)
    {
        if( strcmp(node_name, pNode->mName.data) == 0 )
            return pNode;

        for(unsigned int i=0; i < pNode->mNumChildren; i++)
        {
            aiNode* tmp = FindNodeRecursivelyByName(pNode->mChildren[i], node_name);

            if(tmp != NULL)
                return tmp;
        }

        return NULL;
    }

    aiNode* FindNodeByName(char* node_name)
    {
        aiNode* node = FindNodeRecursivelyByName( pScene->mRootNode, node_name);
        GS_ASSERT(node != NULL);
        return node;
    }



/*
Another complication is how to use interpolation. It is possible to create a transformation matrix from the rotation, location and scaling, 
but interpolating rotation using transformation matrices are not good. If you interpolate an object moved in an arch defined 
by two keys 90 degrees rotated to each other, the result will be a straight line (at best). It actually looks quite fun, 
but is not good. Therefore, you usually should do the interpolation on the quaternions first, then create the transformation matrix.
*/
    //get matrix for a node, if not is not animated, just returns the scene matrix
    struct Mat4 get_anim_matrix(int frame_time, aiNodeAnim** node_channels, int node_channel_max, aiNode* node)
    {
        for(int i=0; i<node_channel_max; i++)
        {
            aiNodeAnim* anim = node_channels[i];
            //printf("node channel= %s \n", anim->mNodeName.data);

            if( strcmp(anim->mNodeName.data, node->mName.data) == 0 )
            {
                GS_ASSERT(anim->mNumPositionKeys == anim->mNumRotationKeys);
                int tmax = anim->mNumPositionKeys;
                
                aiVectorKey pos =  anim->mPositionKeys[frame_time % tmax];
                aiQuatKey rot = anim->mRotationKeys[frame_time % tmax];
                GS_ASSERT( pos.mTime == rot.mTime );

                //CONVERT TO MATRIX
                return quantenion_to_rotation_matrix( rot.mValue, pos.mValue );
            }

        }
        //if cannot find, then node does not have an animation and use this
        return _ConvertMatrix(node->mTransformation);
    }


    /*
        Stuff for testing
    */
/*
    unsigned int texture1;
    SDL_Surface* s;

    void init_texture();
    void draw_skeleton(float x, float y, float z);
    void draw(float x, float y, float z);  //draws for testing
*/
};



/*
    for(int i=0; i<nli; i++)
        aiMesh* mesh = ml[i];
*/

/*
    struct _Vertex
    {
        struct Vec3 v;
        float ux,uy;
    };

    struct _VertexWeight
    {
        int bone_index; //index into bone matrix
        int vertex_index;     //mesh vertex index
        float weight;   //weight
    };    
*/


/*
    struct aiMesh* mesh;

    int bvln;       //base vertex list max
    struct _Vertex* bvl; //base vertex list

    int vln;    //number of vertices

    int* via;   //vertex index array
    int viam;   //vertex index array max

    int vwlm;
    struct _VertexWeight* vwl;
*/

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


    BodyPartMesh()
    {
        bvl  = NULL;
        tbvl = NULL;
        tvl  = NULL;
        vwl  = NULL;
        via  = NULL;
    }

    ~BodyPartMesh()
    {
        delete[] bvl;
        delete[] tbvl;
        delete[] tvl;
        delete[] vwl;
        delete[] via;
    }

    //assumes only one mesh per node
    void load(struct _Mesh* mesh)
    {

        //copy name
        mesh_name = copy_string(mesh->node->mName.data);

        //base vertex list
        bvlm = mesh->bvln;
        bvl = new struct _Vertex[bvlm];
        tbvl = new struct _Vertex[bvlm];

        for(int i=0;i<bvlm;i++)
            bvl[i] = mesh->bvl[i];

        //vertex list
        tvln = mesh->vln;
        tvl = new struct _Vertex[tvln];

        //vertex list index array
        viam = mesh->viam;
        via = new int[viam];

        for(int i=0;i<viam;i++)
            via[i] = mesh->via[i];

        //vertex weight list
        vwlm = mesh->vwlm;
        vwl = new struct _VertexWeight[vwlm];
        
        for(int i=0;i<vwlm;i++)
            vwl[i] = mesh->vwl[i];
    }
};


class BodyAnimation
{
    /*
        Assume all keyframes contain the same nodes
    */
    public:
    char* animation_name;
    float length;   //length of animation
    
    struct _quanterion
    {
        float x,y,z,w;
    };


    //copy these pointers over
    char** nnl;                           //node name list
    struct Mat4* node_mTransformation;    //node bind transform list
    int nm;                               //node max

    //bind pos 

    //need to know keyframes and need to m
    // -affected mpdes
    // -timees

    struct Mat4 get_anim_matrix_old(int frame_time, aiNodeAnim** node_channels, int node_channel_max, aiNode* node)
    {

        for(int i=0; i<node_channel_max; i++)
        {
            aiNodeAnim* anim = node_channels[i];
            //printf("node channel= %s \n", anim->mNodeName.data);

            if( strcmp(anim->mNodeName.data, node->mName.data) == 0 )
            {
                GS_ASSERT(anim->mNumPositionKeys == anim->mNumRotationKeys);
                int tmax = anim->mNumPositionKeys;
                
                aiVectorKey pos =  anim->mPositionKeys[frame_time % tmax];
                aiQuatKey rot = anim->mRotationKeys[frame_time % tmax];
                GS_ASSERT( pos.mTime == rot.mTime );

                //CONVERT TO MATRIX
                return quantenion_to_rotation_matrix( rot.mValue, pos.mValue );
            }

        }

        //if cannot find, then node does not have an animation and use this
        return _ConvertMatrix(node->mTransformation);

    }

/*
    void apply_animation()
    {
        static int _fcount = 0;
        static int frame_time = 0;

        _fcount++;
        if(_fcount % 30 == 0)
            frame_time++;

        aiAnimation* anim = pScene->mAnimations[0];

        aiNodeAnim** node_channels = anim->mChannels;
        int node_channels_max = anim->mNumChannels;


        boneMatrix = mat4_mult(get_anim_matrix(frame_time, node_channels, node_channels_max, tempNode), boneMatrix );
    }
*/
    
};


/*
    struct Node
    {
        char* name;
        struct aiNode* node;
        struct Mat4 mTransformation;
        struct Node* p;     //parent
        struct Node** c;     //children
        int cn;             //children number
        int index;          //index for list
    };

    struct Node* _nl;
    int _nlm;

    struct BoneNode
    {
        char* name;
        struct Mat4 mOffsetMatrix;
        struct aiNode* parent_node;   //parent node
        int parent_index;
    };

    struct BoneNode* bnl;
    int bnlm;
*/

class BodyMesh
{
    public:

    class BodyPartMesh* ml; //body part mesh list
    int mlm;                 //body part mesh list max
/*
    struct Node
    {
        char* name;
        int parent_index;
    };
*/
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

    BodyMesh()
    {
        nnl = NULL;
        npl = NULL;
        node_mTransformation = NULL;

        bnl = NULL;
        bone_mOffsetMatrix = NULL;
        bpl = NULL;

        tbone_matrix = NULL;
        tnode_matrix = NULL;

        s = NULL;
    }

    ~BodyMesh()
    {
        if(nnl != NULL) delete[] nnl;
        if(npl != NULL) delete[] npl;
        if(node_mTransformation != NULL) delete[] node_mTransformation;

        if(bnl != NULL) delete[] bnl;
        if(bone_mOffsetMatrix != NULL) delete[] bone_mOffsetMatrix;
        if(bpl != NULL) delete[] bpl;

        if(tbone_matrix != NULL) delete[] tbone_matrix;
        if(tnode_matrix != NULL) delete[] tnode_matrix;
    }

    void load(class ModelLoader* ML)
    {
        //load node list

        nm = ML->_nlm;
        nnl = new char*[nm];
        npl = new int[nm];

        node_mTransformation = new struct Mat4[nm];  //mTransformation;
        tnode_matrix =  new struct Mat4[nm];

        for(int i=0;i<nm; i++)
        {  
            GS_ASSERT(ML->_nl[i].index == i);
            nnl[i] = ML->_nl[i].name;
            npl[i] = (ML->_nl[i].p == NULL ? 0 : ML->_nl[i].p->index);
            if(ML->_nl[i].p != NULL)
                printf("node %d parent %d p2= %d \n", i, npl[i], ML->_nl[i].p->index);
            node_mTransformation[i] = ML->_nl[i].mTransformation;
        }

        //load bone list

        blm = ML->bnlm;
        bnl = new char*[blm];
        
        bone_mOffsetMatrix = new struct Mat4[blm];
        tbone_matrix = new struct Mat4[blm];

        bpl = new int[blm];

        for(int i=0;i<blm;i++)
        {
            bnl[i] = ML->bnl[i].name; 
            bone_mOffsetMatrix[i] = ML->bnl[i].mOffsetMatrix;
            bpl[i] = ML->bnl[i].parent_index;

            if(bpl[i] == -1)
            {
                printf("ERROR: bone parent for bone %i is -1 \n", i);
            }
        }

        GS_ASSERT(ML->_nl[0].p == 0);
        //load meshes

        mlm = ML->_mlm;
        ml = new class BodyPartMesh[mlm];

        for(int i=0; i< ML->_mlm; i++)
            ml[i].load( &(ML->_ml[i]) );

        init_texture();
    }

    void draw_prep()
    {
        static int* _set = NULL;
        if( _set == NULL) _set = new int[nm];
        for(int i=0; i<nm; i++) _set[i] = 0;

        _set[0] = 1;
        tnode_matrix[0] = node_mTransformation[0];

/*
    char** nnl;         //node name list
    int* npl;           //node parent list
    struct Mat4* node_mTransformation;    //node transform list, mTransformation
    int nm;       
*/

        //printf("nm= %d \n", nm);

        //printf("node %02d: name= %s \n", 0, nnl[0]);

        for(int i=1; i<nm; i++)
        {
            if(npl[i] == -1)
            {
                //printf("ERROR: node %d parent is -1 \n", i);
                continue;
            }
            //printf("node %02d: name= %s parent= %d \n" , i, nnl[i], bpl[i] );
            //printf("node %02d: name= %s parent= %d parent_name= %s \n", i, nnl[i], npl[i], nnl[npl[i]] );

            GS_ASSERT(_set[i] == 0);
            GS_ASSERT(_set[npl[i]] == 1);

            tnode_matrix[i] = mat4_mult( tnode_matrix[npl[i]], node_mTransformation[i] );
            _set[i] = 1;
        }

        //bone matrix preperation

/*
    char** bnl;     //bone name list
    struct Mat4* bone_mOffsetMatrix;
    int* bpl;        //bone parent list; index of parent node
    int blm;        //bone list max
*/

        for(int i=0; i<blm; i++)
        {
            if(bpl[i] == -1)
            {
                //printf("ERROR: bone %d parent is -1 \n", i);
                continue;
            }
            //printf("node %02d: name= %s parent= %d \n" , i, nnl[i], bpl[i] );
            //printf("bone %02d: name= %s parent= %d parent_name= %s \n", i, bnl[i], bpl[i], nnl[bpl[i]] );

            GS_ASSERT(_set[bpl[i]] == 1);

            tbone_matrix[i] = mat4_mult( tnode_matrix[bpl[i]], bone_mOffsetMatrix[i] );
        }

    }


/*
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
*/

/*
    struct _VertexWeight
    {
        int bone_index; //index into bone matrix
        int vertex_index;       //mesh vertex index
        float weight;   //weight
    };
*/

    void draw(float x, float y, float z)
    {

        /*
            Move this stuff onto the mesh
        */
        for(int i=0; i<mlm; i++)
        {
            class BodyPartMesh* m = &ml[i];

            for(int j=0; j<m->bvlm; j++)
            {
                m->tbvl[j].ux = m->bvl[j].ux;
                m->tbvl[j].uy = m->bvl[j].uy;

                m->tbvl[j].v.x = 0.0f;
                m->tbvl[j].v.y = 0.0f;
                m->tbvl[j].v.z = 0.0f;
            }

            for(int j=0; j<m->vwlm; j++)
            {
                struct _VertexWeight w = m->vwl[j];
                int bone_index   = w.bone_index;
                int vertex_index = w.vertex_index;
                float weight     = w.weight;

                GS_ASSERT(vertex_index < m->bvlm);
                GS_ASSERT(bone_index < blm);
                GS_ASSERT(vertex_index < m->bvlm);
                
                Vec3 v = vec3_mat3_apply(m->bvl[vertex_index].v, tbone_matrix[bone_index] );

                struct Mat4 out = tbone_matrix[bone_index];
                GS_ASSERT(out._f[0*4+3] == 0.0f && out._f[1*4+3] == 0.0f && out._f[2*4+3] == 0.0f && out._f[3*4+3] == 1.0f)

                m->tbvl[vertex_index].v.x += weight*v.x;
                m->tbvl[vertex_index].v.y += weight*v.y;
                m->tbvl[vertex_index].v.z += weight*v.z;
            }
        }

        for(int i=0; i<mlm; i++)
        {
            class BodyPartMesh* m = &ml[i];
            GS_ASSERT(m->viam == m->tvln);
            for(int j=0; j<m->viam; j++)
            {
                int index = m->via[j];
                GS_ASSERT(index < m->viam && index >= 0);
                m->tvl[j] = m->tbvl[index];
            }

        }

        glColor4ub(255,255,255,255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glBegin(GL_TRIANGLES);

        for(int i=0; i<mlm; i++)
        {
            class BodyPartMesh* m = &ml[i];

            for(int j=0; j<m->tvln; j++)
            {
                struct _Vertex v = m->tvl[j];

                glTexCoord2f(v.ux, v.uy );
                glVertex3f(v.v.x +x , v.v.y +y , v.v.z +z); //swap y and z
            }
        }

        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
        check_gl_error();

    }

    unsigned int texture1;
    SDL_Surface* s;

    void init_texture()
    {
        GS_ASSERT(s == NULL);
        s = create_surface_from_file("./media/mesh/body_template.png");
        //s = create_surface_from_file("./media/mesh/test.png");

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum texture_format;
        if (s->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
        
        glDisable(GL_TEXTURE_2D);
    }


};


class ModelLoader* model_loader;
class BodyMesh* body_mesh;


void init()
{
    return; //DEBUG

    size_t bsize = 0;
    char* buffer = read_file_to_buffer( (char*) "media/mesh/player.dae", &bsize);
    GS_ASSERT(buffer != NULL);
    if (buffer == NULL) return;
    
    int aFlag = aiProcess_Triangulate | 
    //aiProcess_GenUVCoords | 
    //aiProcess_TransformUVCoords |
    aiProcess_ValidateDataStructure;// |
    //aiProcess_RemoveComponent; //strip components on

    char* aHint = NULL;
    aiPropertyStore* property_store = aiCreatePropertyStore();
    int aFlagRemove = aiComponent_TANGENTS_AND_BITANGENTS | aiComponent_COLORS | aiComponent_LIGHTS | aiComponent_CAMERAS | aiComponent_TEXTURES | aiComponent_MATERIALS;

    aiSetImportPropertyInteger(property_store, AI_CONFIG_PP_RVC_FLAGS, aFlagRemove);
    //const struct aiScene* pScene = aiImportFileFromMemory(buffer, bsize, aFlag , aHint);
    const struct aiScene* pScene = aiImportFileFromMemoryWithProperties(buffer, bsize, aFlag , aHint, property_store);
    free(buffer);
    aiReleasePropertyStore(property_store);


    //bt = new BoneTree;
    //bt->init( (aiScene*) pScene);
    model_loader = new ModelLoader;
    model_loader->init((aiScene*) pScene);

    body_mesh = new BodyMesh;
    body_mesh->load(model_loader);

    body_mesh->draw_prep();
}


void draw()
{
    return; //DEBUG
    
    struct Vec3 p = ClientState::location_pointer;

    if(ClientState::location_pointer_set == false)
    {
        p.x = 128.0f;
        p.y = 128.0f;
        p.z = 128.0f;
    }

    body_mesh->draw_prep();
    body_mesh->draw(p.x, p.y, p.z + 3.0f);

    //bt->draw_skeleton(p.x+0.0, p.y+0.0f, p.z + 5.0f);

    return;

}

void teardown()
{
    return; //DEBUG
    
    if(model_loader != NULL) delete model_loader;
    if(body_mesh != NULL) delete body_mesh;
}


#if 0
        for(unsigned int j=0; j<mesh->mNumBones; j++)
        {
            aiBone* bone = mesh->mBones[j];
            
            aiNode* node = FindNodeRecursivelyByName( pScene->mRootNode, bone->mName.data);
            GS_ASSERT(node != NULL)
            // start with the mesh-to-bone matrix 
            Mat4 boneMatrix = _ConvertMatrix(bone->mOffsetMatrix);  //node to vertex matrix?
            GS_ASSERT(boneMatrix._f[0*4+3] == 0.0f && boneMatrix._f[1*4+3] == 0.0f && boneMatrix._f[2*4+3] == 0.0f && boneMatrix._f[3*4+3] == 1.0f);

             // and now append all node transformations down the parent chain until we're back at mesh coordinates again
            aiNode* tempNode = node;

            int _index = 0;
            while( tempNode )
            {
                _index++;

                if(_print)
                {
                    printf("\tbone: %02d %02d bone name= %s \n", j, _index, tempNode->mName.data);
                //boneMatrices[a] *= tempNode->mTransformation;   // check your matrix multiplication order here!!!
                    //mat4_print( _ConvertMatrix(tempNode->mTransformation) ) ;
                }
                //boneMatrix = mat4_mult(boneMatrix, _ConvertMatrix(tempNode->mTransformation));
                
                boneMatrix = mat4_mult(get_anim_matrix(frame_time, node_channels, node_channels_max, tempNode), boneMatrix );
                GS_ASSERT(boneMatrix._f[0*4+3] == 0.0f && boneMatrix._f[1*4+3] == 0.0f && boneMatrix._f[2*4+3] == 0.0f && boneMatrix._f[3*4+3] == 1.0f);
                //boneMatrix = mat4_mult(_ConvertMatrix(tempNode->mTransformation), boneMatrix);
                //armature is the last node that gets multiplied in
                
                if( strcmp(tempNode->mName.data, "Armature") == 0 )
                    break;
                if(tempNode == NULL)
                {
                    //GS_ASSERT(tempNode != NULL);
                    break;
                }
                tempNode = tempNode->mParent;
            }

            boneMatrix = mat4_mult(m_GlobalInverseTransform, boneMatrix);

            //node = FindNodeRecursivelyByName( pScene->mRootNode, bone->mName.data);
            //boneMatrix = get_anim_matrix(frame_time, node_channels, node_channels_max, node);
            //boneMatrix = mat4_mult(get_anim_matrix(frame_time, node_channels, node_channels_max, node), boneMatrix );

            //boneMatrix = mat4_transpose(boneMatrix); //transpose it because its wrong
            if(_print)
            {
                printf("final matrix: mesh: %02d %02d mesh name= %s \n", i,j, nl[i]->mName.data);
                mat4_print(boneMatrix) ;
            }

            for(unsigned int k=0; k<bone->mNumWeights; k++)
            {
                int index = offset + bone->mWeights[k].mVertexId;
                float weight = bone->mWeights[k].mWeight;

                GS_ASSERT(index >= offset);
                GS_ASSERT(index < offset+num);
                if(j==0) count++;

                if(_print)
                    vec3_print(bvl[index].v);

                Vec3 v = vec3_mat3_apply(bvl[index].v, boneMatrix);

                if(_print)
                {
                    printf("Vertex %02d \n", index);
                    vec3_print(bvl[index].v);

                    vec3_print(v);
                    mat4_print(boneMatrix);
                }

                tbvl[index].v.x += weight*v.x;
                tbvl[index].v.y += weight*v.y;
                tbvl[index].v.z += weight*v.z;
            }
        }
    }

    for(int i=0; i<bvlm; i++)
    {
        tbvl[i].v.x += x;
        tbvl[i].v.y += y;
        tbvl[i].v.z += z;
    }

    for(int i=0; i<vlm; i++)
    {
        int index = via[i];
        tvl[i] = tbvl[index];
    }

    glBegin(GL_TRIANGLES);
    for(int i=0; i<vlm; i++)
        struct _Vertex v = tvl[i];
        glTexCoord2f(v.ux, v.uy );
        glVertex3f(v.v.x, v.v.y, v.v.z); //swap y and z


    }

#endif 

#if 0
void ModelLoader::init_texture()
{
    GS_ASSERT(s == NULL);
    s = create_surface_from_file("./media/mesh/body_template.png");
    //s = create_surface_from_file("./media/mesh/test.png");

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum texture_format;
    if (s->format->Rmask == 0x000000ff)
        texture_format = GL_RGBA;
    else
        texture_format = GL_BGRA;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
    
    glDisable(GL_TEXTURE_2D);
}

void ModelLoader::draw(float x, float y, float z)
{
    //printf("nlm= %d vlm= %d \n", nlm, vlm);
    static int _fcount = 0;
    static int frame_time = 0;

    _fcount++;
    if(_fcount % 30 == 0)
        frame_time++;

    //Mat4 m_GlobalInverseTransform = mat4_inverse( mat4_transpose(_ConvertMatrix(pScene->mRootNode->mTransformation )));

    aiAnimation* anim = pScene->mAnimations[0];

    aiNodeAnim** node_channels = anim->mChannels;
    int node_channels_max = anim->mNumChannels;

    bool _print  = false;

    for(int i=0; i<bvlm; i++)
    {
        tbvl[i].ux = bvl[i].ux;
        tbvl[i].uy = bvl[i].uy;

        tbvl[i].v.x = 0.0;  // 0.0f + x
        tbvl[i].v.y = 0.0;
        tbvl[i].v.z = 0.0;
    }
    //printf("nli= %i \n", nli);

    int count = 0;
    for(int i=0; i<nli; i++)
    {
        aiMesh* mesh = ml[i];

        //printf("%i: num bones= %i \n", i, mesh->mNumBones);

        int offset = bvlo[i];
        int num = bvln[i];

        GS_ASSERT(mesh->mNumBones != 0);


        if(_print)
            printf("mesh: %02d mesh name= %s \n", i, nl[i]->mName.data);

        for(unsigned int j=0; j<mesh->mNumBones; j++)
        {
            aiBone* bone = mesh->mBones[j];
            
            aiNode* node = FindNodeRecursivelyByName( pScene->mRootNode, bone->mName.data);
            GS_ASSERT(node != NULL)
            // start with the mesh-to-bone matrix 
            Mat4 boneMatrix = _ConvertMatrix(bone->mOffsetMatrix);  //node to vertex matrix?
            GS_ASSERT(boneMatrix._f[0*4+3] == 0.0f && boneMatrix._f[1*4+3] == 0.0f && boneMatrix._f[2*4+3] == 0.0f && boneMatrix._f[3*4+3] == 1.0f);

             // and now append all node transformations down the parent chain until we're back at mesh coordinates again
            aiNode* tempNode = node;

            int _index = 0;
            while( tempNode )
            {
                _index++;

                if(_print)
                {
                    printf("\tbone: %02d %02d bone name= %s \n", j, _index, tempNode->mName.data);
                    //boneMatrices[a] *= tempNode->mTransformation;   // check your matrix multiplication order here!!!
                    //mat4_print( _ConvertMatrix(tempNode->mTransformation) ) ;
                }
                //boneMatrix = mat4_mult(boneMatrix, _ConvertMatrix(tempNode->mTransformation));
                
                boneMatrix = mat4_mult(get_anim_matrix(frame_time, node_channels, node_channels_max, tempNode), boneMatrix );
                GS_ASSERT(boneMatrix._f[0*4+3] == 0.0f && boneMatrix._f[1*4+3] == 0.0f && boneMatrix._f[2*4+3] == 0.0f && boneMatrix._f[3*4+3] == 1.0f);
                //boneMatrix = mat4_mult(_ConvertMatrix(tempNode->mTransformation), boneMatrix);
                //armature is the last node that gets multiplied in
                
                if( strcmp(tempNode->mName.data, "Armature") == 0 )
                    break;
                if(tempNode == NULL)
                {
                    //GS_ASSERT(tempNode != NULL);
                    break;
                }
                tempNode = tempNode->mParent;

            }

            //boneMatrix = mat4_mult(m_GlobalInverseTransform, boneMatrix);

            //node = FindNodeRecursivelyByName( pScene->mRootNode, bone->mName.data);
            //boneMatrix = get_anim_matrix(frame_time, node_channels, node_channels_max, node);
            //boneMatrix = mat4_mult(get_anim_matrix(frame_time, node_channels, node_channels_max, node), boneMatrix );

            //boneMatrix = mat4_transpose(boneMatrix); //transpose it because its wrong
            if(_print)
            {
                printf("final matrix: mesh: %02d %02d mesh name= %s \n", i,j, nl[i]->mName.data);
                mat4_print(boneMatrix) ;
            }

            for(unsigned int k=0; k<bone->mNumWeights; k++)
            {
                int index = offset + bone->mWeights[k].mVertexId;
                float weight = bone->mWeights[k].mWeight;

                GS_ASSERT(index >= offset);
                GS_ASSERT(index < offset+num);
                if(j==0) count++;

                if(_print)
                    vec3_print(bvl[index].v);

                Vec3 v = vec3_mat3_apply(bvl[index].v, boneMatrix);

                if(_print)
                {
                    printf("Vertex %02d \n", index);
                    vec3_print(bvl[index].v);

                    vec3_print(v);
                    mat4_print(boneMatrix);
                }

                tbvl[index].v.x += weight*v.x;
                tbvl[index].v.y += weight*v.y;
                tbvl[index].v.z += weight*v.z;
            }
        }
    }

    for(int i=0; i<bvlm; i++)
    {
        tbvl[i].v.x += x;
        tbvl[i].v.y += y;
        tbvl[i].v.z += z;
    }

    for(int i=0; i<vlm; i++)
    {
        int index = via[i];
        tvl[i] = tbvl[index];
    }

    glColor4ub(255,255,255,255);
    glEnable(GL_TEXTURE_2D);
    //GL_ASSERT(GL_TEXTURE_2D, true);

    glBindTexture(GL_TEXTURE_2D, texture1);

    glBegin(GL_TRIANGLES);
    for(int i=0; i<vlm; i++)
    {
        struct _Vertex v = tvl[i];

        //vec3_print(v.v);
        glTexCoord2f(v.ux, v.uy );
        glVertex3f(v.v.x, v.v.y, v.v.z); //swap y and z
    }

    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    check_gl_error();
}

void ModelLoader::draw_skeleton(float x, float y, float z)
{

    static int _fcount = 0;
    static int frame_time = 0;

    _fcount++;
    if(_fcount % 30 == 0)
        frame_time++;

    aiAnimation* anim = pScene->mAnimations[0];

    aiNodeAnim** node_channels = anim->mChannels;
    int node_channels_max = anim->mNumChannels;

    glDisable(GL_TEXTURE_2D);

    static const bool _print = false;

    for(int i=0; i<nli; i++)
    {
        aiMesh* mesh = ml[i]; 

        GS_ASSERT(mesh->mNumBones != 0);

        printf("mesh: %02d mesh name= %s \n", i, nl[i]->mName.data);

        for(unsigned int j=0; j<mesh->mNumBones; j++)
        {
            aiBone* bone = mesh->mBones[j];

            aiNode* node = FindNodeRecursivelyByName( pScene->mRootNode, bone->mName.data);
            GS_ASSERT(node != NULL)
            // start with the mesh-to-bone matrix 

            Mat4 boneMatrix = _ConvertMatrix(bone->mOffsetMatrix);  //node to vertex matrix?
            //Mat4 boneMatrix = mat4_identity();
            /*
                !!!

                What if the bone matrix node has an animation channel?
            */

            //Mat4 tmp = mat4_mult(boneMatrix, mat4_inverse(boneMatrix));
            //mat4_print(boneMatrix);

            GS_ASSERT(boneMatrix._f[0*4+3] == 0.0f && boneMatrix._f[1*4+3] == 0.0f && boneMatrix._f[2*4+3] == 0.0f && boneMatrix._f[3*4+3] == 1.0f);

            // and now append all node transformations down the parent chain until we're back at mesh coordinates again
            aiNode* tempNode = node;

            int _index = 0;
            while( tempNode )
            {
                _index++;
                //printf("\tnode: %02d %02d node name= %s \n", j, _index, tempNode->mName.data);

                if(_print)
                {
                    printf("\tbone: %02d %02d bone name= %s \n", j, _index, tempNode->mName.data);
                    //boneMatrices[a] *= tempNode->mTransformation;   // check your matrix multiplication order here!!!
                    //mat4_print( _ConvertMatrix(tempNode->mTransformation) ) ;
                }
                    //boneMatrix = mat4_mult(boneMatrix, _ConvertMatrix(tempNode->mTransformation));
                
                boneMatrix = mat4_mult(get_anim_matrix(frame_time, node_channels, node_channels_max, tempNode), boneMatrix );
                GS_ASSERT(boneMatrix._f[0*4+3] == 0.0f && boneMatrix._f[1*4+3] == 0.0f && boneMatrix._f[2*4+3] == 0.0f && boneMatrix._f[3*4+3] == 1.0f);

                if( strcmp(tempNode->mName.data, "Armature") == 0 )
                    break;
                if(tempNode == NULL)
                {
                    //GS_ASSERT(tempNode != NULL);
                    break;
                }
                tempNode = tempNode->mParent;
            }

            //boneMatrix = mat4_mult( mat4_transpose(_ConvertMatrix(bone->mOffsetMatrix)) , boneMatrix);

            const float size = 1.0f;
            glBegin(GL_LINES);
                
                Vec3 vv;
                vv.x = boneMatrix._f[4*3+0];
                vv.y = boneMatrix._f[4*3+1];
                vv.z = boneMatrix._f[4*3+2];

                printf("bone: %02d %02d node name= %s x,y,z= %.02f %.02f %.02f \n", j, _index, node->mName.data, vv.x,vv.y,vv.z );
                mat4_print(boneMatrix);

                Vec3 vf;
                vf.x = size*boneMatrix._f[4*0+0];
                vf.y = size*boneMatrix._f[4*0+1];
                vf.z = size*boneMatrix._f[4*0+2];

                glColor4ub(255,0,0,255);

                glVertex3f(x+vv.x, y+vv.y, z+vv.z);
                glColor4ub(0,255,0,255);

                glVertex3f(x+vv.x+vf.x, y+vv.y+vf.y, z+vv.z+vf.z);
            glEnd();
        }
    }



    glColor4ub(255,255,255,255);
    glDisable(GL_TEXTURE_2D);

    check_gl_error();
}
#endif

}
