#version 120

uniform vec3 InCameraPos;
uniform mat4 InRotMatrix;

attribute vec3 InNormal;
attribute vec3 InColor;

const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));   //fixed light source?

varying float diffuse_light;
varying vec3 base_color;

void main(void) 
{
    vec4 pos = gl_ModelViewProjectionMatrix * InRotMatrix * gl_Vertex;

    // set light diffusion
    vec3 look = normalize(pos.xyz - InCameraPos);
    diffuse_light = abs(dot(look, InNormal));           //use camera position

    // set position
    gl_Position = pos;

    // set color
    base_color = InColor;
}

