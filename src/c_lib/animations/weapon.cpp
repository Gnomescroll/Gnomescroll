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

static ItemType rendered_item = NULL_ITEM_TYPE;
static bool equipped_item_animating = false;
static int equipped_item_animation_tick = 0;
static int equipped_item_animation_tick_nudge = 1;
static bool equipped_item_continuous_animation = false;

static class EquippedItemConfig sprite_config;
static class EquippedItemConfig voxel_config;
class EquippedItemConfig voxelized_sprite_config;
static class EquippedItemConfig sprite_config_other;
static class EquippedItemConfig voxel_config_other;
static class EquippedItemConfig voxelized_sprite_config_other;

static class EquippedItemConfig* current_config;
static class EquippedItemConfig* current_config_other;

static class EquippedItemAlignment animation_state;

void EquippedItemConfig::parse()
{
    size_t size = 0;
    char* buffer = read_file_to_buffer(this->filename, &size);
    IF_ASSERT(buffer == NULL) return;
    size_t index = 0;
    int read = 0;
    sscanf(&buffer[index], "scale: %f %n", &this->scale, &read);
    index += read;
    sscanf(&buffer[index], "focal.dz: %f %n", &this->alignment.focal.dz, &read);
    index += read;
    sscanf(&buffer[index], "focal.dxy: %f %n", &this->alignment.focal.dxy, &read);
    index += read;
    sscanf(&buffer[index], "focal.depth: %f %n", &this->alignment.focal.depth, &read);
    index += read;
    sscanf(&buffer[index], "origin.dz: %f %n", &this->alignment.origin.dz, &read);
    index += read;
    sscanf(&buffer[index], "origin.dxy: %f %n", &this->alignment.origin.dxy, &read);
    index += read;
    sscanf(&buffer[index], "origin.depth: %f %n", &this->alignment.origin.depth, &read);
    index += read;
    GS_ASSERT(index <= size);
    free(buffer);
}

void init_weapon_sprite()
{
    sprite_config.set_filename("equipped_sprite_alignment.cfg");
    voxel_config.set_filename("equipped_voxel_alignment.cfg");
    voxelized_sprite_config.set_filename("equipped_voxelized_sprite_alignment.cfg");
    sprite_config_other.set_filename("equipped_sprite_alignment_other.cfg");
    voxel_config_other.set_filename("equipped_voxel_alignment_other.cfg");
    voxelized_sprite_config_other.set_filename("equipped_voxelized_sprite_alignment_other.cfg");

    sprite_config.parse();
    voxel_config.parse();
    voxelized_sprite_config.parse();
    sprite_config_other.parse();
    voxel_config_other.parse();
    voxelized_sprite_config_other.parse();

    current_config = &voxelized_sprite_config;
    current_config_other = &voxelized_sprite_config_other;
}

void move_focal_vertical(float delta)
{    // vertical distance
    current_config->alignment.focal.dz += delta;
}

void move_focal_horizontal(float delta)
{    // horizontal distance
    current_config->alignment.focal.dxy += delta;
}

void move_focal_depth(float delta)
{    // distance from agent plane
    current_config->alignment.focal.depth += delta;
}

void move_origin_vertical(float delta)
{    // vertical distance along agent plane
    current_config->alignment.origin.dz += delta;
}

void move_origin_horizontal(float delta)
{    // horizontal distance along agent plane
    current_config->alignment.origin.dxy += delta;
}

void move_origin_depth(float delta)
{    // distance from agent plane
    current_config->alignment.origin.depth += delta;
}

void dilate_equipped_sprite(float delta)
{
    current_config->scale += delta;
}

void cycle_current_config()
{
    const int nconf = 3;
    static int _cur = 0;
    _cur = (_cur+1)%nconf;
    printf("Selected config: ");
    switch (_cur)
    {
        case 0:
            current_config = &voxelized_sprite_config;
            printf("voxelized sprite");
            break;

        case 1:
            current_config = &voxel_config;
            printf("voxel config");
            break;

        case 2:
            current_config = &sprite_config;
            printf("sprite config");
            break;

        default:
            GS_ASSERT(false)
            break;
    }
    printf("\n");

}

