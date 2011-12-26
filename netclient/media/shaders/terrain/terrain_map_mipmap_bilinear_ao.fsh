
varying vec3 texCoord;
varying mat2 lightMatrix0;

uniform sampler2DArray base_texture;

void main() 
{
	//float gamma = 2.2f;

	vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
	vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

	float tmp = dot(vx, lightMatrix0 * vy);

	vec3 color = tmp*(texture2DArray(base_texture, texCoord.xyz).rgb);	

	gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
}
