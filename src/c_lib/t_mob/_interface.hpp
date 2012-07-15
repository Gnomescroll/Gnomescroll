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

	cBone** bca; //bone child array
	int bcan; //bone child array index
	//std::vector<cBone*> Children;

	cBone()
	{
		parent = NULL;
		name = NULL;
	}
	

	~cBone() {}
};


// there is some type of alignment issue with my mat4 and the aimatrix4x4 class, so the copy must be manually
void ConvertMatrix(struct Mat4* out, const aiMatrix4x4& in)
{
	out._11 = in.a1;
	out._12 = in.a2;
	out._13 = in.a3;
	out._14 = in.a4;

	out._21 = in.b1;
	out._22 = in.b2;
	out._23 = in.b3;
	out._24 = in.b4;

	out._31 = in.c1;
	out._32 = in.c2;
	out._33 = in.c3;
	out._34 = in.c4;

	out._41 = in.d1;
	out._42 = in.d2;
	out._43 = in.d3;
	out._44 = in.d4;
}

class MeshAnimation
{
	public:
// ------------------------------------------------------------------------------------------------
// Recursively creates an internal node structure matching the current scene and animation.
	class cBone* CreateBoneTree( aiNode* pNode, cBone* pParent)
	{
		class cBone* bone = new cBone();// create a node
		bone->name = pNode->mName.data;// get the name of the bone
		bone->parent = pParent; //set the parent, in the case this is the root node, it will be null

		printf("bone name= %s \n", bone->name);
		//BonesByName[bone->Name] = bone;// use the name as a key
		//set bone name lookup


		TransformMatrix(bone->LocalTransform, pNode->mTransformation);
		bone->LocalTransform.Transpose();
		bone->OriginalLocalTransform = bone->LocalTransform;// a copy saved
		CalculateBoneToWorldTransform(bone);

		// continue for all child nodes and assign the created internal nodes as our children
		// recursivly call this function on all children
		for( unsigned int a = 0; a < pNode->mNumChildren; a++)
		{
			bone->Children.push_back(CreateBoneTree( pNode->mChildren[a], bone));
		}
		return bone;
	}

}