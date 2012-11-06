


class cBone 
{
    public:

    char* name;

    Mat4 offset;
    Mat4 local_transform;
    Mat4 global_transform;
    Mat4 original_local_transform;

    //mat4 Offset, local_transform, global_transform, Originallocal_transform;
    cBone* parent;

    cBone* bca; //bone child array
    int bcan; //bone child array index
    //std::vector<cBone*> Children;

    cBone()
    {
        parent = NULL;
        name = NULL;

        bca = NULL;
        bcan = 0;
    }
    

    ~cBone()
    {
        if (bca != NULL) delete[] bca;
    }

    void set_children(int num)
    {
        GS_ASSERT(bca == NULL)
        bcan = num;
        bca = new cBone[num];
    }
};


struct Mat4 Mat4_transpose(struct Mat4 m1)
{
    struct Mat4 m2;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            m2.f[i][j] = m1.f[j][i];
        }
    }
    return m2;
}
// there is some type of alignment issue with my mat4 and the aimatrix4x4 class, so the copy must be manually
void ConvertMatrix(struct Mat4& out, const aiMatrix4x4& in)
{
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
}

void CalculateBoneToWorldTransform(class cBone* bone);
void UpdateTransforms(cBone* bone);

class MeshAnimation
{
    public:
// ------------------------------------------------------------------------------------------------
// Recursively creates an internal node structure matching the current scene and animation.
    void CreateBoneTree(class cBone* bone, aiNode* pNode, cBone* pParent)
    {
        bone->name = pNode->mName.data;// get the name of the bone
        bone->parent = pParent; //set the parent, in the case this is the root node, it will be null

        printf("bone name= %s \n", bone->name);
        //BonesByName[bone->Name] = bone;// use the name as a key
        //set bone name lookup


        ConvertMatrix(bone->local_transform, pNode->mTransformation);
        bone->local_transform = Mat4_transpose(bone->local_transform);  //combine conversion and transpose?

        bone->original_local_transform = bone->local_transform;// a copy saved

        CalculateBoneToWorldTransform(bone);

        // continue for all child nodes and assign the created internal nodes as our children
        // recursivly call this function on all children

        bone->set_children(pNode->mNumChildren);

        for(unsigned int i=0; i < pNode->mNumChildren; i++)
        {
            CreateBoneTree( &bone->bca[i], pNode->mChildren[i], bone);
        }
    }

};

/*
    This is slow and does ~n^2 more matrix multiplications than it should in tree depth
*/

// Calculates the global transformation matrix for the given internal node
void CalculateBoneToWorldTransform(class cBone* bone)
{
    bone->global_transform = bone->local_transform;
    class cBone* parent = bone->parent;

    // this will climb the nodes up along through the parents concentating all the matrices to get the Entity to World transform, or in this case, the Bone To World transform
    while( parent )
    {
        bone->global_transform = mat4_mult(bone->global_transform, parent->local_transform);
        parent = parent->parent; // get the parent of the bone we are working on 
    }
}

// Recursively updates the internal node transformations from the given matrix array
// ** calls updateTransforms on each bone in skeleton **
void UpdateTransforms(cBone* bone) 
{
    CalculateBoneToWorldTransform(bone);// update global transform as well

    for(int i=0; i<bone->bcan; i++)
        UpdateTransforms(&bone->bca[i]);
}

/*
ASSIMP_API const C_STRUCT aiScene* aiImportFileFromMemory( 
    const char* pBuffer,
    unsigned int pLength,
    unsigned int pFlags,
    const char* pHint);
*/

//unsigned int mNumMeshes;
//unsigned int* mMeshes;    <---index into aiScene mesh array!


//struct aiScene
//C_STRUCT aiMesh** mMeshes;





    printf("Animations: \n");

    printf("Scene animations: %d \n", pScene->mNumAnimations);


    for(unsigned int i=0; i<pScene->mNumAnimations; i++)
    {
        aiAnimation* anim = pScene->mAnimations[i];


        printf("anim %02d: name= %s duration= %f ticks_per_second= %f channels= %d mesh_channels= %d \n", i,
            anim->mName.data,
            anim->mDuration,
            anim->mTicksPerSecond,
            anim->mNumChannels,
            anim->mNumMeshChannels
            );


        for(unsigned int j=0; j<anim->mNumChannels; j++)
        {
            aiNodeAnim* node_anim = anim->mChannels[j];

            printf("\tnode_anim: %02d name= %s position_keys= %d rotation_keys= %d \n", j,
            node_anim->mNodeName.data,
            node_anim->mNumPositionKeys,    //aiVectorKey* mPositionKeys;
            node_anim->mNumRotationKeys     //aiQuatKey* mRotationKeys;
            );


            for(unsigned int k=0; k<node_anim->mNumPositionKeys; k++)
            {
                printf("aiVector %i: time %f \n", k, node_anim->mPositionKeys[k].mTime );
            }
            //struct aiVectorKey
            //double mTime; //The time of this key
            //C_STRUCT aiVector3D mValue;  //The value of this key

            //struct aiQuatKey
            //double mTime;     //The time of this key 
            //C_STRUCT aiQuaternion mValue;  //The value of this key
        }

    }



