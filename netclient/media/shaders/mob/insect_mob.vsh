#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

// Attributes
//attribute vec3 InVertex;
attribute vec2 InTexCoord;

// Varying
varying vec2 texCoord;

void main(void) 
{                      
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
        texCoord = InTexCoord;

}
