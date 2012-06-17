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

attribute vec3 InCE1;
attribute vec3 InCE2;
attribute vec3 InCE3;
attribute vec3 InCE4;


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
//varying float fogFragZ;


const float WORLD_RADIUS        = 512.0;
const float WORLD_RADIUS_SQUARED = 262144.0;

void main(void) 
{      



        vec4 vertex = vec4(InVertex+ChunkPosition, 1.0);


        vec3 d = vertex.xyz - gl_ModelViewMatrixInverse[3].xyz;
        float distanceSquared = dot(d,d);
        //float distanceSquared = vertex.x * vertex.x + vertex.y * vertex.y;
               
        vertex.z -= WORLD_RADIUS - sqrt(max(1.0 - distanceSquared / WORLD_RADIUS_SQUARED, 0.0)) * WORLD_RADIUS;


        gl_Position = gl_ModelViewProjectionMatrix * vertex;

        fogFragDepth = distance(vertex.xyz, gl_ModelViewMatrixInverse[3].xyz);

        inColor = InRGB.rgb;
 
        texCoord = InTexCoord;

        lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );

}


/*
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
        //fogFragZ = gl_ModelViewMatrixInverse[3].z - vertex.z;

        inColor = InRGB.rgb;
 
        texCoord = InTexCoord;

        lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );

}
*/