void print_sprite_alignment_config()
{
    printf("Sprite:\n");
    sprite_config.print();
    printf("Voxel:\n");
    voxel_config.print();
    printf("Voxelized sprite:\n");
    voxelized_sprite_config.print();
}

static Vec3 compute_point_offset(Vec3 position, Vec3 forward, Vec3 right, Vec3 up,
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
    using TextureSheetLoader::item_texture_sheet_loader;
    GS_ASSERT(item_texture_sheet_loader->texture != 0);
    if (item_texture_sheet_loader->texture == 0) return false;

    // set up opengl state
    glColor4ub(255,255,255,255);

    GL_ASSERT(GL_BLEND, false);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glBindTexture(GL_TEXTURE_2D, item_texture_sheet_loader->texture);

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

static void draw_planar_sprite(ItemType item_type, Vec3 origin, Vec3 right, Vec3 up)
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
GLint cull_face_mode = GL_BACK;

bool draw_voxel_gl_begin(GLint cull_mode)
{
    IF_ASSERT(t_map::block_textures_normal == 0) return false;

    glColor4ub(255,255,255,255);

    GL_ASSERT(GL_BLEND, false);
    GL_ASSERT(GL_ALPHA_TEST, false);
    GS_ASSERT(cull_mode != GL_INVALID_ENUM);

    // save culling state
    cull_face_enabled = glIsEnabled(GL_CULL_FACE);
    glGetIntegerv(GL_CULL_FACE_MODE, &cull_face_mode);

    // enable culling
    if (!cull_face_enabled)
        glEnable(GL_CULL_FACE);
    glCullFace(cull_mode);

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

    glDisable(GL_TEXTURE_2D);

    CHECK_GL_ERROR();
}

static void draw_voxel(ItemType item_type, Vec3 origin, Vec3 forward, Vec3 right, Vec3 up)
{
    IF_ASSERT(item_type == NULL_ITEM_TYPE) return;
    const int sprite_id = Item::get_particle_voxel_texture(item_type);
    GS_ASSERT(sprite_id != ERROR_SPRITE);
    const float sprite_width = 32.0f/512.0f;

    float tx = (2.0f / 32.0f) * (sprite_id % (512/32));
    float ty = (2.0f / 32.0f) * (sprite_id / (512/32));

    Draw::drawTexturedMinivox(
        origin, right, forward, up,
        tx, ty, sprite_width);
}

static void select_current_config(ItemType item_type)
{
    if (Item::item_type_is_voxel(item_type))
        current_config = &voxel_config;
    else if (Options::animation_level <= 1)
            current_config = &sprite_config;
    else
        current_config = &voxelized_sprite_config;
}

static void select_current_config_other(ItemType item_type)
{
    if (Item::item_type_is_voxel(item_type))
        current_config_other = &voxel_config_other;
    else if (Options::animation_level <= 1)
            current_config_other = &sprite_config_other;
    else
        current_config_other = &voxelized_sprite_config_other;
}

void draw_equipped_item(ItemType item_type)
{    // draw item in hud
    if (item_type == NULL_ITEM_TYPE)
        item_type = Item::get_item_type("fist");
    if (equipped_item_animating && rendered_item != item_type)
    {
        //const char* old_name = Item::get_item_name(rendered_item);
        //const char* new_name = Item::get_item_name(item_type);
        //GS_ASSERT(old_name != NULL);
        //GS_ASSERT(new_name != NULL);
        //GS_ASSERT(strcmp(old_name, new_name) != 0);
        //if (old_name != NULL && new_name != NULL)
            //printf("Equipped item animating but weapon switched from %s to %s\n",
                //old_name, new_name);
    }
    rendered_item = item_type;
    IF_ASSERT(agent_camera == NULL) return;
    select_current_config(item_type);

    // camera state
    struct Vec3 position = agent_camera->get_position();
    struct Vec3 forward = agent_camera->forward_vector();
    struct Vec3 camera_right = agent_camera->right_vector();
    struct Vec3 up = agent_camera->up_vector();

    // calculate focal,origin points from camera and focal/origin deltas
    struct Vec3 focal;
    struct Vec3 origin;

    if (equipped_item_animating)
    {    // use animated state
        focal = compute_point_offset(
            position, forward, camera_right, up,
            animation_state.focal.dxy, animation_state.focal.dz, animation_state.focal.depth);

        origin = compute_point_offset(
            position, forward, camera_right, up,
            animation_state.origin.dxy, animation_state.origin.dz, animation_state.origin.depth);
    }
    else
    {    // use defaults
        focal = compute_point_offset(
            position, forward, camera_right, up,
            current_config->alignment.focal.dxy, current_config->alignment.focal.dz, current_config->alignment.focal.depth);

        origin = compute_point_offset(
            position, forward, camera_right, up,
            current_config->alignment.origin.dxy, current_config->alignment.origin.dz, current_config->alignment.origin.depth);
    }

    // use focal and origin points to calculate right vector

    struct Vec3 right = vec3_init(1,0,0);
    GS_ASSERT(!vec3_equal(focal, origin));
    if (!vec3_equal(focal, origin))
        right = vec3_sub(focal, origin);
    right = vec3_normalize(right);

    forward = vec3_cross(right, up);
    forward = vec3_normalize(forward);

    origin = translate_position(origin);

    // scale to size
    up = vec3_scalar_mult(up, current_config->scale);
    right = vec3_scalar_mult(right, current_config->scale);
    forward = vec3_scalar_mult(forward, current_config->scale);

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
        if (Options::animation_level <= 1)
        {
            bool works = draw_sprite_gl_begin();
            if (works)
            {
                draw_planar_sprite(item_type, origin, right, up);
                draw_sprite_gl_end();
            }
        }
        else
        {
            bool works = draw_voxelized_sprite_gl_begin(GL_BACK);
            if (works)
            {
                int sprite_id = Item::get_sprite_index_for_type(item_type);
                origin = quadrant_translate_position(current_camera_position, origin);
                struct Mat4 m;
                mat4_from_vec3(m, forward, right, up, origin);
                draw_voxelized_sprite(sprite_id, m);
                draw_voxelized_sprite_gl_end();
            }
        }

    }
}

