#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

uniform vec4 InPosition;
attribute vec2 InTexCoord;

//varying float diffuse;

// Varying
varying vec2 texCoord;

//const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));

varying vec3 normal1;

void main(void) 
{                      
    gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex+InPosition);
    texCoord = InTexCoord;

    //vec3 normal = gl_NormalMatrix * gl_Normal; //needs to rotate?
    //vec3 normal = gl_Normal;
    normal1 = gl_Normal;
    //diffuse = abs(dot(normal, light_dir ));
    //diffuse = 0.5;
}
