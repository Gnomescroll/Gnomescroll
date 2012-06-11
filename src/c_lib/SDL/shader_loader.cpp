#include "shader_loader.hpp"

#include <common/compat_gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *textFileRead(char *fn)
{
    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fn != NULL)
    {
        fp = fopen(fn,"rt");
        if (fp == NULL)
        {
            printf("Error: cannot open file %s \n", fn);
            return NULL;
        }

        if (fp != NULL)
        {

          fseek(fp, 0, SEEK_END);
          count = ftell(fp);
          rewind(fp);

            if (count > 0)
            {
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
void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s",infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s",infoLog);
        free(infoLog);
    }
}

// loads a vertex and fragment shader into a program
void load_shaders(char *vert, char* frag, GLuint* prog)
{
    GLuint v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    GLuint f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char* vs = textFileRead(vert);
    char* fs = textFileRead(frag);

    glShaderSourceARB(v, 1, (const char**)&vs, NULL);
    glShaderSourceARB(f, 1, (const char**)&fs, NULL);

    free(vs);
    free(fs);

    glCompileShaderARB(v); printShaderInfoLog(v); ///diag
    glCompileShaderARB(f); printShaderInfoLog(f); ///diag

    GLuint p;
    p = glCreateProgramObjectARB();

    glAttachObjectARB(p,v);
    glAttachObjectARB(p,f);

    glLinkProgramARB(p);
    printProgramInfoLog(p); // print diagonostic information
    *prog = p;
}

//returns true if error
bool shader_error_check(int shader) 
{
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        printf("Shader compilation failed! \n");
        return true;
    }

    glGetProgramiv(shader, GL_LINK_STATUS, &status);
    if(status != GL_TRUE)
    {
        printf("Shader linking failed! \n");
        return true;
     }

  return false;
}