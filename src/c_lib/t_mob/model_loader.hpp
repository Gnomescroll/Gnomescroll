#pragma once




#include <physics/mat4.hpp>

#ifdef __MSVC__
extern "C"
{
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h> //defines for postprocessor
#include <assimp/config.h>
}
#else
    #undef __cplusplus
    extern "C"
    {
    #include <assimp/cimport.h>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h> //defines for postprocessor
    #include <assimp/config.h>
    }
    #define __cplusplus
#endif



namespace t_mob
{

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
        int vertex_index;		//mesh vertex index
        float weight; 	//weight
    };


    struct _Mesh
    {
        struct aiMesh* mesh;

        int bvln;       //base vertex list max
        struct _Vertex bvl; //base vertex list

        int vlm;    //number of vertices

        int* via;   //vertex index array
        int viam;   //vertex index array max

        int vwlm;
        struct _VertexWeight* vwl;
    };


class ModelLoader
{
    public:

    ModelLoader() :
    pScene(NULL),
    nl(NULL), 
    ml(NULL),
    //tvl(NULL),
    //vll(NULL), vln(NULL),
    //bvl(NULL), tbvl(NULL),
    bvlo(NULL), bvln(NULL),
    //via(NULL),
    s(NULL)
    {}
    
    ~ModelLoader()
    {
        if (s != NULL) SDL_FreeSurface(s);
        if (pScene != NULL) aiReleaseImport(pScene);
        if (nl != NULL) delete[] nl;
        if (ml != NULL) delete[] ml;
        //if (tvl != NULL) delete[] tvl;
        if (vll != NULL) delete[] vll;
        if (vln != NULL) delete[] vln;
        if (bvl != NULL) delete[] bvl;
        //if (tbvl != NULL) delete[] tbvl;
        if (bvlo != NULL) delete[] bvlo;
        if (bvln != NULL) delete[] bvln;
        //if (via != NULL) delete[] via;
    }

    aiScene* pScene;    //the scene

    int nlm;        //node list max
    aiNode** nl;    //node list
    int nli;        //node list index (just a counter variable)

    aiMesh** ml;    //mesh list


    //int* vll;           //offset of vertices in list for each mesth
    int* vln;           //number of vertices in each mech

    void init(aiScene* _pScene)
    {
        pScene = _pScene;

        nli = 0;
        count_nodes(pScene->mRootNode); //count the nodes with meshes
        nlm = nli;
        nl = new aiNode*[nlm];
        
        ml = new aiMesh*[nlm];

        for(int i=0; i<nlm; i++) nl[i] = NULL;
        for(int i=0; i<nlm; i++) ml[i] = NULL;

        nli = 0;
        set_node_parents(pScene->mRootNode);

        //vll = new int[nlm];
        vln = new int[nlm];

        init_base_vertex_list();
        init_texture();
        //draw();
        init_bone_list();
        init_node_list();

        initialize_meshes();
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

            GS_ASSERT(pNode->mNumMeshes == 0 || pNode->mNumMeshes == 1);    //each node should only have one mesh?
            int mesh_index = pNode->mMeshes[0]; //grab the first mesh
            ml[nli] = pScene->mMeshes[mesh_index];
            if(pNode->mNumMeshes == 0)
                ml[nli] == NULL;
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
        struct _Vertex bvl; //base vertex list

        int vln;    //number of vertices

        int* via;   //vertex index array
        int viam;   //vertex index array max

        int vwlm;
        struct _VertexWeight* vwl;
    };
*/

    struct _Mesh* _ml;
    int _mln;

