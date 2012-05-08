#pragma once


//#include <stdlib.h>
//#include <stdio.h>
//#include "obj.h"

namespace obj_load
{


struct ObjVertex
{
   float x, y, z;
};

typedef ObjVertex ObjNormal;

struct ObjTexCoord
{
   float u, v;
};

struct ObjTriangle
{
   int Vertex[3];
   int Normal[3];
   int TexCoord[3];
};

struct ObjModel
{
   int nVertex, nNormal, nTexCoord, nTriangle;

   ObjVertex* VertexArray;
   ObjNormal* NormalArray;
   ObjTexCoord* TexCoordArray;
   ObjTriangle* TriangleArray;
};

struct Vertex
{
	float x,y,z;
	float u,v;
	float n[3];
};

ObjModel* ObjLoadModel(char*, size_t);
size_t    ObjLoadFile(char*, char**);


ObjModel* m_model = NULL;

struct Vertex* v_array = NULL;
int v_num = 0;

void load_model()
{
   char* memory = NULL;
   size_t bytes = ObjLoadFile((char*) "media/mesh/mob1.obj", &memory);

   ObjModel* model = ObjLoadModel(memory, bytes);

   m_model = model;

   printf("Object Model: nTriangle= %d \n", model->nTriangle);
   printf("Object Model: nVertex= %d \n", model->nVertex);
   printf("Object Model: nTexCoord= %d \n", model->nTexCoord);
   printf("Object Model: nNormal= %d \n", model->nNormal);
 
 	v_array = new Vertex[model->nTriangle];

 	for(int i=0; i<model->nTriangle; i++)
 	{

 		for(int j=0; j<3; j++)
 		{
	 		struct Vertex v;

	 		int iv = model->TriangleArray[i].Vertex[j];
	 		int itx = model->TriangleArray[i].TexCoord[j];

	 		printf("triangle: %i,  iv= %i itx= %i \n", i, iv, itx);
	 		v.x = model->VertexArray[iv].x;
	 		v.y = model->VertexArray[iv].y;
	 		v.z = model->VertexArray[iv].z;
	 		v.u = model->TexCoordArray[itx].u;
	 		v.v = model->TexCoordArray[itx].v;
	 		////v.n[0] = 
	 		//v.n[1] = 
	 		//v.n[2] =

	 		v_array[i] = v;
 		}
 	}
}

void init_shader()
{


}

void init_texture()
{

}

void init_draw_model()
{
	init_shader();
	init_texture();
}


void draw_model(float x, float y, float z)
{



}

void free_model(struct ObjModel* model)
{
   free(model->NormalArray);
   free(model->TexCoordArray);
   free(model->TriangleArray);
   free(model->VertexArray);
   free(model);
}


}