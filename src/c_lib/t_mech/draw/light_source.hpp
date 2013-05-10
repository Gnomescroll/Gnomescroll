#pragma once

#include <t_mech/_interface.hpp>
#include <t_mech/mech_state.hpp>
#include <t_mech/_interface.hpp>

#include <t_mech/config/_interface.hpp>
#include <t_mech/mesh_loader.hpp>

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <physics/vec3.hpp>
#include <animations/common.hpp>


using Animations::VertexElementListTexture;

namespace t_mech
{



class MechLightEffect
{
    public:

    unsigned int texture;
    class Shader shader;
    unsigned int shader_TexCoord;

    VertexElementListTexture* insect_mob_vlist;

    void init()
    {
        insect_mob_vlist = NULL;
        insect_mob_vlist = new VertexElementListTexture;

        init_texture();
        init_shader();
    }


    MechLightEffect()
    {
    }

    ~MechLightEffect()
    {
        delete insect_mob_vlist;
    }

    void init_texture()
    {
        SDL_Surface* s = create_surface_from_file(MEDIA_PATH "sprites/mech/light_00.png");
        IF_ASSERT(s == NULL)
            return;

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        GLenum internalFormat = 4; //GL_RGBA;
        GLenum format = get_texture_format(s);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels);
        glDisable(GL_TEXTURE_2D);

        //SDL_FreeSurface(s);
    }

    void init_shader()
    {
        shader.set_debug(false);

        shader.load_shader("mech light source shader",
            MEDIA_PATH "shaders/effect/light_source.vsh",
            MEDIA_PATH "shaders/effect/light_source.fsh");

        shader_TexCoord = shader.get_attribute("InTexCoord");
    }


    inline void prep_light0()
    {

    /*
        float x = quadrant_translate_f(current_camera_position.x, this->x);
        float y = quadrant_translate_f(current_camera_position.y, this->y);
        float z = this->z;


        v1 = vec3_init(x, y, z2+z);
        v2 = vec3_init(x+tw*sinf(f1*i), y+th*cosf(f1*i), z0+z);
        v3 = vec3_init(x+tw*sinf(f1*(i+1)), y+th*cosf(f1*(i+1)), z0+z);

        insect_mob_vlist->push_vertex(v1, 0.5f, 0.5f);
        insect_mob_vlist->push_vertex(v2, sinf(f1*i)/2 + 0.5f,  cosf(f1*i)/2 + 0.5f);
        insect_mob_vlist->push_vertex(v3, sinf(f1*(i+1))/2 + 0.5f,  cosf(f1*(i+1))/2 + 0.5f);
    */
    }


    void prep()
    {
        prep_light0();


        const int mlm = mech_list->mlm;
        const struct Mech* mla = mech_list->mla;


        for (int i=0; i<mlm; i++)
        {
        /*
            if (mla[i].id == -1) continue;

            switch (mla[i].render_type)
            {
                case MECH_RENDER_TYPE_0:
                    push_crystal_vertex(mla[i]);
                    break;
                case MECH_RENDER_TYPE_1:
                    break;
                case MECH_RENDER_TYPE_2:
                    break;
                case MECH_RENDER_TYPE_3:
                    push_render_type_3(mla[i]);
                    break;
                case MECH_RENDER_TYPE_4: //torch
                    push_render_type_4(mla[i]);
                    break;
                case MECH_RENDER_TYPE_NONE:
                    GS_ASSERT(false);
                    break;
            }
        */

        }

        insect_mob_vlist->buffer();
    }

    void draw()
    {

        if (insect_mob_vlist->vertex_number == 0) return;

        GS_ASSERT(insect_mob_vlist->VBO != 0);
        if (insect_mob_vlist->VBO == 0) return;

        glColor3ub(255,255,255);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindBuffer(GL_ARRAY_BUFFER, insect_mob_vlist->VBO);

        glUseProgramObjectARB(shader.shader);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableVertexAttribArray(shader_TexCoord);

        int offset = 0;
        glVertexPointer(3, GL_FLOAT, insect_mob_vlist->stride, (GLvoid*)offset);
        offset += 3 * sizeof(GL_FLOAT);
        glVertexAttribPointer(shader_TexCoord, 2, GL_FLOAT, GL_FALSE, insect_mob_vlist->stride, (GLvoid*)offset);

        glDrawArrays(GL_TRIANGLES,0, insect_mob_vlist->vertex_number);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableVertexAttribArray(shader_TexCoord);
        glUseProgramObjectARB(0);


    }
};



}   // t_mech
