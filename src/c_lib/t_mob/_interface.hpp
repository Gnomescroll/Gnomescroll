#pragma once

#include <physics/mat4.hpp>

#undef __cplusplus
extern "C"
{
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h> //defines for postprocessor
#include <assimp/config.h>
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
ASSIMP_API const C_STRUCT aiScene* aiImportFileFromMemory( 
	const char* pBuffer,
	unsigned int pLength,
	unsigned int pFlags,
	const char* pHint);
*/

//unsigned int mNumMeshes;
//unsigned int* mMeshes;	<---index into aiScene mesh array!


//struct aiScene
//C_STRUCT aiMesh** mMeshes;

class BoneTree
{
	public:
	BoneTree() {}


	int nlm;  		//node list max
	aiNode** nl; 	//node list
	int nli; 		//node list index

	aiMesh** ml;	//mesh list, the mesh for node i
	aiScene* pScene;

	struct _Vertex
	{
		struct Vec3 v;
		float ux,uy;
	};
/*
	struct _VertexWeight
	{
		int index;
		float weight;
	};
*/

	int vli;				//vertex list index
	int vlm; 				//vertex list max
	//struct _Vertex* vl;		//vertex list
	struct _Vertex* tvl;	//temporary vertex list, for drawing
	//struct _VertexWeight* vwl;	//vertex weight list

	int* vll;			//offset of vertices in list for each mesth
	int* vln;			//number of vertices in each mech

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
		set_node_parents(pScene->mRootNode, 0);

		count_vertices();
		//vl = new _Vertex[vlm];
		tvl = new _Vertex[vlm];
		//vwl =  new _VertexWeight[vlm];

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

	void set_node_parents(aiNode* pNode, int parent_index)
	{
		int index = -1;
		if(pNode->mNumMeshes != 0)
		{
			//GS_ASSERT(parent_index != -1);
			GS_ASSERT(nli < nlm);
			index = nli;
			nl[nli] = pNode;

			int mesh_index = pNode->mMeshes[0];
			ml[nli] = pScene->mMeshes[mesh_index];

			//npl[nli] = parent_index; 
			nli++;
		}
		for(unsigned int i=0; i < pNode->mNumChildren; i++)
		{
			set_node_parents(pNode->mChildren[i], index);
		}
	}

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
				ml[i] = mesh;
				//vli += mesh->mNumVertices;
				vli += 3*mesh->mNumFaces;
			}
		}
		vlm = vli;
	}

	void set_vertices()
	{
		GS_ASSERT(nli == nlm);

		//printf("nlm= %d nli= %d \n", nlm, nli);

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
		if(count != vli || count != vlm)
		{
			printf("Set_vertices Warning: vertex count= %d vli= %d vlm= %d \n", count, vli, vlm);
		}
	}

	int bvlm;				//base vertex list max;
	struct _Vertex* bvl;	//base vertex list
	struct _Vertex* tbvl;	//base vertex list for matrix transforms/drawing

	int* bvlo;				//offset for vertices in base list
	int* bvln;				//number of vertices in base vertex list

	int bvllm;				//max index for vertex lookup table
	int* bvll;				//base vertex lookup
	
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

			bvlo[i] = vcount;				//base vertex offset
			bvln[i] = mesh->mNumVertices;	//number of base vertex for mesh

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


	//int vlm; 			//vertex list max
	//struct _Vertex* vl;	//vertex list
	//struct _Vertex* tvl; //temporary vertex list, for drawing
	//int vli;			//vertex list index
	//int* vll;			//vertex list loop
	//int* vln;			//number of vertices in mesh

	static void _ConvertMatrix(struct Mat4& out, const aiMatrix4x4& in)
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



	int bam;		//bone array max
	aiBone** ba; 	//bone array
	int* bal;		//bone array lookup: maps mesh to starting offset in ball
	int* ball;		// ball[bal[mesh_num] + bone_num] is matrix index

	struct Mat4* bbma;		//bone base matrix array
	struct Mat4* bma;		//bone matrix array


	void count_bones(int* count, aiNode* pNode)
	{
		GS_ASSERT(pNode->mNumMeshes == 0);	//RECOVER FROM THIS
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

		int bone_count = 0;
		count_bones(&bone_count, pNode);

		printf("DAE LOADER: %d bones \n", bone_count);


		bam = bone_count;
	}	


	void init_bone_list()
	{
		//count bones
		int bone_count = 0;
		int _bone_count = 0;

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

		printf("bone_count= %d _bone_count= %d nli= %d \n", bone_count, _bone_count, nli);

		printf("stcmp: %d \n", strcmp(ml[1]->mBones[0]->mName.data, ml[2]->mBones[0]->mName.data) );

		count_bones();
	}


	unsigned int texture1;
	SDL_Surface* s;

	void init_texture()
	{
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

	void draw(float x, float y, float z)
	{
		//printf("nlm= %d vlm= %d \n", nlm, vlm);

		for(int i=0; i<bvlm; i++)
		{
			tbvl[i].ux = bvl[i].ux;
			tbvl[i].uy = bvl[i].uy;

			tbvl[i].v.x = 0.0;	// 0.0f + x
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

			for(unsigned int j=0; j<mesh->mNumBones; j++)
			{
				aiBone* bone = mesh->mBones[j];
				aiMatrix4x4 offset_matrix = bone->mOffsetMatrix;

				struct Mat4 mat;
				_ConvertMatrix(mat, offset_matrix);
				//printf("%d === \n", i);
				//print_mat4(mat);

				for(unsigned int k=0; k<bone->mNumWeights; k++)
				{
					int index = offset + bone->mWeights[k].mVertexId;
					float weight = bone->mWeights[k].mWeight;

					//printf("weight= %f \n", weight);

					GS_ASSERT(index >= offset);
					GS_ASSERT(index < offset+num);
					if(j==0) count++;

				#if 0
					//SIMD version
					Vec3 v = vec3_mat3_apply(vl[index].v, mat);
					v = vec3_scalar_mult(v, weight);
					tvl[index] = vec3_add(tvl[index].v, v);
				#else
					Vec3 v = vec3_mat3_apply(bvl[index].v, mat);
					tbvl[index].v.x += weight*v.x;
					tbvl[index].v.y += weight*v.y;
					tbvl[index].v.z += weight*v.z;
				#endif

					//unsigned int mNumWeights; //number of vertices affected by this bone
					//C_STRUCT aiVertexWeight* mWeights; //The vertices affected by this bone

					//struct aiVertexWeight 
					//unsigned int mVertexId; //! Index of the vertex which is influenced by the bone.
					//! The strength of the influence in the range (0...1).
					//! The influence from all bones at one vertex amounts to 1.
					//float mWeight;

				}
			}
		}
	/*
		for(int i=0; i<vlm; i++)
		{
			int index = bvll[i];
			if(index < 0 || index >= bvlm)
			{
				printf("%d: index= %d \n", i, index);
			}
			tvl[i] = tbvl[index];
		}
	*/
		for(int i=0; i<vlm; i++)
		{
			int index = bvll[i];
			tvl[i] = tbvl[index];
			tvl[i].v.x += x;
			tvl[i].v.y += y;
			tvl[i].v.z += z;
		}

		glColor4ub(255,255,255,255);
		glEnable(GL_TEXTURE_2D);
		//GL_ASSERT(GL_TEXTURE_2D, true);

		glBindTexture(GL_TEXTURE_2D, texture1);
#if 1
		glBegin(GL_TRIANGLES);
		for(int i=0; i<vlm; i++)
		{

			//if(i >= vll[1]) continue;

			struct _Vertex v = tvl[i];

			//vec3_print(v.v);
        	glTexCoord2f(v.ux, v.uy );
			glVertex3f(v.v.x, v.v.y, v.v.z);
		}

		glEnd();
		
		glBindTexture(GL_TEXTURE_2D, 0);
		check_gl_error();

#else


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
		//printf("count: %d vlm= %d \n", count, vlm);
		//mesh->mTextureCoords[0]

	}

};

