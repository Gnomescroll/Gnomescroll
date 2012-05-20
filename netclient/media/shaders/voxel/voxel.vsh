#version 120

uniform mat3 InRotationMatrix;
uniform vec3 InTranslation;

attribute vec3 InNormal;
attribute vec4 InAO;
attribute vec2 InTex;

varying float diffuse;

varying mat2 AOMatrix;
varying vec2 texCoord;

const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));

//float rand(vec2 co){
    //return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
//}

//varying float fogFragDepth;

void main(void) 
{
    gl_FrontColor = gl_Color;
    vec4 pos = vec4(0.0, 0.0, 0.0, 1.0);
    pos.xyz = (InRotationMatrix*gl_Vertex.xyz) + InTranslation;
    gl_Position = gl_ModelViewProjectionMatrix * pos;


    //fogFragDepth = distance(vertex.xyz, gl_ModelViewMatrixInverse[3].xyz);

    /* apply rotation matrix to normal */
    vec3 normal = gl_NormalMatrix*(InRotationMatrix*InNormal);


    //const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));

    diffuse = abs(dot(normal, light_dir ));
    texCoord = InTex;
    AOMatrix = mat2( InAO[0], InAO[1], InAO[2], InAO[3] );
}

