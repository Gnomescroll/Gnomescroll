#version 120

//varying vec4 _rgba;

//varying vec3 normal;

varying float diffuse;

varying mat2 AOMatrix;
varying vec2 texCoord;

#define gamma 2.2f
#define diffuse_p .30f //diffuse lighting weight
#define ambient_p .70f //ambient lighting weight

void main() 
{

	//AO
	vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
	vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);
	float ao = dot(vx, AOMatrix * vy);

	vec3 color = ao*(diffuse_p*(diffuse*gl_Color.rgb) + ambient_p*gl_Color.rgb);

	gl_FragColor.rgb = pow(color, vec3(1.0f / gamma) );

/*
	if( gl_FragCoord.x < 640 )
	{
		vec3 color = light_p*(gl_Color.rgb * diffuse) + (1-light_p)*gl_Color.rgb*ao_factor ;
		gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
	} 
	else 
	{
		gl_FragColor.rgb = vec3(ao_factor, ao_factor, ao_factor);
	}
*/

}
