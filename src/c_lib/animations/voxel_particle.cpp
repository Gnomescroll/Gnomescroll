#include "voxel_particle.hpp"

#include <animations/common.hpp>
#include <item/particle/item_particle.hpp>
#include <voxel/common_data.h>

namespace Animations
{

class SHADER voxel_particle_shader;

GLint voxel_particle_TexCoord;
GLint voxel_particle_Normal;
GLint voxel_particle_CameraPos;

VertexElementList2* voxel_particle_vlist = NULL;

void init_voxel_particle()
{
    voxel_particle_shader.set_debug(true);

    voxel_particle_shader.load_shader( "voxel particle",
        "./media/shaders/animation/voxel_particle.vsh",
        "./media/shaders/animation/voxel_particle.fsh" );
    
    voxel_particle_TexCoord = voxel_particle_shader.get_attribute("InTexCoord");
    voxel_particle_Normal = voxel_particle_shader.get_attribute("InNormal");
    voxel_particle_CameraPos = voxel_particle_shader.get_uniform("InCameraPos");

    GS_ASSERT(voxel_particle_vlist == NULL);
    voxel_particle_vlist = new VertexElementList2;
}

void teardown_voxel_particle()
{
    if (voxel_particle_vlist != NULL) delete voxel_particle_vlist;
}

static const struct Vec3 base_normals[6] = {
    {{{1,0,0}}},
    {{{0,1,0}}},
    {{{0,0,1}}},
    {{{-1,0,0}}},
    {{{0,-1,0}}},
    {{{0,0,-1}}},
};

void prep_voxel_particles()
{

    /*
        Temp
    */
    static float xrot = 0.0;
    static float yrot = 0.0;
    static float zrot = 0.0;
    yrot += 0.01;
    xrot += 0.005;

    GS_ASSERT(voxel_particle_vlist != NULL);
    if (voxel_particle_vlist == NULL)
    {
        voxel_particle_vlist->vertex_number = 0;
        return;
    }
    
    class ItemParticle::ItemParticle_list* list = ItemParticle::item_particle_list;
    GS_ASSERT(list != NULL);
    if (list == NULL)
    {
        voxel_particle_vlist->vertex_number = 0;
        return;
    }
    
    if (list->n_max <= 0 || list->num <= 0)
    {
        voxel_particle_vlist->vertex_number = 0;
        return;
    }
    
    for (int i=0; i<list->n_max; i++)
    {
        class ItemParticle::ItemParticle* item = list->a[i];
        if (item == NULL) continue;
        if (!item->is_voxel || !item->should_draw) continue;
        item->voxel.delta_rotation(0.01f, 0.0f);
        
        // do fulstrum test
        Vec3 p = quadrant_translate_position(current_camera_position, item->verlet.position);
        Vec3 center = vec3_add(item->voxel.normal, vec3_add(item->voxel.right, vec3_add(item->voxel.forward, p)));
        if (sphere_fulstrum_test(center.x, center.y, center.z, ITEM_PARTICLE_RENDER_SCALE) == false) continue;

        p.z += ITEM_PARTICLE_RENDER_SCALE / 2.0f;   // render offset

        float tx_min = item->voxel.tx;
        float tx_max = item->voxel.tx + item->voxel.sprite_width;
        float ty_min = item->voxel.ty;
        float ty_max = item->voxel.ty + item->voxel.sprite_width;
        
        Vec3 forward = item->voxel.forward;
        Vec3 normal = item->voxel.normal;
        Vec3 right = item->voxel.right;

        // fill vertex buffer
#if 0
        for (int i=0; i<8; i++)
        {
            v_buffer[3*i+0] = v_set2[3*i+0]*forward.x + v_set2[3*i+1]*right.x + v_set2[3*i+2]*normal.x;
            v_buffer[3*i+1] = v_set2[3*i+0]*forward.y + v_set2[3*i+1]*right.y + v_set2[3*i+2]*normal.y;
            v_buffer[3*i+2] = v_set2[3*i+0]*forward.z + v_set2[3*i+1]*right.z + v_set2[3*i+2]*normal.z;
        }
        
        for (int i=0; i<6; i++)
        for (int j=0; j<4; j++)
        {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
#else

        const float voxel_size = 0.25; //MODIFY THIS FOR EACH VOXEL

        struct Vec3 veb[8];     //vertex positions
        struct Vec3 vn[6];      //normals
        struct Vec3 veb2[6*4];  //vertex array for rendering

        for (int i=0; i<8; i++)
        {
            veb[i].x = voxel_size*v_set2[3*i+0];
            veb[i].y = voxel_size*v_set2[3*i+1];
            veb[i].z = voxel_size*v_set2[3*i+2];
        }

        struct Mat3 rotation_matrix = mat3_euler_rotation(xrot, yrot, zrot);    //SET THESE FOR EACH VOXEL

        //rotate normals
        for (int i=0; i<6; i++)
        {
            vn[i] = vec3_apply_rotation(v_normal_vec3[i], rotation_matrix);   //rotate
        }

        //rotate and translate cube vertices
        for (int i=0; i<8; i++)
        {
            veb[i] = vec3_apply_rotation(veb[i], rotation_matrix);  //rotate
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
#endif
        // draw voxel
        for (int i=0; i<6; i++)
        {
            struct Vec3 n = vn[i];
        
            voxel_particle_vlist->push_vertex(veb2[4*i+0], tx_min, ty_min, n);
            voxel_particle_vlist->push_vertex(veb2[4*i+1], tx_min, ty_max, n);
            voxel_particle_vlist->push_vertex(veb2[4*i+2], tx_max, ty_max, n);
            voxel_particle_vlist->push_vertex(veb2[4*i+3], tx_max, ty_min, n);
        }
    }
    
    //voxel_particle_vlist->compute_face_normals(4);    //DEPRECATE
    
    voxel_particle_vlist->buffer();
}

void draw_voxel_particles()
{
    GS_ASSERT(current_camera != NULL);
    if (current_camera == NULL) return;

    GS_ASSERT(t_map::block_textures_normal != 0);
    if (t_map::block_textures_normal == 0) return;

    GS_ASSERT(voxel_particle_vlist != NULL);
    if (voxel_particle_vlist == NULL) return;

    if (voxel_particle_vlist->vertex_number == 0) return;

    glColor4ub(255,255,255,255);

    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

    glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);

    glUseProgramObjectARB(voxel_particle_shader.shader);

    glBindBuffer(GL_ARRAY_BUFFER, voxel_particle_vlist->VBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(voxel_particle_TexCoord);
    glEnableVertexAttribArray(voxel_particle_Normal);

    Vec3 pos = current_camera->get_position();
    glUniform3f(voxel_particle_CameraPos, pos.x, pos.y, pos.z);

    glVertexPointer(3, GL_FLOAT, voxel_particle_vlist->stride, (GLvoid*)0);    
    glVertexAttribPointer(voxel_particle_TexCoord, 2, GL_FLOAT, GL_FALSE, voxel_particle_vlist->stride, (GLvoid*) 12 );
    glVertexAttribPointer(voxel_particle_Normal, 3, GL_FLOAT, GL_FALSE, voxel_particle_vlist->stride, (GLvoid*) 20 );
    
    glDrawArrays(GL_QUADS, 0, voxel_particle_vlist->vertex_number);

    glDisableVertexAttribArray(voxel_particle_TexCoord);
    glDisableVertexAttribArray(voxel_particle_Normal);
    glDisableClientState(GL_VERTEX_ARRAY);
    glUseProgramObjectARB(0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
}

}   // Animations
