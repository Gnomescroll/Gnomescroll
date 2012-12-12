#version 120

uniform vec3 InCameraPos;
uniform vec3 InPos;
uniform mat3 InRotMatrix;

attribute vec2 InXY;
attribute vec3 InColor;

const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));   //fixed light source?

varying float diffuse_light;
varying vec3 base_color;

void main(void) 
{
    vec3 base_pos = vec3(InXY) + InPos;
    // TODO -- what is gl_Vertex bound to again?
    base_pos =  InRotMatrix * vec3(0,0,1);  // ????
    
    vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;

    // set light diffusion
    vec3 look = normalize(pos.xyz - InCameraPos);
    diffuse_light = abs(dot(look, InNormal));           //use camera position

    // set color
    base_color = InColor;

    gl_Position = pos;
}

