#version 120

uniform sampler2D base_texture;

varying float diffuse_light;
varying vec3 base_color;

#define diffuse_p .15f //diffuse lighting weight
#define ambient_p .85f //ambient lighting weight

void main() 
{
    vec3 color = (diffuse_light*diffuse_p + ambient_p) * base_color;
    //color = pow(base_color, vec3(1.0f / 2.2f));  // gamma correction  TOO BRIGHT
    gl_FragColor.rgb = color;
}