static bool get_other_agent_render_params(AgentID agent_id, Vec3* pOrigin, Vec3* pForward, Vec3* pRight, Vec3* pUp)
{    // draw item in other players' hands
    IF_ASSERT(!isValid(agent_id)) return false;

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return false;

    class Voxels::VoxelVolume* vv = a->get_arm();
    IF_ASSERT(vv == NULL) return false;

    // HACKED UP MODEL DEPENDENT CRAP
    struct Vec3 up = a->arm_forward();
    struct Vec3 forward = a->arm_right();
    struct Vec3 right = vec3_scalar_mult(a->arm_up(), -1);
    float offset = (((vv->zdim)/2) * vv->scale);// + (current_config->scale / 2.0f);
    struct Vec3 origin = vec3_add(a->arm_center(), vec3_scalar_mult(right, offset));
    origin = vec3_add(origin, vec3_scalar_mult(right, -0.2f));
    origin = vec3_add(origin, vec3_scalar_mult(forward, 0.02f));
    origin = vec3_add(origin, vec3_scalar_mult(up, -0.17f));
    origin = translate_position(origin);

    if (!sphere_fulstrum_test_translate(origin.x, origin.y, origin.z, current_config->scale))
        return false;

    // scale to size
    *pUp = vec3_scalar_mult(up, current_config->scale);
    *pRight = vec3_scalar_mult(right, current_config->scale);
    *pForward = vec3_scalar_mult(forward, current_config->scale);

    *pOrigin = origin;

    return true;
}

