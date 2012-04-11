#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

uniform sampler2D base_texture;

varying vec3 texCoord;


void main() 
{
	//float gamma = 2.2f;

	vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
	vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

	float tmp = dot(vx, lightMatrix * vy);

	//vec3 color = inColor.rgb;	
	

    vec3 color = (tmp*((texture2DArray(base_texture, texCoord.xyz)).rgb));	    
    
    uniform sampler2D base_texture;

	vec3 color2 = vec3(inColor.r/intensity, inColor.g/intensity, inColor.b/intensity);
	color = color2 * color;


	gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );


}



