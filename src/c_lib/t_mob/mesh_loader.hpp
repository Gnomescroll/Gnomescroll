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



class ModelLoader
{
    public:

    ModelLoader() :
    pScene(NULL),
    nl(NULL), 
    ml(NULL),
    tvl(NULL),
    vll(NULL), vln(NULL),
    bvl(NULL), tbvl(NULL),
    bvlo(NULL), bvln(NULL),
    bvll(NULL),
    s(NULL)
    {}
    
    ~ModelLoader()
    {
        if (s != NULL) SDL_FreeSurface(s);
        if (pScene != NULL) aiReleaseImport(pScene);
        if (nl != NULL) delete[] nl;
        if (ml != NULL) delete[] ml;
        if (tvl != NULL) delete[] tvl;
        if (vll != NULL) delete[] vll;
        if (vln != NULL) delete[] vln;
        if (bvl != NULL) delete[] bvl;
        if (tbvl != NULL) delete[] tbvl;
        if (bvlo != NULL) delete[] bvlo;
        if (bvln != NULL) delete[] bvln;
        if (bvll != NULL) delete[] bvll;
    }

    aiScene* pScene;    //the scene

    int nlm;        //node list max
    aiNode** nl;    //node list
    int nli;        //node list index (just a counter variable)

    aiMesh** ml;    //mesh list

    struct _Vertex
    {
        struct Vec3 v;
        float ux,uy;
    };

    int vli;                //vertex list index
    int vlm;                //vertex list max
    struct _Vertex* tvl;    //temporary vertex list, for drawing

    int* vll;           //offset of vertices in list for each mesth
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

        //set_mesh_list();
        count_vertices();
        tvl = new _Vertex[vlm];

        vll = new int[nlm];
        vln = new int[nlm];

        init_base_vertex_list();
        set_vertices();
        init_texture();
        //draw();
        init_bone_list();
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

