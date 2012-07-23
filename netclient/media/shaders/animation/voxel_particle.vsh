#version 120

uniform vec3 InLook;

attribute vec3 InNormal;

attribute vec2 InTexCoord;
varying vec2 texCoord;

varying float lightIntensity;

void main(void) 
{                      
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    texCoord = InTexCoord;
        
    lightIntensity = dot(InLook, InNormal);
}

