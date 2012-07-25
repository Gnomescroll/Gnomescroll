#include "block_damage.hpp"

#include <SDL/draw_functions.hpp>
#include <SDL/texture_loader.hpp>

namespace Animations
{
    
int predicted_block_damage = 0;

const int BLOCK_DAMAGE_LEVELS = 6;

static GLuint block_damage_texture = 0;

static struct Vec3 veb[8];     //vertex positions
static struct Vec3 veb2[6*4];  //vertex array for rendering

// load texture
void init_block_damage()
{
    GS_ASSERT(block_damage_texture == 0);
    int ret = create_texture_from_file("./media/sprites/block_damage.png", &block_damage_texture);
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

static void render(int sprite_index, int x, int y, int z)
{
    GS_ASSERT(block_damage_texture != 0);
    if (block_damage_texture == 0) return;
    
    if (sprite_index < 0) return;
    GS_ASSERT(sprite_index < BLOCK_DAMAGE_LEVELS);

    Vec3 pos = vec3_init(x,y,z);
    pos = vec3_add(pos, vec3_init(0.5f, 0.5f, 0.5f));   // center
    pos = quadrant_translate_position(current_camera_position, pos);

    const float MARGIN = 0.01f;
    const float size = (1.0f + MARGIN) / 2.0f;
    
    const float inc = 32.0f/512.0f;
    float tx_min = (sprite_index % 16) * inc;
    float tx_max = tx_min + inc;
    float ty_min = (sprite_index / 16) * inc;
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

    glColor4ub(255,255,255,128);

    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.2f);
    
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
    
    glDisable(GL_ALPHA_TEST);
}

void render_block_damage()
{
    int dmg = t_map::requested_block_damage;
    int max_dmg = maxDamage(t_map::requested_block_type);
    dmg += predicted_block_damage;
    if (dmg < 0) dmg = 0;
    if (dmg >= max_dmg) return;

    int sprite_index = get_sprite_index(dmg, max_dmg);

    int x=0,y=0,z=0;
    t_map::get_requested_block_position(&x, &y, &z);

    render(sprite_index, x,y,z);
}

}   // Animations
