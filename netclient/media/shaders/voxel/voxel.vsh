#version 120

uniform mat3 InRotationMatrix;
uniform vec3 InTranslation;

attribute vec3 InNormal;
attribute vec4 InAO;

varying float diffuse;

//varying vec2 texCoord;

void main(void) 
{
	gl_FrontColor = gl_Color;
	vec3 pos = (InRotationMatrix*gl_Vertex.xyz) + InTranslation;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(pos.x, pos.y, pos.z, 1);

	/* apply rotation matrix to normal */
	vec3 normal = gl_NormalMatrix*(InRotationMatrix*InNormal);
	const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));

	diffuse = abs(dot(normal, light_dir ));
	

}