#if 0
     *  name is usually empty (length is zero). */
    C_STRUCT aiString mName;

    /** Duration of the animation in ticks.  */
    double mDuration;

    /** Ticks per second. 0 if not specified in the imported file */
    double mTicksPerSecond;

    /** The number of bone animation channels. Each channel affects
     *  a single node. */
    unsigned int mNumChannels;

    /** The node animation channels. Each channel affects a single node. 
     *  The array is mNumChannels in size. */
    C_STRUCT aiNodeAnim** mChannels;

    /** The number of mesh animation channels. Each channel affects
     *  a single mesh and defines vertex-based animation. */
    unsigned int mNumMeshChannels;

    /** The mesh animation channels. Each channel affects a single mesh. 
     *  The array is mNumMeshChannels in size. */
    C_STRUCT aiMeshAnim** mMeshChannels;

    /** The array of animations. 
    *
    * All animations imported from the given file are listed here.
    * The array is mNumAnimations in size.
    */
    //C_STRUCT aiAnimation** mAnimations;
#endif


/*
    printf("pScene->mMeshes: \n");

    for(unsigned int i=0; i < pScene->mNumMeshes; i++)
    {
        printf("Mesh %02d: %s \n", i, pScene->mMeshes[i]->mName.data);
    }
*/
#if 0
    for(unsigned int i=0; i < pScene->mNumMeshes; i++)
    {
        printf("Mesh %02d: name= %s numBones= %d \n", i, pScene->mMeshes[i]->mName.data, pScene->mMeshes[i]->mNumBones);

        for(unsigned int j=0; j < pScene->mMeshes[i]->mNumBones; j++)
        {
            printf("\tBone %02d: %s \n", j, pScene->mMeshes[i]->mBones[j]->mName.data);
        }
    }
#endif



/*
    if(!pScene->HasAnimations()) return;
    Release();
    
    Skeleton = CreateBoneTree( pScene->mRootNode, NULL);
    ExtractAnimations(pScene);
    
    for (unsigned int i = 0; i < pScene->mNumMeshes;++i){
        const aiMesh* mesh = pScene->mMeshes[i];
        
        for (unsigned int n = 0; n < mesh->mNumBones;++n){
            const aiBone* bone = mesh->mBones[n];
            std::map<std::string, cBone*>::iterator found = BonesByName.find(bone->mName.data);
            if(found != BonesByName.end()){// FOUND IT!!! woohoo, make sure its not already in the bone list
                bool skip = false;
                for(size_t j(0); j< Bones.size(); j++){
                    std::string bname = bone->mName.data;
                    if(Bones[j]->Name == bname) {
                        skip = true;// already inserted, skip this so as not to insert the same bone multiple times
                        break;
                    }
                }
                if(!skip){// only insert the bone if it has not already been inserted
                    std::string tes = found->second->Name;
                    TransformMatrix(found->second->Offset, bone->mOffsetMatrix);
                    found->second->Offset.Transpose();// transpoce their matrix to get in the correct format
                    Bones.push_back(found->second);
                    BonesToIndex[found->first] = Bones.size()-1;
                }
            } 
        }
    }
    Transforms.resize( Bones.size());
    float timestep = 1.0f/30.0f;// 30 per second
    for(size_t i(0); i< Animations.size(); i++){// pre calculate the animations
        SetAnimIndex(i);
        float dt = 0;
        for(float ticks = 0; ticks < Animations[i].Duration; ticks += Animations[i].TicksPerSecond/30.0f){
            dt +=timestep;
            Calculate(dt);
            Animations[i].Transforms.push_back(std::vector<mat4>());
            std::vector<mat4>& trans = Animations[i].Transforms.back();
            for( size_t a = 0; a < Transforms.size(); ++a){
                mat4 rotationmat =  Bones[a]->Offset * Bones[a]->GlobalTransform;
                trans.push_back(rotationmat);
            }
        }
    }
    OUTPUT_DEBUG_MSG("Finished loading animations with "<<Bones.size()<<" bones");
*/


    /** The name of the node affected by this animation. The node  must exist and it must be unique.*/
    //C_STRUCT aiString mNodeName;

    /** The number of position keys */
    //unsigned int mNumPositionKeys;

    /** The position keys of this animation channel. Positions are 
     * specified as 3D vector. The array is mNumPositionKeys in size.
     *
     * If there are position keys, there will also be at least one
     * scaling and one rotation key.*/
    //C_STRUCT aiVectorKey* mPositionKeys;

    /** The number of rotation keys */
    //unsigned int mNumRotationKeys;

    /** The rotation keys of this animation channel. Rotations are 
     *  given as quaternions,  which are 4D vectors. The array is 
     *  mNumRotationKeys in size.
     *
     * If there are rotation keys, there will also be at least one
     * scaling and one position key. */
    //C_STRUCT aiQuatKey* mRotationKeys;

    /** The number of scaling keys */
    //unsigned int mNumScalingKeys;

    /** The scaling keys of this animation channel. Scalings are 
     *  specified as 3D vector. The array is mNumScalingKeys in size.
     *
     * If there are scaling keys, there will also be at least one
     * position and one rotation key.*/
    //C_STRUCT aiVectorKey* mScalingKeys;



    //printf("animation0: name= %s duration= %f ticks_per_second= %f number_of_node_channels= %d mesh_channels= %d \n",
    //    anim->mName.data, anim->mDuration, anim->mTicksPerSecond, anim->mNumChannels, anim->mNumMeshChannels);

    //C_STRUCT aiString mName;
    /** Duration of the animation in ticks.  */
    //double mDuration;
    /** Ticks per second. 0 if not specified in the imported file */
    //double mTicksPerSecond;
    /** The number of bone animation channels. Each channel affects a single node. */
    //unsigned int mNumChannels;
    //C_STRUCT aiNodeAnim** mChannels;

    //unsigned int mNumAnimations; 
    //_STRUCT aiAnimation** mAnimations;


        //ml[i]->mMeshes[index]->mName.data,
        //ml[i]->mMeshes[index]->mNumVertices,
        //ml[i]->mMeshes[index]->mNumFaces);s

    /*
        unsigned int mNumVertices;
        unsigned int mNumFaces;
        C_STRUCT aiVector3D* mVertices; //the vertices
        C_STRUCT aiVector3D* mNormals;
        C_STRUCT aiVector3D* mTextureCoords[AI_MAX_NUMBER_OF_TEXTURECOORDS];
        unsigned int mNumUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS];
        C_STRUCT aiFace* mFaces;
    */