void PrintBoneTree(const aiScene* pScene, int num, aiNode* pNode)
{
	printf("aiNode: %02d pNode name= %s \n", num, pNode->mName.data);

	for(unsigned int i=0; i < pNode->mNumMeshes; i++)
	{
		unsigned int index = pNode->mMeshes[i];
		printf("\tMesh: %02d index %02d: name= %s vertices= %d faces= %d \n", i, index, 
			pScene->mMeshes[index]->mName.data,
			pScene->mMeshes[index]->mNumVertices,
			pScene->mMeshes[index]->mNumFaces);

		aiMesh* mesh = pScene->mMeshes[index];
		for(unsigned int j=0; j < mesh->mNumBones; j++)
		{
			//aiBone* bone = mesh->mBones[j];

			printf("\t\tBone %02d: %s affects %d vertices \n", j, 
				mesh->mBones[j]->mName.data,
				mesh->mBones[j]->mNumWeights
				);
			//C_STRUCT aiVertexWeight* mWeights; //The vertices affected by this bone
			//C_STRUCT aiMatrix4x4 mOffsetMatrix; //! Matrix that transforms from mesh space to bone space in bind pose
		
			for(unsigned int k=0; k < mesh->mBones[j]->mNumWeights; k++)
			{
				//aiVertexWeight* vertex_weight = bone->mWeights[k];
				//printf("vertex index: %d weight: %f \n", vertex_weight->)

				//bone->mWeights[k].mVertexId;
				//bone->mWeights[k].mWeight;

				//struct aiVertexWeight 
				//unsigned int mVertexId; //! Index of the vertex which is influenced by the bone.
				//! The strength of the influence in the range (0...1).
				//! The influence from all bones at one vertex amounts to 1.
				//float mWeight;
			}
		}
	}

	for(unsigned int i=0; i < pNode->mNumChildren; i++)
	{
		PrintBoneTree(pScene, num+1, pNode->mChildren[i]);
	}
}



