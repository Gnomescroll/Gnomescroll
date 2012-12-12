
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

#define GL_TEXTURE_3D                     0x806F
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_CLAMP_TO_EDGE                  0x812F
float x_rotate = 0;
float y_rotate = 0;

int texture_image;
int texture_clut;
int program;
float image_aspect;

#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
#if defined(__APPLE__) || defined(MACOSX)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#endif

#if defined(__APPLE__) || defined(MACOSX)
	#define APIENTRY
#endif

#if !defined(__APPLE__) && !defined(MACOSX)
typedef unsigned int GLhandleARB;
#endif

#define GL_VERTEX_SHADER_ARB                        0x8B31
#define GL_FRAGMENT_SHADER_ARB                      0x8B30
#define GL_TEXTURE0_ARB								0x84C0
//#define GL_TEXTURE_3D								0x806F	
	
GLhandleARB (APIENTRY *p_glCreateShaderObjectARB)(GLenum  shaderType);
GLvoid      (APIENTRY *p_glDeleteObjectARB)(GLhandleARB obj);
GLhandleARB (APIENTRY *p_glCreateProgramObjectARB)(GLvoid);
GLvoid      (APIENTRY *p_glAttachObjectARB)(GLhandleARB containerObj, GLhandleARB obj);
GLvoid      (APIENTRY *p_glShaderSourceARB)(GLhandleARB shaderObj, GLsizei count, const char **string, const GLint *length);
GLvoid      (APIENTRY *p_glCompileShaderARB)(GLhandleARB shaderObj);
GLvoid      (APIENTRY *p_glLinkProgramARB)(GLhandleARB programObj);
GLvoid      (APIENTRY *p_glUseProgramObjectARB)(GLhandleARB programObj);
GLint       (APIENTRY *p_glGetUniformLocationARB)(GLhandleARB programObj, const char *name);
GLvoid      (APIENTRY *p_glUniform4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLvoid      (APIENTRY *p_glUniform1iARB)(GLint location, GLint i);
GLvoid		(APIENTRY *p_glUniformMatrix4fvARB)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
GLvoid		(APIENTRY *p_glUniformMatrix3fvARB)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
GLint       (APIENTRY *p_glGetAttribLocationARB)(GLhandleARB programObj, const char *name);
GLvoid		(APIENTRY *p_glVertexAttrib4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLvoid		(APIENTRY *p_glActiveTextureARB)(GLenum texture);

GLvoid		(APIENTRY *p_glBindAttribLocationARB)(GLhandleARB programObj, GLuint index, const char *name);
GLvoid		(APIENTRY *p_glVertexAttribPointerARB)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
GLvoid		(APIENTRY *p_glEnableVertexAttribArrayARB)(GLuint index);
GLvoid		(APIENTRY *p_glDisableVertexAttribArrayARB)(GLuint index);

GLvoid		(APIENTRY *p_glGetInfoLogARB)(GLhandleARB object, GLsizei maxLenght, GLsizei *length, char *infoLog);
GLvoid		(APIENTRY *p_glTexImage3DEXT)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);

//wglGetProcAddress("glMultiTexCoord1fARB");

void shader_init(void)
{
	if(glutExtensionSupported("GL_ARB_shading_language_100"))
	{
		p_glCreateShaderObjectARB = wglGetProcAddress("glCreateShaderObjectARB");
		p_glDeleteObjectARB = wglGetProcAddress("glDeleteObjectARB");
		p_glCreateProgramObjectARB = wglGetProcAddress("glCreateProgramObjectARB");
		p_glAttachObjectARB = wglGetProcAddress("glAttachObjectARB");
		p_glShaderSourceARB = wglGetProcAddress("glShaderSourceARB");
		p_glCompileShaderARB = wglGetProcAddress("glCompileShaderARB");
		p_glLinkProgramARB = wglGetProcAddress("glLinkProgramARB");
		p_glUseProgramObjectARB = wglGetProcAddress("glUseProgramObjectARB");
		p_glGetUniformLocationARB = wglGetProcAddress("glGetUniformLocationARB");
		p_glUniform4fARB = wglGetProcAddress("glUniform4fARB");
		p_glUniform1iARB = wglGetProcAddress("glUniform1iARB");
		p_glUniformMatrix3fvARB = wglGetProcAddress("glUniformMatrix3fvARB");
		p_glUniformMatrix4fvARB = wglGetProcAddress("glUniformMatrix4fvARB");
		p_glBindAttribLocationARB = wglGetProcAddress("glBindAttribLocationARB");
		p_glGetAttribLocationARB = wglGetProcAddress("glGetAttribLocationARB");
		p_glVertexAttrib4fARB = wglGetProcAddress("glVertexAttrib4fARB");
		p_glVertexAttribPointerARB = wglGetProcAddress("glVertexAttribPointerARB");
		p_glEnableVertexAttribArrayARB = wglGetProcAddress("glEnableVertexAttribArrayARB");
		p_glDisableVertexAttribArrayARB = wglGetProcAddress("glDisableVertexAttribArrayARB");
		p_glActiveTextureARB = wglGetProcAddress("glActiveTextureARB");
		p_glGetInfoLogARB = wglGetProcAddress("glGetInfoLogARB");
	}
	if(glutExtensionSupported("GL_EXT_texture3D"))
		p_glTexImage3DEXT =	wglGetProcAddress("glTexImage3DEXT");
}

int shader_create()
{
	FILE *f;
	char vertex[4096], fragment[4096], buf[4096];
	char *c;
	int vertex_obj, fragment_obj, prog_obj;
	int i;
	if((f = fopen("vertex_shader.txt", "r")) != NULL)
	{
		for(i = 0; (vertex[i] = fgetc(f)) != EOF; i++);
		vertex[i] = 0;
		fclose(f);
	}else
	{
		printf("No vertex_shader.txt file found\n");
		exit(0);
	}
	if((f = fopen("fragment_shader.txt", "r")) != NULL)
	{
		for(i = 0; (fragment[i] = fgetc(f)) != EOF; i++);
		fragment[i] = 0;
		fclose(f);
	}else
	{
		printf("No fragment_shader.txt file found\n");
		exit(0);
	}

	prog_obj = p_glCreateProgramObjectARB();
	vertex_obj = p_glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_obj = p_glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	p_glAttachObjectARB(prog_obj, vertex_obj);
	p_glAttachObjectARB(prog_obj, fragment_obj);
	i = strlen(vertex);
	c = vertex;
	p_glShaderSourceARB(vertex_obj, 1, &c, &i);
	p_glCompileShaderARB(vertex_obj);
	p_glGetInfoLogARB(vertex_obj, 4096, &i, buf);
	printf("Vertex Compile:\n%s", buf);
	i = strlen(fragment);
	c = fragment;
	p_glShaderSourceARB(fragment_obj, 1, &c, &i);
	p_glCompileShaderARB(fragment_obj);
	p_glGetInfoLogARB(fragment_obj, 4096, &i, buf);
	printf("Fragment Compile:\n%s", buf);
	p_glLinkProgramARB(prog_obj);
	p_glGetInfoLogARB(prog_obj, 4096, &i, buf);
	printf("Program Link:\n%s", buf);
	p_glUseProgramObjectARB(prog_obj);
	p_glUniform1iARB(p_glGetUniformLocationARB(prog_obj, "clut"), 0);
	p_glUniform1iARB(p_glGetUniformLocationARB(prog_obj, "image"), 1);
	p_glUseProgramObjectARB(0);	
	return prog_obj;
}

void render_image()
{
	float vertex[8] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5}, uv[8] = {0, 1, 1, 1, 1, 0, 0, 0};
	glColor4f(1, 1, 1, 1);
	vertex[0] /= image_aspect;
	vertex[2] /= image_aspect;
	vertex[4] /= image_aspect;
	vertex[6] /= image_aspect;
	p_glUseProgramObjectARB(program);
	p_glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, texture_clut);	
	p_glActiveTextureARB(GL_TEXTURE0_ARB + 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_image);

	glVertexPointer(2, GL_FLOAT , 0, vertex);
	glTexCoordPointer(2, GL_FLOAT, 0, uv);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	p_glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_3D);
	p_glUseProgramObjectARB(0);

}

