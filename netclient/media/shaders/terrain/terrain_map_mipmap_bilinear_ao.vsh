
attribute vec3 InTexCoord0;
attribute vec4 InLightMatrix0;

//varying float intensity;

varying vec3 texCoord;
varying mat2 lightMatrix0;

varying vec4 inColor;

void main(void) 
{			

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	inColor = gl_Color;

	texCoord = InTexCoord0;

	lightMatrix0 = mat2(InLightMatrix0[0], InLightMatrix0[1], InLightMatrix0[2],InLightMatrix0[3] );

}
