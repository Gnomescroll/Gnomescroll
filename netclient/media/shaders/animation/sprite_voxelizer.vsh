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
    // add the sprite pixel offset to the position, and rotate through the matrix
    // this is our gl_Vertex
    vec4 vert = vec4((vec3(InXY) + InPos) * InRotMatrix);  // multiply V*M becuase M is row major

    // calculate final position
    vec4 pos = gl_ModelViewProjectionMatrix * vert;

    // set light diffusion
    vec3 look = normalize(pos.xyz - InCameraPos);
    diffuse_light = abs(dot(look, InNormal));           //use camera position

    // set position
    gl_Position = pos;

    // set color
    base_color = InColor;
}

