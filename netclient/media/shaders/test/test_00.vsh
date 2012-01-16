/*
void main()
{

    // the following three lines provide the same result

//  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
//  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_Position = ftransform();
}
*/

void main() {			
	// Set the front color to the color passed through with glColor*f
	gl_FrontColor = gl_Color;
	// Set the position of the current vertex 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
