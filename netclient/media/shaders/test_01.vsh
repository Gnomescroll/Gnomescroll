
/*
void main() {			
	// Set the front color to the color passed through with glColor*f
	gl_FrontColor = gl_Color;
	// Set the position of the current vertex 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
*/

void main() {			
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// Set the position of the current vertex 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
