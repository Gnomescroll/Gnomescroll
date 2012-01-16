
varying float intensity;
varying vec3 texCoord;
varying mat2 lightMatrix0;

uniform sampler2DArray base_texture;

void main() 
{
	if( gl_FragCoord.x < 640 )
	{
		vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
		vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

		float tmp = dot(vx, lightMatrix0 * vy);

		//vec3 color = tmp*(texture2DArray(base_texture, texCoord.xyz).rgb);	
		
		vec3 color = vec3( tmp, tmp, tmp);
		gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
	} 
	else 
	{
		//vec3 color = intensity*(texture2DArray(base_texture, texCoord.xyz).rgb);	
		vec3 color = vec3( intensity, intensity, intensity);
		gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
	}
}
