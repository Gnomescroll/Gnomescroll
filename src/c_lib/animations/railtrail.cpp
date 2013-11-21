/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "railtrail.hpp"

namespace Animations
{

const int RAILTRAIL_TTL = 3;

inline void RailTrailEffect::reset()
{
    this->ttl = RAILTRAIL_TTL;
}

//void RailTrailEffect::tick()
//{
    ////const float tick_rate = 1.0f / 30.0f;
    ////this->start = vec3_add(this->start, vec3_scalar_mult(this->end, tick_rate));
    ////this->start = translate_position(this->start);

    //// spin particles
//}

//void RailTrailEffect::draw(Vec3 camera)
//{

//}

void RailTrailEffect::tick()
{
    if (Options::animation_level <= 0) return;

    static const float span = 0.5f; // space between particle layers
    static const float spin_span = PI / 8;

    const float dist = vec3_distance(this->end, this->start);
    const float step = span / (dist);
    Vec3 fwd = vec3_sub(this->end, this->start);
    //this->start = vec3_add(this->start, vec3_scalar_mult(vec3_normalize(this->start), 0.005f));

    // RIGHT SETTINGS WOULD LOOK GOOD ON MOB IMPACTS ---> voxel_explode(this->end, /*min*/ 4, /*max*/ 14, /*size*/ 0.2f, /*force*/ 0.1f, COLOR_GREEN);

    float theta, phi;
    vec3_to_angles(fwd, &theta, &phi);

    float curr_spin = 0.0f;
    //float curr_spin = (PI/16) * this->ttl;
    for (float fl=0.0f; fl<=1.0f; fl+=step)
    {
        Vec3 curr = vec3_interpolate(this->start, this->end, fl);
        //float r = 0.17f; // quadratic radius
        float r = 0.08f; // quadratic radius

        Vec3 spiral = vec3_init(
            r * cosf(curr_spin),
            0,
            r * sinf(curr_spin)
        );
        Vec3 spiral2 = vec3_init(
            r * cosf(curr_spin + PI),
            0,
            r * sinf(curr_spin + PI)
        );

        spiral = vec3_euler_rotation(spiral, theta-0.5f, 0, phi-0.5f);
        spiral2 = vec3_euler_rotation(spiral2, theta-0.5f, 0, phi-0.5f);
        spiral = vec3_add(curr, spiral);
        spiral2 = vec3_add(curr, spiral2);

        spiral = translate_position(spiral);
        spiral2 = translate_position(spiral2);

        //float anim_scale = float(Options::animation_level)/3.0f;
        //n = anim_scale*float(n);
        Particle::Shrapnel *s;

        s = Particle::create_shrapnel(spiral, /*vel*/ vec3_init(0,0,0));
        if (s == NULL) return;

        //s->ttl = randrange(8, 15);
        //s->scale = 0.1f;
        //s->texture_index = 54;
        s->ttl = randrange(2, 4);
        //s->ttl = 2;
        s->scale = 0.06f;
        s->texture_index = 22;

        s = Particle::create_shrapnel(spiral2, /*vel*/ vec3_init(0,0,0));
        if (s == NULL) return;

        //s->ttl = randrange(8, 15);
        //s->scale = 0.1f;
        s->texture_index = 54;
        s->ttl = randrange(2, 4);
        //s->ttl = 2;
        s->scale = 0.06f;
        s->texture_index = 22;

        //draw_quad(curr, span, theta, phi);
        curr_spin += spin_span;
        if (curr_spin >= PI*2)
            curr_spin = 0.0f;
    }
}

void RailTrailEffect::draw_quad(const Vec3& p, float r, float theta, float phi) // quadratic radius
{ // with no rotation modifications, it faces upwards
    static const float tx_min = 0.0f;
    static const float tx_max = 1.0f;
    static const float ty_min = 0.0f;
    static const float ty_max = 1.0f;

    Vec3 bl, tl, tr, br;
    bl = tl = tr = br = vec3_init(0.0f);

    // FIXME: MOVE SHIT LIKE THIS TO BE CALC'ED ONLY ONCE then applied to all the interpolated points
    // rotate the y & z (pitch) .... prob need to reverse/mirror the y
    float radian = phi*PI;

    // first setup up-facing quad with correct PITCH
    // bottom edge
    bl.y = br.y = -r*sinf(radian);
    bl.z = br.z = r*cosf(radian);
    //printf("bl.y: %8.2f  bl.z: %8.2f \n", bl.y, bl.z);

    // top edge
    tl.y = tr.y = r*sinf(radian);
    tl.z = tr.z = -r*cosf(radian);
    //printf("tl.y: %8.2f  tl.z: %8.2f \n", tl.y, tl.z);

    // sides
    bl.x = tl.x = -r;
    br.x = tr.x = r;

    // do YAW
    bl = vec3_euler_rotation(bl, theta-0.5f, 0, 0);
    tl = vec3_euler_rotation(tl, theta-0.5f, 0, 0);
    tr = vec3_euler_rotation(tr, theta-0.5f, 0, 0);
    br = vec3_euler_rotation(br, theta-0.5f, 0, 0);

    // translate to world space
    bl = vec3_add(p, bl);
    tl = vec3_add(p, tl);
    tr = vec3_add(p, tr);
    br = vec3_add(p, br);

    // draw
    glTexCoord2f(tx_max, ty_max);
    glVertex3f(bl.x, bl.y, bl.z);  // Bottom left
    glTexCoord2f(tx_min, ty_max);
    glVertex3f(tl.x, tl.y, tl.z);  // Top left
    glTexCoord2f(tx_min, ty_min);
    glVertex3f(tr.x, tr.y, tr.z);  // Top right
    glTexCoord2f(tx_max, ty_min);
    glVertex3f(br.x, br.y, br.z);  // Bottom right
}

//void RailTrailEffectList::draw()
//{
    //IF_ASSERT(current_camera == NULL) return;
    //if (this->num == 0) return;

    ////_LAST_TICK();
    ////_GET_MS_TIME();

    //glColor3ub(255,255,255);

    //GL_ASSERT(GL_DEPTH_TEST, true);
    //GL_ASSERT(GL_DEPTH_WRITEMASK, false);
    //GL_ASSERT(GL_BLEND, true);

    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, hitscan_texture_id);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    //glBegin(GL_QUADS);

    //for (size_t i=0; i<this->num; i++)
        //a[i].draw(current_camera->get_position());

    //glEnd();
    //glDisable(GL_TEXTURE_2D);
//}

void RailTrailEffectList::tick()
{
    for (size_t i=0; i<this->num; i++)
    {
        a[i].tick();
        a[i].ttl--;

        if (a[i].ttl <= 0)
            destroy(i);
    }
}

}   // Animations
