#pragma once

#if DC_CLIENT
#include <c_lib/camera/camera.hpp>
#endif

void drawBillboardSprite(Vec3 position, int texture_index, float texture_scale)
{
    #if DC_CLIENT
    position.z += texture_scale;
    
    if (current_camera == NULL
    || !current_camera->in_view(position.x, position.y, position.z))
        return;

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
    #endif
}
