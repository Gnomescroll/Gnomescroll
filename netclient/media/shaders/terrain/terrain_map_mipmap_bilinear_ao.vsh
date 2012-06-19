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

//attribute int InNormal;

attribute vec4 InLightMatrix; //intensity for AO at each corner of quad

//attribute vec3 InCE1;
//attribute vec3 InCE2;
//attribute vec3 InCE3;
//attribute vec3 InCE4;


/*
Uniform
*/

uniform vec3 ChunkPosition;
uniform vec3 NormalArray[6];

/*
Varying
*/
varying vec3 texCoord;

#ifdef GL_EXT_gpu_shader4
    flat varying vec4 lightMatrix;
#else
    varying vec4 lightMatrix;
#endif


//flat varying mat2 lightMatrix;
 
varying vec3 inColor;

varying float fogFragDepth;

void main(void) 
{                      
    //vec4 vertex = vec4(InVertex+ChunkPosition, 1.0);
    //gl_Position = gl_ModelViewProjectionMatrix * vec4(InVertex, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * InVertex;

    //fogFragDepth = distance(vertex.xyz, gl_ModelViewMatrixInverse[3].xyz);
    fogFragDepth = distance(InVertex.xy, gl_ModelViewMatrixInverse[3].xy);

    inColor = InRGB;

    texCoord = InTexCoord;

    lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );

    //_lightMatrix = InLightMatrix;
}