#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

/*
Attributes
*/

attribute vec4 InVertex;
attribute vec4 InTexCoord;
attribute vec3 InRGB;

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

varying mat2 lightMatrix; 

varying vec2 texCoord;
varying vec2 texCoord3;

varying vec3 inColor;

void main(void) 
{              
    //InVertex.w = 1.0;
    gl_Position = gl_ModelViewProjectionMatrix * InVertex;

    inColor = InRGB;
 
    texCoord = InTexCoord.xy;

    vec2 tmp = (0.96f)*(InTexCoord.xy - vec2(0.5f,0.5f) )+ vec2(0.5f,0.5f);
    texCoord3 = 0.0625f*tmp +InTexCoord.zw;

    lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );
}

    /* (InTexCoord.xy - vec2(0.5,0.5)) * .94 */
