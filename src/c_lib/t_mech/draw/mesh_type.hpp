#pragma once

#include <t_mech/_interface.hpp>
#include <t_mech/mech_state.hpp>
#include <t_mech/_interface.hpp>

#include <t_mech/config/_interface.hpp>
#include <t_mech/mesh_loader.hpp>

namespace t_mech
{

//class MechListMeshMeshRenderer
//class MechListMeshShader
//class MechListMeshRenderer

/*
    Use this as example for other shaders
*/

class MechListMeshMeshRenderer
{
    public:
        // visibility will default to private unless you specify it
        struct Vertex
        {
            float x,y,z; //0
            float tx,ty; //12
            float btx,bty; //20  //get rid of this
            unsigned char color[4]; //28
            unsigned char brightness[4]; //32
            unsigned char light[4]; //36

        };

        static const int stride = sizeof(Vertex);

        struct Vertex* va;  //vertex array
        int vi; //vertex index
        int vm; //vertex max

        unsigned int VBO; //for drawing

        struct Vertex v; //set this and then push vertex

    MechListMeshMeshRenderer() :
        va(NULL), vi(0), vm(32), VBO(0)
    {
        this->va = (struct Vertex*)malloc(vm*sizeof(struct Vertex));
        memset(&this->v, 0, sizeof(this->v));
    }

    ~MechListMeshMeshRenderer()
    {
        free(va);
    }


    void vertex3f(float x, float y, float z)
    {
        v.x = x;
        v.y = y;
        v.z = z;
    }

    void tex2f(float tx, float ty)
    {
        v.tx = tx;
        v.ty = ty;
    }

    void light(int l0, int l1)
    {
        v.light[0] = 8 + l0*16;
        v.light[1] = 8 + l1*16;
    }

    void push_vertex()
    {
        va[vi] = v;
        vi++;
        if (vi == vm)
        {
            vm *= 2;
            va = (struct Vertex*) realloc(va, vm*sizeof(struct Vertex));
        }
    }

    void reset()
    {
        vi = 0;
    }

    void buffer()
    {
        if (VBO == 0) glGenBuffers(1, &VBO);

        if (vi != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vi*stride, NULL, GL_STATIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, vi*stride, va, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else
        {
            if (vi > 0)
            {
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        }
    }

};



class MechListMeshShader
{
    public:

    SDL_Surface* s;

    unsigned int texture1;
    class Shader* shader;

    //uniforms
    int CameraPosition;
    //attributes
    int TexCoord;
    int Brightness;
    int InLight;

    MechListMeshShader()
    : s(NULL), shader(NULL)
    {
        init_texture();
        init_shader();
    }

    ~MechListMeshShader()
    {
        if (s != NULL) SDL_FreeSurface(s);
        if (shader != NULL) delete shader;
    }

    void init_shader()
    {
        shader = new Shader;
        shader->set_debug(true);

        shader->load_shader("mech_mesh_shader",
            MEDIA_PATH "shaders/effect/mech_list.vsh",
            MEDIA_PATH "shaders/effect/mech_list.fsh");

        CameraPosition =    shader->get_uniform("CameraPosition");

        TexCoord    =       shader->get_attribute("InTexCoord");
        Brightness  =       shader->get_attribute("InBrightness");
        InLight     =       shader->get_attribute("InLight");
    }

    void init_texture()
    {
        s = create_surface_from_file(MEDIA_PATH "sprites/mech/mesh_00.png");

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        GLenum texture_format = get_texture_format(s);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
        glDisable(GL_TEXTURE_2D);
    }

};


class MechListMeshRenderer
{
    public:

    class MechListMeshShader shader;
    class MechListMeshMeshRenderer vertex_list;

    void draw()
    {
        if (vertex_list.vi == 0) return;
        if (!shader.shader->shader_valid) return;


        glColor4ub(255,255,255,255);

        GL_ASSERT(GL_BLEND, false);
        GL_ASSERT(GL_DEPTH_WRITEMASK, true);

        glDisable(GL_TEXTURE_2D);

        glEnable(GL_CULL_FACE);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_list.VBO);


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shader.texture1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, generate_clut_texture2());

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, t_map::generate_clut_light_texture());

        GLint base_texture = glGetUniformLocation(shader.shader->shader, "base_texture");
        GLint clut_texture = glGetUniformLocation(shader.shader->shader, "clut_texture");
        GLint clut_light_texture = glGetUniformLocation(shader.shader->shader, "clut_light_texture");

        GS_ASSERT(clut_texture != 0);
        GS_ASSERT(base_texture != 0);
        GS_ASSERT(clut_light_texture != 0);

        shader.shader->enable_attributes();

        glUniform1i(base_texture, 1); //Texture unit 0 is for base_texture
        glUniform1i(clut_texture, 2); //Texture unit 1 is for clut_texture
        glUniform1i(clut_light_texture, 3); //Texture unit 2 is for base_texture


        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, vertex_list.stride, (GLvoid*)0);
        glVertexAttribPointer(shader.TexCoord, 2, GL_FLOAT, GL_FALSE, vertex_list.stride, (GLvoid*)12);
        glVertexAttribPointer(shader.InLight, 2, GL_UNSIGNED_BYTE, GL_TRUE, vertex_list.stride, (GLvoid*)36);