void draw_equipped_voxel_item_other_agent(AgentID agent_id, ItemType item_type)
{
    current_config = &voxel_config_other;
    static int fist = Item::get_item_type("fist");
    if (item_type == NULL_ITEM_TYPE || item_type == fist) return;    // dont draw a fist
    Vec3 origin, forward, right, up;
    bool valid = get_other_agent_render_params(agent_id, &origin, &forward, &right, &up);
    if (!valid) return;
    draw_voxel(item_type, origin, forward, right, up);
}

void draw_equipped_sprite_item_other_agent(AgentID agent_id, ItemType item_type)
{
    current_config = &sprite_config_other;
    static int fist = Item::get_item_type("fist");
    if (item_type == NULL_ITEM_TYPE || item_type == fist) return;    // dont draw a fist
    Vec3 origin, forward, right, up;
    bool valid = get_other_agent_render_params(agent_id, &origin, &forward, &right, &up);
    if (!valid) return;
    draw_planar_sprite(item_type, origin, right, up);
}

void draw_equipped_voxelized_sprite_item_other_agent(AgentID agent_id, ItemType item_type)
{
    current_config = &voxelized_sprite_config_other;
    static int fist = Item::get_item_type("fist");
    if (item_type == NULL_ITEM_TYPE || item_type == fist) return;    // dont draw a fist
    Vec3 origin, forward, right, up;
    bool valid = get_other_agent_render_params(agent_id, &origin, &forward, &right, &up);
    if (!valid) return;
    int sprite_id = Item::get_sprite_index_for_type(item_type);
    struct Mat4 m;
    mat4_from_vec3(m, forward, right, up, origin);
    draw_voxelized_sprite(sprite_id, m);
}

void begin_equipped_item_animation(ItemType item_type, bool continuous)
{
    stop_equipped_item_animation();

    if (item_type == NULL_ITEM_TYPE)
    {
        static ItemType fist = Item::get_item_type("fist");
        item_type = fist;
    }

    // begin action animation for item type
    equipped_item_animating = true;
    rendered_item = item_type;
    equipped_item_continuous_animation = continuous;

    equipped_item_animation_tick = 0;

    // copy default state
    animation_state.focal.dz = current_config->alignment.focal.dz;
    animation_state.focal.dxy = current_config->alignment.focal.dxy;
    animation_state.focal.depth = current_config->alignment.focal.depth;
    animation_state.origin.dz = current_config->alignment.origin.dz;
    animation_state.origin.dxy = current_config->alignment.origin.dxy;
    animation_state.origin.depth = current_config->alignment.origin.depth;
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
    animation_state.origin.depth = current_config->alignment.origin.depth;
    if (equipped_item_animation_tick < duration/2)
        animation_state.origin.depth += delta * (float)equipped_item_animation_tick;
    else
        animation_state.origin.depth += delta * (float)(duration - equipped_item_animation_tick);

    // clamp
    if (animation_state.origin.depth < current_config->alignment.origin.depth) animation_state.origin.depth = current_config->alignment.origin.depth;
}

void stop_equipped_item_animation()
{
    // force stop current action animation
    equipped_item_animating = false;
    equipped_item_animation_tick = 0;
    equipped_item_animation_tick_nudge = 1;
}

void draw_placement_outline(ItemType item_type)
{
    GL_ASSERT(GL_TEXTURE_2D, false);

    int cube_height = Item::get_item_cube_height(item_type);
    if (cube_height <= 0) return;

    // get open block
    const int max_dist = 4.0f;
    struct Vec3i pos;
    bool collided = ClientState::player_agent.nearest_open_block(max_dist, pos);
    if (!collided) return;

    // center it

    glColor4ub(255,255,255,255);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int z=0; z<cube_height; z++, pos.z++)
    {
        float size;
        struct Vec3 p = vec3_init(pos);
        p = quadrant_translate_position(current_camera_position, p);
        if (t_map::get(pos) == 0)
        {
            size = 0.995f;
            glColor3ub(1,1,1);
        }
        else
        {
            size = 1.005f;
            glColor3ub(180,20,20);
        }

        p = vec3_add(p, vec3_init(0.5f));

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

}   // Animations
