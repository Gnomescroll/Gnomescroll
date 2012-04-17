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

	void load_shader(const char* name, const char* vertex_shader_file, const char* fragment_shader_file)
	{

	    shader = glCreateProgramObjectARB();
	    vert_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	    frag_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	    char *vs, *fs;

	    if(DEBUG) printf("set shader: %s \n", name);

	    vs = textFileRead( (char*) vertex_shader_file );
	    fs = textFileRead( (char*) fragment_shader_file );

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

	unsigned int get_attribute(const char* attribute_name)
	{
		return glGetAttribLocation(shader, attribute_name);
	}

	unsigned int get_uniform(const char* uniform_name)
	{
		return glGetUniformLocation(shader, uniform_name);
	}	
};

/*
	Textured Vertex element without normal
*/

struct vertexElement1
{
    struct Vec3 pos;
    float tx,ty;
};

class VertexElementList1
{
	public:

	const static stride = sizeof(struct vertexElement1);

	struct vertexElement1* vlist;
	int vlist_index;
	int vlist_max;

	unsigned int VBO;
	unsigned int vertex_number;

	VertexElementList1()
	{
		VBO = 0;

		vlist_index = 0;
		vlist_max = 1024;
		vlist = (vertexElement1*) malloc(1024*sizeof(struct vertexElement1));
	}

	__attribute__((always_inline))
	void push_vertex(struct Vec3 pos, float tx, float ty)
	 {
	    vlist[vlist_index].pos = pos;
	    vlist[vlist_index].tx = tx;
	    vlist[vlist_index].ty = ty;

	    vlist_index++;

	    if(vlist_index >= vlist_max)
	    {
	    	vlist_max *= 2;
	    	vlist = (vertexElement1*) realloc(vlist, vlist_max*sizeof(struct vertexElement1));
	    }
	 }
/*
	void reset_index()
	{
		vlist_index = 0;
	}
*/
	//upload data to card for drawing
	void buffer()
	{
		if(VBO == 0) glGenBuffers(1, &VBO);

    	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    	glBufferData(GL_ARRAY_BUFFER, vlist_index*stride, NULL, GL_DYNAMIC_DRAW);
    	glBufferData(GL_ARRAY_BUFFER, vlist_index*stride, vlist, GL_DYNAMIC_DRAW);

    	vertex_number = vlist_index;

    	vlist_index = 0;
	}

};

/*
	Textyred vertex element with normal
*/

struct vertexElement2
{
    struct Vec3 pos;

    float tx,ty;
    struct Vec3 n;
};

class VertexElementList2
{
	public:

	const static stride = sizeof(struct vertexElement2);

	struct vertexElement2* vlist;
	int vlist_index;
	int vlist_max;

	unsigned int VBO;
	unsigned int vertex_number;

	VertexElementList2()
	{
		vlist_index = 0;
		vlist_max = 1024;
		vlist = (vertexElement2*) malloc(1024*sizeof(struct vertexElement2));
	}

	~VertexElementList2()
	{
		free(vlist);
	}

	__attribute__((always_inline))
	void push_vertex(struct Vec3 pos, float tx, float ty)
	 {
	    vlist[vlist_index].pos = pos;
	    vlist[vlist_index].tx = tx;
	    vlist[vlist_index].ty = ty;

	    vlist_index++;

	    if(vlist_index >= vlist_max)
	    {
	    	vlist_max *= 2;
	    	vlist = (vertexElement2*) realloc(vlist, vlist_max*sizeof(struct vertexElement2));
	    }
	 }
/*
	 void reset_index()
	 {
	 	vlist_index = 0;
	 }
*/
	//upload data to card for drawing
	void buffer()
	{
		if(VBO == 0) glGenBuffers(1, &VBO);

    	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    	glBufferData(GL_ARRAY_BUFFER, vlist_index*stride, NULL, GL_DYNAMIC_DRAW);
    	glBufferData(GL_ARRAY_BUFFER, vlist_index*stride, vlist, GL_DYNAMIC_DRAW);

    	vertex_number = vlist_index;

    	vlist_index = 0;
	}

};


}