
uniform mat3 InRotationMatrix;
uniform vec3 InTranslation;

//attribute vec3 InCood0;
//attribute vec4 InRGBA;

//varying vec4 _rgba;

void main(void) 
{
	gl_FrontColor = gl_Color;

	vec4 pos = gl_Vertex + InTranslation;
	gl_Position = gl_ModelViewProjectionMatrix * pos;

	//gl_Position = gl_ModelViewProjectionMatrix * InCood0;
	//_rgba = InRGBA;
}
