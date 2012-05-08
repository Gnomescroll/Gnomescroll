#pragma once


//#include <stdlib.h>
//#include <stdio.h>
//#include "obj.h"

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

ObjModel* ObjLoadModel(char*, size_t);
size_t    ObjLoadFile(char*, char**);

void load_model()
{
   char* memory = NULL;
   size_t bytes = ObjLoadFile((char*) "media/mesh/mob1.obj", &memory);

   ObjModel* model = ObjLoadModel(memory, bytes);

   printf("Object Model has: %d faces!\n", model->nTriangle);
   
   free(model->NormalArray);
   free(model->TexCoordArray);
   free(model->TriangleArray);
   free(model->VertexArray);
   free(model);

   //system("PAUSE");
   
   return;
}