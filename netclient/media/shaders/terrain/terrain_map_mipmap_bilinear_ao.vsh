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

#ifdef GL_EXT_gpu_shader4
    flat varying vec4 _lightMatrix;
#else
    varying vec4 _lightMatrix;
#endif


//flat varying mat2 lightMatrix;
 
varying vec3 inColor;

varying float fogFragDepth;
//varying float fogFragZ;




void main(void) 
{                      
    //vec4 vertex = vec4(InVertex+ChunkPosition, 1.0);
    //gl_Position = gl_ModelViewProjectionMatrix * vec4(InVertex, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * InVertex;

    //fogFragDepth = distance(vertex.xyz, gl_ModelViewMatrixInverse[3].xyz);
    fogFragDepth = distance(InVertex.xy, gl_ModelViewMatrixInverse[3].xy);

    inColor = InRGB;

    texCoord = InTexCoord;

    //lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );

    _lightMatrix = InLightMatrix;
}


/*
//const float WORLD_RADIUS        = 2500.0;
//const float WORLD_RADIUS_SQUARED = 6250000.0;

//const float WORLD_RADIUS        = 512.0;
//const float WORLD_RADIUS_SQUARED = 262144.0;

const float WORLD_RADIUS        = 1024.0;
const float WORLD_RADIUS_SQUARED = 1024.0*1024.0;
const float ONE_OVER_WORLD_RADIUS_SQUARED = 1.0/(1024.0*1024.0);

//const float WORLD_RADIUS        = 128.0;
//const float WORLD_RADIUS_SQUARED = 16384.0;

void main(void) 
{      
        vec4 vertex = vec4(InVertex+ChunkPosition, 1.0);


        //vec3 d = vertex.xyz - gl_ModelViewMatrixInverse[3].xyz;
        //float distanceSquared = dot(d,d);
        //float distanceSquared = vertex.x * vertex.x + vertex.y * vertex.y;

        float distance = distance(vertex.xyz, gl_ModelViewMatrixInverse[3].xyz);
        float distanceSquared = ONE_OVER_WORLD_RADIUS_SQUARED* distance*distance;
               
        //vertex.z -= WORLD_RADIUS - sqrt(max( (1.0 - distanceSquared), 0.0)) * WORLD_RADIUS;
        vertex.z -= WORLD_RADIUS - sqrt( 1.0 - distanceSquared);

        gl_Position = gl_ModelViewProjectionMatrix * vertex;

        fogFragDepth = distance;

        inColor = InRGB.rgb;
 
        texCoord = InTexCoord;

        lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );
}
*/