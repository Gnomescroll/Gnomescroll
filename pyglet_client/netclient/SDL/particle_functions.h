#define NO_SDL_GLEXT
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "camera.h"

#include "math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int _init_particle_functions();

int _draw_particle(int id, float size, float x, float y, float z);
int _draw_particle2(int id, float size, float x, float y, float z);

int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1);
int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1);


static inline char *textFileRead(char *fn) {
    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fn != NULL) {
        fp = fopen(fn,"rt");

        if (fp != NULL) {

      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

//info log print
static inline void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
        free(infoLog);
    }
}

static inline void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
        free(infoLog);
    }
}