/*
aiMesh
    unsigned int mPrimitiveTypes;
    unsigned int mNumVertices;
    unsigned int mNumFaces;

    C_STRUCT aiVector3D* mVertices; //the vertices
    C_STRUCT aiVector3D* mNormals;

    C_STRUCT aiVector3D* mTextureCoords[AI_MAX_NUMBER_OF_TEXTURECOORDS];
    unsigned int mNumUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS];

    C_STRUCT aiFace* mFaces;

    unsigned int mNumBones;
    C_STRUCT aiBone** mBones;
*/

#if 0
        //check_gl_error();
        glBegin(GL_TRIANGLES);

        //printf("=== \n");

        for(int i=0; i<nlm; i++)
        {
            aiMesh* mesh = ml[i];
            //int index1 = bvlo[i];
            for(unsigned int j=0; j<mesh->mNumFaces; j++)
            {

                for(int k=0; k<3; k++)
                {
                    GS_ASSERT( mesh->mFaces[j].mNumIndices == 3);
                    GS_ASSERT( mesh->mNumUVComponents[0] == 2);

                    int index1 = mesh->mFaces[j].mIndices[k];
                    //int index2 = mesh->mFaces[j].mIndices[(k+1)%3];

                    aiVector3D pos = mesh->mVertices[index1];
                    aiVector3D tex = mesh->mTextureCoords[0][index1];

                    struct _Vertex v; 
                    v.v.x = pos.x ;
                    v.v.y =  pos.y ;
                    v.v.z = pos.z ;

                    v.ux =  tex.x;
                    v.uy =  1.0 -tex.y;

                    //printf("pos= %f %f %f tex= %f %f \n", v.v.x,v.v.y,v.v.z, v.ux,v.uy);
                    glTexCoord2f(v.ux, v.uy );
                    glVertex3f(v.v.x +x, v.v.y+y, v.v.z+z);
                }

            }
        }

        glEnd();


/*
        glBegin(GL_QUADS);

        float xmin = x + 0.0;
        float xmax = x + 1.0;

        float ymin = y + 0.0;
        float ymax = y + 1.0;

        //upper left, counter clockwise
        glTexCoord2f(0,1 );
        glVertex3f(xmax, ymax, z+2.0);

        glTexCoord2f(0,0);
        glVertex3f(xmax, ymin, z+2.0);

        glTexCoord2f(1,0);
        glVertex3f(xmin, ymin, z+2.0);

        glTexCoord2f(1,1);
        glVertex3f(xmin, ymax, z+2.0);

        glEnd();
*/

        glBindTexture(GL_TEXTURE_2D, 0);
        check_gl_error();
#endif
