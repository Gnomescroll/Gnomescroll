
//uniform
//attribute - vertex shader read only
//varying - read/write in vertex shader, read only in fragment

varying float intensity;

//attribute vec3 texCoordin;

attribute vec3 InTexCoord0;
attribute mat2 LightMatrix0;

varying vec3 texCoord;

void main(void) {			

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	intensity = gl_Color.r;

	//texCoord =  vec3(vec2(gl_MultiTexCoord0.st), 0.0f);
	//texCoord = vec3(gl_MultiTexCoord0);
	texCoord = InTexCoord0;

	//texCoord =  vec3(vec2(InTexCoord0.st), 1.0f);

	//gl_FrontColor = gl_Color;
}

	
//tex2 = tex;
//texCoord = vec2(gl_MultiTexCoord0);
//gl_TexCoord[0] = gl_MultiTexCoord0;
	


/*
GLSL built-in attributes in a vertex shader:

gl_Vertex				4D vector representing the vertex position
gl_Normal				3D vector representing the vertex normal
gl_Color				4D vector representing the vertex color
gl_MultiTexCoordX			4D vector representing the texture coordinate of texture unit X
*/


/*
GLSL built-in uniforms:

gl_ModelViewMatrix			4x4 Matrix representing the model-view matrix.
gl_ModelViewProjectionMatrix		4x4 Matrix representing the model-view-projection matrix.
gl_NormalMatrix				3x3 Matrix representing the inverse transpose model-view matrix.
					This matrix is used for normal transformation.//
*/

/*
void main() {			
	// Set the front color to the color passed through with glColor*f
	//gl_FrontColor = gl_Color;
	
	// Set the position of the current vertex 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	gl_TexCoord[0] = gl_MultiTexCoord0;

}
*/
