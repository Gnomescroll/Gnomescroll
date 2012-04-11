#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

uniform sampler2D base_texture;

varying vec2 texCoord;


void main() 
{
	//float gamma = 2.2f;
	
    vec4 color = texture2D(base_texture, texCoord.xy);

    gl_FragColor = color;
	
    //gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );


}



