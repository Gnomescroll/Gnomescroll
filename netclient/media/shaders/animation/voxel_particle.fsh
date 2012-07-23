#version 120

uniform sampler2D base_texture;

varying vec2 texCoord;
varying float lightIntensity;

void main() 
{
    vec3 color = lightIntensity*(texture2D(base_texture, texCoord).rgb);      
    color = pow(color, vec3(1.0f / 2.2f));  // gamma correction
    gl_FragColor.rgb = color;
}



