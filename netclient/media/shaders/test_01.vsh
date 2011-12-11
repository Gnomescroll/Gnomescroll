
/*
void main() {			
	// Set the front color to the color passed through with glColor*f
	gl_FrontColor = gl_Color;
	// Set the position of the current vertex 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
*/

uniform sampler2D color_texture;
void main() {
    // Set the output color of our current pixel
    gl_FragColor = texture2D(color_texture, gl_TexCoord[0].st);
}
