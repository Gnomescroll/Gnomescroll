#include "weapon.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/compat_gl.h>
#include <item/_interface.hpp>
#include <item/particle/_interface.hpp>
#include <SDL/texture_sheet_loader.hpp>
#include <common/draw/draw.hpp>

namespace Animations
{

static int rendered_item = NULL_ITEM_TYPE;
static bool equipped_item_animating = false;
static int equipped_item_animation_tick = 0;
static int equipped_item_animation_tick_nudge = 1;
static bool equipped_item_continuous_animation = false;

// offsets when equipped_item_animating
static float anim_focal_dz = 0.0f;
static float anim_focal_dxy = 0.0f;
static float anim_focal_depth = 0.0f;
static float anim_origin_dz = 0.0f;
static float anim_origin_dxy = 0.0f;
static float anim_origin_depth = 0.0f;

// config; adjustments for aligning weapon sprite
static float focal_dz = 0.0f;
static float focal_dxy = 0.0f;
static float focal_depth = 0.0f;

static float origin_dz = 0.0f;
static float origin_dxy = 0.0f;
static float origin_depth = 0.0f;

static float sprite_scale = 0.3f;

void parse_equipment_sprite_alignment_config()
{
    // get file contents
    char* CONFIG_FILENAME = (char*)
        "./media/config/equipped_sprite_alignment.cfg";
    
    int size = 0;
    char* buffer = read_file_to_buffer(CONFIG_FILENAME, &size);
    GS_ASSERT(buffer != NULL);
    if (buffer == NULL) return;
    GS_ASSERT(size > 0);
    if (size <= 0 && buffer != NULL)
    {
        free(buffer);
        return;
    }
    
    int index = 0;
    int read = 0;
    
    // scanf for alignment config    
    sscanf(buffer+index, "sprite_scale: %f %n", &sprite_scale, &read);
    index += read;
    sscanf(buffer+index, "focal_dz: %f %n", &focal_dz, &read);
    index += read;
    sscanf(buffer+index, "focal_dxy: %f %n", &focal_dxy, &read);
    index += read;
    sscanf(buffer+index, "focal_depth: %f %n", &focal_depth, &read);
    index += read;
    sscanf(buffer+index, "origin_dz: %f %n", &origin_dz, &read);
    index += read;
    sscanf(buffer+index, "origin_dxy: %f %n", &origin_dxy, &read);
    index += read;
    sscanf(buffer+index, "origin_depth: %f %n", &origin_depth, &read);
    index += read;

    GS_ASSERT(index <= size);
    
    // free file contents
    free(buffer);
}

void move_focal_vertical(float delta)
{    // vertical distance
    focal_dz += delta;
}

void move_focal_horizontal(float delta)
{    // horizontal distance
    focal_dxy += delta;
}

void move_focal_depth(float delta)
{    // distance from agent plane
    focal_depth += delta;
}

void move_origin_vertical(float delta)
{    // vertical distance along agent plane
    origin_dz += delta;
}

void move_origin_horizontal(float delta)
{    // horizontal distance along agent plane
    origin_dxy += delta;
}

void move_origin_depth(float delta)
{    // distance from agent plane
    origin_depth += delta;
}

void dilate_equipped_sprite(float delta)
{
    sprite_scale += delta;
}

void print_sprite_alignment_config()
{
    printf("Sprite alignment:\n");
    printf("sprite_scale: %f\n", sprite_scale);
    printf("focal_dz: %f\n", focal_dz);
    printf("focal_dxy: %f\n", focal_dxy);
    printf("focal_depth: %f\n", focal_depth);
    printf("origin_dz: %f\n", origin_dz);
    printf("origin_dxy: %f\n", origin_dxy);
    printf("origin_depth: %f\n", origin_depth);
}

static Vec3 compute_point_offset(
    Vec3 position, Vec3 forward, Vec3 right, Vec3 up,
    float dxy, float dz, float depth)
{
    // move horizontal
    right = vec3_scalar_mult(right, dxy);
    Vec3 final = vec3_add(position, right);

    // move vertical
    up = vec3_scalar_mult(up, dz);
    final = vec3_add(final, up);

    // move depth
    forward = vec3_scalar_mult(forward, depth);
    final = vec3_add(final, forward);
    
    return final;
}

bool draw_sprite_gl_begin()
{
    // setup texture
    using TextureSheetLoader::ItemSheetTexture;
    GS_ASSERT(ItemSheetTexture != 0);
    if (ItemSheetTexture == 0) return false;

    // set up opengl state
    glColor4ub(255,255,255,255);
    
    GL_ASSERT(GL_BLEND, false);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glBindTexture(GL_TEXTURE_2D, ItemSheetTexture);

    // vertex calls
    glBegin(GL_QUADS);

    return true;
}

void draw_sprite_gl_end()
{
    glEnd();
    
    // cleanup
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);

