#version 120

//varying vec4 _rgba;

//varying vec3 normal;

varying float diffuse;

//varying mat2 AOMatrix;
//varying vec2 texCoord;

void main() 
{

	//AO
	//vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
	//vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);
	//float ao_factor = dot(vx, AOMatrix * vy);

	const float light_p = .30; //percentage for light component mixing
	
	//vec3 color = light_p*(gl_Color.rgb * diffuse) + (1-light_p)*gl_Color.rgb ;

	vec3 color = light_p*(gl_Color.rgb * diffuse) + (1-light_p)*gl_Color.rgb;
	gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );

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
	//vec3 color = light_p*(gl_Color.rgb * diffuse) + (1-light_p)*gl_Color.rgb*ao_factor ;
	//vec3 color = ao_factor*(light_p*(gl_Color.rgb * diffuse) + (1-light_p)*gl_Color.rgb) ;

	//gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
}