    void initialize_meshes()
    {

        int mesh_count = 0;
        
        for(int i=0; i<nlm; i++)
        {
            aiNode* node = nl[i];
            GS_ASSERT(node->mNumMeshes == 0 || node->mNumMeshes == 1); 
            if(node->mNumMeshes == 1)
                mesh_count++;
        }

        _mln = mesh_count;
        _ml = new struct _Mesh[_mln];

        for(int i=0; i<nlm; i++)
        {
            aiNode* node = nl[i];
            int mesh_index = node->mMeshes[0]; //grab the first mesh
            if(node->mNumMeshes == 1)
            {
                int mesh_index = node->mMeshes[0]; //grab the first mesh
                _ml[mesh_count].mesh = pScene->mMeshes[mesh_index];
                mesh_count++;
            }
        }

        //set vertex base array
        for(int i=0; i<_mln; i++)
        {
            aiMesh* mesh = _ml[i].mesh;

            _ml[i].bvln = mesh->mNumVertices;
            _ml[i].bvl = new _Vertex[_ml[i].bvln];

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
        
        for(int i=0; i<_mln; i++)
        {
            aiMesh* mesh = _ml[i].mesh;
            _ml[i].vln = 3*mesh->mNumFaces;
        }

        // vertex index array

        for(int i=0; i<_mln; i++)
        {
            aiMesh* mesh = _ml[i].mesh;

            GS_ASSERT(mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);
            GS_ASSERT(mesh->mTextureCoords[0] != NULL);

            _ml[i].viam = 3*mesh->mNumFaces;
            _ml[i].via = new int[3*mesh->mNumFaces;];

            for(unsigned int j=0; j<mesh->mNumFaces; j++)
            {
                for(int k=0; k<3; k++)
                {
                    _ml[i].via[3*j+k] = mesh->mFaces[j].mIndices[k];
                }
            }

            for(unsigned int j=0; j<mesh->mNumFaces; j++)
            {
                GS_ASSERT(mesh->mFaces[j].mNumIndices == 3);
                GS_ASSERT(mesh->mNumUVComponents[0] == 2);
            }
        }

        // vertex weight

        int index = 0;

        for(int i=0; i<_mln; i++)
        {
            int index = 0;
            aiMesh* mesh = _ml[i].mesh;
            for(int j=0; j<mesh->mNumBones; j++)
            {
                aiBone* bone = mesh->mBones[j];
                index += bone->mNumWeights;
            }
            _ml[i].vwlm = index;
            _ml[i].vwl = new _VertexWeight[index];
        }

        for(int i=0; i<_mln; i++)
        {
            int index = 0;
            aiMesh* mesh = _ml[i].mesh;
            for(int i=0; i<mesh->mNumBones; i++)
            {

        for(int i=0; i<mesh->mNumBones; i++)
        {
            aiBone* bone = mesh->mBones[i];

            for(unsigned int j=0; j<bone->mNumWeights; j++)
            {
                int bone_index =            ml->get_bone_index(bone);       //index of bone matrix
                vwl[index].bone_index =     bone_index;
                vwl[index].vertex_index =   bone->mWeights[j].mVertexId;
                vwl[index].weight =         bone->mWeights[j].mWeight;
                index++;
            }
        }


    }


#if 0
    int bvlm;               //base vertex list max;
    struct _Vertex* bvl;    //base vertex list

    int* bvlo;              //offset for vertices in base list
    int* bvln;              //number of vertices in base vertex list

    //int viam;              //max index for vertex lookup table
    //int* via;              //base vertex lookup
    
    void init_base_vertex_list()
    {
        bvlo = new int[nlm];
        bvln = new int[nlm];

        //count number of faces and vertices total
        int vcount = 0;
        int fcount = 0;
        for(int i=0; i<nlm; i++)
        {
            aiMesh* mesh = ml[i];

            bvlo[i] = vcount;               //base vertex offset
            bvln[i] = mesh->mNumVertices;   //number of base vertex for mesh

            vcount += mesh->mNumVertices;
            fcount += mesh->mNumFaces;
        }

        bvlm = vcount;
        bvl = new _Vertex[bvlm];
        //tbvl = new _Vertex[bvlm];

        //viam = 3*fcount;
        //via = new int[viam];
        //for(int i=0; i<viam; i++) via[i] = -1;
        //GS_ASSERT(viam = vlm);


        //save array of base vertices
        vcount = 0;
        for(int i=0; i<nlm; i++)
        {
            aiMesh* mesh = ml[i];
            //int index1 = bvlo[i];
            GS_ASSERT(vcount == bvlo[i]);
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
                GS_ASSERT(bvlo[i] + (int)(j) == vcount);
                bvl[bvlo[i] + j] = v;
                vcount++;
            }
        }
        GS_ASSERT(vcount == bvlm);
        //save mapping from vertex index to base vertex
    }
#endif 

    //int vlm;          //vertex list max
    //struct _Vertex* vl;   //vertex list
    //struct _Vertex* tvl; //temporary vertex list, for drawing
    //int vli;          //vertex list index
    //int* vll;         //vertex list loop
    //int* vln;         //number of vertices in mesh

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
#if 1
        //count bones
        int bone_count = 0;
        int _bone_count = 0;

        //count number of times bone appears
        for(int i=0; i<nli; i++)
        {
            aiMesh* mesh = ml[i];
            for(unsigned int j=0; j<mesh->mNumBones; j++)
            {
                //aiBone* bone = mesh->mBones[j];
                //printf("bone %d,%d name: %s \n", i,j, bone->mName.data);
                bool new_bone = true;
                for(int _i=0; _i<=i; _i++) 
                {
                    for(unsigned int _j=0; _j<ml[_i]->mNumBones; _j++) 
                    {
                        if(_i == i && _j == j) break;
                        if(strcmp(ml[i]->mBones[j]->mName.data, ml[_i]->mBones[_j]->mName.data) == 0)
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
        //printf("%d %d \n", ml[3]->mBones[0], ml[4]->mBones[0]);
        //printf("bone_count= %d _bone_count= %d nlm= %d \n", bone_count, _bone_count, nlm);
        //printf("stcmp: %d \n", strcmp(ml[1]->mBones[0]->mName.data, ml[2]->mBones[0]->mName.data) );

        //set bone list
        bnlm = bone_count;
        bnl = new BoneNode[bnlm];

        for(int i=0; i<bnlm; i++)
        {
            bnl[i].name = NULL;
            bnl[i].parent_node = NULL;
        }

        //populate bone list
        int bcount = 0;
        for(int i=0; i<nli; i++)
        {
            aiMesh* mesh = ml[i];
            for(unsigned int j=0; j<mesh->mNumBones; j++)
            {
                aiBone* bone = mesh->mBones[j];
                
                for(int k=0; k<bcount;k++)
                {
                    if( bone_in_list(bone, bone_count) == true )
                        continue;
                    
                    bnl[bcount].name = copy_string(bone->mName.data);
                    bnl[bcount].mOffsetMatrix = _ConvertMatrix(bone->mOffsetMatrix);
                    bnl[bcount].parent_node =  FindNodeRecursivelyByName( pScene->mRootNode, bone->mName.data);
                    bnl[bcount].parent_index = -1;

                    bcount++;
                    
                }
            }
        }

#endif
    }


    bool node_in_list(struct aiNode* node, int node_count)
    {
        for(int i=0; i<node_count; i++ )
        {
            if(strcmp(node->mName.data, _nl[i].name) == 0)
                return true;
        }
        return false;
    }

    int node_index_from_list(struct aiNode* node)
    {
        for(int i=0; i<_nlm; i++ )
        {
            if(strcmp(node->mName.data, _nl[i].name) == 0)
                return i;
        }

        GS_ASSERT(false);
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
        node->index = *index;
        *index++;
        for(int i=0; i<node->cn; i++)
            _set_node_index(index, node->c[i] );
    }

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
                    continue;
                //insert

                _nl[node_count].name            = copy_string(tempNode->mName.data);
                _nl[node_count].mTransformation = _ConvertMatrix(tempNode->mTransformation);
                _nl[node_count].node            = tempNode;
                _nl[node_count].p               = NULL;
                _nl[node_count].c               = NULL;
                _nl[node_count].cn              = 0;
                _nl[node_count].index           = -1;

                if( strcmp(tempNode->mName.data, "Armature") == 0 )
                {
                    _nl[node_count].p = NULL;
                    break;
                }
                if(tempNode == NULL)
                    break;
                tempNode = tempNode->mParent;
            }
        }

        _nlm = node_count;
        printf("init_node_list: node_count= %d nlm= %d _nlm= %d \n", node_count, nlm, _nlm);

        //set parent node
        for(int i=0; i<_nlm; i++)
        {
            aiNode* node = _nl[i].node;
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
            GS_ASSERT(_nl[i].index < _nl[i].p->index);
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

    struct Mat4 quantenion_to_rotation_matrix(aiQuaternion q, aiVector3D pos)
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
    unsigned int texture1;
    SDL_Surface* s;

    void init_texture();
    void draw_skeleton(float x, float y, float z);
    void draw(float x, float y, float z);  //draws for testing


    void rationalize_bone_matrices();

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

class BodyPartMesh
{

	public:

    char* mesh_name;

    int bvlm;                   //base vertex list max
    struct _Vertex* bvl;        //base vertex list

    struct _Vertex* vl;         //vertex list
    int vlm;                    //vertex list max

    struct _VertexWeight* vwl;  //vertex weight list
    int vwlm;                   //vertex weight list

    int* via;                   //vertex index array
    int viam;

	BodyPartMesh()
	{
        bvl = NULL;
        vl  = NULL;
        vwl = NULL;
        via = NULL;
	}

	~BodyPartMesh()
	{

	}

    //assumes only one mesh per node
	void load(class ModelLoader* ml, int mesh_index, aiMesh* mesh, aiNode* node)
	{

        //copy name
        mesh_name = new char[strlen(node->mName.data)+1];
        mesh_name = strcpy(mesh_name, node->mName.data);

        //copy base list
        int bvl_offset = ml->bvlo[mesh_index];
        bvlm = ml->bvln[mesh_index];
        bvl = new _Vertex[bvlm];
        for(int i=0; i<bvlm; i++)
            bvl[i] = ml->bvl[i+bvl_offset];

        /*
        for(int i=0; i<nlm; i++)
        {
            aiMesh* mesh = ml[i];
            for(unsigned int j=0; j<mesh->mNumVertices; j++)
            {
                GS_ASSERT(bvlo[i] + (int)(j) == vcount);
                bvl[bvlo[i] + j] = v;
        }
        */

        //allocate vertex list
        int vl_num = ml->vln[mesh_index];
        vl = new _Vertex[vl_num];
        vlm = vl_num;

        //load vertex index array
/*
        viam = ml->viam;
        via = new int[viam];
        for(int i;i<viam;i++)
            via[i] = ml->via[i];
*/

/*
        int vcount = 0;
        for(unsigned int j=0; j<mesh->mNumFaces; j++)
        {
            for(int k=0; k<3; k++)
            {
                via[vcount] = bvlo[i] + mesh->mFaces[j].mIndices[k];
                vcount++;
            }
        }
*/

        //vertex weight lists

        int num_weights = 0;

        for(unsigned int i=0; i<mesh->mNumBones; i++)
            num_weights += mesh->mBones[i]->mNumWeights;

        vwl = new _VertexWeight[num_weights];
        vwlm = num_weights;

        int index = 0;


    }
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

    BodyMesh()
    {
        nnl = NULL;
        npl = NULL;
        node_mTransformation = NULL;
        bnl = NULL;
        bone_mOffsetMatrix = NULL;
        bpl = NULL;
    }

    ~BodyMesh()
    {
        if(nnl != NULL) delete[] nnl;
        if(npl != NULL) delete[] npl;
        if(node_mTransformation != NULL) delete[] node_mTransformation;
        if(bnl != NULL) delete[] bnl;
        if(bone_mOffsetMatrix != NULL) delete[] bone_mOffsetMatrix;
        if(bpl != NULL) delete[] bpl;
    }

    void load(class ModelLoader* ml)
    {
        //load node list

        nm = ml->_nlm;
        nnl = new char*[nm];
        npl = new int[nm];
        node_mTransformation = new struct Mat4[nm];  //mTransformation;

        for(int i=0;i<nm; i++)
        {  
            GS_ASSERT(ml->_nl[i].index == i);
            nnl[i] = ml->_nl[i].name;
            npl[i] = (ml->_nl[i].p == NULL ? 0 : ml->_nl[i].p->index);
            node_mTransformation[i] = ml->_nl[i].mTransformation;
        }

        //load bone list

        blm = ml->bnlm;
        bnl = new char*[blm];
        bone_mOffsetMatrix = new struct Mat4[blm];
        bpl = new int[blm];

        for(int i=0;i<blm;i++)
        {
            bnl[i] = ml->bnl[i].name; 
            bone_mOffsetMatrix[i] = ml->bnl[i].mOffsetMatrix;
            bpl[i] = ml->bnl[i].parent_index;
        }
        //load meshes


    }
};

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

                glColor4ub(255,0,0,0);

                glVertex3f(x+vv.x, y+vv.y, z+vv.z);
                glColor4ub(0,255,0,0);

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
