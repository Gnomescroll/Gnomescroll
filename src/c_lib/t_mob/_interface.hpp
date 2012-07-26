#pragma once

#include <physics/mat4.hpp>

#undef __cplusplus
extern "C"
{
#include <assimp/cimport.h>
#include <assimp/scene.h>
//#include <assimp/aiPostProcess.h>
}
#define __cplusplus


namespace t_mob
{

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
	

	~cBone() {}

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
		bone->local_transform = Mat4_transpose(bone->local_transform);	//combine conversion and transpose?

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

	// this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
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
void SceneAnimator::Init(const aiScene* pScene)
{// this will build the skeleton based on the scene passed to it and CLEAR EVERYTHING
	if(!pScene->HasAnimations()) 
	{
		printf("Error: no animation \n");
		return;
	}
	
	Skeleton = CreateBoneTree( pScene->mRootNode, NULL);
	ExtractAnimations(pScene);
	
	for (unsigned int i = 0; i < pScene->mNumMeshes;++i)
	{
		const aiMesh* mesh = pScene->mMeshes[i];
		
		for (unsigned int n = 0; n < mesh->mNumBones;++n)
		{
			const aiBone* bone = mesh->mBones[n];
			std::map<std::string, cBone*>::iterator found = BonesByName.find(bone->mName.data);
			
			//found->second is a bone
			if(found != BonesByName.end())
			{// FOUND IT!!! woohoo, make sure its not already in the bone list
				bool skip = false;
				for(size_t j(0); j< Bones.size(); j++)
				{
					std::string bname = bone->mName.data;
					if(Bones[j]->Name == bname)
					{
						skip = true;// already inserted, skip this so as not to insert the same bone multiple times
						break;
					}
				}
				if(!skip)
				{// only insert the bone if it has not already been inserted
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
	for(size_t i(0); i< Animations.size(); i++)
	{// pre calculate the animations
		SetAnimIndex(i);
		float dt = 0;
		for(float ticks = 0; ticks < Animations[i].Duration; ticks += Animations[i].TicksPerSecond/30.0f)
		{
			dt +=timestep;
			Calculate(dt);
			Animations[i].Transforms.push_back(std::vector<mat4>());
			std::vector<mat4>& trans = Animations[i].Transforms.back();
			for( size_t a = 0; a < Transforms.size(); ++a)
			{
				mat4 rotationmat =  Bones[a]->Offset * Bones[a]->global_transform;
				trans.push_back(rotationmat);
			}
		}
	}
	//OUTPUT_DEBUG_MSG("Finished loading animations with "<<Bones.size()<<" bones");
}

*/

/** Reads the given file from a given memory buffer,
 * 
 * If the call succeeds, the contents of the file are returned as a pointer to an
 * aiScene object. The returned data is intended to be read-only, the importer keeps 
 * ownership of the data and will destroy it upon destruction. If the import fails, 
 * NULL is returned.
 * A human-readable error description can be retrieved by calling aiGetErrorString(). 
 * @param pBuffer Pointer to the file data
 * @param pLength Length of pBuffer, in bytes
 * @param pFlags Optional post processing steps to be executed after 
 *   a successful import. Provide a bitwise combination of the 
 *   #aiPostProcessSteps flags. If you wish to inspect the imported
 *   scene first in order to fine-tune your post-processing setup,
 *   consider to use #aiApplyPostProcessing().
 * @param pHint An additional hint to the library. If this is a non empty string,
 *   the library looks for a loader to support the file extension specified by pHint
 *   and passes the file to the first matching loader. If this loader is unable to 
 *   completely the request, the library continues and tries to determine the file
 *   format on its own, a task that may or may not be successful. 
 *   Check the return value, and you'll know ...
 * @return A pointer to the imported data, NULL if the import failed.
 *
 * @note This is a straightforward way to decode models from memory buffers, but it 
 * doesn't handle model formats spreading their data across multiple files or even
 * directories. Examples include OBJ or MD3, which outsource parts of their material
 * stuff into external scripts. If you need the full functionality, provide a custom 
 * IOSystem to make Assimp find these files.
 */

/*
ASSIMP_API const C_STRUCT aiScene* aiImportFileFromMemory( 
	const char* pBuffer,
	unsigned int pLength,
	unsigned int pFlags,
	const char* pHint);
*/

void PrintBoneTree(int num, aiNode* pNode)
{
	printf("num: %2d bone name= %s \n", num, pNode->mName.data);

	for(unsigned int i=0; i < pNode->mNumChildren; i++)
	{
		PrintBoneTree(num+1, pNode->mChildren[i]);
	}
}

void init()
{
	int bsize;
	//char* buffer = read_file_to_buffer( (char*) "media/mesh/collada_test.dae", &bsize);
	char* buffer = read_file_to_buffer( (char*) "media/mesh/3d_max_test.3ds", &bsize);


	int aFlag = 0;
	char* aHint = NULL;
	const struct aiScene* pScene = aiImportFileFromMemory(buffer, bsize, aFlag , aHint);


	if(pScene == NULL)
	{
		printf("Assimp Scene Load Error: %s \n", aiGetErrorString() );

		abort();
	}
/*
	if(!pScene->HasAnimations() )
	{
		printf("Error: scene has no animation \n";
	}
*/

	//aiString.data is char*
	printf("Scene animations: %d \n", pScene->mNumAnimations);

	PrintBoneTree(0, pScene->mRootNode);	//these are actually meshes
	//pScene->mRootNode


	for(unsigned int i=0; i < pScene->mNumMeshes; i++)
	{
		printf("Mesh %2d: %s \n", i, pScene->mMeshes[i]->mName.data);

	}


	for(unsigned int i=0; i < pScene->mNumMeshes; i++)
	{
		printf("Mesh %2d: %s \n", i, pScene->mMeshes[i]->mName.data);

		for(unsigned int j=0; j < pScene->mMeshes[i]->mNumBones; j++)
		{

			printf("Bone %2d: %s \n", j, pScene->mMeshes[i]->mBones[j]->mName.data);
		}

	}

	printf("Succes\n");

	abort();
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


}


void draw()

{


}


void teardown()
{

}

}