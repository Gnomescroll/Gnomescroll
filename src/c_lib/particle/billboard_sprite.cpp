#include "billboard_sprite.hpp"

namespace Particle
{

#if DC_CLIENT
void BillboardSprite::draw(Vec3 v)
{
    v = quadrant_translate_position(current_camera_position, v);
    if (point_fulstrum_test(v.x, v.y, v.z) == false)
        return;

    Vec3 up = vec3_init(
        model_view_matrix[0]*this->scale,
        model_view_matrix[4]*this->scale,
        model_view_matrix[8]*this->scale
    );
    Vec3 right = vec3_init(
        model_view_matrix[1]*this->scale,
        model_view_matrix[5]*this->scale,
        model_view_matrix[9]*this->scale
    );

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(this->texture_index%16)* (1.0f/16.0f);
    tx_max = tx_min + (1.0f/16.0f);
    ty_min = (float)(this->texture_index/16)* (1.0f/16.0f);
    ty_max = ty_min + (1.0f/16.0f);

    Vec3 p = vec3_sub(v, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z);
}
#endif

#if DC_CLIENT
void BillboardSprite::draw_from_bottom(Vec3 v)
{
    v = quadrant_translate_position(current_camera_position, v);
    //v.z += this->scale / 2;
    v.z += this->scale;
    
    if (point_fulstrum_test(v.x, v.y, v.z) == false)
        return;

    Vec3 up = vec3_init(
        model_view_matrix[0]*this->scale,
        model_view_matrix[4]*this->scale,
        model_view_matrix[8]*this->scale
    );
    Vec3 right = vec3_init(
        model_view_matrix[1]*this->scale,
        model_view_matrix[5]*this->scale,
        model_view_matrix[9]*this->scale
    );

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(this->texture_index%16)* (1.0f/16.0f);
    tx_max = tx_min + (1.0f/16.0f);
    ty_min = (float)(this->texture_index/16)* (1.0f/16.0f);
    ty_max = ty_min + (1.0f/16.0f);

    Vec3 p = vec3_sub(v, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z);
}
#endif

BillboardSprite::BillboardSprite()
:
scale(1.0f)
#if DC_CLIENT
,texture_index(0)
#endif
{}

}   // Particle
