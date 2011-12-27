
//uniform mat3 axis;
//uniform vec3 offset;

//attribute vec3 InCood0;
//attribute vec4 InRGBA;

//varying vec4 _rgba;

void main(void) 
{
	gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	//gl_Position = gl_ModelViewProjectionMatrix * InCood0;
	//_rgba = InRGBA;
}
