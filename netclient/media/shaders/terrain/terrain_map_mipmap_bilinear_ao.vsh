#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#extension GL_EXT_texture_array : enable

/*
Attributes
*/

attribute vec4 InVertex;
attribute vec3 InTexCoord;
attribute vec3 InRGB;
attribute vec2 InLight;

//attribute int InNormal;

attribute vec4 InLightMatrix; //intensity for AO at each corner of quad

/*
Uniform
*/

uniform vec4 InOffset;

/*
Varying
*/

#ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix;
    flat varying float skyLight;
    flat varying float playerLight;
#else
    varying mat2 lightMatrix;
    varying float skyLight;
    varying float playerLight;
#endif

varying vec3 texCoord;
varying vec3 inColor;
varying float fogFragDepth;


void main(void) 
{
    vec4 v = vec4(InVertex);
    v.xy += InOffset.xy;
    gl_Position = gl_ModelViewProjectionMatrix * v;

    fogFragDepth = distance(v.xy, gl_ModelViewMatrixInverse[3].xy);

    inColor = InRGB;

    texCoord = InTexCoord;

    lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );
    skyLight = InLight.x;
    playerLight = InLight.y;
}
