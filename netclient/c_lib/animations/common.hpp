#pragma once


namespace Animations 
{


class SHADER
{
	public:
	unsigned int shader;
	unsigned int frag_shader;
	unsigned int vert_shader;

	bool DEBUG;
	SHADER()
	{
		shader = 0;
		frag_shader = 0;
		vert_shader = 0;
		
		DEBUG = true;
	}

	void set_debug(bool value)
	{
		DEBUG = value;
	}

	void load_shader(char* name, char* vertex_shader_file, char* fragment_shader_file)
	{

	    shader = glCreateProgramObjectARB();
	    vert_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	    frag_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	    char *vs, *fs;

	    if(DEBUG) printf("set shader: %s \n", name);

	    vs = textFileRead( vertex_shader_file );
	    fs = textFileRead( fragment_shader_file );

	    glShaderSourceARB(vert_shader, 1, (const GLcharARB**)&vs, NULL);
	    glShaderSourceARB(frag_shader, 1, (const GLcharARB**)&fs, NULL);
	    
	    glCompileShaderARB(vert_shader);
	    if(DEBUG) printShaderInfoLog(vert_shader);

	    glCompileShaderARB(frag_shader);
	    if(DEBUG) printShaderInfoLog(frag_shader);
	    
	    glAttachObjectARB(shader, vert_shader);
	    glAttachObjectARB(shader, frag_shader);

	    glLinkProgramARB(shader);

	    if(DEBUG) printProgramInfoLog(shader);
	}

};

struct vertexElement
{
    struct Vec3 pos;
    float tx,ty;
};

struct vertexElement2
{
    struct Vec3 pos;

    float tx,ty;
    struct Vec3 n;
};


}