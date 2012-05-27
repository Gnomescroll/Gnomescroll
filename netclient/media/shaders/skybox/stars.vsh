#version 120

attribute vec2 InTexCoord;
varying vec2 texCoord;

void main(void) 
{                      
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    texCoord = InTexCoord;
}
