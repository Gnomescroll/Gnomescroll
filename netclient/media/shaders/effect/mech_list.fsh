#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

//uniform sampler2D base_texture;

uniform sampler3D clut_texture;
uniform sampler2D base_texture;

varying vec2 texCoord;

void main() 
{	
    gl_FragColor = texture2D(base_texture, texCoord.xy);
}



