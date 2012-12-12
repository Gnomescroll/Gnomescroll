
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

float *hald_clut = NULL;
unsigned int hald_clut_size_x; 
unsigned int hald_clut_size_y;
unsigned int hald_clut_level = 0;
unsigned int render_level = 16;
unsigned int render_mode = 0;
unsigned int screen_x_size = 800;
unsigned int screen_y_size = 800;

float x_rotate = 0;
float y_rotate = 0;

void render_cage()
{
	float data[] = {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1};
	glVertexPointer(3, GL_FLOAT , 0, data);
	glColorPointer(3, GL_FLOAT, 0, data);	
	glDrawArrays(GL_LINES, 0, 24);
}

void set_vertex(float *vertex, int pos, float x, float y, float z)
{
	vertex[pos * 3] = x;
	vertex[pos * 3 + 1] = y;
	vertex[pos * 3 + 2] = z;
}

void cube_set(float *vertex, float *color, float *pos, float size, float gap, float *col)
{
	int i;
	gap = size * (1 - gap);
	for(i = 0; i < 24; i++)
		set_vertex(color, i, col[0], col[1], col[2]);
	set_vertex(vertex, 0, pos[0] + size, pos[1] + gap, pos[2] + gap);
	set_vertex(vertex, 1, pos[0] + size, pos[1] - gap, pos[2] + gap);
	set_vertex(vertex, 2, pos[0] + size, pos[1] - gap, pos[2] - gap);
	set_vertex(vertex, 3, pos[0] + size, pos[1] + gap, pos[2] - gap);

	set_vertex(vertex, 4, pos[0] - size, pos[1] - gap, pos[2] + gap);
	set_vertex(vertex, 5, pos[0] - size, pos[1] + gap, pos[2] + gap);
	set_vertex(vertex, 6, pos[0] - size, pos[1] + gap, pos[2] - gap);
	set_vertex(vertex, 7, pos[0] - size, pos[1] - gap, pos[2] - gap);

	set_vertex(vertex, 8, pos[0] - gap, pos[1] + size, pos[2] + gap);
	set_vertex(vertex, 9, pos[0] + gap, pos[1] + size, pos[2] + gap);
	set_vertex(vertex, 10, pos[0] + gap, pos[1] + size, pos[2] - gap);
	set_vertex(vertex, 11, pos[0] - gap, pos[1] + size, pos[2] - gap);

	set_vertex(vertex, 12, pos[0] + gap, pos[1] - size, pos[2] + gap);
	set_vertex(vertex, 13, pos[0] - gap, pos[1] - size, pos[2] + gap);
	set_vertex(vertex, 14, pos[0] - gap, pos[1] - size, pos[2] - gap);
	set_vertex(vertex, 15, pos[0] + gap, pos[1] - size, pos[2] - gap);

	set_vertex(vertex, 16, pos[0] + gap, pos[1] - gap, pos[2] + size);
	set_vertex(vertex, 17, pos[0] + gap, pos[1] + gap, pos[2] + size);
	set_vertex(vertex, 18, pos[0] - gap, pos[1] + gap, pos[2] + size);
	set_vertex(vertex, 19, pos[0] - gap, pos[1] - gap, pos[2] + size);

	set_vertex(vertex, 20, pos[0] + gap, pos[1] + gap, pos[2] - size);
	set_vertex(vertex, 21, pos[0] + gap, pos[1] - gap, pos[2] - size);
	set_vertex(vertex, 22, pos[0] - gap, pos[1] - gap, pos[2] - size);
	set_vertex(vertex, 23, pos[0] - gap, pos[1] + gap, pos[2] - size);
}

