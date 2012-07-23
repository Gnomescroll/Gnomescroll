#version 120

uniform vec3 InCameraPos;

attribute vec3 InNormal;

attribute vec2 InTexCoord;
varying vec2 texCoord;

varying float diffuse_light;

void main(void) 
{                      
    vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;
    vec3 look = normalize(pos.xyz - InCameraPos);


    //diffuse_light = dot(look, InNormal);
    diffuse_light = abs(dot(look, InNormal));

    gl_Position = pos;
    texCoord = InTexCoord;
}

