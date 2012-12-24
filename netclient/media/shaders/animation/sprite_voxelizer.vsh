#version 120

//uniform vec3 InCameraPos;
uniform mat4 InMatrix;

attribute vec3 InNormal;
attribute vec4 InAOMatrix;
attribute vec2 InAOInterpolate;

const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));   //fixed light source?

varying float diffuse_light;
varying mat2 AOMatrix;
varying vec2 AOInterpolate;

void main(void) 
{
    vec4 pos = gl_ModelViewProjectionMatrix * InMatrix * gl_Vertex;

    // set light diffusion
 

    //lighting -1
    vec3 normal = gl_NormalMatrix*(mat3(InMatrix)*InNormal);
    diffuse_light = abs(dot(light_dir, normal));


/*
    //lighting 0
    vec3 normal = mat3(InMatrix)*InNormal;
    diffuse_light = abs(dot(light_dir, normal));
*/

/*
    //lighting 1
    vec3 look = normalize(pos.xyz - InCameraPos);
    vec3 normal = gl_NormalMatrix*(mat3(InMatrix)*InNormal);
    diffuse_light = abs(dot(look, normal));
*/

/*  
    //lighting 2
    vec3 look = normalize(pos.xyz - InCameraPos);
    diffuse_light = abs(dot(look, (InMatrix * vec4(InNormal, 0.0f)).xyz));           //use camera position
*/

    // ambient occlusion
    AOInterpolate = InAOInterpolate;
    AOMatrix = mat2(InAOMatrix[0], InAOMatrix[1], InAOMatrix[2], InAOMatrix[3]);

    // set position
    gl_Position = pos;

    gl_FrontColor = gl_Color;
}