        glDrawArrays(GL_QUADS, 0, vertex_list.vi);

        glDisableClientState(GL_VERTEX_ARRAY);

        shader.shader->disable_attributes();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);

        //glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //glDisable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_CULL_FACE);

        CHECK_GL_ERROR();

    }

    void draw_transparent()
    {
        if (vertex_list.vi == 0) return;
        if (!shader.shader->shader_valid) return;

        glColor4ub(255,255,255,255);

        GL_ASSERT(GL_BLEND, true);
        GL_ASSERT(GL_DEPTH_WRITEMASK, false);

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_list.VBO);


        //glBindTexture(GL_TEXTURE_2D, shader.texture1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shader.texture1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, generate_clut_texture());

        GLint base_texture = glGetUniformLocation(shader.shader->shader, "base_texture");
        GLint clut_texture = glGetUniformLocation(shader.shader->shader, "clut_texture");
        //GLint clut_light_texture = glGetUniformLocation(shader.shader->shader, "clut_light_texture");

        GS_ASSERT(clut_texture != 0);
        GS_ASSERT(base_texture != 0);

        shader.shader->enable_attributes();

        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, vertex_list.stride, (GLvoid*)0);
        glVertexAttribPointer(shader.TexCoord, 2, GL_FLOAT, GL_FALSE, vertex_list.stride, (GLvoid*)32);

        //glUniform4f(InTranslation, (GLfloat*) p.f);
        //glUniform3fv(InTranslation, 1, (GLfloat*) p.f);

        glDrawArrays(GL_QUADS, 0, vertex_list.vi);

        glDisableClientState(GL_VERTEX_ARRAY);

        shader.shader->disable_attributes();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glActiveTexture(GL_TEXTURE0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);
        
        CHECK_GL_ERROR();

    }

    MechListMeshRenderer()
    {}

    ~MechListMeshRenderer()
    {}

    void prep();

    void push_render_mesh(const struct Mech &m);

};

/*
struct _MECH
{
    int x,y,z; //position
    int type;  //type
    int direction; //direction

    float size;
    float rotation;
    int offset;

    bool active;
};
*/

//torch
void MechListMeshRenderer::push_render_mesh(const struct Mech &m)
{

/*
    static const float vin[72] =
    {
        1,1,1, 0,1,1, 0,0,1, 1,0,1, //top
        0,1,0, 1,1,0, 1,0,0, 0,0,0, //bottom
        1,0,1, 1,0,0, 1,1,0, 1,1,1, //north
        0,1,1, 0,1,0, 0,0,0, 0,0,1, //south
        1,1,1, 1,1,0, 0,1,0, 0,1,1, //west
        0,0,1, 0,0,0, 1,0,0, 1,0,1  //east
    };
*/

    static class MeshInstance* MI; // load_mesh(const char* filename)
    static class MeshLoader* ML = NULL;
    if(ML == NULL)
    {
        printf("loading mesh: \n");
        ML = new MeshLoader;
        MI = ML->load_mesh(MEDIA_PATH "sprites/mech/mesh/light_rod.mesh");
    }

#if !PRODUCTION
    static int _counter = 0;
    _counter++;

    if(_counter % 60 == 1)
    {
        delete MI;
        MI = NULL;
        MI = ML->load_mesh(MEDIA_PATH "sprites/mech/mesh/light_rod.mesh");
    }
#endif

    float wx = (float) (m.x) + 0.001f;
    float wy = (float) (m.y) + 0.001;
    float wz = (float) (m.z) + 0.0f;

    //fulstrum test
    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;

    wx = quadrant_translate_f(cx, wx);
    wy = quadrant_translate_f(cy, wy);

    if (!sphere_fulstrum_test(wx, wy, wz, 0.6f))
        return;

    //int tex_id = mech_attributes[m.type].sprite;
    GS_ASSERT(mech_attributes[m.type].type != -1);

    //int side = m.side;


    int env_light = t_map::get_envlight(m.x,m.y,m.z);
    int sky_light = t_map::get_skylight(m.x,m.y,m.z); 
    vertex_list.light(sky_light, env_light);

    const int imax = MI->van;
    const MeshInstance::Vertex* va = MI->va;
    for(int i=0; i<imax; i++)
    {
        vertex_list.vertex3f(wx+va[i].x, wy+va[i].y, wz+va[i].z);
        vertex_list.tex2f(va[i].tx, va[i].ty);
        vertex_list.push_vertex();
    }

}


void MechListMeshRenderer::prep()
{

    vertex_list.reset();

    const int mlm = mech_list->mlm;
    const struct Mech* mla = mech_list->mla;

    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == -1) continue;

        if(mla[i].render_type == MECH_RENDER_TYPE_4)
            push_render_mesh(mla[i]);
    }

    vertex_list.buffer();

}

}   // t_mech
