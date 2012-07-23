#version 120

uniform vec3 InLook;

attribute vec3 InNormal;

attribute vec2 InTexCoord;
varying vec2 texCoord;

varying float lightIntensity;

void main(void) 
{                      
    vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;
    vec3 look = normalize(pos.xyz - InLook);
    lightIntensity = dot(look, InNormal);

    gl_Position = pos;
    texCoord = InTexCoord;
}

