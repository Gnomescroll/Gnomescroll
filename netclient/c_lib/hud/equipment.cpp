#include "equipment.hpp"

#include <c_lib/SDL/texture_loader.h>

namespace HudEquipment {

static SDL_Surface* surface;
static GLuint texture;

void init_surface() {

    //int ret;
    //ret = create_texture_and_surface_from_file(char* filename, SDL_Surface* surface, GLuint* tex);
    //if (ret) printf("Loading gradient texture failed with code %d\n", ret);


}

void init() {
    init_surface();
}


void draw() {
}

//cython
void draw_equipment(){draw();}

}
