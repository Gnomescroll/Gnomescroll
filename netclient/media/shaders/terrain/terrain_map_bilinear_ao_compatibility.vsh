#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix; 
#else
    varying mat2 lightMatrix; 
#endif


/*
Attributes
*/

attribute vec4 InVertex;
attribute vec2 InTexCoord;
attribute vec2 InTexCoord2;
attribute vec3 InRGB;
attribute vec2 InLight;

//attribute int InNormal;

attribute vec4 InLightMatrix; //intensity for AO at each corner of quad

/*
Uniform
*/

//uniform vec3 ChunkPosition;
//uniform vec3 NormalArray[6];

/*
Varying
*/

varying vec2 texCoord;
varying vec2 texCoord2;
varying vec3 inColor;

varying float skyLight;
varying float playerLight;

void main(void) 
{              
    //InVertex.w = 1.0;
    gl_Position = gl_ModelViewProjectionMatrix * InVertex;

    inColor = InRGB;
 
    texCoord = InTexCoord.xy;
    texCoord2 = InTexCoord2.xy;

    lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );

    skyLight = InLight[0];
    playerLight = InLight[1];
}

    /* (InTexCoord.xy - vec2(0.5,0.5)) * .94 */
