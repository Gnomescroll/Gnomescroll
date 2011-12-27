
uniform mat4 InRotationMatrix;
//uniform vec3 InTranslation;

//attribute vec3 InCood0;
//attribute vec4 InRGBA;

//varying vec4 _rgba;

void main(void) 
{
	gl_FrontColor = gl_Color;

	vec4 pos = InRotationMatrix*gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * pos;

	//gl_Position = gl_ModelViewProjectionMatrix * InCood0;
	//_rgba = InRGBA;
}