/*
	Documentation on removal step
	http://assimp.sourceforge.net/lib_html/config_8h.html#a97ac2ef7a3967402a223f3da2640b2b3

	aiComponent_TANGENTS_AND_BITANGENTS
	aiComponent_COLORS
	aiComponent_LIGHTS 
	aiComponent_CAMERAS 
	aiComponent_TEXTURES //remove embedded textures
	aiComponent_MATERIALS 
*/

/*
	Flag reference
	http://assimp.sourceforge.net/lib_html/postprocess_8h.html

	aiProcess_ImproveCacheLocality  Reorders triangles for better vertex cache locality.
	aiProcess_ValidateDataStructure 

	aiProcess_JoinIdenticalVertices  
	If this flag is not specified, no vertices are referenced by more than one face and no index buffer is required for rendering.

	Add cache locality and join identical values even if not using index buffers

	aiProcess_TransformUVCoords 

*/

	class BoneTree* bt;

void init()
{
	int bsize;
	//char* buffer = read_file_to_buffer( (char*) "media/mesh/collada_test.dae", &bsize);
	//char* buffer = read_file_to_buffer( (char*) "media/mesh/3d_max_test.3ds", &bsize);
	
	char* buffer = read_file_to_buffer( (char*) "media/mesh/player.dae", &bsize);
	//char* buffer = read_file_to_buffer( (char*) "media/mesh/test4.dae", &bsize);


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


	if(pScene == NULL)
	{
		printf("Assimp Scene Load Error: %s \n", aiGetErrorString() );

		abort();
	}

	printf("BT: start bone tree: \n");
	//BoneTree bt;
	bt = new BoneTree;
	bt->init( (aiScene*) pScene);
	printf("BT: bone tree finished\n");

	printf("Bone tree: \n");
	return;
	PrintBoneTree(pScene, 0, pScene->mRootNode);	//these are actually meshes
	//pScene->mRootNode
	return; //debug

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
			node_anim->mNumPositionKeys,	//aiVectorKey* mPositionKeys;
			node_anim->mNumRotationKeys		//aiQuatKey* mRotationKeys;
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
	printf("Succes\n");

	exit(0);
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
	if(ClientState::location_pointer_set == false)
		return;

	Vec3 loc = ClientState::location_pointer;
	bt->draw(loc.x, loc.y, loc.z+ 1.0);
}


void teardown()
{
	delete bt;
}

}