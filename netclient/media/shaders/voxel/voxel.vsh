
uniform mat4 InRotationMatrix;

//uniform vec3 InTranslation;
//attribute vec3 InCood0;
//attribute vec4 InRGBA;
//varying vec4 _rgba;

attribute vec3 InNormal;

//varying float diffuse;

varying vec3 normal;

void main(void) 
{
	gl_FrontColor = gl_Color;

	vec4 pos = InRotationMatrix*gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * pos;

	normal = InNormal;
	//vec3 test = normalize(vec3(0.56, 0.68, 0.23));
 
	//diffuse = dot(InNormal, test );

	//gl_Position = gl_ModelViewProjectionMatrix * InCood0;
	//_rgba = InRGBA;
}
