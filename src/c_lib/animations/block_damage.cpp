#include "block_damage.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <SDL/draw_functions.hpp>
#include <SDL/texture_loader.hpp>

namespace Animations
{
    
int predicted_block_damage = 0;
bool damaging_block = false;

const int BLOCK_DAMAGE_LEVELS = 6;

static GLuint block_damage_texture = 0;
static const int texture_width = 256;
static const int sprite_width = 32;

static struct Vec3 veb[8];     //vertex positions
static struct Vec3 veb2[6*4];  //vertex array for rendering

static int tick_since_last_damage_change = 0;
static int ticks_begin_fade = 30;
static int ticks_end_fade = 60;

// load texture
void init_block_damage()
{
    GS_ASSERT(block_damage_texture == 0);
    //int ret = create_texture_from_file("./media/sprites/block_damage.png", &block_damage_texture, GL_NEAREST, GL_NEAREST);
    int ret = create_texture_from_file("./media/sprites/block_damage.png", &block_damage_texture, GL_LINEAR, GL_LINEAR);

    GS_ASSERT(ret == 0);
}

static int get_sprite_index(int dmg, int max_dmg)
{
    // calculate dmg level (sprite index)
    if (dmg <= 0) return -1;
    if (dmg > max_dmg) dmg = max_dmg;
    float t = ((float)dmg)/((float)max_dmg);
    int level = (int)(t * BLOCK_DAMAGE_LEVELS);
    if (level >= BLOCK_DAMAGE_LEVELS) level = BLOCK_DAMAGE_LEVELS - 1;
    return level;
}

static void render(int sprite_index, int x, int y, int z, float margin)
{
    GS_ASSERT(block_damage_texture != 0);
    if (block_damage_texture == 0) return;
    
    if (sprite_index < 0) return;
    GS_ASSERT(sprite_index < BLOCK_DAMAGE_LEVELS);

    Vec3 pos = vec3_init(x,y,z);
    pos = vec3_add(pos, vec3_init(0.5f, 0.5f, 0.5f));   // center
    pos = quadrant_translate_position(current_camera_position, pos);

    const float size = (1.0f + margin) / 2.0f;
    
    const float inc = ((float)sprite_width)/((float)texture_width);
    float tx_min = (sprite_index % (texture_width/sprite_width)) * inc;
    float tx_max = tx_min + inc;
    float ty_min = (sprite_index / (texture_width/sprite_width)) * inc;
    float ty_max = ty_min + inc;

    // scale & translate vertices
    for (int i=0; i<8; i++)
    {
        veb[i].x = size*v_set2[3*i+0] + pos.x;
        veb[i].y = size*v_set2[3*i+1] + pos.y;
        veb[i].z = size*v_set2[3*i+2] + pos.z;
    }

    //copy vertices into quad
    for (int i=0; i<6; i++)
    {
        veb2[4*i+0] = veb[q_set[4*i+0]];
        veb2[4*i+1] = veb[q_set[4*i+1]];
        veb2[4*i+2] = veb[q_set[4*i+2]];
        veb2[4*i+3] = veb[q_set[4*i+3]];
    }

    unsigned char alpha = 255;
    float t = tick_since_last_damage_change - ticks_begin_fade;
    t = ((float)ticks_end_fade - t)/(float)ticks_end_fade;
    if (tick_since_last_damage_change > ticks_begin_fade)
        alpha = (unsigned char)((float)alpha) * t;
    glColor4ub(255,255,255,alpha);

    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, block_damage_texture);
        
    // draw block damage overlay
    glBegin(GL_QUADS);
    for (int i=0; i<6; i++)
    {
        glTexCoord2f(tx_min, ty_min);
        glVertex3f(veb2[4*i+0].x, veb2[4*i+0].y, veb2[4*i+0].z);
        
        glTexCoord2f(tx_min, ty_max);
        glVertex3f(veb2[4*i+1].x, veb2[4*i+1].y, veb2[4*i+1].z);
        
        glTexCoord2f(tx_max, ty_max);
        glVertex3f(veb2[4*i+2].x, veb2[4*i+2].y, veb2[4*i+2].z);
        
        glTexCoord2f(tx_max, ty_min);
        glVertex3f(veb2[4*i+3].x, veb2[4*i+3].y, veb2[4*i+3].z);
    }
    glEnd();
    
    glDisable(GL_BLEND);
}

void render_block_damage()
{
    if (Options::animation_level <= 0) return;
    
    if (damaging_block)
        tick_since_last_damage_change = 0;
    else
        tick_since_last_damage_change++;
    
    damaging_block = false;
    
    if (tick_since_last_damage_change > ticks_begin_fade+ticks_end_fade)
        return;
    
    int x=0,y=0,z=0;
    t_map::get_requested_block_position(&x, &y, &z);
    if (x < 0 || y < 0 || z < 0) return;
    int b = t_map::get(x,y,z);
    if (b <= 0) return;
    if (maxDamage(b) == 0xff) return;   // dont render damage for invincible blocks
    
    Vec3 dest = vec3_add(vec3_init(x,y,z), vec3_init(0.5f, 0.5f, 0.5f));
    float dist = vec3_length(vec3_sub(dest, current_camera_position));
    float margin = 0.005f;
    if (dist > 4.0f) margin *= 2;
    if (dist < 8.0f) margin *= 2;
    if (dist > 16.0f) margin *= 2;
    if (dist > 32.0f) margin *= 2;

    int dmg = t_map::requested_block_damage;
    int max_dmg = maxDamage(t_map::requested_block_type);
    dmg += predicted_block_damage;
    if (dmg < 0) dmg = 0;
    if (dmg >= max_dmg) dmg = max_dmg;

    int sprite_index = get_sprite_index(dmg, max_dmg);

    render(sprite_index, x,y,z, margin);
}

}   // Animations
