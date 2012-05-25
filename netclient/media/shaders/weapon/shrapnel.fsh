#version 120

uniform sampler2D base_texture;

varying vec2 texCoord;

void main() 
{

    gl_FragColor = texture2D(base_texture, texCoord.xy);

}



