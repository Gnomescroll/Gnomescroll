#include "voxel_particle.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <animations/common.hpp>
#include <item/particle/item_particle.hpp>
#include <voxel/constants.hpp>
#include <common/color.hpp>

namespace Animations
{

class Shader textured_voxel_particle_shader;
GLint textured_voxel_particle_TexCoord;
GLint textured_voxel_particle_Normal;
//GLint textured_voxel_particle_CameraPos;
VertexElementListTextureNormal* textured_voxel_particle_vlist = NULL;

class Shader colored_voxel_particle_shader;
GLint colored_voxel_particle_Normal;
GLint colored_voxel_particle_Color;
//GLint colored_voxel_particle_CameraPos;
VertexElementListColor* colored_voxel_particle_vlist = NULL;

void init_voxel_particle()
{
    textured_voxel_particle_shader.set_debug(true);

    textured_voxel_particle_shader.load_shader( "textured voxel particle",
        MEDIA_PATH "shaders/animation/textured_voxel_particle.vsh",
        MEDIA_PATH "shaders/animation/textured_voxel_particle.fsh" );

    textured_voxel_particle_TexCoord = textured_voxel_particle_shader.get_attribute("InTexCoord");
    textured_voxel_particle_Normal = textured_voxel_particle_shader.get_attribute("InNormal");
    //textured_voxel_particle_CameraPos = textured_voxel_particle_shader.get_uniform("InCameraPos");

    GS_ASSERT(textured_voxel_particle_vlist == NULL);
    textured_voxel_particle_vlist = new VertexElementListTextureNormal;

    colored_voxel_particle_shader.set_debug(true);

    colored_voxel_particle_shader.load_shader( "colored voxel particle",
        MEDIA_PATH "shaders/animation/colored_voxel_particle.vsh",
        MEDIA_PATH "shaders/animation/colored_voxel_particle.fsh" );

    colored_voxel_particle_Normal = colored_voxel_particle_shader.get_attribute("InNormal");
    colored_voxel_particle_Color = colored_voxel_particle_shader.get_attribute("InColor");
    //colored_voxel_particle_CameraPos = colored_voxel_particle_shader.get_uniform("InCameraPos");

    GS_ASSERT(colored_voxel_particle_vlist == NULL);
    colored_voxel_particle_vlist = new VertexElementListColor;
}

void teardown_voxel_particle()
{
    delete textured_voxel_particle_vlist;
    delete colored_voxel_particle_vlist;
}

static void prep_textured_voxel_particles()
{
    IF_ASSERT(textured_voxel_particle_vlist == NULL) return;
    using ItemParticle::item_particle_list;
    IF_ASSERT(item_particle_list == NULL) return;
    class Particle::TexturedMinivox_list* particle_list = Particle::textured_minivox_list;
    IF_ASSERT(particle_list == NULL)
    {
        textured_voxel_particle_vlist->vertex_number = 0;
        return;
    }

    if ((item_particle_list->max <= 0 || item_particle_list->ct <= 0) &&
        (particle_list->n_max <= 0 || particle_list->num <= 0))
    {
        textured_voxel_particle_vlist->vertex_number = 0;
        return;
    }

    static struct Vec3 veb[8];     //vertex positions
    static struct Vec3 vn[6];      //normals
    static struct Vec3 veb2[6*4];  //vertex array for rendering

    static const float tp = 1.0f/512.0f;    // pixel width
    static const float tw = tp * 32.0f;     // sprite width
    static const float th = tw;             // sprite height

    for (size_t i=0; i<item_particle_list->max; i++)
    {
        if (item_particle_list->objects[i].id == item_particle_list->null_id) continue;
        class ItemParticle::ItemParticle* item = &item_particle_list->objects[i];
        if (!item->is_voxel || !item->should_draw) continue;
        item->voxel.delta_rotation(0.01f, 0.0f);
        if (!t_map::isValidCube(item->voxel.cube_type)) continue;

        // frustum test
        float size = item->voxel.size;
        struct Vec3 p = quadrant_translate_position(current_camera_position, item->verlet.position);
        struct Vec3 center = p;
        center.x += size;
        center.y += size;
        if (!sphere_fulstrum_test(center.x, center.y, center.z, size)) continue;

        p.z += size;   // render offset

        for (int i=0; i<8; i++)
        {
            veb[i].x = size*v_set2[3*i+0];
            veb[i].y = size*v_set2[3*i+1];
            veb[i].z = size*v_set2[3*i+2];
        }

        //rotate normals
        for (int i=0; i<6; i++)
        {
            vn[i] = vec3_apply_rotation(v_normal_vec3[i], item->voxel.rotation_matrix);   //rotate
        }

        //rotate and translate cube vertices
        for (int i=0; i<8; i++)
        {
            veb[i] = vec3_apply_rotation(veb[i], item->voxel.rotation_matrix);  //rotate
            veb[i] = vec3_add(p, veb[i]);                           //translate
        }

        //copy vertices into quad
        for (int i=0; i<6; i++)
        {
            veb2[4*i+0] = veb[q_set[4*i+0]];
            veb2[4*i+1] = veb[q_set[4*i+1]];
            veb2[4*i+2] = veb[q_set[4*i+2]];
            veb2[4*i+3] = veb[q_set[4*i+3]];
        }

        // draw voxel
        for (int i=0; i<6; i++)
        {
            int tex_id = t_map::get_cube_side_texture(item->voxel.cube_type, i);

            float tx = (tex_id % 16) * tw;
            float ty = (tex_id / 16) * th;

            float tx_min = tx;
            float tx_max = tx + tw;
            float ty_min = ty;
            float ty_max = ty + th;

            textured_voxel_particle_vlist->push_vertex(veb2[4*i+3], vn[i], tx_min, ty_min);
            textured_voxel_particle_vlist->push_vertex(veb2[4*i+0], vn[i], tx_min, ty_max);
            textured_voxel_particle_vlist->push_vertex(veb2[4*i+1], vn[i], tx_max, ty_max);
            textured_voxel_particle_vlist->push_vertex(veb2[4*i+2], vn[i], tx_max, ty_min);
        }
    }

    // iterate particle animations
    for (size_t i=0; i<particle_list->num; i++)
    {
        class Particle::TexturedMinivox* particle = &particle_list->a[i];
        particle->voxel.delta_rotation();

        // frustum test
        float size = particle->voxel.size;
        struct Vec3 p = quadrant_translate_position(current_camera_position, particle->verlet.position);
        struct Vec3 center = p;
        center.x += size;
        center.y += size;
        if (!sphere_fulstrum_test(center.x, center.y, center.z, size)) continue;

        p.z += size;   // render offset

        float tx_min = particle->voxel.tx;
        float tx_max = particle->voxel.tx + particle->voxel.sprite_width;
        float ty_min = particle->voxel.ty;
        float ty_max = particle->voxel.ty + particle->voxel.sprite_width;

        for (int i=0; i<8; i++)
        {
            veb[i].x = size*v_set2[3*i+0];
            veb[i].y = size*v_set2[3*i+1];
            veb[i].z = size*v_set2[3*i+2];
        }

        //rotate normals
        for (int i=0; i<6; i++)
        {
            vn[i] = vec3_apply_rotation(v_normal_vec3[i], particle->voxel.rotation_matrix);   //rotate
        }

        //rotate and translate cube vertices
        for (int i=0; i<8; i++)
        {
            veb[i] = vec3_apply_rotation(veb[i], particle->voxel.rotation_matrix);  //rotate
            veb[i] = vec3_add(p, veb[i]);                           //translate
        }

        //copy vertices into quad
        for (int i=0; i<6; i++)
        {
            veb2[4*i+0] = veb[q_set[4*i+0]];
            veb2[4*i+1] = veb[q_set[4*i+1]];
            veb2[4*i+2] = veb[q_set[4*i+2]];
            veb2[4*i+3] = veb[q_set[4*i+3]];
        }

        // draw voxel
        for (int i=0; i<6; i++)
        {
            textured_voxel_particle_vlist->push_vertex(veb2[4*i+0], vn[i], tx_min, ty_min);
            textured_voxel_particle_vlist->push_vertex(veb2[4*i+1], vn[i], tx_min, ty_max);
            textured_voxel_particle_vlist->push_vertex(veb2[4*i+2], vn[i], tx_max, ty_max);
            textured_voxel_particle_vlist->push_vertex(veb2[4*i+3], vn[i], tx_max, ty_min);
        }
    }

    textured_voxel_particle_vlist->buffer();
}

static void prep_colored_voxel_particles()
{
    GS_ASSERT(colored_voxel_particle_vlist != NULL);
    if (colored_voxel_particle_vlist == NULL)
        return;

    class Particle::ColoredMinivox_list* list = Particle::colored_minivox_list;
    GS_ASSERT(list != NULL);
    if (list == NULL)
    {
        colored_voxel_particle_vlist->vertex_number = 0;
        return;
    }

    if (list->n_max <= 0 || list->num <= 0)
    {
        colored_voxel_particle_vlist->vertex_number = 0;
        return;
    }

    static struct Vec3 veb[8];     //vertex positions
    static struct Vec3 vn[6];      //normals
    static struct Vec3 veb2[6*4];  //vertex array for rendering

    for (size_t i=0; i<list->num; i++)
    {
        Particle::ColoredMinivox* vox = &list->a[i];
        vox->voxel.delta_rotation();

        // frustum test
        float size = vox->voxel.size;
        Vec3 p = quadrant_translate_position(current_camera_position, vox->verlet.position);
        struct Vec3 center = p;
        center.x += size;
        center.y += size;
        if (!sphere_fulstrum_test(center.x, center.y, center.z, size)) continue;

        for (int i=0; i<8; i++)
        {
            veb[i].x = size*v_set2[3*i+0];
            veb[i].y = size*v_set2[3*i+1];
            veb[i].z = size*v_set2[3*i+2];
        }

        //rotate normals
        for (int i=0; i<6; i++)
        {
            vn[i] = vec3_apply_rotation(v_normal_vec3[i], vox->voxel.rotation_matrix);   //rotate
        }

        //rotate and translate cube vertices
        for (int i=0; i<8; i++)
        {
            veb[i] = vec3_apply_rotation(veb[i], vox->voxel.rotation_matrix);  //rotate
            veb[i] = vec3_add(p, veb[i]);                           //translate
        }

        //copy vertices into quad
        for (int i=0; i<6; i++)
        {
            veb2[4*i+0] = veb[q_set[4*i+0]];
            veb2[4*i+1] = veb[q_set[4*i+1]];
            veb2[4*i+2] = veb[q_set[4*i+2]];
            veb2[4*i+3] = veb[q_set[4*i+3]];
        }

        Color c3;

        c3.c[0] = vox->voxel.color.c[0];
        c3.c[1] = vox->voxel.color.c[1];
        c3.c[2] = vox->voxel.color.c[2];

        // draw voxel
        for (int i=0; i<6; i++)
        {
            colored_voxel_particle_vlist->push_vertex(veb2[4*i+0], vn[i], c3);
            colored_voxel_particle_vlist->push_vertex(veb2[4*i+1], vn[i], c3);
            colored_voxel_particle_vlist->push_vertex(veb2[4*i+2], vn[i], c3);
            colored_voxel_particle_vlist->push_vertex(veb2[4*i+3], vn[i], c3);
        }
    }

    colored_voxel_particle_vlist->buffer();
}

void prep_voxel_particles()
{
    prep_textured_voxel_particles();
    prep_colored_voxel_particles();
}

void draw_textured_voxel_particles()
{
    if (!textured_voxel_particle_shader.shader_valid) return;

    GS_ASSERT(current_camera != NULL);
    if (current_camera == NULL) return;

    GS_ASSERT(t_map::block_textures_normal != 0);
    if (t_map::block_textures_normal == 0) return;

    GS_ASSERT(textured_voxel_particle_vlist != NULL);
    if (textured_voxel_particle_vlist == NULL) return;

    if (textured_voxel_particle_vlist->vertex_number == 0) return;

    glColor4ub(255,255,255,255);

    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

    glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);

