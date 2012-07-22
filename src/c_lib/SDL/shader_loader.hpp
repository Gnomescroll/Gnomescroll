#pragma once

char *textFileRead(char *fn);

//info log print
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

void load_shaders(char *vert, char* frag, GLuint* prog);

//returns true on error
bool shader_error_occured(int shader);

class SHADER
{
    public:
    unsigned int shader;
    unsigned int frag_shader;
    unsigned int vert_shader;

    bool DEBUG1;
    
    char* name; //shader name

    int attribute_array[16]; //atribute list
    int attribute_index;

    int uniform_array[16]; //uniform list
    int uniform_index;

    bool shader_valid; //is shader valid?

    SHADER()
    {
        shader = 0;
        frag_shader = 0;
        vert_shader = 0;
        
        DEBUG1 = true;
        name = NULL;

        attribute_index = 0;
        uniform_index = 0;

        shader_valid = false;
    }

    void set_debug(bool value)
    {
        DEBUG1 = value;
    }

    void load_shader(const char* _name, const char* vertex_shader_file, const char* fragment_shader_file)
    {
        //set shader name
        GS_ASSERT(name == NULL);
        name = new char[strlen(_name)];
        name = strcpy(name, _name);

        char *vs, *fs;

        printf("Loading shader: %s\n", name);

        vs = textFileRead( (char*) vertex_shader_file );
        fs = textFileRead( (char*) fragment_shader_file );
        GS_ASSERT(vs != NULL);
        GS_ASSERT(fs != NULL);
        if (vs == NULL || fs == NULL)
        {
            if (vs != NULL) free(vs);
            if (fs != NULL) free(fs);
            return;
        }

        shader = glCreateProgramObjectARB();
        vert_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        frag_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

        glShaderSourceARB(vert_shader, 1, (const GLcharARB**)&vs, NULL);
        glShaderSourceARB(frag_shader, 1, (const GLcharARB**)&fs, NULL);
        
        free(vs);
        free(fs);

        glCompileShaderARB(vert_shader);
        if(glIsShader(vert_shader) == false)
        {
            printf("vertex shader failed with error: %s \n", name);
            printShaderInfoLog(vert_shader);
            shader_valid = false;
            return;
        }
        else
        {
            if(DEBUG1) printShaderInfoLog(vert_shader);
        }

        glCompileShaderARB(frag_shader);
        if(glIsShader(frag_shader) == false)
        {
            printf("fragment shader failed with error: %s \n", name);
            printShaderInfoLog(frag_shader);
            shader_valid = false;
            return;
        }
        else
        {
            if(DEBUG1) printShaderInfoLog(frag_shader);
        }

        glAttachObjectARB(shader, vert_shader);
        glAttachObjectARB(shader, frag_shader);

        glLinkProgramARB(shader);

        if(glIsShader(shader) == false)
        {
            printf("shader failed with error: %s \n", name);
            printShaderInfoLog(shader);
            shader_valid = false;
            return;
        }
        else
        {
            if(DEBUG1) printShaderInfoLog(shader);
        }

        //if(DEBUG1) printProgramInfoLog(shader);
        shader_valid = true;
    }

    int get_attribute(const char* attribute_name)
    {
        if(shader_valid == false) return -1;
        if(attribute_index == 16) GS_ABORT();

        int attribute = glGetAttribLocation(shader, attribute_name);

        if(attribute == -1)
        {
            printf("SHADER: get_attribute failed. %s %s \n", name, attribute_name);
            return -1;
        }
        attribute_array[attribute_index] = attribute;
        attribute_index++;
        return attribute;
    }

    int get_uniform(const char* uniform_name)
    {
        if(shader_valid == false) return -1;
        if(uniform_index == 16) GS_ABORT();
        int uniform = glGetUniformLocation(shader, uniform_name);

        if(uniform == -1)
        {
            printf("SHADER: get_uniform failed. %s %s \n", name, uniform_name);
            return -1;
        }
        uniform_array[uniform_index] = uniform;
        uniform_index++;
        return uniform;
    }   
};
