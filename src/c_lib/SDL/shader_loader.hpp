#pragma once

#include <SDL/SDL_functions.hpp>

//info log print
void print_shader_info_log(GLhandleARB obj);
void print_program_info_log(GLhandleARB obj);

void load_shaders(const char *vert, const char* frag, GLhandleARB* prog);

//returns true on error
bool shader_linking_error(GLhandleARB shader, const char* name);
bool shader_compiler_error(GLhandleARB shader, const char* name);

class Shader
{
    public:
        GLhandleARB shader;
        GLhandleARB frag_shader;
        GLhandleARB vert_shader;
        GLint attribute_array[16]; //atribute list
        int attribute_index;
        GLint uniform_array[16]; //uniform list
        int uniform_index;

        char* name; //shader name
        char* vs;
        char* fs;
        bool debug;
        bool shader_valid;

    Shader() :
        shader(0), frag_shader(0), vert_shader(0),
        attribute_index(0), uniform_index(0), name(NULL), vs(NULL), fs(NULL),
        debug(true), shader_valid(false)
    {
        memset(this->attribute_array, 0, sizeof(this->attribute_array));
        memset(this->uniform_array, 0, sizeof(this->uniform_array));
    }

    ~Shader()
    {
        free(vs);
        free(fs);
        free(this->name);
    }

    void set_debug(bool value)
    {
        this->debug = value;
    }

    void load_shader(const char* _name, const char* vertex_shader_file,
                     const char* fragment_shader_file)
    {
        GS_ASSERT(this->vs == NULL);
        GS_ASSERT(this->fs == NULL);
        //set shader name
        if (this->name != NULL)
        {
            free(this->name);
            name = NULL;
        }
        size_t name_len = strlen(_name);
        this->name = (char*)malloc((name_len+1)*sizeof(char));
        strncpy(this->name, _name, name_len);
        this->name[name_len] = '\0';

        size_t size = 0;
        this->vs = read_file_to_buffer(vertex_shader_file, &size);
        IF_ASSERT(this->vs == NULL) return;
        this->fs = read_file_to_buffer(fragment_shader_file, &size);
        IF_ASSERT(this->fs == NULL)
        {
            free(this->vs);
            this->vs = NULL;
            return;
        }

        shader = glCreateProgramObjectARB();
        vert_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        frag_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

        glShaderSourceARB(vert_shader, 1, (const GLcharARB**)&vs, NULL);
        glShaderSourceARB(frag_shader, 1, (const GLcharARB**)&fs, NULL);

        glCompileShaderARB(vert_shader);
        if (debug)
            print_shader_info_log(vert_shader);
        this->shader_valid = (!shader_compiler_error(vert_shader, this->name));

        glCompileShaderARB(frag_shader);
        if (debug)
            print_shader_info_log(frag_shader);
        this->shader_valid = (!shader_compiler_error(frag_shader, this->name));

        glAttachObjectARB(shader, vert_shader);
        glAttachObjectARB(shader, frag_shader);

        glLinkProgramARB(shader);
        if (debug)
            print_shader_info_log(shader);

        this->shader_valid = (!shader_linking_error(shader, this->name));

        CHECK_GL_ERROR();
    }

    GLint get_attribute(const char* attribute_name)
    {
        if (!this->shader_valid) return -1;
        GS_ASSERT(attribute_index < 16);
        if (attribute_index >= 16) return -1;

        GLint attribute = glGetAttribLocation(shader, attribute_name);
        if (attribute == -1)
        {
            printf("Shader: get_attribute failed. shader: %s attribute: %s\n",
                   name, attribute_name);
            return -1;
        }
        CHECK_GL_ERROR();

        attribute_array[attribute_index] = attribute;
        attribute_index++;
        return attribute;
    }

    GLint get_uniform(const char* uniform_name)
    {
        if (!this->shader_valid) return -1;
        IF_ASSERT(uniform_index >= 16) return -1;
        GLint uniform = glGetUniformLocation(shader, uniform_name);

        IF_ASSERT(uniform == -1)
        {
            printf("Shader: get_uniform failed. shader: %s uniform: %s\n",
                   name, uniform_name);
            return -1;
        }
        CHECK_GL_ERROR();

        uniform_array[uniform_index] = uniform;
        uniform_index++;
        return uniform;
    }


    void enable_attributes()
    {
        GS_ASSERT(shader_valid);
        glUseProgramObjectARB(this->shader);
        for (int i=0; i<attribute_index; i++)
        {
            glEnableVertexAttribArray(attribute_array[i]);
        }
    }

    void disable_attributes()
    {
        GS_ASSERT(shader_valid);
        for (int i=0; i<attribute_index; i++)
        {
            glDisableVertexAttribArray(attribute_array[i]);
        }
        glUseProgramObjectARB(0);
    }

};
