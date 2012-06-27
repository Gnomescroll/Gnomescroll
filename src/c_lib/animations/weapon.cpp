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

static float item_scale = 0.3f;

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
	item_scale += delta;
}

void print_sprite_alignment_config()
{
	printf("Sprite alignment:\n");
	printf("focal_dz: %f\n", focal_dz);
	printf("focal_dxy: %f\n", focal_dxy);
	printf("focal_depth: %f\n", focal_depth);
	printf("origin_dz: %f\n", origin_dz);
	printf("origin_dxy: %f\n", origin_dxy);
	printf("origin_depth: %f\n", origin_depth);
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
	
	// get world space coordinates (configured)
	const float CAMERA_DIST = 0.15f;
	
	//// place sprite at camera position + distance
	Vec3 position = agent_camera->get_position();
	Vec3 forward = agent_camera->forward_vector();
	//position = vec3_add(position, vec3_scalar_mult(forward, CAMERA_DIST));

	//// rotate forward along xy plane
	//const float ROT = 20.0f;
	//const float THETA = (ROT/180.0f) * 3.14159f;	// radians
	//Vec3 right = vec3_copy(forward);
	//right.z = 0;
	//normalize_vector(&right);
	//float x = right.x * cos(THETA) - right.y * sin(THETA);
	//float y = right.x * sin(THETA) + right.y * cos(THETA);
	//right.x = x;
	//right.y = y;
	//right.z = forward.z; 
	//normalize_vector(&right);
	//right = vec3_scalar_mult(right, item_scale);

	Vec3 camera_right = agent_camera->right_vector();
	//// adjust weapon position
    //const float dxy = 0.14f;	// magic numbers
    //const float dz = 0.0f;
	//position.x = position.x + dxy * camera_right.x;
	//position.y = position.y + dxy * camera_right.y;
	//position.z = position.z + dz;


	// calculate focal,origin points from camera and focal/origin deltas
	
	// depths need to use the forward vector
	// xy needs to use the right vector
	// z assumes up vector of {0,0,1}, but we may need to adjust using correct up vector

	// move depth
	Vec3 focal = vec3_add(position, vec3_scalar_mult(forward, focal_depth));
	
	// move vertical
	focal.z += focal_dz;
	
	// move horizontal
	Vec3 dright = camera_right;
	dright.z = 0;
	normalize_vector(&dright);
	dright = vec3_scalar_mult(dright, focal_dxy);
	focal = vec3_add(focal, dright);

	// move horizontal
	dright = camera_right;
	dright.z = 0;
	normalize_vector(&dright);
	dright = vec3_scalar_mult(dright, origin_dxy);
	Vec3 origin = vec3_add(position, dright);
	
	// move vertical
	origin.z += origin_dz;

	// move depth
	origin = vec3_add(origin, vec3_scalar_mult(forward, origin_depth));
	
	// up vector
    Vec3 up = vec3_init(
		model_view_matrix[1],
		model_view_matrix[5],
		model_view_matrix[9]
	);

	// use focal and origin points to calculate right vector
	Vec3 right = vec3_sub(focal, origin);
	normalize_vector(&right);

	// scale to size
	up = vec3_scalar_mult(up, item_scale);
	right = vec3_scalar_mult(right, item_scale);

	// set up opengl state
	glColor4ub(255,255,255,255);
    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

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
