
/*
Fragment shader failed to compile with the following errors:
ERROR: 0:13: error(#202) No matching overloaded function found pow
ERROR: 0:13: error(#160) Cannot convert from 'const float' to '3-component vector of float'
*/

uniform sampler2D color_texture;

varying float intensity;

void main() {
	// Set the output color of our current pixel

	//gl_FragColor = texture2D(color_texture, gl_TexCoord[0].st);

	float gamma = 2.2f;
	vec3 color = intensity*texture2D(color_texture, gl_TexCoord[0].st).rgb;
	gl_FragColor.rgb = pow(color, vec3(1.0 / gamma) ); 	//this line!!!
	gl_FragColor.a = 1.0;
	
	//gl_FragColor.rgb = pow(vec3(finalCol.r, finalCol.g,finalCol.b), 1.0 / 2.2);
	//gl_FragColor.a = 1.0;
	//gl_FragColor = vec4( gamma, finalCol.a);
	//gl_FragColor = intensity*texture2D(color_texture, gl_TexCoord[0].st);

/*
	vec2 uv = gl_TexCoord[0].xy;
	if(gl_FragCoord.x < 640)
	{
		//gl_FragColor.rgb = vec3(0.0f, 0.5f,0.0f );
		gl_FragColor = intensity*texture2D(color_texture, gl_TexCoord[0].st);
	} 
	else {
		//gl_FragColor = texture2D(color_texture, gl_TexCoord[0].st);
		gl_FragColor = gl_Color;
	}
*/
	
	//gl_FragColor = gl_Color;

}

/*
!!!
X-Plane's physical shader is based on conditional compilation - that is, for any given state vector of "tricks" we want to use, we recompile the shader with some #defines at the front which turn features on and off. The result is a large number of shaders, none of which need conditional logic in-shader. Fill rate isn't consumed by features we don't use.
*/

/*
GLSL Built-In Varyings:

gl_FrontColor				4D vector representing the primitives front color
gl_BackColor				4D vector representing the primitives back color
gl_TexCoord[X]				4D vector representing the Xth texture coordinate
There are some other built-in varyings. See reference [2], page 44 for a full list.

And last but not least there are some built-in types which are used for shader output:

gl_Position				4D vector representing the final processed vertex position. Only 
					available in vertex shader.
gl_FragColor				4D vector representing the final color which is written in the frame 
					buffer. Only available in fragment shader.
gl_FragDepth				float representing the depth which is written in the depth buffer.
					Only available in fragment shader.

*/