            GS_ASSERT(pNode->mNumMeshes < 2);
            int mesh_index = pNode->mMeshes[0]; //grab the first mesh
            ml[nli] = pScene->mMeshes[mesh_index];
            nli++;
        }
        for(unsigned int i=0; i < pNode->mNumChildren; i++)
        {
            set_node_parents(pNode->mChildren[i]);
        }
    }

    void count_vertices()
    {
        vli = 0;

        for(int i=0; i<nli; i++)
        {
            struct aiNode* pNode = nl[i];
            for(unsigned int i=0; i < pNode->mNumMeshes; i++)
            {
                unsigned int index = pNode->mMeshes[i];
                aiMesh* mesh = pScene->mMeshes[index];
                vli += 3*mesh->mNumFaces;
            }
        }
        vlm = vli;
    }


    void set_vertices()
    {
        GS_ASSERT(nli == nlm);

        int count = 0;
        for(int i=0; i<nlm; i++)
        {
            aiMesh* mesh = ml[i];
            GS_ASSERT(mesh != NULL);

            if(mesh == NULL)
            {
                printf("set_vertices error: mesh %d is null\n", i);
                continue;
            }

            vll[i] = count;
            vln[i] = mesh->mNumVertices;

            GS_ASSERT(mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);
            GS_ASSERT(mesh->mTextureCoords[0] != NULL);

            //printf("%02d: mNumFaces= %d mNumVertices= %d \n", i, mesh->mNumFaces, mesh->mNumVertices);
            for(unsigned int j=0; j<mesh->mNumFaces; j++)
            {
                GS_ASSERT(mesh->mFaces[j].mNumIndices == 3);
                GS_ASSERT(mesh->mNumUVComponents[0] == 2);
                //printf("max tex= %d \n", AI_MAX_NUMBER_OF_TEXTURECOORDS);

                for(int k=0; k<3; k++)
                {
                    count++;
                }
            }
        }

        if(count != vli || count != vlm)
        {
            printf("Set_vertices Warning: vertex count= %d vli= %d vlm= %d \n", count, vli, vlm);
        }
    }

    int bvlm;               //base vertex list max;
    struct _Vertex* bvl;    //base vertex list
    struct _Vertex* tbvl;   //base vertex list for matrix transforms/drawing

    int* bvlo;              //offset for vertices in base list
    int* bvln;              //number of vertices in base vertex list

    int bvllm;              //max index for vertex lookup table
    int* bvll;              //base vertex lookup
    
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
        tbvl = new _Vertex[bvlm];

        bvllm = 3*fcount;
        bvll = new int[bvllm];
        for(int i=0; i<bvllm; i++) bvll[i] = -1;
        GS_ASSERT(bvllm = vlm);


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
        vcount = 0;
        for(int i=0; i<nlm; i++)
        {
            aiMesh* mesh = ml[i];
            //int index1 = bvlo[i];
            for(unsigned int j=0; j<mesh->mNumFaces; j++)
            {
                for(int k=0; k<3; k++)
                {
                    bvll[vcount] = bvlo[i] + mesh->mFaces[j].mIndices[k];
                    vcount++;
                }
            }
        }
        GS_ASSERT(vcount == vlm);

        printf("vcount= %d bvlm= %d \n", vcount, bvlm);
    }

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

    int bam;        //bone array max
    aiBone** ba;    //bone array

    int* bal;       //bone array lookup: maps mesh to starting offset in ball
    int* ball;      // ball[bal[mesh_num] + bone_num] is matrix index
    int  ballm;

    struct Mat4* bbma;      //bone base matrix array
    struct Mat4* bma;       //bone matrix array

    void count_bones(int* count, aiNode* pNode)
    {
        GS_ASSERT(pNode->mNumMeshes == 0);  //RECOVER FROM THIS
        for(unsigned int i=0; i < pNode->mNumChildren; i++)
        {
            *count++;
            count_bones(count, pNode->mChildren[i]);
        }
    }

    void count_bones()
    {
        
        //aiNode* pNode = pScene->mRootNode;
        aiNode* pNode = NULL;

        for(unsigned int i=0; i < pScene->mRootNode->mNumChildren; i++)
        {
            if( strcmp("Armature", pScene->mRootNode->mChildren[i]->mName.data) == 0)
            {
                pNode = pScene->mRootNode->mChildren[i];
                break;
            }
        }

        if(pNode == NULL)
        {
            printf("DAE LOAD ERROR: There is no 'Armature' node under root node \n");
            abort();
        }

        aiNode* _pNode = NULL;

        //get the root_bone as child of the Amature
        for(unsigned int i=0; i < pNode->mNumChildren; i++)
        {
            if( strcmp("root_bone", pNode->mChildren[i]->mName.data) == 0)
            {
                _pNode = pNode->mChildren[i];
                break;
            }
        }
        if(_pNode == NULL)
        {
            printf("DAE LOAD ERROR: There is no 'root_bone' under 'Armature' node \n");
            abort();
        }

        pNode = _pNode;

        printf("root_bone: has %d children \n", pNode->mNumChildren); //recursively count the children of the root node
        int bone_count = 0;
        count_bones(&bone_count, pNode);

        printf("DAE LOADER: %d bones \n", bone_count);
    }   


    void init_bone_list()
    {
        //count bones
        int bone_count = 0;
        int _bone_count = 0;

        //count number of times bone appears
        for(int i=0; i<nli; i++)
        {
            aiMesh* mesh = ml[i];
            for(unsigned int j=0; j<mesh->mNumBones; j++)
            {
                aiBone* bone = mesh->mBones[j];


                printf("bone %d,%d name: %s \n", i,j, bone->mName.data);

                bool new_bone = true;

                for(int _i=0; _i<=i; _i++) 
                {
                    for(unsigned int _j=0; _j<ml[_i]->mNumBones; _j++) 
                    {
                        if(_i == i && _j == j) break;
                        //if(bone == ml[_i]->mBones[_j])
                        //printf("cmp: %d %d : %d %d \n", i,j, _i,_j);
                        if(strcmp(ml[i]->mBones[j]->mName.data, ml[_i]->mBones[_j]->mName.data) == 0)
                        {
                            //printf("bone %d,%d matches bone %d,%d \n", i,j, _i,_j);
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

        printf("bone_count= %d _bone_count= %d nlm= %d \n", bone_count, _bone_count, nlm);

        //printf("stcmp: %d \n", strcmp(ml[1]->mBones[0]->mName.data, ml[2]->mBones[0]->mName.data) );

        count_bones();
    }


    unsigned int texture1;
    SDL_Surface* s;

    void init_texture();


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


    void draw_skeleton(float x, float y, float z)
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

                    tempNode = tempNode->mParent;

                    if(tempNode == NULL)
                        break;
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

                    //printf("vec: \n");
                    //vec3_print(vv);
                    //vec3_print(vf);

                    glVertex3f(x+vv.x+vf.x, y+vv.y+vf.y, z+vv.z+vf.z);
                glEnd();
            }
        }



        glColor4ub(255,255,255,255);
        glDisable(GL_TEXTURE_2D);
/*
        glBegin(GL_POINTS);
        for(int i=0; i<vlm; i++)
        {
            //glVertex3f(v.v.x, v.v.y, v.v.z);
        }
        glEnd();
*/
        check_gl_error();
    }

    //mValue

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

/*
        m._f[12] = 0.0f;
        m._f[13] = 0.0f;
        m._f[14] = 0.0f;
*/

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

    //Scene*Armature*Bone1*Bone2*Offs
    void draw_test(float x, float y, float z);  //draws for testing

};




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

void ModelLoader::draw_test(float x, float y, float z)
{
    //printf("nlm= %d vlm= %d \n", nlm, vlm);
    static int _fcount = 0;
    static int frame_time = 0;

    _fcount++;
    if(_fcount % 30 == 0)
        frame_time++;

    Mat4 m_GlobalInverseTransform = mat4_inverse( mat4_transpose(_ConvertMatrix(pScene->mRootNode->mTransformation )));

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


            #endif
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
        int index = bvll[i];
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


}