void correct_pixel(float *input, float *output, float *clut, unsigned int level)
{
	int color, red, green, blue, i, j;
	float tmp[6], r, g, b;
	level *= level;

	red = input[0] * (float)(level - 1);
	if(red > level - 2)
		red = (float)level - 2;
	if(red < 0)
		red = 0;

	green = input[1] * (float)(level - 1);
	if(green > level - 2)
		green = (float)level - 2;
	if(green < 0)
		green = 0;

	blue = input[2] * (float)(level - 1);
	if(blue > level - 2)
		blue = (float)level - 2;
	if(blue < 0)
		blue = 0;

	r = input[0] * (float)(level - 1) - red;
	g = input[1] * (float)(level - 1) - green;
	b = input[2] * (float)(level - 1) - blue;

	color = red + green * level + blue * level * level;

	i = color * 3;
	j = (color + 1) * 3;

	tmp[0] = clut[i++] * (1 - r) + clut[j++] * r;
	tmp[1] = clut[i++] * (1 - r) + clut[j++] * r;
	tmp[2] = clut[i] * (1 - r) + clut[j] * r;

	i = (color + level) * 3;
	j = (color + level + 1) * 3;

	tmp[3] = clut[i++] * (1 - r) + clut[j++] * r;
	tmp[4] = clut[i++] * (1 - r) + clut[j++] * r;
	tmp[5] = clut[i] * (1 - r) + clut[j] * r;

	output[0] = tmp[0] * (1 - g) + tmp[3] * g;
	output[1] = tmp[1] * (1 - g) + tmp[4] * g;
	output[2] = tmp[2] * (1 - g) + tmp[5] * g;

	i = (color + level * level) * 3;
	j = (color + level * level + 1) * 3;

	tmp[0] = clut[i++] * (1 - r) + clut[j++] * r;
	tmp[1] = clut[i++] * (1 - r) + clut[j++] * r;
	tmp[2] = clut[i] * (1 - r) + clut[j] * r;

	i = (color + level + level * level) * 3;
	j = (color + level + level * level + 1) * 3;

	tmp[3] = clut[i++] * (1 - r) + clut[j++] * r;
	tmp[4] = clut[i++] * (1 - r) + clut[j++] * r;
	tmp[5] = clut[i] * (1 - r) + clut[j] * r;

	tmp[0] = tmp[0] * (1 - g) + tmp[3] * g;
	tmp[1] = tmp[1] * (1 - g) + tmp[4] * g;
	tmp[2] = tmp[2] * (1 - g) + tmp[5] * g;

	output[0] = output[0] * (1 - b) + tmp[0] * b;
	output[1] = output[1] * (1 - b) + tmp[1] * b;
	output[2] = output[2] * (1 - b) + tmp[2] * b;
}

void render_cube()
{
	float *data, *color, pos[3], corrected[3];
	int i, j, k, l = 0, size = render_level;
	data = malloc((sizeof *data) * 3 * 24 * size * size * size);
	color = malloc((sizeof *color) * 3 * 24 * size * size * size);
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			for(k = 0; k < size; k++)
			{
				pos[0] = ((float)i + 0.5) / (float)size; 
				pos[1] = ((float)j + 0.5) / (float)size; 
				pos[2] = ((float)k + 0.5) / (float)size;
				correct_pixel(pos, corrected, hald_clut, hald_clut_level);
				cube_set(&data[l * 3 * 24], &color[l * 3 * 24], corrected, 0.25 / (float)size, 0.0, pos);
				l++;
			}
		}
	}
	glVertexPointer(3, GL_FLOAT , 0, data);
	glColorPointer(3, GL_FLOAT, 0, color);
	glDrawArrays(GL_QUADS, 0, 24 * size * size * size);
	free(data);
	free(color);
}

void render_lines()
{
	float *data, pos[3], corrected[3];
	int i, j, k, l = 0, size = render_level;
	data = malloc((sizeof *data) * 3 * 2 * size * size * size);
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			for(k = 0; k < size; k++)
			{
				data[l * 6 + 0] = ((float)i + 0.5) / (float)size; 
				data[l * 6 + 1] = ((float)j + 0.5) / (float)size; 
				data[l * 6 + 2] = ((float)k + 0.5) / (float)size;
				correct_pixel(&data[l * 6 + 0], &data[l * 6 + 3], hald_clut, hald_clut_level);
				l++;
			}
		}
	}
	glVertexPointer(3, GL_FLOAT , 0, data);
	glColorPointer(3, GL_FLOAT, 0, data);
	glDrawArrays(GL_LINES, 0, 2 * size * size * size);
	free(data);
}

