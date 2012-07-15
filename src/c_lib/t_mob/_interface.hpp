#pragma once

#include <physics/mat4.hpp>

#undef __cplusplus
extern "C"
{
#include <assimp.h>
#include <aiScene.h>
#include <aiPostProcess.h>
}
#define __cplusplus

class cBone 
{
	public:

	char* name;

	Mat4 offset;
	Mat4 local_transform;
	Mat4 global_transform;
	Mat4 original_local_transform;

	//mat4 Offset, LocalTransform, GlobalTransform, OriginalLocalTransform;
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


void Mat4_transpose(struct Mat4 m1)
{
	struct Mat4 m2;

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			m2[i][j] = m1[j][i];
		}
	}
	return m2;
}
// there is some type of alignment issue with my mat4 and the aimatrix4x4 class, so the copy must be manually
void ConvertMatrix(struct Mat4& out, const aiMatrix4x4& in)
{
	out[0][0] = in.a1;
	out[0][1] = in.a2;
	out[0][2] = in.a3;
	out[0][3] = in.a4;

	out[1][0] = in.b1;
	out[1][1] = in.b2;
	out[1][2] = in.b3;
	out[1][3] = in.b4;

	out[2][0] = in.c1;
	out[2][1] = in.c2;
	out[2][2] = in.c3;
	out[2][3] = in.c4;

	out[3][0] = in.d1;
	out[3][1] = in.d2;
	out[3][2] = in.d3;
	out[3][3] = in.d4;
}

class MeshAnimation
{
	public:
// ------------------------------------------------------------------------------------------------
// Recursively creates an internal node structure matching the current scene and animation.
	class cBone* CreateBoneTree(class cBone* bone, aiNode* pNode, cBone* pParent)
	{
		bone->name = pNode->mName.data;// get the name of the bone
		bone->parent = pParent; //set the parent, in the case this is the root node, it will be null

		printf("bone name= %s \n", bone->name);
		//BonesByName[bone->Name] = bone;// use the name as a key
		//set bone name lookup


		ConvertMatrix(bone->LocalTransform, pNode->mTransformation);
		bone->LocalTransform = Mat4_transpose(bone->LocalTransform);	//combine conversion and transpose?

		bone->OriginalLocalTransform = bone->LocalTransform;// a copy saved
		
		CalculateBoneToWorldTransform(bone);

		// continue for all child nodes and assign the created internal nodes as our children
		// recursivly call this function on all children

		bone->set_children(pNode->mNumChildren);

		for(int i=0; ii < pNode->mNumChildren; i++)
		{
			CreateBoneTree( &bone->bca[i], pNode->mChildren[a], bone);
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
		bone->GlobalTransform = mat4_mult(bone->GlobalTransform, parent->LocalTransform);
		parent = parent->parent; // get the parent of the bone we are working on 
	}
}

// Recursively updates the internal node transformations from the given matrix array
void UpdateTransforms(cBone* pNode) 
{
	CalculateBoneToWorldTransform( pNode);// update global transform as well

	for(int i=0; i<pNode->bcan; i++)
		UpdateTransforms(pNode->bca[i]);
}

