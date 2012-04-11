#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#extension GL_EXT_texture_array : enable

varying vec3 texCoord;

# ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix;
#else
    varying mat2 lightMatrix;
#endif

varying vec3 inColor;
 
uniform sampler2DArray base_texture;

void main() 
{
	//float gamma = 2.2f;

	vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
	vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

	float tmp = dot(vx, lightMatrix * vy);

	//vec3 color = inColor.rgb;	
	

    vec3 color = (tmp*((texture2DArray(base_texture, texCoord.xyz)).rgb));	    
    //vec3 color = tmp*vec3(1.0, 0.0, 0.0);


	float intensity = 3.0f / (inColor.r + inColor.g + inColor.b);
	vec3 color2 = intensity*inColor;
	color = color2 * color;


	gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
	//gl_FragColor.rgb = vec3(1.0,0,0);

}



