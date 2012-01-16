varying vec3 texCoord;
varying mat2 lightMatrix0;
 
varying vec4 inColor;
 
uniform sampler2DArray base_texture;

void main() 
{
	//float gamma = 2.2f;

	vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
	vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

	float tmp = dot(vx, lightMatrix0 * vy);

	//vec3 color = inColor.rgb;	
	vec3 color = tmp*(texture2DArray(base_texture, texCoord.xyz).rgb);	

	float intensity = (inColor.r + inColor.g + inColor.b) / 3.0f;
	vec3 color2 = vec3(inColor.r/intensity, inColor.g/intensity, inColor.b/intensity);
	
	color = color2 * color;
	gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
}


/*
void main() 
{
        //float gamma = 2.2f;
 
        vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
        vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);
 
        float tmp = dot(vx, lightMatrix0 * vy);
       

	//float att  = tmp / float(inColor.r + inColor.g + inColor.b);
	//float intensity = (inColor.r + inColor.g + inColor.b) / att;
	//intensity /= att;


	//float intensity = (inColor.r + inColor.g + inColor.b) / ( tmp / float(inColor.r + inColor.g + inColor.b) )

        vec3 color = tmp*(texture2DArray(base_texture, texCoord.xyz).rgb);      

        vec3 color2 = vec3(inColor.r/intensity, inColor.g/intensity, inColor.b/intensity);
        
        color = color2 * color;
        gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
}
*/

/*
varying vec3 texCoord;
varying mat2 lightMatrix0;

varying vec4 inColor;

uniform sampler2DArray base_texture;
*/



