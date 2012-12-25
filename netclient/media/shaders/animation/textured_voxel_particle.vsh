#version 120

//uniform vec3 InCameraPos;

attribute vec3 InNormal;

attribute vec2 InTexCoord;
varying vec2 texCoord;

varying float diffuse_light;

const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));   //fixed light source?

void main(void) 
{                      
    vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;
    //vec3 look = normalize(pos.xyz - InCameraPos);


    //Light 2
    diffuse_light = dot(light_dir, InNormal);

/*
    //Light 1
    vec3 look = normalize(pos.xyz - InCameraPos);
    diffuse_light = abs(dot(look, InNormal));           //use camera position
*/

    //diffuse_light = dot(look, InNormal);
    
    //diffuse_light = abs(dot(look, InNormal));           //use camera position
    //diffuse_light = abs(dot(light_dir, InNormal));    //use fixed light source

    gl_Position = pos;
    texCoord = InTexCoord;
}
