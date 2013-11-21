#include "obj_loader.hpp"

//#include <stdlib.h>
//#include <stdio.h>
#include <memory.h>

//#include "obj.h"

namespace obj_load
{

ObjModel* ObjLoadModel(char* memory, size_t size)
{
    char* p = NULL;
    char* e = NULL;
    ObjModel* ret = (ObjModel*) calloc(1, sizeof(ObjModel));
    memset(ret, 0, sizeof(ObjModel));

    p = memory;
    e = memory + size;

    while (p != e)
    {
              if (memcmp(p, "vn", 2) == 0) ret->nNormal++;
        else if (memcmp(p, "vt", 2) == 0) ret->nTexCoord++;
        else if (memcmp(p, "v",  1) == 0) ret->nVertex++;
        else if (memcmp(p, "f",  1) == 0) ret->nTriangle++;

        while (*p++ != (char) 0x0A);
    }

    ret->VertexArray   = (ObjVertex*)   malloc(sizeof(ObjVertex) * ret->nVertex);
    ret->NormalArray   = (ObjNormal*)   malloc(sizeof(ObjNormal) * ret->nNormal);
    ret->TexCoordArray = (ObjTexCoord*) malloc(sizeof(ObjTexCoord) * ret->nTexCoord);
    ret->TriangleArray = (ObjTriangle*) malloc(sizeof(ObjTriangle) * ret->nTriangle);

    p = memory;

   int nV = 0, nN = 0, nT = 0, nF = 0;

    while (p != e)
    {
        if (memcmp(p, "vn", 2) == 0)
        {
            sscanf(p, "vn %f %f %f", &ret->NormalArray[nN].x,
                                  &ret->NormalArray[nN].y,
                                  &ret->NormalArray[nN].z);
            nN++;
        }
        else if (memcmp(p, "vt", 2) == 0)
        {
            sscanf(p, "vt %f %f", &ret->TexCoordArray[nT].u,
                                  &ret->TexCoordArray[nT].v);
            nT++;
        }
        else if (memcmp(p, "v", 1) == 0) /* or *p == 'v' */
        {
            sscanf(p, "v %f %f %f", &ret->VertexArray[nV].x,
                                    &ret->VertexArray[nV].y,
                                    &ret->VertexArray[nV].z);
            nV++;
        }
        else if (memcmp(p, "f", 1) == 0) /* or *p == 'f' */
        {
            sscanf(p, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &ret->TriangleArray[nF].Vertex[0],
                &ret->TriangleArray[nF].TexCoord[0],
                &ret->TriangleArray[nF].Normal[0],
                &ret->TriangleArray[nF].Vertex[1],
                &ret->TriangleArray[nF].TexCoord[1],
                &ret->TriangleArray[nF].Normal[1],
                &ret->TriangleArray[nF].Vertex[2],
                &ret->TriangleArray[nF].TexCoord[2],
                &ret->TriangleArray[nF].Normal[2]);

            /*
            sscanf(p, "f %d/%d %d/%d %d/%d",
                &ret->TriangleArray[nF].Vertex[0],
                &ret->TriangleArray[nF].TexCoord[0],

                &ret->TriangleArray[nF].Vertex[1],
                &ret->TriangleArray[nF].TexCoord[1],

                &ret->TriangleArray[nF].Vertex[2],
                &ret->TriangleArray[nF].TexCoord[2]
            );
            */
            nF++;
        }

        while (*p++ != (char) 0x0A);
    }

    return ret;
}

size_t ObjLoadFile(char* szFileName, char** memory)
{
    size_t bytes = 0;
    FILE* file = fopen(szFileName, "rt");

    if (file == NULL) return bytes;

    fseek(file, 0, SEEK_END);
    size_t end = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = NULL;
    if (end > 0)
    {
        buffer = (char*) calloc(end+1, sizeof(char));
        bytes = fread(buffer, sizeof(char), end, file);
    }

    *memory = buffer;

    fclose(file);

    return bytes;
}

}
