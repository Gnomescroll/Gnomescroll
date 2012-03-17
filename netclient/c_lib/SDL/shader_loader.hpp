#pragma once

char *textFileRead(char *fn);

//info log print
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

void load_shaders(char *vert, char* frag, GLuint* prog);
