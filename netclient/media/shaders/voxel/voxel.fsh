#version 120

//varying vec4 _rgba;

//varying vec3 normal;

varying float diffuse;

varying mat2 AOMatrix;
varying vec2 texCoord;

#define gamma 2.2f
#define diffuse_p .30f //diffuse lighting weight
#define ambient_p .70f //ambient lighting weight

//varying float fogFragDepth;

void main() 
{

	//AO
	vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
	vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);
	float ao = dot(vx, AOMatrix * vy);

	//vec3 color = ao*(diffuse_p*(diffuse*gl_Color.rgb) + ambient_p*gl_Color.rgb);
	vec3 color = diffuse_p*(diffuse*gl_Color.rgb) + ao*ambient_p*gl_Color.rgb;  //this one looks better

	gl_FragColor.rgb = pow(color, vec3(1.0f / gamma) );

/*
	if(fogFragDepth <= fog_start)
	{
		gl_FragColor.rgb = color;
	}
	else
	{
		//gl_FragColor.rgb = fog_color;
		float fogFactor = (fogFragDepth - fog_start) / fog_depth;

		if(fogFactor >= 1.0) discard;
		gl_FragColor.rgb = mix( color, fog_color, fogFactor);
	}
*/

}
