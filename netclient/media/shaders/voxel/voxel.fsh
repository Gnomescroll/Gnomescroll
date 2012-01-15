
//varying vec4 _rgba;

varying vec3 normal;

//varying float diffuse;

void main() 
{

	vec3 test = normalize(vec3(0.56, 0.68, 0.23)); 
	float diffuse = dot(normal, test );

	
	gl_FragColor = gl_Color * diffuse;

	//gl_FragColor = gl_Color * 0.5;

	//gl_FragColor.rgb = vec3(0.0, 0.0, 1.0);

	//gl_FragColor.rgba = rgba; 
	//gl_FragColor.rgba = rgba;
	

	//vec3 color = tmp*(texture2DArray(base_texture, texCoord.xyz).rgb);	
	//gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
}
