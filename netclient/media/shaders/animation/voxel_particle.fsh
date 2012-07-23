#version 120

uniform sampler2D base_texture;

varying vec2 texCoord;
varying float diffuse_light;

#define diffuse_p .30f //diffuse lighting weight
#define ambient_p .70f //ambient lighting weight

void main() 
{
    vec3 color = texture2D(base_texture, texCoord).rgb;    
    color = (diffuse_light*diffuse_p+ambient_p) *color;  
    //color = pow(color, vec3(1.0f / 2.2f));  // gamma correction
    gl_FragColor.rgb = color;
}



