#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#extension GL_EXT_texture_array : enable

/*
Attributes
*/

attribute vec3 InVertex;
attribute vec3 InTexCoord;
attribute vec3 InRGB;

//attribute int InNormal;

attribute vec4 InLightMatrix; //intensity for AO at each corner of quad

/*
Uniform
*/

uniform vec3 ChunkPosition;
uniform vec3 NormalArray[6];

/*
Varying
*/
varying vec3 texCoord;

# ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix;
#else
    varying mat2 lightMatrix;
#endif

//flat varying mat2 lightMatrix;
 
varying vec3 inColor;

varying float fogFragDepth;

void main(void) 
{                      
        //vec3 Normal = NormalArray[inColor[4]*255];

        ///vec4 eyePos = gl_ModelViewMatrix * gl_Vertex;
        //gl_FogFragCoord = abs(eyePos.z/eyePos.w);

        vec4 vertex = vec4(InVertex+ChunkPosition, 1.0);
        gl_Position = gl_ModelViewProjectionMatrix * vertex;

        //vec4 eyePos = gl_ModelViewMatrix * vertex;
        //fogFragDepth = abs(eyePos.z/eyePos.w);

        //fogFragDepth = length(gl_Position.xyz);
        fogFragDepth = distance(vertex.xyz, gl_ModelViewMatrixInverse[3].xyz);

        inColor = InRGB.rgb;
 
        texCoord = InTexCoord;

        lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );
 
}

//texCoord = vec3(InTexCoord[0], InTexCoord[1], 0.0);

/*
attribute vec3 InTexCoord0;
attribute vec4 InLightMatrix0;

//varying float intensity;

varying vec3 texCoord;
varying mat2 lightMatrix0;

varying vec4 inColor;

void main(void) 
{			

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	inColor = gl_Color;

	texCoord = InTexCoord0;

	lightMatrix0 = mat2(InLightMatrix0[0], InLightMatrix0[1], InLightMatrix0[2],InLightMatrix0[3] );

}
*/
