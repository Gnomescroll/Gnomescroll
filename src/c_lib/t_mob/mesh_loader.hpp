#pragma once



#undef __cplusplus
extern "C"
{
#include <assimp.h>
#include <aiScene.h>
#include <aiPostProcess.h>
}
#define __cplusplus

class cAnimationChannel
{
	public:
	std::string Name;
	std::vector<aiVectorKey> mPositionKeys;
	std::vector<aiQuatKey> mRotationKeys;
	std::vector<aiVectorKey> mScalingKeys;
};

class cAnimEvaluator
{
	public:

	cAnimEvaluator(): mLastTime(0.0f), TicksPerSecond(0.0f), Duration(0.0f), PlayAnimationForward(true) {}
	cAnimEvaluator( const aiAnimation* pAnim);
	void Evaluate( float pTime, std::map<std::string, cBone*>& bones);

	std::vector<mat4>& GetTransforms(float dt){ return Transforms[GetFrameIndexAt(dt)]; }
	unsigned int GetFrameIndexAt(float time);

	std::string Name;
	std::vector<cAnimationChannel> Channels;
	bool PlayAnimationForward;// play forward == true, play backward == false
	float mLastTime, TicksPerSecond, Duration;	
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int> > mLastPositions;
	std::vector<std::vector<mat4>> Transforms;//, QuatTransforms;/** Array to return transformations results inside. */
};


//	std::vector<cAnimEvaluator> Animations;// a std::vector that holds each animation

/*
	cBone* Skeleton; // Root node of the internal scene structure 
	std::map<std::string, cBone*> BonesByName;// Name to node map to quickly find nodes by their name 
	std::map<std::string, unsigned int> BonesToIndex;// Name to node map to quickly find nodes by their name 
	std::map<std::string, uint32_t> AnimationNameToId;// find animations quickly
	std::vector<cBone*> Bones;// DO NOT DELETE THESE when the destructor runs... THEY ARE JUST COPIES!!
	std::vector<mat4> Transforms;// temp array of transfrorms
*/

// there is some type of alignment issue with my mat4 and the aimatrix4x4 class, so the copy must be manually
void TransformMatrix(mat4& out, const aiMatrix4x4& in)
{
	out._11=in.a1;
	out._12=in.a2;
	out._13=in.a3;
	out._14=in.a4;

	out._21=in.b1;
	out._22=in.b2;
	out._23=in.b3;
	out._24=in.b4;

	out._31=in.c1;
	out._32=in.c2;
	out._33=in.c3;
	out._34=in.c4;

	out._41=in.d1;
	out._42=in.d2;
	out._43=in.d3;
	out._44=in.d4;
}

class cBone
{
public:

	std::string Name;
	mat4 Offset, LocalTransform, GlobalTransform, OriginalLocalTransform;
	cBone* Parent;
	std::vector<cBone*> Children;

	cBone() :Parent(0){}
	~cBone(){ for(size_t i(0); i< Children.size(); i++) delete Children[i]; }
};

// ------------------------------------------------------------------------------------------------
// Recursively creates an internal node structure matching the current scene and animation.
cBone* SceneAnimator::CreateBoneTree( aiNode* pNode, cBone* pParent)
{
	cBone* internalNode = new cBone();// create a node
	internalNode->Name = pNode->mName.data;// get the name of the bone
	internalNode->Parent = pParent; //set the parent, in the case this is the root node, it will be null

	BonesByName[internalNode->Name] = internalNode;// use the name as a key
	TransformMatrix(internalNode->LocalTransform, pNode->mTransformation);
	internalNode->LocalTransform.Transpose();
	internalNode->OriginalLocalTransform = internalNode->LocalTransform;// a copy saved
	CalculateBoneToWorldTransform(internalNode);

	// continue for all child nodes and assign the created internal nodes as our children
	// recursivly call this function on all children
	for( unsigned int a = 0; a < pNode->mNumChildren; a++)
	{
		internalNode->Children.push_back(CreateBoneTree( pNode->mChildren[a], internalNode));
	}
	return internalNode;
}


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
				mat4 rotationmat =  Bones[a]->Offset * Bones[a]->GlobalTransform;
				trans.push_back(rotationmat);
			}
		}
	}
	//OUTPUT_DEBUG_MSG("Finished loading animations with "<<Bones.size()<<" bones");
}


// ------------------------------------------------------------------------------------------------
// Calculates the node transformations for the scene. 
void SceneAnimator::Calculate(float pTime)
{
	//if( (CurrentAnimIndex < 0) | (CurrentAnimIndex >= Animations.size()) ) return;// invalid animation
	
	Animations[CurrentAnimIndex].Evaluate( pTime, BonesByName);
	UpdateTransforms(Skeleton);
}


/*
	Prune?
*/
void SceneAnimator::ExtractAnimations(const aiScene* pScene)
{
	for(size_t i(0); i< pScene->mNumAnimations; i++)
	{
		Animations.push_back(cAnimEvaluator(pScene->mAnimations[i]) );// add the animations
	}
	for(uint32_t i(0); i< Animations.size(); i++)
	{// get all the animation names so I can reference them by name and get the correct id
		AnimationNameToId.insert(std::map<std::string, uint32_t>::value_type(Animations[i].Name, i));
	}
	CurrentAnimIndex=0;
	SetAnimation("Idle");
}

bool SceneAnimator::SetAnimation(const std::string& name)
{
	std::map<std::string, uint32_t>::iterator itr = AnimationNameToId.find(name);
	int32_t oldindex = CurrentAnimIndex;
	if(itr !=AnimationNameToId.end()) CurrentAnimIndex = itr->second;
	return oldindex != CurrentAnimIndex;
}
