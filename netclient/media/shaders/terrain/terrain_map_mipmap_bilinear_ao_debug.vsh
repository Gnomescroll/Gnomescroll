
varying float intensity;
varying vec3 texCoord;
varying mat2 lightMatrix0;

attribute vec3 InTexCoord0;
attribute vec4 InLightMatrix0;

void main(void) 
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	intensity = gl_Color.r;

	texCoord = InTexCoord0;

	//lightMatrix0 = mat2(InLightMatrix0, InLightMatrix1);
	lightMatrix0 = mat2(InLightMatrix0[0], InLightMatrix0[1], InLightMatrix0[2],InLightMatrix0[3] );
}
