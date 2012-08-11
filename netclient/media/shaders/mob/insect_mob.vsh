#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

// Attributes
//attribute vec3 InVertex;
attribute vec2 InTexCoord;

varying float diffuse;

// Varying
varying vec2 texCoord;

const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));

void main(void) 
{                      
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    texCoord = InTexCoord;

	//diffuse = abs(dot(normal, light_dir ));
    diffuse = 0.5;
}