float *load_targa(char *file_name, unsigned int *x, unsigned int *y)
{
	FILE *image;
	float *draw = NULL;
	unsigned int i, j, identfeald, type, x_size, y_size, alpha;

	if((image = fopen(file_name, "rb")) == NULL)
	{
		printf("can not open file: %s\n", file_name);
		exit(0);
	}

	identfeald = fgetc(image);
	if(0 != fgetc(image))
	{
		printf("Error: File %s a non suported palet image\n", file_name);
		exit(0);
	}
	type = fgetc(image);
	if(2 != type) /* type must be 2 uncompressed RGB */
	{
		printf("Error: File %s is not a uncompressed RGB image\n", file_name);
		exit(0);
	}
	for(i = 3; i < 12; i++)
		fgetc(image); /* ignore some stuff */
	x_size = fgetc(image);
	x_size += 256 * fgetc(image);
	y_size = fgetc(image);
	y_size += 256 * fgetc(image);

	alpha = fgetc(image);

	if(alpha != 24 && alpha != 32) /* type must be 24 or 32 bits RGB */
	{
		printf("Error: File %s is not a 24 or 32 bit RGB image\n", file_name);
		exit(0);
	}

	for(i = 0; i < identfeald; i++)
		fgetc(image); /* ignore some stuff */
	*x = x_size;
	*y = y_size;
	
	draw = malloc((sizeof *draw) * x_size * y_size * 3);

	if(alpha == 32)
	{
		for(j = 0; j < y_size; j++)
		{
			for(i = 0; i < x_size; i++)
			{
				fgetc(image); /* ignore alpha */
				draw[((y_size - j - 1) * x_size + i) * 3 + 2] = (float)fgetc(image) / (float)255.0;
				draw[((y_size - j - 1) * x_size + i) * 3 + 1] = (float)fgetc(image) / (float)255.0;
				draw[((y_size - j - 1) * x_size + i) * 3 + 0] = (float)fgetc(image) / (float)255.0;
			}
		}
	}else for(j = 0; j < y_size; j++)
	{
		for(i = 0; i < x_size; i++)
		{			
			draw[((y_size - j - 1) * x_size + i) * 3 + 0] = (float)fgetc(image) / (float)255.0;
			draw[((y_size - j - 1) * x_size + i) * 3 + 2] = (float)fgetc(image) / (float)255.0;
			draw[((y_size - j - 1) * x_size + i) * 3 + 1] = (float)fgetc(image) / (float)255.0;
		}
	}
	fclose(image);
	return draw;
}

void passive_mouse_func(int x, int y)
{
	x_rotate = x_rotate * 0.1 + ((float)x / (float)screen_x_size * 560) * 0.9;
	y_rotate = y_rotate * 0.1 + ((float)y / (float)screen_y_size * 180 - 90) * 0.9;
}

void keyboard_func(unsigned char key, int x, int y)
{
	if(key == '+')
		render_level++;
	if(key == '-' && render_level > 1)
		render_level--;
	if(key == 'q')
		exit(0);
	if(key == ' ')
		render_mode = !render_mode;
}


void display_func(void)
{
	static int i = 0;
	double matrix[16] = {0.7069, 0.5773, -0.4081, 0, -0.7069, 0.5773, -0.4081, 0, 0, 0.5773, 0.8165, 0, 0, -0.866, 0, 1};
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0, 0, -2);
	glRotatef(y_rotate, 1, 0, 0);
	glRotatef(x_rotate, 0, 1, 0);
	glMultMatrixd(matrix);
	render_cage();
	if(render_mode)
		render_lines();
	else
		render_cube();
	glPopMatrix();
	glutSwapBuffers();
}


void reshape_view(int x_size, int y_size)
{
	float w, h, fov, aspect;
	w = screen_x_size = x_size;
	h = screen_y_size = y_size;
	fov = 0.6;
	glMatrixMode(GL_PROJECTION);
	aspect = w / h;
	glLoadIdentity();
	glFrustum(-fov * 0.005, fov * 0.005, (-fov / aspect) * 0.005, (fov / aspect) * 0.005, 0.005, 100.0);
	if(aspect > w / h)
		glViewport((int)0, (int)(h - (w / aspect)) / 2, (int)w, (int)(w / aspect));	
	else
		glViewport((int)(w - (h * aspect)) / 2, 0, (int)(h * aspect), (int)h);
	glMatrixMode(GL_MODELVIEW);	
}

void idle_func(void)
{
   glutPostRedisplay();
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(800, 800);
	glutCreateWindow("HALD CLUT viewer Press SPACE to change mode and +/- to change resolution");
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutPassiveMotionFunc(passive_mouse_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_view);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
	reshape_view(800, 800);
	if(argc != 2)
	{
		printf("Usage: %s <clut file name>\n", argv[0]);
		exit(0);
	}
	hald_clut = load_targa(argv[1], &hald_clut_size_x, &hald_clut_size_y);

	if(hald_clut_size_x != hald_clut_size_y)
	{
		printf("Error: CLUT image is not square\n");
		exit(0);
	}
	for(hald_clut_level = 1; hald_clut_level * hald_clut_level * hald_clut_level < hald_clut_size_x; hald_clut_level++);
	if(hald_clut_level * hald_clut_level * hald_clut_level > hald_clut_size_x)
	{
		printf("Error: CLUT image %u is a non valid level\n", argv[2]);
		exit(0);
	}
	glutMainLoop();
}