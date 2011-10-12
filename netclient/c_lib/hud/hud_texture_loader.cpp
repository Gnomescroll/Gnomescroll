#include "hud_texture_loader.hpp"

int item_sheet_texture;
int item_slot_texture;
int inventory_background_texture;

int get_item_sheet_texture() {
	return item_sheet_texture;
}

int get_item_slot_texture() {
	return item_slot_texture;
}

int get_inventory_background_texture() {
	return inventory_background_texture;
}

int _init_item_sheet_texture() {
	SDL_Surface *surface;
	surface=IMG_Load("./media/texture/hud/item_sheet_01.png"); ///does this need to be freed?
	if(!surface) {
	    printf("_init_item_sheet_texture, IMG_Load: %s \n", IMG_GetError());
	    return 0;
	}
	if(surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

	glEnable(GL_TEXTURE_2D);

	glGenTextures( 1, (GLuint*)&item_sheet_texture );
	glBindTexture( GL_TEXTURE_2D, item_sheet_texture );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int texture_format;
	if (surface->format->Rmask == 0x000000ff) texture_format = GL_RGBA; else texture_format = GL_BGRA;

	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
	glDisable(GL_TEXTURE_2D);
	return 0;
}

int _init_item_slot_texture() {
	SDL_Surface *surface;
	surface=IMG_Load("./media/texture/hud/item_slot_41x41.png"); ///does this need to be freed?
	if(!surface) {
	    printf("_init_item_slot_texture, IMG_Load: %s \n", IMG_GetError());
	    return 0;
	}
	if(surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

	glEnable(GL_TEXTURE_2D);

	glGenTextures( 1, (GLuint*)&item_slot_texture );
	glBindTexture( GL_TEXTURE_2D, item_slot_texture);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int texture_format;
	if (surface->format->Rmask == 0x000000ff) texture_format = GL_RGBA; else texture_format = GL_BGRA;

	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
	glDisable(GL_TEXTURE_2D);
	return 0;
}

int _init_inventory_background_texture() {
	SDL_Surface *surface;
	surface=IMG_Load("./media/texture/hud/inventory_background_texture_621x352.png"); ///does this need to be freed?
	if(!surface) {
	    printf("_init_inventory_background_texture, IMG_Load: %s \n", IMG_GetError());
	    return 0;
	}
	if(surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

	glEnable(GL_TEXTURE_2D);

	glGenTextures( 1, (GLuint*)&inventory_background_texture );
	glBindTexture( GL_TEXTURE_2D, inventory_background_texture);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int texture_format;
	if (surface->format->Rmask == 0x000000ff) texture_format = GL_RGBA; else texture_format = GL_BGRA;

	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
	glDisable(GL_TEXTURE_2D);
	return 0;
}

int init_hud_texture_loader() {
	_init_item_sheet_texture();
	_init_item_slot_texture();
	_init_inventory_background_texture();
}