    CHECK_GL_ERROR();
}

static void draw_planar_sprite(int item_type, Vec3 origin, Vec3 right, Vec3 up)
{
    origin = quadrant_translate_position(current_camera_position, origin);
    int item_sprite = Item::get_sprite_index_for_type(item_type);
    if (item_sprite == ERROR_SPRITE) return;

    // get texture coordinates
    const float SPRITE_WIDTH = 1.0f/16.0f;
    const int TEXTURE_SPRITE_WIDTH = 16;
    float tx_min = (item_sprite % TEXTURE_SPRITE_WIDTH) * SPRITE_WIDTH;
    float tx_max = tx_min + SPRITE_WIDTH;
    float ty_min = (item_sprite / TEXTURE_SPRITE_WIDTH) * SPRITE_WIDTH;
    float ty_max = ty_min + SPRITE_WIDTH;

    Vec3 p = vec3_sub(origin, vec3_add(right, up));
    glTexCoord2f(tx_max, ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(origin, vec3_sub(up, right));
    glTexCoord2f(tx_max, ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(origin, vec3_add(up, right));
    glTexCoord2f(tx_min, ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(origin, vec3_sub(right, up));
    glTexCoord2f(tx_min, ty_max);
    glVertex3f(p.x, p.y, p.z);
}

// used for restoring cull face state
static GLboolean cull_face_enabled = false;
static GLint cull_face_mode = GL_BACK;

bool draw_voxel_gl_begin(GLint cull_mode)
{
    GS_ASSERT(t_map::block_textures_normal != 0);
    if (t_map::block_textures_normal == 0) return false;
    
    glColor4ub(255,255,255,255);

    GL_ASSERT(GL_BLEND, false);
    GL_ASSERT(GL_ALPHA_TEST, false);

    // save culling state
    cull_face_enabled = glIsEnabled(GL_CULL_FACE);
    glGetIntegerv(GL_CULL_FACE_MODE, &cull_face_mode);
    
    // enable backface culling
    if (!cull_face_enabled)
        glEnable(GL_CULL_FACE);
    if (cull_face_mode != cull_mode && cull_mode != GL_INVALID_ENUM)
        glCullFace(cull_mode);    // backface culling
    
    // draw textured voxels
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal); // block texture sheet

    glBegin(GL_QUADS);

    return true;
}

void draw_voxel_gl_end()
{
    glEnd();

    // restore culling state
    if (cull_face_mode != GL_INVALID_ENUM)
        glCullFace(cull_face_mode);
    if (!cull_face_enabled)
        glDisable(GL_CULL_FACE);

    CHECK_GL_ERROR();
}

static void draw_voxel(int item_type, Vec3 origin, Vec3 forward, Vec3 right, Vec3 up)
{
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    const int sprite_id = Item::get_particle_voxel_texture(item_type);
    GS_ASSERT(sprite_id != ERROR_SPRITE);
    const float sprite_width = 32.0f/512.0f;
    
    float tx = (2.0f / 32.0f) * (sprite_id % (512/32));
    float ty = (2.0f / 32.0f) * (sprite_id / (512/32));
    
    Draw::drawTexturedMinivox(
        origin, right, forward, up,
        tx, ty, sprite_width);
}

void draw_equipped_item(int item_type)
{    // draw item in hud
    if (item_type == NULL_ITEM_TYPE)
        item_type = Item::get_item_type("fist");

    GS_ASSERT(!equipped_item_animating || rendered_item == item_type);
    if (equipped_item_animating && rendered_item != item_type)
    {
        const char* old_name = Item::get_item_name(rendered_item);
        const char* new_name = Item::get_item_name(item_type);
        GS_ASSERT(old_name != NULL);
        GS_ASSERT(new_name != NULL);
        GS_ASSERT(strcmp(old_name, new_name) != 0);
        if (old_name != NULL && new_name != NULL)
            printf("Equipped item animating but weapon switched from %s to %s\n",
                old_name, new_name);
    }
        
    rendered_item = item_type;

    if (agent_camera == NULL) return;
    
    // camera state
    Vec3 position = agent_camera->get_position();
    Vec3 forward = agent_camera->forward_vector();
    Vec3 camera_right = agent_camera->right_vector();
    Vec3 up = agent_camera->up_vector();

    // calculate focal,origin points from camera and focal/origin deltas
    Vec3 focal;
    Vec3 origin;
    
    if (equipped_item_animating)
    {    // use animated state
        focal = compute_point_offset(
            position, forward, camera_right, up,
            anim_focal_dxy, anim_focal_dz, anim_focal_depth);

        origin = compute_point_offset(
            position, forward, camera_right, up,
            anim_origin_dxy, anim_origin_dz, anim_origin_depth);
    }
    else
    {    // use defaults
        focal = compute_point_offset(
            position, forward, camera_right, up,
            focal_dxy, focal_dz, focal_depth);

        origin = compute_point_offset(
            position, forward, camera_right, up,
            origin_dxy, origin_dz, origin_depth);
    }

    // use focal and origin points to calculate right vector

    Vec3 right = vec3_init(1,0,0);
    GS_ASSERT(!vec3_equal(focal, origin));
    if (!vec3_equal(focal, origin))
        right = vec3_sub(focal, origin);
    normalize_vector(&right);

    forward = vec3_cross(right, up);
    normalize_vector(&forward);

    // scale to size
    up = vec3_scalar_mult(up, sprite_scale);
    right = vec3_scalar_mult(right, sprite_scale);
    forward = vec3_scalar_mult(forward, sprite_scale);

    origin = translate_position(origin);
    
    GL_ASSERT(GL_DEPTH_TEST, false);
    if (Item::item_type_is_voxel(item_type))
    {
        bool works = draw_voxel_gl_begin(GL_FRONT);
        if (works)
        {
            draw_voxel(item_type, origin, forward, right, up);
            draw_voxel_gl_end();
        }
    }
    else
    {
        bool works = draw_sprite_gl_begin();
        if (works)
        {
            draw_planar_sprite(item_type, origin, right, up);
            draw_sprite_gl_end();
        }
    }
}

static bool get_other_agent_render_params(int agent_id, Vec3* pOrigin, Vec3* pForward, Vec3* pRight, Vec3* pUp)
{    // draw item in other players' hands
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;

    Agent_state* a = ClientState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return false;

    class Voxel_volume* vv = a->get_arm();
    GS_ASSERT(vv != NULL);
    if (vv == NULL) return false;
    
    // HACKED UP MODEL DEPENDENT CRAP
    struct Vec3 right = vec3_scalar_mult(a->arm_up(), -1);
    float offset = (((vv->zdim)/2) * vv->scale) + (sprite_scale / 2.0f);
    struct Vec3 origin = vec3_add(a->arm_center(), vec3_scalar_mult(right, offset));
    origin = translate_position(origin);
    
    if (sphere_fulstrum_test_translate(origin.x, origin.y, origin.z, sprite_scale) == false)
        return false;

    struct Vec3 up = a->arm_forward();
    struct Vec3 forward = a->arm_right();

    // scale to size
    *pUp = vec3_scalar_mult(up, sprite_scale);
    *pRight = vec3_scalar_mult(right, sprite_scale);
    *pForward = vec3_scalar_mult(forward, sprite_scale);

    *pOrigin = origin;
    
    return true;
}

void draw_equipped_voxel_item_other_agent(int agent_id, int item_type)
{
    static int fist = Item::get_item_type("fist");
    if (item_type == NULL_ITEM_TYPE || item_type == fist) return;    // dont draw a fist
    Vec3 origin, forward, right, up;
    bool valid = get_other_agent_render_params(agent_id, &origin, &forward, &right, &up);
    if (!valid) return;
    draw_voxel(item_type, origin, forward, right, up);
}

void draw_equipped_sprite_item_other_agent(int agent_id, int item_type)
{
    static int fist = Item::get_item_type("fist");
    if (item_type == NULL_ITEM_TYPE || item_type == fist) return;    // dont draw a fist
    Vec3 origin, forward, right, up;
    bool valid = get_other_agent_render_params(agent_id, &origin, &forward, &right, &up);
    if (!valid) return;
    draw_planar_sprite(item_type, origin, right, up);
}

void begin_equipped_item_animation(int item_type, bool continuous)
{
    stop_equipped_item_animation();
    
    if (item_type == NULL_ITEM_TYPE)
        item_type = Item::get_item_type("fist");
        
    // begin action animation for item type
    equipped_item_animating = true;
    rendered_item = item_type;
    equipped_item_continuous_animation = continuous;

    equipped_item_animation_tick = 0;
    
    // copy default state
    anim_focal_dz = focal_dz;
    anim_focal_dxy = focal_dxy;
    anim_focal_depth = focal_depth;
    anim_origin_dz = origin_dz;
    anim_origin_dxy = origin_dxy;
    anim_origin_depth = origin_depth;
}

void tick_equipped_item_animation()
{
    if (!equipped_item_animating) return;

    equipped_item_animation_tick += equipped_item_animation_tick_nudge;

    const int duration = 10; // ticks
    if (equipped_item_animation_tick > duration)
    {
        if (equipped_item_continuous_animation)
            equipped_item_animation_tick_nudge *= -1;
        else
        {
            stop_equipped_item_animation();
            return;
        }
    }
    if (equipped_item_animation_tick < 0)
    {
        if (equipped_item_continuous_animation)
            equipped_item_animation_tick_nudge *= -1;
    }
    
    // calculate offsets based on tick value
    const float delta = 0.05f;
    anim_origin_depth = origin_depth;
    if (equipped_item_animation_tick < duration/2)
        anim_origin_depth += delta * (float)equipped_item_animation_tick;
    else
        anim_origin_depth += delta * (float)(duration - equipped_item_animation_tick);
    
    // clamp
    if (anim_origin_depth < origin_depth) anim_origin_depth = origin_depth;
}

void stop_equipped_item_animation()
{
    // force stop current action animation
    equipped_item_animating = false;
    equipped_item_animation_tick = 0;
    equipped_item_animation_tick_nudge = 1;
}
    
void draw_placement_outline(int item_type)
{
    GL_ASSERT(GL_TEXTURE_2D, false);
    
    int cube_height = Item::get_item_cube_height(item_type);
    if (cube_height <= 0) return;

    // get open block
    const int max_dist = 4.0f;
    const int z_low = 4;
    const int z_high = 3;
    int* b = ClientState::playerAgent_state.nearest_open_block(max_dist, z_low, z_high);
    if (b == NULL) return;

    // center it

    glColor4ub(255,255,255,255);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int z=0; z<cube_height; z++)
    {
        float size;
        struct Vec3 p = vec3_init(b[0], b[1], b[2]+z);
        p = quadrant_translate_position(current_camera_position, p);
        if (t_map::get(b[0], b[1], b[2]+z) == 0)
        {
            size = 0.995f;
            glColor3ub(1,1,1);
        }
        else
        {
            size = 1.005f;
            glColor3ub(180,20,20);
        }
            
        p = vec3_add(p, vec3_init(0.5f, 0.5f, 0.5f));

        // render
        Vec3 q;
        int k;
        for (int i=0; i<12; i++)
        for (int j=0; j<2; j++)
        {
            k = 3 * vertex_index2[2*i+j];
            q.x = p.x + v_set2[k+0]*size*0.5f;
            q.y = p.y + v_set2[k+1]*size*0.5f;
            q.z = p.z + v_set2[k+2]*size*0.5f;
            glVertex3f(q.x, q.y, q.z);
        }
    }
    glEnd();
    CHECK_GL_ERROR();
}

void init_weapon_sprite()
{
    parse_equipment_sprite_alignment_config();
}

}   // Animations