    glUseProgramObjectARB(textured_voxel_particle_shader.shader);

    glBindBuffer(GL_ARRAY_BUFFER, textured_voxel_particle_vlist->VBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(textured_voxel_particle_TexCoord);
    glEnableVertexAttribArray(textured_voxel_particle_Normal);

    //Vec3 pos = current_camera->get_position();
    //glUniform3f(textured_voxel_particle_CameraPos, pos.x, pos.y, pos.z);

    int offset = 0;
    glVertexPointer(3, GL_FLOAT, textured_voxel_particle_vlist->stride, (GLvoid*)offset);
    offset += 3 * sizeof(GL_FLOAT);
    glVertexAttribPointer(textured_voxel_particle_Normal, 3, GL_FLOAT, GL_FALSE, textured_voxel_particle_vlist->stride, (GLvoid*)offset);
    offset += 3 * sizeof(GL_FLOAT);
    glVertexAttribPointer(textured_voxel_particle_TexCoord, 2, GL_FLOAT, GL_FALSE, textured_voxel_particle_vlist->stride, (GLvoid*)offset);

    glDrawArrays(GL_QUADS, 0, textured_voxel_particle_vlist->vertex_number);

    glDisableVertexAttribArray(textured_voxel_particle_TexCoord);
    glDisableVertexAttribArray(textured_voxel_particle_Normal);
    glDisableClientState(GL_VERTEX_ARRAY);
    glUseProgramObjectARB(0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    CHECK_GL_ERROR();
}

void draw_colored_voxel_particles()
{
    GS_ASSERT_LIMIT(colored_voxel_particle_shader.shader_valid, 1);
    if (!colored_voxel_particle_shader.shader_valid) return;

    GS_ASSERT(current_camera != NULL);
    if (current_camera == NULL) return;

    GS_ASSERT(colored_voxel_particle_vlist != NULL);
    if (colored_voxel_particle_vlist == NULL) return;

    if (colored_voxel_particle_vlist->vertex_number == 0) return;

    glColor4ub(255,255,255,255);

    glEnable(GL_CULL_FACE);

    GL_ASSERT(GL_TEXTURE_2D, false);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

    glUseProgramObjectARB(colored_voxel_particle_shader.shader);

    glBindBuffer(GL_ARRAY_BUFFER, colored_voxel_particle_vlist->VBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(colored_voxel_particle_Normal);
    glEnableVertexAttribArray(colored_voxel_particle_Color);

    //struct Vec3 pos = current_camera->get_position();
    //glUniform3f(colored_voxel_particle_CameraPos, pos.x, pos.y, pos.z);

    int offset = 0;
    glVertexPointer(3, GL_FLOAT, colored_voxel_particle_vlist->stride, (GLvoid*)offset);
    offset += sizeof(struct Vec3);
    glVertexAttribPointer(colored_voxel_particle_Normal, 3, GL_FLOAT, GL_FALSE, colored_voxel_particle_vlist->stride, (GLvoid*)offset);
    offset += sizeof(struct Vec3);
    glVertexAttribPointer(colored_voxel_particle_Color, 3, GL_UNSIGNED_BYTE, GL_TRUE, colored_voxel_particle_vlist->stride, (GLvoid*)offset);
    offset += sizeof(Color);

    glDrawArrays(GL_QUADS, 0, colored_voxel_particle_vlist->vertex_number);

    glDisableVertexAttribArray(colored_voxel_particle_Normal);
    glDisableVertexAttribArray(colored_voxel_particle_Color);
    glDisableClientState(GL_VERTEX_ARRAY);
    glUseProgramObjectARB(0);

    glDisable(GL_CULL_FACE);

    CHECK_GL_ERROR();
}

}   // Animations
