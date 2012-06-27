#include "weapon.hpp"

#include <item/_interface.hpp>
#include <item/particle/_interface.hpp>
#include <SDL/texture_sheet_loader.hpp>

namespace Animations
{

static int rendered_item = NULL_ITEM_TYPE;

void draw_equipped_item(int item_type)
{	// draw item in hud

	// setup texture
	using TextureSheetLoader::ItemSheetTexture;
    GS_ASSERT(ItemSheetTexture != 0);
    if (ItemSheetTexture == 0) return;

	if (current_camera == NULL) return;

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
	const float ITEM_SCALE = 0.12f;
	const float CAMERA_DIST = 0.15f;
	
	// place sprite at camera position + distance
	Vec3 position = current_camera->get_position();
	Vec3 forward = current_camera->forward_vector();
	position = vec3_add(position, vec3_scalar_mult(forward, CAMERA_DIST));

	// rotate forward along xy plane
	const float ROT = 20.0f;
	const float THETA = (ROT/180.0f) * 3.14159f;	// radians
	Vec3 right = vec3_copy(forward);
	right.z = 0;
	normalize_vector(&right);
	float x = right.x * cos(THETA) - right.y * sin(THETA);
	float y = right.x * sin(THETA) + right.y * cos(THETA);
	right.x = x;
	right.y = y;
	right.z = forward.z; 
	normalize_vector(&right);
	right = vec3_scalar_mult(right, ITEM_SCALE);

	Vec3 camera_right = current_camera->right_vector();
	// adjust weapon position
    const float dxy = 0.14f;	// magic numbers
    const float dz = 0.0f;
	position.x = position.x + dxy * camera_right.x;
	position.y = position.y + dxy * camera_right.y;
	position.z = position.z + dz;

	// up vector
    Vec3 up = vec3_init(
		model_view_matrix[1],
		model_view_matrix[5],
		model_view_matrix[9]
	);
	up = vec3_scalar_mult(up, ITEM_SCALE);

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
	
	Vec3 p = vec3_sub(position, vec3_add(right, up));
    glTexCoord2f(tx_min, ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_sub(up, right));
    glTexCoord2f(tx_max, ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_add(up, right));
    glTexCoord2f(tx_max, ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(position, vec3_sub(right, up));
    glTexCoord2f(tx_min, ty_min);
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
    
}   // Animations
