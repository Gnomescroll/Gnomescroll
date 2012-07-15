#pragma once

#include <assimp.h>
#include <aiScene.h>
#include <aiPostProcess.h>

/*
	cBone* Skeleton; // Root node of the internal scene structure 
	std::map<std::string, cBone*> BonesByName;// Name to node map to quickly find nodes by their name 
	std::map<std::string, unsigned int> BonesToIndex;// Name to node map to quickly find nodes by their name 
	std::map<std::string, uint32_t> AnimationNameToId;// find animations quickly
	std::vector<cBone*> Bones;// DO NOT DELETE THESE when the destructor runs... THEY ARE JUST COPIES!!
	std::vector<mat4> Transforms;// temp array of transfrorms
*/


void SceneAnimator::Release()
{	// this should clean everything up 
	CurrentAnimIndex = -1;
	Animations.clear();// clear all animations
	delete Skeleton;// This node will delete all children recursivly
	Skeleton = NULL;// make sure to zero it out
}

void SceneAnimator::Init(const aiScene* pScene){// this will build the skeleton based on the scene passed to it and CLEAR EVERYTHING
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
}
void SceneAnimator::Save(std::ofstream& file){
	// first recursivly save the skeleton
	if(Skeleton)
		SaveSkeleton(file, Skeleton);

	uint32_t nsize = static_cast<uint32_t>(Animations.size());
	file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of animations	
	for(uint32_t i(0); i< nsize; i++){
		Animations[i].Save(file);
	}

	nsize = static_cast<uint32_t>(Bones.size());
	file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of bones

	for(uint32_t i(0); i< Bones.size(); i++){
		nsize = static_cast<uint32_t>(Bones[i]->Name.size());
		file.write(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the size of the bone name
		file.write(Bones[i]->Name.c_str(), nsize);// the name of the bone
	}

}
void SceneAnimator::Load(std::ifstream& file){
	Release();// make sure to clear this before writing new data
	Skeleton = LoadSkeleton(file, NULL);
	uint32_t nsize = 0;
	file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of animations
	Animations.resize(nsize);
	//OUTPUT_DEBUG_MSG("Extracting Animations . . ");
	for(uint32_t i(0); i< nsize; i++){
		Animations[i].Load(file);
	}
	for(uint32_t i(0); i< Animations.size(); i++){// get all the animation names so I can reference them by name and get the correct id
		AnimationNameToId.insert(std::map<std::string, uint32_t>::value_type(Animations[i].Name, i));
	}
	if(Animations.size() >0) CurrentAnimIndex =0;// set it to the first animation if there are any
	char bname[250];
	file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the number of bones
	Bones.resize(nsize);

	for(uint32_t i(0); i< Bones.size(); i++){	
		file.read(reinterpret_cast<char*>(&nsize), sizeof(uint32_t));// the size of the bone name
		file.read(bname, nsize);// the size of the bone name
		bname[nsize]=0;
		std::map<std::string, cBone*>::iterator found = BonesByName.find(bname);
		BonesToIndex[found->first] = i;
		cBone* tep = found->second;
		Bones[i]=tep;
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
	//OUTPUT_DEBUG_MSG("Finished loading animations with "<<Bones.size()<<" bones");
}