int texture_make(float *data, unsigned int dimensions, unsigned int x, unsigned int y, unsigned int z)
{
	int token, texture_id;
	glGenTextures(1, &texture_id);
	if(dimensions == 3)
		token = GL_TEXTURE_3D;
	else
		token = GL_TEXTURE_2D;
	glEnable(token);
	glBindTexture(token, texture_id);
	glTexParameterf(token, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(token, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(token, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(token, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(token, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if(dimensions == 3)
		p_glTexImage3DEXT(GL_TEXTURE_3D, 0, GL_RGB, x, y, z, 0, GL_RGB, GL_FLOAT, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_FLOAT, data);
	glDisable(token);
	return texture_id;
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
	glTranslatef(0, 0, -1.5);
	glRotatef(y_rotate, 1, 0, 0);
	glRotatef(x_rotate, 0, 1, 0);
	render_image();
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
	glClearColor(0.1, 0.1, 0.1, 0.1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
	shader_init();
	reshape_view(800, 800);
	program = shader_create();
	if(argc != 3)
	{
		printf("Usage: %s <clut file name> <image file name>\n", argv[0]);
		exit(0);
	}
	printf("Loading CLUT...\n");
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
	texture_clut = texture_make(hald_clut, 3, hald_clut_level * hald_clut_level, hald_clut_level * hald_clut_level, hald_clut_level * hald_clut_level);
	printf("Loading IMAGE...\n");
	hald_clut = load_targa(argv[2], &hald_clut_size_x, &hald_clut_size_y);
	texture_image = texture_make(hald_clut, 2, hald_clut_size_x, hald_clut_size_y, 0);
	image_aspect = (float)hald_clut_size_y / (float)hald_clut_size_x;
	glutMainLoop();
}