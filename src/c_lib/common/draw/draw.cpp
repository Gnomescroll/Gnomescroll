/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "draw.hpp"

#if DC_SERVER
dont_include_this_file_on_server
#endif

#include <camera/camera.hpp>
#include <camera/fulstrum_test.hpp>
#include <common/common.hpp>
#include <voxel/constants.hpp>

namespace Draw
{

static float v_buffer[3*8];
static float s_buffer[6*(4*3)];

void draw_axial_billboard_sprite(const Vec3& position, int texture_index,
                                 float texture_scale, int sprites_wide)
{
    texture_scale *= 0.5f;
    IF_ASSERT(sprites_wide <= 0) return;
    Vec3 pos = quadrant_translate_position(current_camera_position, position);
    pos.z += texture_scale;

    if (!sphere_fulstrum_test(pos, texture_scale))
        return;

    Vec3 up = vec3_init(0, 0, 1);
    Vec3 forward = vec3_sub(pos, current_camera_position);
    forward.z = 0.0f;
    if (unlikely(vec3_length_squared(forward) == 0))
        return;
    forward = vec3_normalize(forward);
    Vec3 right = vec3_normalize(vec3_cross(forward, up));

    up = vec3_scalar_mult(up, texture_scale);
    right = vec3_scalar_mult(right, texture_scale);

    float factor = 1.0f / sprites_wide;
    float tx_min = float(texture_index % sprites_wide) * factor;
    float tx_max = tx_min + factor;
    float ty_min = float(texture_index / sprites_wide) * factor;
    float ty_max = ty_min + factor;

    Vec3 p = vec3_sub(pos, vec3_add(right, up));
    glTexCoord2f(tx_min, ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(pos, vec3_sub(up, right));
    glTexCoord2f(tx_min, ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(pos, vec3_add(up, right));
    glTexCoord2f(tx_max, ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(pos, vec3_sub(right, up));
    glTexCoord2f(tx_max, ty_max);
    glVertex3f(p.x, p.y, p.z);
}

void drawBillboardSprite(const Vec3& position, int texture_index,
                         float texture_scale, int sprites_wide)
{
    texture_scale *= 0.5f;
    IF_ASSERT(sprites_wide <= 0) return;
    Vec3 pos = quadrant_translate_position(current_camera_position, position);
    pos.z += texture_scale;    // draw from bottom

    Vec3 up = vec3_init(model_view_matrix[0]*texture_scale,
                        model_view_matrix[4]*texture_scale,
                        model_view_matrix[8]*texture_scale);
    Vec3 right = vec3_init(model_view_matrix[1]*texture_scale,
                           model_view_matrix[5]*texture_scale,
                           model_view_matrix[9]*texture_scale);

    float factor = 1.0f / sprites_wide;
    float tx_min = float(texture_index % sprites_wide) * factor;
    float tx_max = tx_min + factor;
    float ty_min = float(texture_index / sprites_wide) * factor;
    float ty_max = ty_min + factor;

    Vec3 p = vec3_sub(pos, vec3_add(right, up));
    glTexCoord2f(tx_min, ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(pos, vec3_sub(up, right));
    glTexCoord2f(tx_max, ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(pos, vec3_add(up, right));
    glTexCoord2f(tx_max, ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(pos, vec3_sub(right, up));
    glTexCoord2f(tx_min, ty_min);
    glVertex3f(p.x, p.y, p.z);
}

void drawColoredMinivox(const Vec3& position, const Vec3& forward,
                        const Vec3& right, const Vec3& normal, Color color)
{
    Vec3 p = quadrant_translate_position(current_camera_position, position);
    if (!point_fulstrum_test(p)) return;

    const float x0 = p.x;
    const float y0 = p.y;
    const float z0 = p.z;

    glColor4ub(color.r, color.g, color.b, color.a);

    for (int i=0; i<8; i++)
    {   // fill vertex buffer
        v_buffer[3*i+0] = v_set[3*i+0]*forward.x + v_set[3*i+1]*right.x + v_set[3*i+2]*normal.x;
        v_buffer[3*i+1] = v_set[3*i+0]*forward.y + v_set[3*i+1]*right.y + v_set[3*i+2]*normal.y;
        v_buffer[3*i+2] = v_set[3*i+0]*forward.z + v_set[3*i+1]*right.z + v_set[3*i+2]*normal.z;
    }

    for (int i=0; i<6; i++)
    for (int j=0; j<4; j++)
    {
        s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
        s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
        s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
    }

    for (int i=0; i<6; i++)
    {   // draw voxel
        glVertex3f(x0 + s_buffer[12*i+3*0+0], y0+ s_buffer[12*i+3*0+1], z0+ s_buffer[12*i+3*0+2]);
        glVertex3f(x0 + s_buffer[12*i+3*1+0], y0+ s_buffer[12*i+3*1+1], z0+ s_buffer[12*i+3*1+2]);
        glVertex3f(x0 + s_buffer[12*i+3*2+0], y0+ s_buffer[12*i+3*2+1], z0+ s_buffer[12*i+3*2+2]);
        glVertex3f(x0 + s_buffer[12*i+3*3+0], y0+ s_buffer[12*i+3*3+1], z0+ s_buffer[12*i+3*3+2]);
    }
}

void drawTexturedMinivox(const Vec3& position, const Vec3& forward,
                         const Vec3& right, const Vec3& normal,
                         float tx, float ty, float sprite_width)
{
    Vec3 p = quadrant_translate_position(current_camera_position, position);
    if (!point_fulstrum_test(p)) return;

    const float x0 = position.x;
    const float y0 = position.y;
    const float z0 = position.z;

    float tx_min = tx;
    float tx_max = tx + sprite_width;
    float ty_min = ty;
    float ty_max = ty + sprite_width;

    for (int i=0; i<8; i++)
    {   // fill vertex buffer
        v_buffer[3*i+0] = v_set[3*i+0]*forward.x + v_set[3*i+1]*right.x + v_set[3*i+2]*normal.x;
        v_buffer[3*i+1] = v_set[3*i+0]*forward.y + v_set[3*i+1]*right.y + v_set[3*i+2]*normal.y;
        v_buffer[3*i+2] = v_set[3*i+0]*forward.z + v_set[3*i+1]*right.z + v_set[3*i+2]*normal.z;
    }

    for (int i=0; i<6; i++)
    for (int j=0; j<4; j++)
    {
        s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
        s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
        s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
    }

    for (int i=0; i<6; i++)
    {   // draw voxel
        glTexCoord2f(tx_min, ty_min);
        glVertex3f(x0 + s_buffer[12*i+3*0+0], y0+ s_buffer[12*i+3*0+1], z0+ s_buffer[12*i+3*0+2]);
        glTexCoord2f(tx_min, ty_max);
        glVertex3f(x0 + s_buffer[12*i+3*1+0], y0+ s_buffer[12*i+3*1+1], z0+ s_buffer[12*i+3*1+2]);
        glTexCoord2f(tx_max, ty_max);
        glVertex3f(x0 + s_buffer[12*i+3*2+0], y0+ s_buffer[12*i+3*2+1], z0+ s_buffer[12*i+3*2+2]);
        glTexCoord2f(tx_max, ty_min);
        glVertex3f(x0 + s_buffer[12*i+3*3+0], y0+ s_buffer[12*i+3*3+1], z0+ s_buffer[12*i+3*3+2]);
    }
}

}   // Draw
