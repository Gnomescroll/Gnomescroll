#include "shader_loader.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/compat_gl.h>

char *textFileRead(const char *fn)
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
          count = (int)ftell(fp);
          rewind(fp);

            if (count > 0)
            {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = (int)fread(content,sizeof(char),count,fp);
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
        if (infoLog[0] != '\0')
        {
            printf("%s",infoLog);
            if (charsWritten > 0 && infoLog[charsWritten-1] != '\n')
                printf("\n");
        }
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
        if (infoLog[0] != '\0')
        {
            printf("%s",infoLog);
            if (charsWritten > 0 && infoLog[charsWritten-1] != '\n')
                printf("\n");
        }
        free(infoLog);
    }
}

// loads a vertex and fragment shader into a program
void load_shaders(const char *vert, const char* frag, GLuint* prog)
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

bool shader_compiler_error(int shader)
{
    if (shader == 0)
    {
        printf("shader compiler error: shader creation failed\n");
        return true;
    }
    
    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);  // this function does not assign any value to &status if a shader was not compiled
    glGetError();   // clear error
    if (status == GL_FALSE)
    {
        printf("shader compiler error: Shader compilation failed! \n");
        return true;
    }
    return false;
}

bool shader_linking_error(int program)
{
    if (program <= 0)
    {
        printf("shader linking error: program creation failed\n");
        return true;
    }
    GLint status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    glGetError();   // clear error
    if(status == GL_FALSE)
    {
        printf("shader linking error: Shader linking failed! \n");
        return true;
    }
    return false;
}
