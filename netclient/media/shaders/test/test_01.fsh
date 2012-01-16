
uniform sampler2D color_texture;

void main() {
	// Set the output color of our current pixel
	gl_FragColor = texture2D(color_texture, gl_TexCoord[0].st);
}
