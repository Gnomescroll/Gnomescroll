#pragma once

char *textFileRead(char *fn);

//info log print
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

void load_shaders(char *vert, char* frag, GLuint* prog);

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
