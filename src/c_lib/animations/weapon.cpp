#include "weapon.hpp"

#include <item/_interface.hpp>
#include <item/particle/_interface.hpp>
#include <SDL/texture_sheet_loader.hpp>

namespace Animations
{

static int rendered_item = NULL_ITEM_TYPE;

// adjustments for aligning weapon sprite
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
{	// vertical distance
	focal_dz += delta;
}

void move_focal_horizontal(float delta)
{	// horizontal distance
	focal_dxy += delta;
}

void move_focal_depth(float delta)
{	// distance from agent plane
	focal_depth += delta;
}

void move_origin_vertical(float delta)
{	// vertical distance along agent plane
	origin_dz += delta;
}

void move_origin_horizontal(float delta)
{	// horizontal distance along agent plane
	origin_dxy += delta;
}

void move_origin_depth(float delta)
{	// distance from agent plane
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

void draw_equipped_item(int item_type)
{	// draw item in hud

	// setup texture
	using TextureSheetLoader::ItemSheetTexture;
    GS_ASSERT(ItemSheetTexture != 0);
    if (ItemSheetTexture == 0) return;

	if (agent_camera == NULL) return;

	// get sprite id
	if (item_type == NULL_ITEM_TYPE)
		item_type = Item::get_item_type("fist");
	int item_sprite = Item::get_sprite_index_for_type(item_type);
	//GS_ASSERT(item_sprite != ERROR_SPRITE);	// TODO -- restore this assert, once there is a fist sprite
	if (item_sprite == ERROR_SPRITE) return;

	rendered_item = item_type;

	// get texture coordinates
	const float SPRITE_WIDTH = 1.0f/16.0f;
	const int TEXTURE_SPRITE_WIDTH = 16;
	float tx_min = (item_sprite % TEXTURE_SPRITE_WIDTH) * SPRITE_WIDTH;
	float tx_max = tx_min + SPRITE_WIDTH;
	float ty_min = (item_sprite / TEXTURE_SPRITE_WIDTH) * SPRITE_WIDTH;
	float ty_max = ty_min + SPRITE_WIDTH;
	
	//// place sprite at camera position + distance
	Vec3 position = agent_camera->get_position();
	Vec3 forward = agent_camera->forward_vector();
	Vec3 camera_right = agent_camera->right_vector();
	Vec3 up = agent_camera->up_vector();

	// calculate focal,origin points from camera and focal/origin deltas
	Vec3 focal = compute_point_offset(
		position, forward, camera_right, up,
		focal_dxy, focal_dz, focal_depth);

	Vec3 origin = compute_point_offset(
		position, forward, camera_right, up,
		origin_dxy, origin_dz, origin_depth);

	// use focal and origin points to calculate right vector
	Vec3 right = vec3_sub(focal, origin);
	normalize_vector(&right);

	// scale to size
	up = vec3_scalar_mult(up, sprite_scale);
	right = vec3_scalar_mult(right, sprite_scale);

	// set up opengl state
	glColor4ub(255,255,255,255);
    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_BLEND, false);
	GL_ASSERT(GL_DEPTH_TEST, false);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    glBindTexture(GL_TEXTURE_2D, ItemSheetTexture);

	// vertex calls
	glBegin(GL_QUADS);
	
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

	glEnd();
	
	// cleanup
    glDisable(GL_ALPHA_TEST);
}

void begin_equipped_item_animation(int item_type)
{
	// begin action animation for item type
}

void stop_equipped_item_animation()
{
	// force stop current action animation
}
    
void init_weapon_sprite()
{
	parse_equipment_sprite_alignment_config();
}

}   // Animations
