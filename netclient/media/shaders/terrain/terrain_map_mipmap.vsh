
//uniform
//attribute - vertex shader read only
//varying - read/write in vertex shader, read only in fragment

//vec3 vert_pos;

attribute vec2 vert_uv;
attribute vec4 vert_rgba;

varying float intensity;

void main(void) {			
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// Set the position of the current vertex 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	intensity = gl_Color.r;

	gl_FrontColor = gl_Color;	//needed for white scale
}


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
