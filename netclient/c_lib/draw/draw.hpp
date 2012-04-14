#pragma once

#if DC_CLIENT
#include <c_lib/camera/camera.hpp>
#include <c_lib/camera/fulstrum_test.hpp>
#endif

#include <c_lib/common/common.hpp>
#include <c_lib/voxel/common_data.h>

void drawAxialBillboardSprite(Vec3 position, int texture_index, float texture_scale)
{
    position.z += texture_scale;

    if (point_fulstrum_test(position.x, position.y, position.z) == false)
        return; // fulstrum check

    Vec3 up = vec3_init(
        model_view_matrix[0]*texture_scale,
        model_view_matrix[4]*texture_scale,
        model_view_matrix[8]*texture_scale
    );
    Vec3 right = vec3_init(
        model_view_matrix[1]*texture_scale,
        model_view_matrix[5]*texture_scale,
        model_view_matrix[9]*texture_scale
    );

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(texture_index%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(texture_index/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    Vec3 p = vec3_sub(position, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z);
}

void drawBillboardSprite(Vec3 position, int texture_index, float texture_scale)
{
    position.z += texture_scale;    // draw from bottom

    Vec3 up = vec3_init(
        model_view_matrix[0]*texture_scale,
        model_view_matrix[4]*texture_scale,
        model_view_matrix[8]*texture_scale
    );
    Vec3 right = vec3_init(
        model_view_matrix[1]*texture_scale,
        model_view_matrix[5]*texture_scale,
        model_view_matrix[9]*texture_scale
    );

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(texture_index%16)* (1.0/16.0);   // TODO -- get dimensions from spritesheet lookup
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(texture_index/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    Vec3 p = vec3_sub(position, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z);
}

void drawColoredMinivox(Vec3 position, Vec3 forward, Vec3 right, Vec3 normal, Color color)
{
    const float
        x0 = position.x,
        y0 = position.y,
        z0 = position.z;

    if (point_fulstrum_test(x0,y0,z0) == false) return; // fulstrum check

    glColor3ub(color.r, color.g, color.b);

    // fill vertex buffer
    int i,j;
    for(i=0; i<8; i++) {
        v_buffer[3*i+0] = v_set[3*i+0]*forward.x + v_set[3*i+1]*right.x + v_set[3*i+2]*normal.x;
        v_buffer[3*i+1] = v_set[3*i+0]*forward.y + v_set[3*i+1]*right.y + v_set[3*i+2]*normal.y;
        v_buffer[3*i+2] = v_set[3*i+0]*forward.z + v_set[3*i+1]*right.z + v_set[3*i+2]*normal.z;
    }
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
    }

    // draw voxel
    for(i=0; i<6; i++) {
        glVertex3f(x0 + s_buffer[12*i+3*0+0], y0+ s_buffer[12*i+3*0+1], z0+ s_buffer[12*i+3*0+2]);
        glVertex3f(x0 + s_buffer[12*i+3*1+0], y0+ s_buffer[12*i+3*1+1], z0+ s_buffer[12*i+3*1+2]);
        glVertex3f(x0 + s_buffer[12*i+3*2+0], y0+ s_buffer[12*i+3*2+1], z0+ s_buffer[12*i+3*2+2]);
        glVertex3f(x0 + s_buffer[12*i+3*3+0], y0+ s_buffer[12*i+3*3+1], z0+ s_buffer[12*i+3*3+2]);
    }
}

void drawTexturedMinivox(Vec3 position, Vec3 forward, Vec3 right, Vec3 normal, int sprite_index, float tx, float ty, float pixel_margin)
{
    const float
        x0 = position.x,
        y0 = position.y,
        z0 = position.z;

    if (point_fulstrum_test(x0,y0,z0) == false) return; //check to see if they are in viewing fulstrum

    float tx_min = tx;
    float tx_max = tx + pixel_margin;
    float ty_min = ty;
    float ty_max = ty + pixel_margin;

    // fill vertex buffer
    int i,j;
    for(i=0; i<8; i++) {
        v_buffer[3*i+0] = v_set[3*i+0]*forward.x + v_set[3*i+1]*right.x + v_set[3*i+2]*normal.x;
        v_buffer[3*i+1] = v_set[3*i+0]*forward.y + v_set[3*i+1]*right.y + v_set[3*i+2]*normal.y;
        v_buffer[3*i+2] = v_set[3*i+0]*forward.z + v_set[3*i+1]*right.z + v_set[3*i+2]*normal.z;
    }
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
    }
    // draw voxel
    for(i=0; i<6; i++) {
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

